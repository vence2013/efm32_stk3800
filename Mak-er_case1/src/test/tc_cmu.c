#include "em_cmu.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>


#if (TESTCASE_ENABLE)

void tc_cmu_info( void )
{
	u32 reg;

	printf("Oscillators Configuration\r\n");

	{
		u8 band[16];

		memset(band, 0, 16);
		reg = CMU->HFRCOCTRL;
		switch((reg>>8)&0X07)
		{
		case 0: sprintf((char *)band, "1MHz"); break;
		case 1: sprintf((char *)band, "7MHz"); break;
		case 2: sprintf((char *)band, "11MHz"); break;
		case 3: sprintf((char *)band, "14MHz"); break;
		case 4: sprintf((char *)band, "21MHz"); break;
		case 5: sprintf((char *)band, "28MHz"); break;
		}
		printf("\tHFRCO band:%s\r\n", band);
	}
	{
		u8 mode[16], boost[16], timeout[16];

		memset(mode, 0, 16);
		memset(boost, 0, 16);
		memset(timeout, 0, 16);
		reg = CMU->CTRL;
		switch (reg&0X03) // mode
		{
		case 0: sprintf((char *)mode, "XTAL"); break;
		case 1: sprintf((char *)mode, "BUFEXTCLK"); break;
		case 2: sprintf((char *)mode, "DIGEXTCLK"); break;
		}

		switch ((reg>>2)&0X03) // boost
		{
		case 0: sprintf((char *)boost, "50%%"); break;
		case 1: sprintf((char *)boost, "70%%"); break;
		case 2: sprintf((char *)boost, "80%%"); break;
		case 3: sprintf((char *)boost, "100%%"); break;
		}

		switch ((reg>>9)&0X03) // timeout
		{
		case 0: sprintf((char *)timeout, "8 cycles"); break;
		case 1: sprintf((char *)timeout, "256 cycles"); break;
		case 2: sprintf((char *)timeout, "1K(1024) cycles"); break;
		case 3: sprintf((char *)timeout, "16K cycles"); break;
		}

		printf("\tHFXO mode:%s, boost:%s, timeout:%s\r\n", mode, boost, timeout);
	}

	{
		u8 mode[16], boost[16], timeout[16];

		memset(mode, 0, 16);
		memset(boost, 0, 16);
		memset(timeout, 0, 16);

		reg = CMU->CTRL;
		switch ((reg>>11)&0X03)
		{
		case 0: sprintf((char *)mode, "XTAL"); break;
		case 1: sprintf((char *)mode, "BUFEXTCLK"); break;
		case 2: sprintf((char *)mode, "DIGEXTCLK"); break;
		}

		switch ((reg>>12)&0X01)
		{
		case 0: sprintf((char *)boost, "70%%"); break;
		case 1: sprintf((char *)boost, "100%%"); break;
		}

		switch ((reg>>18)&0X03)
		{
		case 0: sprintf((char *)timeout, "8 cycles"); break;
		case 1: sprintf((char *)timeout, "1K(1024) cycles"); break;
		case 2: sprintf((char *)timeout, "16K cycles"); break;
		case 3: sprintf((char *)timeout, "32K cycles"); break;
		}

		printf("\tLFXO mode:%s, boost:%s, timeout:%s\r\n", mode, boost, timeout);
	}

	{
		printf("Oscillators (Enable, Ready, Select as HFCLK source)\r\n");

		reg = CMU->STATUS;
		printf("\tHFRCO:%d,%d,%d; HFXO:%d,%d,%d; AUXHFRCO:%d,%d; LFRCO:%d,%d,%d; LFXO:%d,%d,%d.\r\n\r\n",
				(reg&0X01 ? 1 : 0), (reg&0X02 ? 1 : 0), (reg&0X400 ? 1 : 0),
				(reg&0X04 ? 1 : 0), (reg&0X08 ? 1 : 0), (reg&0X800 ? 1 : 0),
				(reg&0X10 ? 1 : 0), (reg&0X20 ? 1 : 0),
				(reg&0X040 ? 1 : 0), (reg&0X080 ? 1 : 0), (reg&0X1000 ? 1 : 0),
				(reg&0X100 ? 1 : 0), (reg&0X200 ? 1 : 0), (reg&0X2000 ? 1 : 0));
	}

	printf("High Frequency Portion\r\n");

	{
		u8 div[16];

		reg = CMU->CTRL;
		printf("\tHFCLK Division:%d, Frequency:%ld\r\n", (reg>>14)+1, CMU_ClockFreqGet(cmuClock_HF));

		/* HFCORECLK */
		reg = CMU->HFCORECLKDIV;
		memset(div, 0, 16);
		switch(reg&0X0F)
		{
		case 0: sprintf((char *)div, "/1"); break;
		case 1: sprintf((char *)div, "/2"); break;
		case 2: sprintf((char *)div, "/4"); break;
		case 3: sprintf((char *)div, "/8"); break;
		case 4: sprintf((char *)div, "/16"); break;
		case 5: sprintf((char *)div, "/32"); break;
		case 6: sprintf((char *)div, "/64"); break;
		case 7: sprintf((char *)div, "/128"); break;
		case 8: sprintf((char *)div, "/256"); break;
		case 9: sprintf((char *)div, "/512"); break;
		}
		printf("\t\tHFCORECLK Division:%s, Frequency:%ld\r\n", div, CMU_ClockFreqGet(cmuClock_CORE));

		{
			printf("\t\tHFCORECLK Enabled Peripheral\r\n");

			reg = CMU->HFCORECLKEN0;
			printf("\t\t\t%s%s%s%s%s%s\r\n",
					(reg&0X01) ? "DMA, " : "",
					(reg&0X02) ? "AES, " : "",
					(reg&0X04) ? "USBC, ": "",
					(reg&0X08) ? "USB, " : "",
					(reg&0X10) ? "LE, "  : "",
					(reg&0X20) ? "EBI, " : "");
		}

		/* HFPERCLK */
		reg = CMU->HFPERCLKDIV;
		memset(div, 0, 16);
		switch(reg&0X0F)
		{
		case 0: sprintf((char *)div, "/1"); break;
		case 1: sprintf((char *)div, "/2"); break;
		case 2: sprintf((char *)div, "/4"); break;
		case 3: sprintf((char *)div, "/8"); break;
		case 4: sprintf((char *)div, "/16"); break;
		case 5: sprintf((char *)div, "/32"); break;
		case 6: sprintf((char *)div, "/64"); break;
		case 7: sprintf((char *)div, "/128"); break;
		case 8: sprintf((char *)div, "/256"); break;
		case 9: sprintf((char *)div, "/512"); break;
		}
		printf("\t\tHFPERCLK Enable:%d, Division:%s, Frequency:%ld\r\n", (reg&0X100) ? 1 : 0, div,
				CMU_ClockFreqGet(cmuClock_HFPER));

		{
			printf("\t\tHFPERCLK Enabled Peripheral\r\n");

			reg = CMU->HFPERCLKEN0;
			printf("\t\t\t%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\r\n",
					(reg&0X01) ? "USART0, " : "",
					(reg&0X02) ? "USART1, " : "",
					(reg&0X04) ? "USART2, " : "",
					(reg&0X08) ? "UART0, "  : "",
					(reg&0X10) ? "UART1, "  : "",
					(reg&0X20) ? "TIMER0, " : "",
					(reg&0X40) ? "TIMER1, " : "",
					(reg&0X80) ? "TIMER2, " : "",
					(reg&0X100) ? "TIMER3, " : "",
					(reg&0X200) ? "ACMP0, "  : "",
					(reg&0X400) ? "ACMP1, "  : "",
					(reg&0X800) ? "I2C0, "   : "",
					(reg&0X1000) ? "I2C1, "  : "",
					(reg&0X2000) ? "GPIO, "  : "",
					(reg&0X4000) ? "VCMP, "  : "",
					(reg&0X8000) ? "PRS, "   : "",
					(reg&0X10000) ? "ADC0, "  : "",
					(reg&0X20000) ? "ADC1, "  : "");
		}
	}

	printf("Low Frequency Portion\r\n");

	{
		u8 sel[16];

		memset(sel, 0, 16);
		reg = CMU->LFCLKSEL;
		switch(reg&0X03)
		{
		case 0:	sprintf((char *)sel, "%s", (reg&0X10000) ? "ULFRCO" : "Disable"); break;
		case 1: sprintf((char *)sel, "LFRCO"); break;
		case 2: sprintf((char *)sel, "LFXO"); break;
		case 3: sprintf((char *)sel, "HFCORECLKLEDIV2"); break;
		}
		printf("\tLFA Source:%s, Frequency:%ld\r\n", sel, CMU_ClockFreqGet(cmuClock_LFA));

		{
			printf("\t\tLESENSE\t Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X01) ? 1 : 0, 1<<((CMU->LFAPRESC0>>0)&0X03));
			printf("\t\tRTC\t Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X02) ? 1 : 0, 1<<((CMU->LFAPRESC0>>4)&0X0f));
			printf("\t\tLETIMER0 Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X04) ? 1 : 0, 1<<((CMU->LFAPRESC0>>8)&0X0f));
			printf("\t\tLCD\t Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X08) ? 1 : 0, 16<<((CMU->LFAPRESC0>>12)&0X03));
		}

		reg = CMU->LFCLKSEL;
		memset(sel, 0, 16);
		switch((reg>>3)&0X03)
		{
		case 0:	sprintf((char *)sel, "%s", (reg&0X100000) ? "ULFRCO" : "Disable"); break;
		case 1: sprintf((char *)sel, "LFRCO"); break;
		case 2: sprintf((char *)sel, "LFXO"); break;
		case 3: sprintf((char *)sel, "HFCORECLKLEDIV2"); break;
		}
		printf("\tLFB Source:%s, Frequency:%ld\r\n", sel, CMU_ClockFreqGet(cmuClock_LFB));

		{
			printf("\t\tLEUART0\t Enable:%d, Division:%d\r\n", (CMU->LFBCLKEN0&0X01) ? 1 : 0, 1<<((CMU->LFAPRESC0>>0)&0X03));
			printf("\t\tLEUART1\t Enable:%d, Division:%d\r\n", (CMU->LFBCLKEN0&0X02) ? 1 : 0, 1<<((CMU->LFAPRESC0>>4)&0X03));
		}
	}
}


#if (TESTCASE_MODULE_CMU == TESTCASE_MODULE_SEL)

void tc_cmu_info_display( void )
{
	usart_setup();

	tc_cmu_info();

	while(1)
	{
		;
	}
}

/* watch current during different clock enable.
 * 1. Default: 4.8mA
 * 2. Enable all oscillator: 4.9mA
 * 3. Disable all oscillator: 4.8mA
 * 4. HFRCO 21MHz: 6.6mA
 * 5. HFCORECLK /512: 1.1mA
 *
 * Warning: do not control AUXFRCO, may cause debug problem!
 */
void tc_cmu_control( void )
{
	while(1)
	{
		{
			CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
			CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
			CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
			CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);
			CMU_ClockDivSet(cmuClock_CORE, 1);
		}
		usart_setup();
		printf("\r\n------------------Default-------------------------------\r\n");
		tc_cmu_info();
		delay_ms( 10000 );

		/* all oscillator enable */
		printf("\r\n------------------Enable all Oscillator--------------\r\n");
		{
			CMU_HFXOInit_TypeDef initHFXO = CMU_HFXOINIT_DEFAULT;
			CMU_LFXOInit_TypeDef initLFXO = CMU_LFXOINIT_DEFAULT;

			CMU_HFXOInit(&initHFXO);
			CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
			CMU_LFXOInit(&initLFXO);
			CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
		}
		tc_cmu_info();
		delay_ms( 10000 );

		/* all oscillator disable except HFRCO */
		printf("\r\n------------------Disable all Oscillator(except HFRCO)---\r\n");
		{
			CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
			CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
			CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
		}
		tc_cmu_info();
		delay_ms( 10000 );

		/* HFRCO at 21MHz */
		printf("\r\n------------------HFRCO 21MHz-------------------------------\r\n");
		CMU_HFRCOBandSet(cmuHFRCOBand_21MHz);
		usart_setup();
		tc_cmu_info();
		delay_ms( 10000 );

		/* HFCORECLK /512 */
		printf("\r\n------------------HFCORECLK / 512----------------------------\r\n");
		CMU_ClockDivSet(cmuClock_CORE, 512);
		tc_cmu_info();
		delay_ms( 20 );
	}
}


/**************************************************************************//**
 * @brief CMU IRQ handler.
 *****************************************************************************/
void CMU_IRQHandler(void)
{
	u32 flags = CMU_IntGet();

	if (CMU_IFC_HFXORDY & CMU_IntGet())
	{
		/* Clear interrupt flag */
		CMU_IntClear(CMU_IFC_HFXORDY);

		Drv_led(LED0, LED0);
	}
}

void tc_cmu_hfclk_switch_to_hfxo( void )
{
	CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

	usart_setup();
	tc_cmu_info();

	/* init when oscillator disabled, enable after init */
	CMU_LFXOInit(&lfxoInit);
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	CMU_IntEnable( CMU_IFC_HFXORDY );
	NVIC_ClearPendingIRQ(CMU_IRQn);
	NVIC_EnableIRQ(CMU_IRQn);

	while(CMU->STATUS&CMU_STATUS_HFXORDY) ;
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	usart_setup();
	tc_cmu_info();

	while(1)
	{
		Drv_led_toggle( LED1 );
		delay_ms( 1000 );
	}
}

#endif

#endif
