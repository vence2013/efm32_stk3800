#ifndef __CFG_H__
#define __CFG_H__


/* Include File -------------------------------------------------------------*/

#include "TypeDef.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>


/* Macro Definition ---------------------------------------------------------*/

#define APP_RTC_CLOCK_ENABLE                                     (1u)
#if (APP_RTC_CLOCK_ENABLE)
#define APP_RTC_CLOCK_DEBUG                                      (1u)
#endif


#if (APP_RTC_CLOCK_DEBUG)
#define USART_PRINTF_ENABLE                                      (1u)
#endif


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/


#endif
