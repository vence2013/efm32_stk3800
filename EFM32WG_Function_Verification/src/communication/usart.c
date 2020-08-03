#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "cfg.h"
#include <stdio.h>
#include <string.h>


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


void usart_info( void )
{
	u32 ret;

	printf("USARTx Configuration\r\n");
	{
		u8 ovs[16];

		memset(ovs, 0, sizeof(ovs));
		ret = usart->CTRL;

		switch ((ret>>5)&0X03)
		{
		case 0: sprintf((char *)ovs, "Oversampling: X16, "); break;
		case 1: sprintf((char *)ovs, "Oversampling: X8, "); break;
		case 2: sprintf((char *)ovs, "Oversampling: X6, "); break;
		case 3: sprintf((char *)ovs, "Oversampling: X4, "); break;
		}

		printf("\t%s%s%s%s%s%s%s%s\r\n",
				(ret&USART_CTRL_SYNC) ? "synchronous mode" : "asynchronous mode, ",
				ovs,
				(ret&USART_CTRL_TXBIL) ? "TX Buffer Interrupt Level:HALFFULL, " : "TX Buffer Interrupt Level:EMPTY, ",
				(ret&USART_CTRL_RXINV) ? "Receiver Input Invert, " : "",
				(ret&USART_CTRL_TXINV) ? "Transmitter output Invert, " : "",
				(ret&USART_CTRL_BIT8DV) ? "Bit 8 Default Value: 1, " : "Bit 8 Default Value: 0, ",
				(ret&USART_CTRL_ERRSRX) ? "Disable RX On Error, " : "",
				(ret&USART_CTRL_ERRSTX) ? "Disable TX On Error, " : "");
	}

	printf("\tBaud Rate: %ld\r\n", USART_BaudrateGet(usart));
	{
		u8 bits[16], parity[16], stop[16];

		memset(bits, 0, sizeof(bits));
		memset(parity, 0, sizeof(parity));
		memset(stop, 0, sizeof(stop));
		ret = usart->FRAME;
		switch((ret>>0)&0X0F)
		{
		case 1: sprintf((char *)bits, "4 bits"); break;
		case 2: sprintf((char *)bits, "5 bits"); break;
		case 3: sprintf((char *)bits, "6 bits"); break;
		case 4: sprintf((char *)bits, "7 bits"); break;
		case 5: sprintf((char *)bits, "8 bits"); break;
		case 6: sprintf((char *)bits, "9 bits"); break;
		case 7: sprintf((char *)bits, "10 bits"); break;
		case 8: sprintf((char *)bits, "11 bits"); break;
		case 9: sprintf((char *)bits, "12 bits"); break;
		case 10: sprintf((char *)bits, "13 bits"); break;
		case 11: sprintf((char *)bits, "14 bits"); break;
		case 12: sprintf((char *)bits, "15 bits"); break;
		case 13: sprintf((char *)bits, "16 bits"); break;
		}

		switch ((ret>>8)&0X03)
		{
		case 0: sprintf((char *)parity, "NONE"); break;
		case 1: sprintf((char *)parity, "EVEN"); break;
		case 2: sprintf((char *)parity, "ODD"); break;
		}

		switch ((ret>>12)&0X03)
		{
		case 0: sprintf((char *)stop, "0.5"); break;
		case 1: sprintf((char *)stop, "1.0"); break;
		case 2: sprintf((char *)stop, "1.5"); break;
		case 3: sprintf((char *)stop, "2.0"); break;
		}
		printf("\tFrame Data-Bit:%s, Parity-Bit:%s, Stop-Bit:%s.\r\n", bits, parity, stop);
	}
}


#if (FUNC_VERIFY == FUNC_USART)

u8 recv = 0;

/**************************************************************************//**
 * @brief USART0 RX IRQ Handler
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
	if (USART_IntGet(usart) & USART_IF_RXDATAV)
	{
		led_toggle(LED1);

		recv = USART_Rx(usart);
	}
}

void usart_recv_interrupt( void )
{
	USART_InitAsync_TypeDef initAsync = USART_INITASYNC_DEFAULT;

	/* USART is a HFPERCLK peripheral. Enable HFPERCLK domain and USART0.
	 * We also need to enable the clock for GPIO to configure pins. */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_USART0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* USART_Reset() will be called in InitAsync() */
	USART_InitAsync(usart, &initAsync);
	usart->CTRL |= UART_CTRL_TXBIL;

	/* Configure GPIO pins */
	GPIO_PinModeSet(UART_Print_Port, UART_Print_TX_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_Print_Port, UART_Print_RX_Pin, gpioModeInput, 0);
	usart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC5;

	/* Prepare UART Rx and Tx interrupts */
	USART_IntClear(usart, UART_IF_RXDATAV);
	USART_IntEnable(usart, UART_IF_RXDATAV);
	NVIC_ClearPendingIRQ(USART0_RX_IRQn);
	NVIC_EnableIRQ(USART0_RX_IRQn);

	led_setup();
	usart_info();

	while(1)
	{
		if (recv)
		{
			led_toggle(LED0);
			printf("Received:%02x.\r\n", recv);

			recv = 0;
		}
#if 0
		/* can work without recv interrupt */
		if (USART_StatusGet(usart) & UART_STATUS_RXDATAV)
		{
			u8 ch = USART_Rx(usart);

			led_toggle(LED0);
			printf("Received:%02x.\r\n", ch);
		}
#endif
	}
}


/**************************************************************************//**
 * @brief USART0 TX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 *****************************************************************************/
void USART0_TX_IRQHandler( void )
{
	if (USART_IntGet(usart) & UART_IF_TXC)
	{
		USART_IntClear(usart, UART_IF_TXC);

		led_toggle(LED1);
	}
}

void usart_send_interrupt( void )
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
	usart->CTRL |= UART_CTRL_TXBIL;

	/* Configure GPIO pins */
	GPIO_PinModeSet(UART_Print_Port, UART_Print_TX_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_Print_Port, UART_Print_RX_Pin, gpioModeInput, 0);
	usart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC5;

	/* Prepare UART Rx and Tx interrupts */
	USART_IntClear(usart, UART_IF_TXC);
	USART_IntEnable(usart, UART_IF_TXC);
	NVIC_ClearPendingIRQ(USART0_TX_IRQn);
	NVIC_EnableIRQ(USART0_TX_IRQn);

	led_setup();
	while(1)
	{
		led_toggle(LED0);
		USART_TxDouble(usart, (0X10+tCnt)<<8|(0X20+tCnt));

		tCnt++;
		delay1k( 5000 );
	}
}


/* send counter every seconds, if received then send out */
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

	usart_info();

	while(1)
	{
		USART_Tx(usart, tCnt);


		if (USART_StatusGet( usart ) & USART_STATUS_RXDATAV)
		{
			u8 ch = USART_Rx( usart );

			USART_Tx(usart, ch);
		}

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
