#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "cfg.h"
#include "PinNames.h"


#if (USART_PRINTF_ENABLE)

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

#endif
