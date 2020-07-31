#ifndef __UTILS_H__
#define __UTILS_H__


/* Include File -------------------------------------------------------------*/

#include "TypeDef.h"


/* Macro Definition ---------------------------------------------------------*/


/* Structure Definition -----------------------------------------------------*/

enum { LED0 = 0X01, LED1 = 0X02, LED_ALL = 0X03 };


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void delay(u32 x);
void printf_setup( void );
/* LED */
void led_setup( void );
void led_ctrl(u8 sel, u8 ctrl);
void led_toggle(u8 sel);
/* MCU info */
void rmu_info(u32 resetCause);



#endif
