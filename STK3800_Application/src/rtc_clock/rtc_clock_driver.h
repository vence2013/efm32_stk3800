#ifndef __RTC_CLOCK_DRIVER_H__
#define __RTC_CLOCK_DRIVER_H__


/* Include File -------------------------------------------------------------*/

#include "cfg.h"


/* Macro Definition ---------------------------------------------------------*/


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

#if (APP_RTC_CLOCK_ENABLE)
void bud_setup( void );
void lcd_setup( void );
void burtc_setup( void );
#endif

#endif
