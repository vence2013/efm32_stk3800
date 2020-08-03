#include "em_cmu.h"
#include "em_wdog.h"
#include "cfg.h"


void wdog_info( void )
{
	u32 ret;
	u8 enable, clksel[16], presel[32];

	memset(clksel, 0, 16);
	memset(presel, 0, 16);
	ret = WDOG->CTRL;
	switch ((ret>>8)&0X0F)
	{
	case 0:  sprintf((char *)presel, "   9"); break;
	case 1:  sprintf((char *)presel, "  17"); break;
	case 2:  sprintf((char *)presel, "  33"); break;
	case 3:  sprintf((char *)presel, "  65"); break;
	case 4:  sprintf((char *)presel, " 129"); break;
	case 5:  sprintf((char *)presel, " 257"); break;
	case 6:  sprintf((char *)presel, " 513"); break;
	case 7:  sprintf((char *)presel, "  1k"); break;
	case 8:  sprintf((char *)presel, "  2k"); break;
	case 9:  sprintf((char *)presel, "  4k"); break;
	case 10: sprintf((char *)presel, "  8k"); break;
	case 11: sprintf((char *)presel, " 16k"); break;
	case 12: sprintf((char *)presel, " 32k"); break;
	case 13: sprintf((char *)presel, " 64k"); break;
	case 14: sprintf((char *)presel, "128k"); break;
	case 15: sprintf((char *)presel, "256k"); break;
	}

	switch((ret>>12)&0X03)
	{
	case 0: sprintf((char *)clksel, "ULFRCO"); break;
	case 1: sprintf((char *)clksel, "LFRCO"); break;
	case 2: sprintf((char *)clksel, "LFXO"); break;
	}

	enable = (ret&0X01) ? 1 : 0;
	if (enable)
	{
		printf("WDOG Clock Source: %s, %s watchdog clock cycle\r\n", clksel, presel);
		printf("\t%s%s\r\n",
				(ret&0X10) ? "Configuration lock, " : "",
				(ret&0X40) ? "Software Oscillator Disable Block, " : "");

		printf("\t%s%s%s\r\n",
				(ret&0X04) ? "EM2 Run Enable, " : "",
				(ret&0X08) ? "EM3 Run Enable, " : "",
				(ret&0x20) ? "EM4 Block" : "");
	}
	else
	{
		printf("WDOG Disabled\r\n");
	}

}

#if (FUNC_VERIFY == FUNC_WDOG)

void wdog_feed( void )
{
	WDOG_Init_TypeDef initWdog = WDOG_INIT_DEFAULT;

	printf_setup();
	wdog_info();
	rmu_info();

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* LFRCO 32.768 kHz,
	 * ULFRCO 1kHz, Period 8k, timeout 8s
	 */
	initWdog.clkSel = wdogClkSelULFRCO;
	initWdog.perSel = wdogPeriod_8k;
	WDOG_Init(&initWdog);

	led_setup();
	led_ctrl(LED_ALL, LED_ALL);
	delay1k( 100 );
	led_ctrl(LED_ALL, 0);

	while(1)
	{
		/* Comment this out to test watchdog reset */
		//WDOG_Feed();
	}
}

#endif
