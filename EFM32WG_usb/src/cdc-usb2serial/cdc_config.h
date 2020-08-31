#ifndef __CDC_CONFIG_H
#define __CDC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
**                                                                         **
** Specify total number of endpoints used (in addition to EP0).            **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 3

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 2

/****************************************************************************
**                                                                         **
** Misc. definitions for the interfaces of the composite device.           **
**                                                                         **
*****************************************************************************/

/* Define interface numbers */
#define CDC_CTRL_INTERFACE_NO   (0)
#define CDC_DATA_INTERFACE_NO   (1)
#define NUM_INTERFACES          (2)

#define CDC_NUM_EP_USED         (3)   /* Number of EP's used by CDC function */

/* Define USB endpoint addresses for the interfaces */
#define CDC_EP_DATA_OUT   (0x01)   /* Endpoint for CDC data reception.       */
#define CDC_EP_DATA_IN    (0x81)   /* Endpoint for CDC data transmission.    */
#define CDC_EP_NOTIFY     (0x82)   /* The notification endpoint (not used).  */

/* Define timer ID's */
#define CDC_TIMER_ID        (1)

/* Configuration options for the CDC driver. */
#define CDC_UART_TX_DMA_CHANNEL   (0)
#define CDC_UART_RX_DMA_CHANNEL   (1)
#define CDC_TX_DMA_SIGNAL         DMAREQ_USART1_TXBL
#define CDC_RX_DMA_SIGNAL         DMAREQ_USART1_RXDATAV
#define CDC_UART                  USART1
#define CDC_UART_CLOCK            cmuClock_USART1
#define CDC_UART_ROUTE            (USART_ROUTE_RXPEN   \
                                   | USART_ROUTE_TXPEN \
                                   | USART_ROUTE_LOCATION_LOC1)
#define CDC_UART_TX_PORT          gpioPortD
#define CDC_UART_TX_PIN           0
#define CDC_UART_RX_PORT          gpioPortD
#define CDC_UART_RX_PIN           1
#define CDC_ENABLE_DK_UART_SWITCH()     /* Empty define, this is not a DK. */

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
