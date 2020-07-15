#include "em_cmu.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>


#if (TESTCASE_ENABLE)

void tc_cmu_info( void )
{
	u32 reg;

	{
		u8 mode[16], boost[16], timeout[16];

		printf("Oscillators Configuration\r\n");

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

	{
		u8 div[16];

		printf("High Frequency Portion\r\n");

		reg = CMU->CTRL;
		printf("\tHFCLK Division:%d\r\n", (reg>>14)+1);

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
		printf("\t\tHFCORECLK Division:%s\r\n", div);

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
		printf("\t\tHFPERCLK Enable:%d, Division:%s\r\n", (reg&0X100) ? 1 : 0, div);

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

	{
		u8 sel[16];

		printf("Low Frequency Portion\r\n");

		memset(sel, 0, 16);
		reg = CMU->LFCLKSEL;
		switch(reg&0X03)
		{
		case 0:	sprintf((char *)sel, "%s", (reg&0X10000) ? "ULFRCO" : "Disable"); break;
		case 1: sprintf((char *)sel, "LFRCO"); break;
		case 2: sprintf((char *)sel, "LFXO"); break;
		case 3: sprintf((char *)sel, "HFCORECLKLEDIV2"); break;
		}
		printf("\tLFA Source:%s\r\n", sel);

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
		printf("\tLFB Source:%s\r\n", sel);

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

#endif

#endif
