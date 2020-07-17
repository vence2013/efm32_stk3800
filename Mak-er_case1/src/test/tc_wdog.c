#include "em_cmu.h"
#include "em_wdog.h"
#include "tc.h"
#include "drv.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>


#if (TESTCASE_ENABLE)

void tc_wdog_info( void )
{
	u32 ret;

	printf("---- WDOG Configuration ----\r\n");
	{
		u8 clksel[16], presel[32];

		memset(clksel, 0, 16);
		memset(presel, 0, 32);
		ret = WDOG->CTRL;
		switch ((ret>>8)&0X0F)
		{
		case 0: sprintf((char *)presel, "9 watchdog clock cycle"); break;
		case 1: sprintf((char *)presel, "17 watchdog clock cycle"); break;
		case 2: sprintf((char *)presel, "33 watchdog clock cycle"); break;
		case 3: sprintf((char *)presel, "65 watchdog clock cycle"); break;
		case 4: sprintf((char *)presel, "129 watchdog clock cycle"); break;
		case 5: sprintf((char *)presel, "257 watchdog clock cycle"); break;
		case 6: sprintf((char *)presel, "513 watchdog clock cycle"); break;
		case 7: sprintf((char *)presel, "1k watchdog clock cycle"); break;
		case 8: sprintf((char *)presel, "2k watchdog clock cycle"); break;
		case 9: sprintf((char *)presel, "4k watchdog clock cycle"); break;
		case 10: sprintf((char *)presel, "8k watchdog clock cycle"); break;
		case 11: sprintf((char *)presel, "16k watchdog clock cycle"); break;
		case 12: sprintf((char *)presel, "32k watchdog clock cycle"); break;
		case 13: sprintf((char *)presel, "64k watchdog clock cycle"); break;
		case 14: sprintf((char *)presel, "128k watchdog clock cycle"); break;
		case 15: sprintf((char *)presel, "256k watchdog clock cycle"); break;
		}

		switch((ret>>12)&0X03)
		{
		case 0: sprintf((char *)clksel, "ULFRCO"); break;
		case 1: sprintf((char *)clksel, "LFRCO"); break;
		case 2: sprintf((char *)clksel, "LFXO"); break;
		}

		printf("\t%s%s%s\r\n",
				(ret&0X01) ? "Enable, " : "Disable, ",
				(ret&0X10) ? "Configuration lock, " : "",
				(ret&0X40) ? "Software Oscillator Disable Block, " : "");
		printf("\t%s, %s\r\n", clksel, presel);
		printf("\t%s%s%s\r\n",
				(ret&0X04) ? "EM2 Run Enable, " : "",
				(ret&0X08) ? "EM3 Run Enable, " : "",
				(ret&0x20) ? "EM4 Block" : "");
	}
}

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_WATCHDOG)

void ts_wdog_feed( void )
{
	WDOG_Init_TypeDef initWdog = WDOG_INIT_DEFAULT;

	usart_setup();
	Drv_led_setup();
	Drv_led_flush(LED_ALL, 100);

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* LFRCO 32.768 kHz,
	 * ULFRCO 1kHz, Period 8k, timeout 8s
	 */
	initWdog.clkSel = wdogClkSelULFRCO;
	initWdog.perSel = wdogPeriod_4k;
	WDOG_Init(&initWdog);

	tc_wdog_info();

	while(1)
	{
		WDOG_Feed();

		Drv_led_toggle( LED0 );
		delay_ms( 1000 );
	}
}
#endif

#endif
