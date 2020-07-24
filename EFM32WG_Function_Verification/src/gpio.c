#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>


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
void led_ctrl(u8 mask, u8 ctrl)
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


void gpio_pin_info(GPIO_Port_TypeDef port, unsigned int pin)
{
	GPIO_Mode_TypeDef mode = GPIO_PinModeGet(port, pin);
	u32 ret;

	printf("P%c%d Information\r\n", 'A'+port, pin);
	{
		u8 drivemode[16], modestr[32];

		memset(drivemode, 0, sizeof(drivemode));
		memset(modestr, 0, sizeof(modestr));

		/* Drive Mode */
		ret = GPIO->P[port].CTRL;
		switch ((ret>>0)&0X03)
		{
		case 0: sprintf((char *)drivemode, "STANDARD(6mA)"); break;
		case 1: sprintf((char *)drivemode, "LOWEST(0.1mA)"); break;
		case 2: sprintf((char *)drivemode, "HIGH(20mA)"); break;
		case 3: sprintf((char *)drivemode, "LOW(1mA)"); break;
		}

		/* Pin Mode */
		switch (mode)
		{
		case gpioModeDisabled:
			sprintf((char *)modestr, "Input Disable, %s",
					GPIO_PinOutGet(port, pin) ? "Pull-up, " : "");
			break;
		case gpioModeInput:
			sprintf((char *)modestr, "Input Enable, %s",
					GPIO_PinOutGet(port, pin) ? "Filter, " : "");
			break;
		case gpioModeInputPull:
			sprintf((char *)modestr, "Input Enable, %s",
					GPIO_PinOutGet(port, pin) ? "Pull-up, " : "Pull-down, ");
			break;
		case gpioModeInputPullFilter:
			sprintf((char *)modestr, "Input Enable, Filter, %s",
					GPIO_PinOutGet(port, pin) ? "Pull-up, " : "Pull-down, ");
			break;
		case gpioModePushPull: sprintf((char *)modestr, "Push-pull, "); break;
		case gpioModePushPullDrive:
			sprintf((char *)modestr, "Push-pull, Drive Strength:%s, ", drivemode);
			break;
		case gpioModeWiredOr: sprintf((char *)modestr, "Wired-or, "); break;
		case gpioModeWiredOrPullDown: sprintf((char *)modestr, "Wired-or, Pull-down"); break;
		case gpioModeWiredAnd: sprintf((char *)modestr, "Open-drain, "); break;
		case gpioModeWiredAndFilter: sprintf((char *)modestr, "Open-drain, Filter, "); break;
		case gpioModeWiredAndPullUp: sprintf((char *)modestr, "Open-drain, Pull-up, "); break;
		case gpioModeWiredAndPullUpFilter: sprintf((char *)modestr, "Open-drain, Filter, Pull-up, "); break;
		case gpioModeWiredAndDrive:
			sprintf((char *)modestr, "Open-drain, Drive Strength:%s, ", drivemode);
			break;
		case gpioModeWiredAndDriveFilter:
			sprintf((char *)modestr, "Open-drain, Filter, Drive Strength:%s, ", drivemode);
			break;
		case gpioModeWiredAndDrivePullUp:
			sprintf((char *)modestr, "Open-drain, Pull-up, Drive Strength:%s, ", drivemode);
			break;
		case gpioModeWiredAndDrivePullUpFilter:
			sprintf((char *)modestr, "Open-drain, Filter, Pull-up, Drive Strength:%s, ", drivemode);
			break;
		}

		printf("\tConfiguration:%s\r\n", modestr);
	}

	{
		if ((gpioModeInput <= mode) && (mode <= gpioModeInputPullFilter))
		{
			printf("\tInput: %d\r\n", GPIO_PinInGet(port, pin));
		}
		else if ((gpioModePushPull <= mode) && (mode <= gpioModeWiredAndDrivePullUpFilter))
		{
			printf("\tOutput: %d\r\n", GPIO_PinOutGet(port, pin));
		}
	}
}

#if (FUNC_VERIFY == FUNC_GPIO)

/* Warnning: Do not enter EM4 immediately after power-on, it may cause failure to program again !
 *
 */
