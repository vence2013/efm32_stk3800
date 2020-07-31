#include "em_cmu.h"
#include "em_gpio.h"
#include "cfg.h"
#include "PinNames.h"



/******************************************************************************
 * @brief  LED initialize
 *****************************************************************************/
void led_setup( void )
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(LED_Port, LED0_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(LED_Port, LED1_Pin, gpioModePushPull, 0);
}

/******************************************************************************
 * @brief  LED control
 *****************************************************************************/
void led_ctrl(u8 sel, u8 ctrl)
{
	if (sel&LED0)
	{
		if (ctrl&LED0)
		{
			GPIO_PinOutSet(LED_Port, LED0_Pin);
		}
		else
		{
			GPIO_PinOutClear(LED_Port, LED0_Pin);
		}
	}
	if (sel&LED1)
	{
		if (ctrl&LED1)
		{
			GPIO_PinOutSet(LED_Port, LED1_Pin);
		}
		else
		{
			GPIO_PinOutClear(LED_Port, LED1_Pin);
		}
	}
}

/******************************************************************************
 * @brief  LED toggle
 *****************************************************************************/
void led_toggle(u8 sel)
{
	switch (sel)
	{
	case LED0: GPIO_PinOutToggle(LED_Port, LED0_Pin); break;
	case LED1: GPIO_PinOutToggle(LED_Port, LED1_Pin); break;
	}
}

