#ifndef __USART_H__
#define __USART_H__


/* Include File -------------------------------------------------------------*/


/* Macro Definition ---------------------------------------------------------*/


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void printf_setup( void );
/* Test Case */
void usart_recv_interrupt( void );
void usart_send_interrupt( void );
void usart_send_recv( void );
void usart_printf( void );


#endif
