#include "em_cmu.h"
#include "em_timer.h"
#include "em_adc.h"
#include "em_prs.h"
#include "cfg.h"


#if (FUNC_VERIFY == FUNC_PRS)

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
	if (TIMER_IntGet( TIMER0 ) & TIMER_IF_OF)
	{
		/* Clear flag for TIMER0 overflow interrupt */
		TIMER_IntClear(TIMER0, TIMER_IF_OF);

		/* Toggle LED ON/OFF */
		led_toggle( LED0 );
	}
}

void timer_setup( void )
{
	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

	CMU_ClockEnable(cmuClock_TIMER0, true);

	TIMER_Reset( TIMER0 );

	/* Set TIMER Top value
	 *   HFRCO 14mHz / prescale(1024), ~= 13672/s
	 * */
	TIMER_TopSet(TIMER0, 13672);
	timerInit.prescale = timerPrescale1024;
	TIMER_Init(TIMER0, &timerInit);

	/* Enable overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);
	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);
}

u32 adv = 0;

/**************************************************************************//**
 * @brief ADC0 IRQ handler.
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
	if (ADC_IntGet( ADC0 ) & ADC_IF_SINGLE)
	{
		ADC_IntClear(ADC0, ADC_IF_SINGLE);

		led_toggle( LED1 );
		adv = ADC_DataSingleGet(ADC0);
	}
}

void adc_setup( void )
{
	ADC_Init_TypeDef adcInit = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

	CMU_ClockEnable(cmuClock_ADC0, true);
	CMU_ClockEnable(cmuClock_PRS, true);

	ADC_Init(ADC0, &adcInit);

	/* Initialize ADC single sample conversion */
	initSingle.acqTime = adcAcqTime8;     /* Acquisition time of 8 ADC clock cycles */
	initSingle.reference = adcRefVDD;     /* VDD or AVDD as ADC reference */
	initSingle.input = adcSingleInpVDDDiv3;   /* VDD as ADC input */
	initSingle.prsEnable = true;          /* PRS enable */
	ADC_InitSingle(ADC0, &initSingle);

	/* Select TIMER0 as source and timer overflow as signal */
	PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);

	/* Enable ADC Interrupt when Single Conversion Complete */
	ADC_IntEnable(ADC0, ADC_IEN_SINGLE);
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_EnableIRQ(ADC0_IRQn);
}

/* TIMER0 trigger ADC every 1 second */
void prs_timer_trigger_adc( void )
{
	led_setup();
	printf_setup();

	adc_setup();
	timer_setup();

	while(1)
	{
		if (adv)
		{
			printf("ADC Result:%d.\r\n", adv);
			adv = 0;
		}

	}
}

#endif
