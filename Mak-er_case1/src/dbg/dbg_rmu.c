#include "em_rmu.h"
#include "em_gpio.h"
#include "em_wdog.h"
#include "core_cm4.h"
#include "pindef.h"
#include "dbg.h"
#include "drv.h"
#include <stdio.h>
#include <string.h>


void print_reset_cause( void )
{
	u32 reset_cause;
	u8 buf[64];

	memset(buf, 0, sizeof(64));
	reset_cause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	if (reset_cause&0X1)
	{
		sprintf((char *)buf, "Reset Cause(%08X): Power-on Reset.\r\n", reset_cause);
	}
	else if ((reset_cause&0X9) == 0X8)
	{
		sprintf((char *)buf, "Reset Cause(%08X): External Reset.\r\n", reset_cause);
	}
	else if ((reset_cause&0X13) == 0X10)
	{
		sprintf((char *)buf, "Reset Cause(%08X): Watchdog Reset.\r\n", reset_cause);
	}
	else if ((reset_cause&0X7df) == 0X40)
	{
		sprintf((char *)buf, "Reset Cause(%08X): System Reset.\r\n", reset_cause);
	}
	else
	{
		sprintf((char *)buf, "Reset Cause(%08X): Unkown.\r\n", reset_cause);
	}


	usart_put_data(buf, strlen((char *)buf));
}

/* Reset Cause Test:
 * 1. POR Reset: Plug-in/out USB power
 * 2. Watchdog Reset: wait 8s
 * 3. External Reset:Press RESET button
 * 4. System Reset: uncomment NVIC_SystemReset()
 */
void dbg_rmu_group1( void )
{
	WDOG_Init_TypeDef initWDOG = WDOG_INIT_DEFAULT;

	WDOG_Init(&initWDOG); /* if not feed, reset after 8s */

	delay_ms( 5000 );
	//NVIC_SystemReset();
}

/*
 * Reset Cause Test:
 * 1. wakeup from EM4: no following pin in STK3800
 * Pin wakeup from EM4: A0/A6/C9/F1/F2/E13
 */
void dbg_rmu_group2( void )
{
}

void dbg_rmu( void )
{
	Drv_led_flush(LED0, 100);
	print_reset_cause();

	dbg_rmu_group2();

	while(1)
	{
		if (!GPIO_PinInGet(PB_Port, PB0_Pin))
		{
			delay_ms( 50 );
			if (!GPIO_PinInGet(PB_Port, PB0_Pin))
			{

			}
		}
	}
}
