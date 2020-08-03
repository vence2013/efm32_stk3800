#include "em_cmu.h"
#include "em_gpio.h"
#include "em_leuart.h"
#include "cfg.h"
#include <stdio.h>
#include <string.h>


#if (FUNC_VERIFY == FUNC_LEUART)

static LEUART_TypeDef* leuart   = LEUART0;


void leuart_send_recv( void )
{
	LEUART_Init_TypeDef leuartInit = LEUART_INIT_DEFAULT;
	u8 tCnt = 0;

	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

	/* Enabling clocks, all other remain disabled */
	CMU_ClockEnable(cmuClock_GPIO, true);       /* Enable GPIO clock */
	CMU_ClockEnable(cmuClock_CORELE, true);     /* Enable CORELE clock */
	CMU_ClockEnable(cmuClock_LEUART0, true);    /* Enable LEUART1 clock */

	LEUART_Reset( leuart );
	LEUART_Init(leuart, &leuartInit);

	/* Route LEUART0 TX pin to location 0 */
	leuart->ROUTE = LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN | LEUART_ROUTE_LOCATION_LOC0;
	/* Enable GPIO for LEUART0 TX */
	GPIO_PinModeSet(LEUART_Port, LEUART_TX_Pin, gpioModePushPull, 1);
	/* Enable GPIO for LEUART1 RX */
	GPIO_PinModeSet(LEUART_Port, LEUART_RX_Pin, gpioModeInputPull,1);

	while (1)
	{
		u8 i, ch, str[64];

		memset(str, 0, sizeof(str));
		if (LEUART_StatusGet( leuart ) & LEUART_STATUS_RXDATAV)
		{
			ch = LEUART_Rx( leuart );
			sprintf((char *)str, "%d(%d), ", tCnt, ch);
		}
		else
		{
			sprintf((char *)str, "%d, ", tCnt);
		}

		for (i = 0; i < strlen((const char *)str); i++)
		{
			LEUART_Tx(leuart, str[i]);
		}

		tCnt++;
		delay1k( 1000 );
	}
}

#endif
