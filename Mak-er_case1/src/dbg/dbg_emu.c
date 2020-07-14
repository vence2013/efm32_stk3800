#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "drv.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>


void enable_all_clocks( void )
{
	/* Turning on all oscillators
	 * Then waiting for all oscillators to stabilize
	 */
	CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	/* Switching the CPU clock source to HFXO
	 * This will increase current consumption
	 */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	/* Enabling clocks to all core and peripheral modules */
	#if defined( _CMU_HFCORECLKEN0_MASK )
		CMU->HFCORECLKEN0 = 0xFFFFFFFF;
	#endif

	delay_ms( 5000 );
}

void disable_all_clocks( void )
{
	/* Switching the CPU clock source to HFRCO */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	/* Disabling every oscillator except hfrco and lfxo
	 * NOTE: MAKE SURE NOT TO DISABLE THE CURRENT CPU CLOCK!!
	 */
	CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
	CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
	CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

	/* Disabling all unused clocks. The LE clock must be on in order to use the RTC */
	#if defined( _CMU_HFCORECLKEN0_MASK )
		CMU->HFCORECLKEN0 = CMU_HFCORECLKEN0_LE;
	#endif

	delay_ms( 5000 );
}

void downscale_core_clock( void )
{
	/* Changing the band of the HFRCO */
	#if defined( _CMU_HFRCOCTRL_BAND_MASK )
		CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);
	#endif

	// Setting prescaling of the CPU clock
	CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_4);

	delay_ms( 5000 );
}

void enter_em4( void )
{
  // Configure EM4 behavior
  EMU_EM4Init_TypeDef init = EMU_EM4INIT_DEFAULT;

  EMU_EM4Init(&init);
  EMU_EnterEM4();
}

void energy_modes( void )
{
	delay_ms( 5000 );
	enable_all_clocks();
	disable_all_clocks();
	downscale_core_clock();

	//enter_em4();
	EMU_EnterEM3(false);
}

/**************************************************************************//**
 * @brief EMU IRQ handler.
 *****************************************************************************/
void EMU_IRQHandler(void)
{
	EMU_IntClear( 0X01 );

	Drv_led_toggle( LED0 );
}

void emu_bu( void )
{
	EMU_BUPDInit_TypeDef initBUPD = 	EMU_BUPDINIT_DEFAULT;

	EMU_BUPDInit(&initBUPD);

	EMU_IntEnable( 0X01 );
	NVIC_ClearPendingIRQ(EMU_IRQn);
	NVIC_EnableIRQ(EMU_IRQn);
}

#define RMU_EXTRST_XMASK 		  0x00000009UL
#define RMU_BODREGRST_XMASK     0x0000001FUL
#define RMU_BODUNREGRST_XMASK   0x00000003UL

/* PF1 at middle of DEBUG interface */
void emu_em4_gpio_wakeup( void )
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(gpioPortF, 1, gpioModeInputPullFilter, 1);
	GPIO_EM4EnablePinWakeup(0X08, 0);

	uint32_t rstCause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	{
		u8 buf[64];

		memset(buf, 0, sizeof(buf));
		sprintf((char *)buf, "Reset Cause:%08X\r\n", rstCause);
		usart_put_data(buf, strlen((char *)buf));
	}

	// If the last Reset was due to leaving EM4 via external reset, toggle LEDs. Else, delay and enter EM4
	if (rstCause == 0X0E)
	{
		delay_ms( 3000 );
		EMU_EnterEM4();
	}
	else
	{
		Drv_led_toggle( LED0 );
	}
}

void dbg_emu( void )
{
	emu_em4_gpio_wakeup();

	while(1)
	{
		;
	}
}
