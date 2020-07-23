#include "em_cmu.h"
#include "em_rtc.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>


#if (TESTCASE_ENABLE)

void tc_rtc_info( void )
{
	u32 reg;

	printf("RTC Configuration\r\n");
	reg = RTC->CTRL;
	printf("\t%s%s\r\n",
			(reg&0X01) ? "Enable, " : "Disable, ",
			(reg&0X04) ? "Compare Channel 0 is Top Value, " : "");
	printf("RTC Counter:%ld, Compare 0:%d Compare 1:%d\r\n",
			RTC_CounterGet(), RTC_CompareGet( 0 ), RTC_CompareGet( 1 ));
}

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_RTC)

/**************************************************************************//**
 * @brief RTC IRQ handler.
 *****************************************************************************/
void RTC_IRQHandler(void)
{
	if (RTC_IntGet() & RTC_IF_COMP0)
	{
		RTC_IntClear( RTC_IF_COMP0 );
		RTC_CompareSet(0, RTC_CompareGet( 0 ) + 32);

		Drv_led_toggle( LED0 );
	}

	if (RTC_IntGet() & RTC_IF_COMP1)
	{
		RTC_IntClear( RTC_IF_COMP1 );
		RTC_CompareSet(1, RTC_CompareGet( 1 ) + 64);

		Drv_led_toggle( LED1 );
	}
}

void tc_rtc_interrupt( void )
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

	RTC_CompareSet(0, 32);
	RTC_CompareSet(1, 64);

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ( RTC_IRQn );
	RTC_IntEnable( RTC_IF_COMP0 | RTC_IF_COMP1 );

	usart_setup();

	while(1)
	{
		tc_rtc_info();
		delay_ms( 1000 );
	}
}

#endif

#endif
