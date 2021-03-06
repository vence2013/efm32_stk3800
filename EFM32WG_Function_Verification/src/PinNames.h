/* Pin Definition for STK3800 */
#ifndef __PINDEF_H__
#define __PINDEF_H__


/* Include File -------------------------------------------------------------*/

#include "em_gpio.h"


/* Macro Definition ---------------------------------------------------------*/

/* LED */
#define LED_Port                                          (gpioPortE)
#define LED0_Pin                                                  (2)
#define LED1_Pin                                                  (3)

/* Push Button */
#define PB_Port                                           (gpioPortB)
#define PB0_Pin                                                   (9)
#define PB1_Pin                                                  (10)

/* USART0 #LOC5 */
#define UART_Print_Port                                   (gpioPortC)
#define UART_Print_RX_Pin                                         (1)
#define UART_Print_TX_Pin                                         (0)

/* LEUART0 #LOC0 */
#define LEUART_Port                                       (gpioPortD)
#define LEUART_RX_Pin                                             (5)
#define LEUART_TX_Pin                                             (4)


#endif
