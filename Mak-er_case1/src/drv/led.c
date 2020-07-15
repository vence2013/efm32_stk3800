#include "em_cmu.h"
#include "em_gpio.h"
#include "led.h"
#include "utils.h"
#include "pindef.h"


/******************************************************************************
 * @brief  LED control
 *****************************************************************************/
void Drv_led(u8 mask, u8 ctrl)
{
	if (mask&LED0)
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
	if (mask&LED1)
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
void Drv_led_toggle(u8 sel)
{
	switch (sel)
	{
	case LED0:
		GPIO_PinOutToggle(LED_Port, LED0_Pin);
		break;
	case LED1:
		GPIO_PinOutToggle(LED_Port, LED1_Pin);
		break;
	}
}

/******************************************************************************
 * @brief  LED flush
 *****************************************************************************/
void Drv_led_flush(u8 sel, u8 ms)
{
	Drv_led(sel, LED_ALL);
	delay_ms( ms );
	Drv_led(sel, 0);
}

/******************************************************************************
 * @brief  LED flush
 *****************************************************************************/
void Drv_led_setup( void )
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(LED_Port, LED0_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(LED_Port, LED1_Pin, gpioModePushPull, 0);
}
