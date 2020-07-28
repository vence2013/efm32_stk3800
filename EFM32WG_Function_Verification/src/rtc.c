#include "em_cmu.h"
#include "em_rtc.h"
#include "cfg.h"


void rtc_info( void )
{
	u32 reg;

	reg = RTC->CTRL;
	if (reg&0X01)
	{
		printf("RTC Configuration:\r\n\t%s\r\n",
				(reg&0X04) ? "Compare Channel 0 is Top Value, " : "");

		printf("RTC Counter:%ld, Compare 0: %ld Compare 1: %ld\r\n",
				RTC_CounterGet(), RTC_CompareGet( 0 ), RTC_CompareGet( 1 ));
	}
	else
	{
		printf("RTC Disabled\r\n");
	}
}

#if (FUNC_VERIFY == FUNC_RTC)


/**************************************************************************//**
 * @brief RTC IRQ handler.
 *****************************************************************************/
void RTC_IRQHandler(void)
{
	if (RTC_IntGet() & RTC_IF_COMP0)
	{
		RTC_IntClear( RTC_IF_COMP0 );
		RTC_CompareSet(0, RTC_CompareGet( 0 ) + 32);

		led_toggle( LED0 );
	}

	if (RTC_IntGet() & RTC_IF_COMP1)
	{
		RTC_IntClear( RTC_IF_COMP1 );
		RTC_CompareSet(1, RTC_CompareGet( 1 ) + 64);

		led_toggle( LED1 );
	}
}

void rtc_1s_2s_interrupt( void )
{
	RTC_Init_TypeDef initRTC = RTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_1024);
	CMU_ClockEnable(cmuClock_RTC, true);

	RTC_Reset();
	initRTC.comp0Top = false;
	RTC_Init(&initRTC);

	/* 32768 Hz / 1024 = 32 */
	RTC_CompareSet(0, 32);
	RTC_CompareSet(1, 64);

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ( RTC_IRQn );
	RTC_IntEnable( RTC_IF_COMP0 | RTC_IF_COMP1 );

	led_setup();
	printf_setup();
	rtc_info();
}

#endif
