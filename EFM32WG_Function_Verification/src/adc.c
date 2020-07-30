#include "em_cmu.h"
#include "em_adc.h"
#include "cfg.h"


#if (FUNC_VERIFY == FUNC_ADC)

/* ADC0_CH0/PD0, ADC0_CH1/PD1 */
void adc_single( void )
{
	ADC_Init_TypeDef adcInit = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

	  /* Enable GPIO clock in CMU */
	  CMU_ClockEnable(cmuClock_GPIO, true);
	  /* Enable ADC clock */
	  CMU_ClockEnable(cmuClock_ADC0, true);

	  /* Rest ADC registers */
	  ADC_Reset(ADC0);
	  /* Init common settings for both single conversion and scan mode */
	  adcInit.timebase = ADC_TimebaseCalc(0);
	  adcInit.prescale = ADC_PrescaleCalc(1000000, 0);
	  ADC_Init(ADC0, &adcInit);

	  /* Init for single conversion, measure VDD/3 with 1.25V reference. */
	  singleInit.input      = adcSingleInpVDDDiv3;
	  /* The datasheet specifies a minimum aquisition time when sampling VDD/3 */
	  /* 32 cycles should be safe for all ADC clock frequencies */
	  singleInit.acqTime = adcAcqTime32;
	  ADC_InitSingle(ADC0, &singleInit);

	  printf_setup();
	  while(1)
	  {
		  u32 sampleValue;

		  ADC_Start(ADC0, adcStartSingle);
		  /* Wait while conversion is active */
		  while (ADC0->STATUS & ADC_STATUS_SINGLEACT) ;

		  /* Get ADC result */
		  sampleValue = ADC_DataSingleGet(ADC0);

		  /* Calculate supply voltage relative to 1.25V reference */
		  sampleValue = (sampleValue * 1250 * 3) / 4096;
		  printf("VDD Voltage: %d mV.\r\n", sampleValue);

		  delay1k( 3000 );
	  }
}

#endif
