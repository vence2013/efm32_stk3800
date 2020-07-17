#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_wdog.h"
#include "em_burtc.h"
#include "em_device.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>

#if (TESTCASE_ENABLE)

u32 tc_rmu_info( void )
{
	u32 resetCause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	printf("Reset Cause: 0X%08X\r\n", resetCause);
	printf("\t%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\r\n",
			(resetCause&0X0001) ? "Power On Reset, " : "",
			(resetCause&0X0002) ? "Brown Out Detector Unregulated Domain Reset, " : "",
			(resetCause&0X0004) ? "Brown Out Detector Regulated Domain Reset, " : "",
			(resetCause&0X0008) ? "External Pin Reset, " : "",
			(resetCause&0X0010) ? "Watchdog Reset, " : "",
			(resetCause&0X0020) ? "LOCKUP Reset, " : "",
			(resetCause&0X0040) ? "System Request Reset, " : "",
			(resetCause&0X0080) ? "EM4 Reset, " : "",
			(resetCause&0X0100) ? "EM4 Wake-up Reset, " : "",
			(resetCause&0X0200) ? "AVDD0 Bod Rese, " : "",
			(resetCause&0X0400) ? "AVDD1 Bod Reset, " : "",
			(resetCause&0X0800) ? "Backup Brown Out Detector VDD_DREG, " : "",
			(resetCause&0X1000) ? "Backup Brown Out Detector BU_VIN, " : "",
			(resetCause&0X2000) ? "Backup Brown Out Detector Unregulated Domain, " : "",
			(resetCause&0X4000) ? "Backup Brown Out Detector Regulated Domain, " : "",
			(resetCause&0X8000) ? "Backup mode reset, " : "");

	return resetCause;
}

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_RMU)

void wdog_setup( void )
{
	WDOG_Init_TypeDef initWdog = WDOG_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* LFRCO 32.768 kHz,
	 * ULFRCO 1kHz, Period 8k, timeout 8s
	 */
	initWdog.clkSel = wdogClkSelULFRCO;
	initWdog.perSel = wdogPeriod_8k;
	WDOG_Init(&initWdog);
}

/* Power On Reset: plug in/out power
 *   Power On Reset,
 *   Brown Out Detector Unregulated Domain Reset,
 * Watchdog: 10s cycle
 *   Watchdog Reset,
 * Reset Button:
 *   Brown Out Detector Unregulated Domain Reset, Brown Out Detector Regulated Domain Reset, External Pin Reset,
 * ...
 */
void tc_rmu_reset_cause( void )
{
	wdog_setup();

	usart_setup();
	Drv_led_setup();
	Drv_led_flush(LED_ALL, 100);
	tc_rmu_info();

	while (1)
	{
		;
	}
}

/* [如何进行软复位操作](https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2016/07/17/_-D6yU) */
void tc_rmu_reset_system( void )
{
	usart_setup();
	Drv_led_setup();
	Drv_led_flush(LED_ALL, 100);
	tc_rmu_info();

	delay_ms( 5000 );
	NVIC_SystemReset();
}


/***************************************************************************//**
 * @brief RTC Interrupt Handler
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_IntClear( BURTC_IF_COMP0 );

		Drv_led_toggle( LED1 );
	}
}

/* 10s */
void burtc_setup( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	/* use ULFRCO as source, only 2 frequency : 2kHz when /1, 1kHz other value */
	burtcInit.mode         = burtcModeEM4;
	burtcInit.clkDiv       = burtcClkDiv_2;
	burtcInit.compare0Top  = true;
	BURTC_CompareSet( 0, 10000 ); /* 10s */

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );

	BURTC_Init( &burtcInit );
}


void tc_rmu_wakeup_from_em4( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

	burtc_setup();
	EMU_EM4Init(&em4Init);

	usart_setup();
	Drv_led_setup();
	Drv_led_flush(LED0, 100);

	tc_rmu_info();

	delay_ms( 5000 );
	EMU_EnterEM4();
}


void budSetup(void)
{
	/* Assign default TypeDefs */
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;

	/* Unlock configuration */
	EMU_EM4Lock( false );

	/*Setup EM4 configuration structure */
	em4Init.lockConfig = true;
	EMU_EM4Init( &em4Init );

	/* Initialize EM4 and Backup Power Domain with init structs */
	EMU_BUPDInit( &bupdInit );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}

void tc_rmu_backup_mode( void )
{
	usart_setup();
	Drv_led_setup();
	Drv_led_flush(LED0, 100);

	tc_rmu_info();

	budSetup();
	delay_ms( 2000 );
	EMU_EnterEM2( false );
}

#endif

#endif
