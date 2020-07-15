#ifndef __DBG_H__
#define __DBG_H__


/* Include File -------------------------------------------------------------*/

#include "typedef.h"
#include "dbg_lcd.h"
#include "dbg_usart.h"


/* Macro Definition ---------------------------------------------------------*/


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void dbg_wdog( void );
void dbg_rmu( void );
void dbg_cmu( void );
void dbg_emu( void );


#endif