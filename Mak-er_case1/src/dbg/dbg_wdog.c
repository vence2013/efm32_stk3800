#include "em_wdog.h"
#include "dbg.h"
#include "drv.h"
#include "utils.h"


/* 1. feed    : usart data 0x00 - 0xff
 * 2. not feed: usart data 0x00 - 0x08
 * 3. disabled: usart data 0x00 - 0xff
 */
void dbg_wdog_basic( void )
{
	u8 tCnt = 0;

	/* Default:
	 *   LFRCO: 32.768kHZ
	 *   15/256K  (2^(3+15) + 1)/32.738K = 264145 / 32768 = 8.061s
	 */
	WDOG_Init_TypeDef initWDOG = WDOG_INIT_DEFAULT;

	WDOG_Init(&initWDOG);
	//WDOG_Enable( false );

	while(1)
	{
		//WDOG_Feed();
		usart_put_char( tCnt++ );

		delay_ms( 1000 );
	}
}

/*
 *     run in EM2: LED flush every 8s
 * not run in EM2: LED not flush every 8s
 */
void dbg_wdog_em3( void )
{
	WDOG_Init_TypeDef initWDOG = WDOG_INIT_DEFAULT;

	initWDOG.em2Run = true;
	WDOG_Init(&initWDOG);

	Drv_led_flush(LED0, 100);

	while(1)
	{
		EMU_EnterEM2( false );
	}
}
void dbg_wdog( void )
{
	dbg_wdog_em3();
}
