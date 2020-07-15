#ifndef __LED_H__
#define __LED_H__


/* Include File -------------------------------------------------------------*/

#include "typedef.h"


/* Macro Definition ---------------------------------------------------------*/


/* Structure Definition -----------------------------------------------------*/

enum { LED0 = 0X01, LED1 = 0X02, LED_ALL = 0X03 };


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void Drv_led(u8 mask, u8 ctrl);
void Drv_led_toggle(u8 sel);
void Drv_led_flush(u8 sel, u8 ms);
void Drv_led_setup( void );

#endif
