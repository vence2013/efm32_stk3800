#include "led.h"
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