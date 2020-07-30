#include "em_cmu.h"
#include "em_emu.h"
#include "em_timer.h"
#include "em_prs.h"
#include "cfg.h"


void timer_info( void )
{
	u32 ret;

	{
		u8 mode[16], clksel[16], presc[16];

		memset(mode, 0, sizeof(mode));
		memset(clksel, 0, sizeof(clksel));
		memset(presc, 0, sizeof(presc));

		ret = TIMER0->CTRL;
		switch ((ret>>0)&0X03)
		{
		case 0: sprintf((char *)mode, "UP");     break;
		case 1: sprintf((char *)mode, "DOWN");   break;
		case 2: sprintf((char *)mode, "UPDOWN"); break;
		case 3: sprintf((char *)mode, "QDEC");   break;
		}

		switch((ret>>16)&0X03)
		{
		case 0: sprintf((char *)clksel, "PRESCHFPERCLK"); break;
		case 1: sprintf((char *)clksel, "CC1");           break;
		case 2: sprintf((char *)clksel, "TIMEROUF");      break;
		}

		switch ((ret>>24)&0X0F)
		{
		case 0: sprintf((char *)presc, "DIV1"); break;
		case 1: sprintf((char *)presc, "DIV2"); break;
		case 2: sprintf((char *)presc, "DIV4"); break;
		case 3: sprintf((char *)presc, "DIV8"); break;
		case 4: sprintf((char *)presc, "DIV16"); break;
		case 5: sprintf((char *)presc, "DIV32"); break;
		case 6: sprintf((char *)presc, "DIV64"); break;
		case 7: sprintf((char *)presc, "DIV128"); break;
		case 8: sprintf((char *)presc, "DIV256"); break;
		case 9: sprintf((char *)presc, "DIV512"); break;
		case 10: sprintf((char *)presc, "DIV1024"); break;
		}

		printf("TIMER0 Mode:%s, Clock Source:%s, Presclae:%s\r\n", mode, clksel, presc);
	}

	{
		printf("TIMER0 TOP:%ld, CNT:%ld, CCx:%ld.\r\n",
				TIMER_TopGet( TIMER0 ), TIMER_CounterGet( TIMER0 ), TIMER0->CC[0].CCV);
	}
}

#if (FUNC_VERIFY == FUNC_TIMER)

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
	u32 ret = TIMER_IntGet(TIMER0);

	if (ret & TIMER_IF_OF)
	{
		/* Clear flag for TIMER0 overflow interrupt */
		TIMER_IntClear(TIMER0, TIMER_IF_OF);

		/* Toggle LED ON/OFF */
		led_toggle( LED0 );
		led_ctrl(LED1, LED1);
	}

	if (ret & TIMER_IF_CC0)
	{
		/* Clear flag for TIMER0 overflow interrupt */
		TIMER_IntClear(TIMER0, TIMER_IF_CC0);

		led_toggle( LED1 );
	}
}

void timer_overflow_10ms( void )
{
	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	TIMER_Reset( TIMER0 );
	/* Set TIMER Top value
	 *   HFRCO 14mHz / prescale(8), 1750 * 10 = 10ms
	 * */
	TIMER_TopSet(TIMER0, 17500);
	/* Configure TIMER */
	timerInit.prescale = timerPrescale8;
	TIMER_Init(TIMER0, &timerInit);

	/* Enable overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);
	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);

	led_setup();

	while(1)
	{
		EMU_EnterEM1();
	}
}

/* TIM0_CC0 #3 = PD1 */
void timer_compare_pwm( void )
{
	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
	TIMER_InitCC_TypeDef timerCCInit  = TIMER_INITCC_DEFAULT;

	/* Enable clock for GPIO module */
	CMU_ClockEnable(cmuClock_GPIO, true);
	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	/* Set CC0 pin as output */
	GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
    /* Route CC0 to location and enable pin */
    TIMER0->ROUTE = TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3;

	TIMER_Reset( TIMER0 );
	/* Configure TIMER */
	timerInit.enable   = false;
	timerInit.prescale = timerPrescale8;
	TIMER_Init(TIMER0, &timerInit);
	/* Configure CC channel 0 */
	timerCCInit.cmoa = timerOutputActionToggle;
	timerCCInit.mode = timerCCModePWM;
	TIMER_InitCC(TIMER0, 0, &timerCCInit);

	/* Set TIMER Top value
	 *   HFRCO 14mHz / prescale(8), 1750/1ms
	 * */
	TIMER_TopSet(TIMER0, 17500);
	/* Set compare value starting at 0 - it will be incremented in the interrupt handler */
	TIMER_CompareBufSet(TIMER0, 0, 3500);

	/* Enable overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IF_OF | TIMER_IF_CC0);
	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);

	TIMER_Enable(TIMER0, true);
}

/* Get the CNT value when PB0 press */
void timer_input_capture( void )
{
	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
	TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

	/* Enable clock for GPIO module */
	CMU_ClockEnable(cmuClock_GPIO, true);
	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);
	/* Enable clock for PRS module */
	CMU_ClockEnable(cmuClock_PRS, true);

	/* Configure PB0_PIN as an input for PB0 button with filter and pull-up (dout = 1)*/
	GPIO_PinModeSet(PB_Port, PB0_Pin, gpioModeInputPullFilter, 1);
	/* Select PB0_PIN as external interrupt source*/
	GPIO_IntConfig(PB_Port, PB0_Pin, false, false, false);
	/* Select GPIO as source and PB0_PRS_SIGSEL_PIN as signal for PRS channel 0 */
	PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN9, prsEdgeOff);

	TIMER_Reset( TIMER0 );
	/* Configure CC channel 0 */
	timerCCInit.prsSel   = timerPRSSELCh0;
	timerCCInit.prsInput = true;
	timerCCInit.mode = timerCCModeCapture;
	TIMER_InitCC(TIMER0, 0, &timerCCInit);

	/* Configure timer */
	timerInit.prescale = timerPrescale8;
	TIMER_Init(TIMER0, &timerInit);

	printf_setup();
	while( 1 )
	{
		timer_info();

		delay1k( 1000 );
	}
}

#endif