void gpio_em4_wakup( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

	EMU_EM4Init(&em4Init);

	CMU_ClockEnable(cmuClock_GPIO, true);
	/* PF1(SWDIO) as EM4 wake-up source  */
	GPIO_PinModeSet(gpioPortF, 1, gpioModeInputPullFilter, 1);
	/* Enable wake-up from PF1, Polarity select low */
	GPIO_EM4EnablePinWakeup(GPIO_EM4WUEN_EM4WUEN_F1, 0);

	led_setup();
	led_ctrl(LED0, LED0);
	delay1k( 5000 );
	led_ctrl(LED0, 0);
	while(1)
	{
		EMU_EnterEM4();
	}
}

/******************************************************************************
 * @brief GPIO Odd Interrupt Handler
 *
 *  Increment minute on GPIO interrupt from even numbered pins. Assume no
 *  other odd GPIO interrupts are enabled.
 *
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	GPIO_IntClear(1 << PB0_Pin);

	GPIO_PinOutToggle(LED_Port, LED0_Pin);
}

/******************************************************************************
 * @brief GPIO Even Interrupt Handler
 *
 *  Increment hour on GPIO interrupt from even numbered pins. Assume no
 *  other even GPIO interrupts are enabled.
 *
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
	GPIO_IntClear(1 << PB1_Pin);

	GPIO_PinOutToggle(LED_Port, LED1_Pin);
}

/* Push button 0 trigger interrupt at failling, toggle LED0;
 * Push button 1 trigger interrupt at rising, toggle LED1;
 */
void gpio_extern_interrupt( void )
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* PB9 Configuration */
	GPIO_PinModeSet(PB_Port, PB0_Pin, gpioModeInput, 0);
	GPIO_IntConfig(PB_Port, PB0_Pin, false, true, true);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
    /* PB10 Configuration */
	GPIO_PinModeSet(PB_Port, PB1_Pin, gpioModeInput, 0);
	GPIO_IntConfig(PB_Port, PB1_Pin, true, false, true);
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	GPIO_PinModeSet(LED_Port, LED0_Pin, gpioModePushPullDrive, 0);
	GPIO_PinModeSet(LED_Port, LED1_Pin, gpioModePushPullDrive, 0);
}

/* read push button, set LED to the same; print push button status */
void gpio_input( void )
{
	u8 key, keyPrevious = 0;

	printf_setup();

	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(PB_Port, PB0_Pin, gpioModeInput , 0);
	GPIO_PinModeSet(LED_Port, LED0_Pin, gpioModePushPullDrive, 0);

	gpio_pin_info(PB_Port, PB0_Pin);

	while(1)
	{
		key = GPIO_PinInGet(PB_Port, PB0_Pin);
		if (key != keyPrevious)
		{
			printf("Key: %d.\r\n", key);
		}
		keyPrevious = key;

		/* display Push Button 0 status to LED0*/
		if (key)
		{
			GPIO_PinOutSet(LED_Port, LED0_Pin);
		}
		else
		{
			GPIO_PinOutClear(LED_Port, LED0_Pin);
		}
	}
}

/* LED blinky with different drive strength(6mA, 20mA, 0.1mA) */
void gpio_output( void )
{
	printf_setup();

	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(LED_Port, LED0_Pin, gpioModePushPullDrive, 0);

	/* Test show:
	 *   in hardware STK3800, drive mode standard & high has not much difference.
	 */
	while(1)
	{
		GPIO_DriveModeSet(LED_Port, gpioDriveModeStandard);
		GPIO_PinOutSet(LED_Port, LED0_Pin);
		gpio_pin_info(LED_Port, LED0_Pin);
		delay1k( 3000 );
		GPIO_PinOutClear(LED_Port, LED0_Pin);
		delay1k( 3000 );

		GPIO_DriveModeSet(LED_Port, gpioDriveModeHigh);
		GPIO_PinOutToggle(LED_Port, LED0_Pin);
		gpio_pin_info(LED_Port, LED0_Pin);
		delay1k( 3000 );
		GPIO_PinOutToggle(LED_Port, LED0_Pin);
		delay1k( 3000 );

		GPIO_DriveModeSet(LED_Port, gpioDriveModeLowest);
		GPIO_PinOutToggle(LED_Port, LED0_Pin);
		gpio_pin_info(LED_Port, LED0_Pin);
		delay1k( 3000 );
		GPIO_PinOutToggle(LED_Port, LED0_Pin);
		delay1k( 3000 );
	}
}

void gpio_blinky( void )
{
	led_setup();

	while(1)
	{
		led_ctrl(LED_ALL, LED_ALL);
		delay1k( 1000 );
		led_ctrl(LED_ALL, 0);
		delay1k( 1000 );
	}
}

#endif
