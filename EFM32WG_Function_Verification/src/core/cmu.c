#include "em_cmu.h"
#include "cmu.h"
#include "cfg.h"


void cmu_info( void )
{
	u32 reg;
	u8 hfrco, hfxo, lfrco, lfxo;

	printf("Oscillators (Enable, Ready, Select as HFCLK source)\r\n");
	{
		reg = CMU->STATUS;

		hfrco = reg&0X001 ? 1 : 0;
		hfxo  = reg&0X004 ? 1 : 0;
		lfrco = reg&0X040 ? 1 : 0;
		lfxo  = reg&0X100 ? 1 : 0;
		printf("\tHFRCO:%d,%d,%d; HFXO:%d,%d,%d; AUXHFRCO:%d,%d; LFRCO:%d,%d,%d; LFXO:%d,%d,%d.\r\n\r\n",
				hfrco, (reg&0X02 ? 1 : 0), (reg&0X400 ? 1 : 0),
				hfxo, (reg&0X08 ? 1 : 0), (reg&0X800 ? 1 : 0),
				(reg&0X10 ? 1 : 0), (reg&0X20 ? 1 : 0),
				lfrco, (reg&0X080 ? 1 : 0), (reg&0X1000 ? 1 : 0),
				lfxo, (reg&0X200 ? 1 : 0), (reg&0X2000 ? 1 : 0));
	}

	printf("Oscillators Configuration\r\n");
	if (hfrco)
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
	if (hfxo)
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
	if (lfxo)
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
		u8 hpherclk, div[16];

		reg = CMU->CTRL;
		printf("HFCLK Division:%d, Frequency:%ld\r\n", (reg>>14)+1, CMU_ClockFreqGet(cmuClock_HF));

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
		printf("\tHFCORECLK Division:%s, Frequency:%ld\r\n", div, CMU_ClockFreqGet(cmuClock_CORE));

		{
			reg = CMU->HFCORECLKEN0;
			printf("\t\tEnabled Peripheral: %s%s%s%s%s%s\r\n",
					(reg&0X01) ? "DMA, " : "",
					(reg&0X02) ? "AES, " : "",
					(reg&0X04) ? "USBC, ": "",
					(reg&0X08) ? "USB, " : "",
					(reg&0X10) ? "LE, "  : "",
					(reg&0X20) ? "EBI, " : "");
		}

		/* HFPERCLK */
		reg = CMU->HFPERCLKDIV;
		hpherclk = (reg&0X100) ? 1 : 0;
		if (hpherclk)
		{
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
			printf("\tHFPERCLK Division:%s, Frequency:%ld\r\n", div, CMU_ClockFreqGet(cmuClock_HFPER));

			reg = CMU->HFPERCLKEN0;
			printf("\t\tEnabled Peripheral: %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\r\n",
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
		printf("LFA Source:%s, Frequency:%ld\r\n", sel, CMU_ClockFreqGet(cmuClock_LFA));

		{
			printf("\tLESENSE\t Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X01) ? 1 : 0, 1<<((CMU->LFAPRESC0>>0)&0X03));
			printf("\tRTC\t Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X02) ? 1 : 0, 1<<((CMU->LFAPRESC0>>4)&0X0f));
			printf("\tLETIMER0 Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X04) ? 1 : 0, 1<<((CMU->LFAPRESC0>>8)&0X0f));
			printf("\tLCD\t Enable:%d, Division:%d\r\n", (CMU->LFACLKEN0&0X08) ? 1 : 0, 16<<((CMU->LFAPRESC0>>12)&0X03));
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
		printf("LFB Source:%s, Frequency:%ld\r\n", sel, CMU_ClockFreqGet(cmuClock_LFB));

		{
			printf("\tLEUART0\t Enable:%d, Division:%d\r\n", (CMU->LFBCLKEN0&0X01) ? 1 : 0, 1<<((CMU->LFAPRESC0>>0)&0X03));
			printf("\tLEUART1\t Enable:%d, Division:%d\r\n", (CMU->LFBCLKEN0&0X02) ? 1 : 0, 1<<((CMU->LFAPRESC0>>4)&0X03));
		}
	}
}

#if (FUNC_VERIFY == FUNC_CMU)


/* Current difference in different clock states.
 * 1. Default: 4.5mA
 * 2. Enable all oscillator: 4.7mA
 * 3. HFRCO 21MHz: 7mA
 * 4. HFCORECLK /512: 1.5mA
 * 5. Default(only HFRCO): 4.5mA
 * backup to step 2
 *
 * Warning: do not control AUXFRCO, may cause debug problem!
 */
void cmu_osc_enable_disable( void )
{
	printf_setup();
	cmu_info();

	while(1)
	{
		printf("\r\n------------------Enable all Oscillator--------------\r\n");
		{
			CMU_HFXOInit_TypeDef initHFXO = CMU_HFXOINIT_DEFAULT;
			CMU_LFXOInit_TypeDef initLFXO = CMU_LFXOINIT_DEFAULT;

			CMU_HFXOInit(&initHFXO);
			CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
			CMU_LFXOInit(&initLFXO);
			CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
		}
		cmu_info();
		delay1k( 10000 );

		printf("\r\n------------------HFRCO 21MHz-------------------------------\r\n");
		delay1k( 1 );
		CMU_HFRCOBandSet(cmuHFRCOBand_21MHz);
		printf_setup();
		cmu_info();
		delay1k( 10000 );

		printf("\r\n------------------HFCORECLK / 512----------------------------\r\n");
		CMU_ClockDivSet(cmuClock_CORE, 512);
		cmu_info();
		delay1k( 20 );

		printf("\r\n------------------Default (only HFRCO)-----------------\r\n");
		{
			CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
			CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
			CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
			CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);
			CMU_ClockDivSet(cmuClock_CORE, 1);
		}
		printf_setup();
		cmu_info();
		delay1k( 10000 );
	}
}



/**************************************************************************//**
 * @brief CMU IRQ handler.
 *****************************************************************************/
void CMU_IRQHandler(void)
{
	if (CMU_IFC_HFXORDY & CMU_IntGet())
	{
		/* Clear interrupt flag */
		CMU_IntClear(CMU_IFC_HFXORDY);

		led_ctrl(LED0, LED0);
	}
}

/* Observe the current difference between the two clock source */
void cmu_switch_to_hfxo( void )
{
	CMU_HFXOInit_TypeDef initHFXO = CMU_HFXOINIT_DEFAULT;

	led_setup();
	printf_setup();
	printf("\r\n--------------- HFRCO as Source -------------------------\r\n");
	cmu_info();
	delay1k( 10000 );

	/* enable oscillator before switch!
	 *   although CMU_ClockSelectSet() will enable the selected socillator, but it
	 * may cause CPU clock stop for a while.
	 */
	CMU_HFXOInit(&initHFXO);
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

	CMU_IntEnable( CMU_IFC_HFXORDY );
	NVIC_ClearPendingIRQ(CMU_IRQn);
	NVIC_EnableIRQ(CMU_IRQn);

	/* HFCLK frequency change, must re-call printf_setup() */
	printf_setup();
	printf("\r\n--------------- HFXO as Source -------------------------\r\n");
	cmu_info();

	while(1)
	{
		led_toggle( LED1 );
		delay1k( 1000 );
	}
}

#endif
