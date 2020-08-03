#include "em_cmu.h"
#include "em_acmp.h"
#include "cfg.h"

#if (FUNC_VERIFY == FUNC_ACMP)

void acmp_lightsense( void )
{
	/* Initialize with default settings. */
	ACMP_Init_TypeDef acmpInit = ACMP_INIT_DEFAULT;

	/* The ACMP is a high-frequency peripheral so we need to enable
	 * both the HFPER clock and the ACMP0 clock. */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_ACMP0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0); /* Also the LED0 */
	GPIO_PinOutSet(gpioPortD, 6);

	acmpInit.enable = false;
	ACMP_Init(ACMP0, &acmpInit);

	/* Setup the two channels to compare. The first argument is the negative
	 * channel and the second argument is the positive channel.
	 *
	 * Here we compare channel 6 with the 2.5 V internal reference. When
	 * channel 6 is lower than 2.5 V then the ACMP output is 0 and when
	 * channel 6 voltage is higher than 2.5 V then the ACMP output is 1. */
	ACMP_ChannelSet(ACMP0, acmpChannel1V25, acmpChannel6);

	/* To be able to probe the output we can send the ACMP output to a pin.
	 * The second argument to this function is the pin location which is
	 * device dependent. */
	ACMP_GPIOSetup(ACMP0, 1, true, false);  /* ACMP0#1: PE2 */

	/* Finally we enable the ACMP. */
	ACMP_Enable(ACMP0);
}


/******************************************************************************
 * @brief ACMP0 Interrupt Handler
 *
 ******************************************************************************/
void ACMP0_IRQHandler( void )
{
	if (ACMP_IntGet( ACMP0 ) & ACMP_IF_EDGE)
	{
		ACMP_IntClear(ACMP0, ACMP_IF_EDGE);

		led_toggle( LED1 );
	}
}

void acmp_lightsense_vdd_div_x( void )
{
	/* Initialize with default settings. */
	ACMP_Init_TypeDef acmpInit = ACMP_INIT_DEFAULT;

	/* The ACMP is a high-frequency peripheral so we need to enable
	 * both the HFPER clock and the ACMP0 clock. */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_ACMP0, true);

#if 0
	/* ACMP0 output #1 also the LED0, the following code will be set in led_setup() */
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0); /* Also the LED0 */
#endif
	GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortD, 6);

	acmpInit.enable = false;
	acmpInit.vddLevel = 0X01; /* 3.3 * x / 64 = 0.05 */
	acmpInit.interruptOnFallingEdge = true;
	ACMP_Init(ACMP0, &acmpInit);

	/* Setup the two channels to compare. The first argument is the negative
	 * channel and the second argument is the positive channel.
	 *
	 * Here we compare channel 6 with the 2.5 V internal reference. When
	 * channel 6 is lower than 2.5 V then the ACMP output is 0 and when
	 * channel 6 voltage is higher than 2.5 V then the ACMP output is 1. */
	ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel6);

	/* To be able to probe the output we can send the ACMP output to a pin.
	 * The second argument to this function is the pin location which is
	 * device dependent. */
	ACMP_GPIOSetup(ACMP0, 1, true, false);  /* ACMP0#1: PE2 */

	/* Finally we enable the ACMP. */
	ACMP_Enable(ACMP0);

	ACMP_IntClear(ACMP0, ACMP_IFC_EDGE);
	ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
    NVIC_ClearPendingIRQ(ACMP0_IRQn);
    NVIC_EnableIRQ(ACMP0_IRQn);

	/* ACMP0 output also on LED0 */
	led_setup();
	printf_setup();
	while(1)
	{
		u32 ret = ACMP0->STATUS;

		printf("ACMP0 active:%d, output:%d.\r\n",
				(ret&0X01) ? 1 : 0,
				(ret&0X02) ? 1 : 0);

		delay1k( 1000 );
	}
}

#endif
