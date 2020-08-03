#include "em_cmu.h"
#include "em_adc.h"
#include "em_dac.h"
#include "cfg.h"


#if (FUNC_VERIFY == FUNC_DAC)

/* set to dacConvModeSampleHold will be discharged to 0V soon */
void dac_output( void )
{
	DAC_Init_TypeDef dacInit = DAC_INIT_DEFAULT;
	DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;

	/* Enable the DAC clock */
	CMU_ClockEnable(cmuClock_DAC0, true);

	//dacInit.convMode = dacConvModeSampleHold;
	/* Calculate the DAC clock prescaler value that will result in a DAC clock
	 * close to 500kHz. Second parameter is zero, if the HFPERCLK value is 0, the
	 * function will check what the current value actually is. */
	dacInit.prescale = DAC_PrescaleCalc(500000, 0);
	/* Set reference voltage to 1.25V */
	dacInit.reference = dacRef1V25;
	/* Initialize the DAC and DAC channel. */
	DAC_Init(DAC0, &dacInit);

	initChannel.enable = true;
	DAC_InitChannel(DAC0, &initChannel, 0); /* DAC0_OUT0: PB11*/

	DAC_Channel0OutputSet(DAC0, 2048);
}

void dac_sine( void )
{

}

#endif
