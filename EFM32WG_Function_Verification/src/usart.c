#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "cfg.h"
#include <stdio.h>


static USART_TypeDef* usart   = USART0;


void printf_setup( void )
{
	USART_InitAsync_TypeDef initAsync = USART_INITASYNC_DEFAULT;

	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_USART0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	USART_InitAsync(usart, &initAsync);

	GPIO_PinModeSet(UART_Print_Port, UART_Print_TX_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_Print_Port, UART_Print_RX_Pin, gpioModeInput, 0);
	usart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC5;
}

size_t _write(int handle, const unsigned char* buffer, unsigned char size)
{
	unsigned char nChars = 0;

	if (buffer == 0)
	{
		return 0;
	}

	for (/* Empty*/; size != 0; --size)
	{
		USART_Tx(usart, (u8)buffer[nChars]);
		++nChars;
	}

	return nChars;
}


#if (FUNC_VERIFY == FUNC_USART)

void usart_send_recv( void )
{
	USART_InitAsync_TypeDef initAsync = USART_INITASYNC_DEFAULT;
	u8 tCnt = 0;

	/* USART is a HFPERCLK peripheral. Enable HFPERCLK domain and USART0.
	 * We also need to enable the clock for GPIO to configure pins. */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_USART0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* USART_Reset() will be called in InitAsync() */
	USART_InitAsync(usart, &initAsync);

	/* Configure GPIO pins */
	GPIO_PinModeSet(UART_Print_Port, UART_Print_TX_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_Print_Port, UART_Print_RX_Pin, gpioModeInput, 0);
	usart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC5;

	while(1)
	{
		USART_Tx(usart, tCnt);
		led_ctrl(LED0, (tCnt%2) ? LED0 : 0);

		tCnt++;
		delay1k( 1000 );
	}
}

void usart_printf( void )
{
	u8 tCnt = 0;

	printf_setup();

	while(1)
	{
		printf("Counter:%d.\r\n", tCnt++);

		delay1k( 1000 );
	}
}

#endif
