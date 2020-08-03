#ifndef __GPIO_H__
#define __GPIO_H__


/* Include File -------------------------------------------------------------*/

#include "cfg.h"


/* Macro Definition ---------------------------------------------------------*/


/* Structure Definition -----------------------------------------------------*/

enum { LED0 = 0X01, LED1 = 0X02, LED_ALL = 0X03 };


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void led_setup( void );
void led_ctrl(u8 sel, u8 ctrl);
void led_toggle(u8 sel);
/* Test Case */
void gpio_em4_wakup( void );
void gpio_extern_interrupt( void );
void gpio_input( void );
void gpio_output( void );
void gpio_blinky( void );

#endif
