/* Used as header file template */
#ifndef __CFG_H__
#define __CFG_H__


/* Include File -------------------------------------------------------------*/

#include "PinNames.h"
#include "TypeDef.h"
#include "util.h"
#include <stdio.h>
#include <string.h>


/* Macro Definition ---------------------------------------------------------*/

/* Core */
#define FUNC_CMU                                                ( 1u)
#define FUNC_BURTC                                              ( 2u)
#define FUNC_EMU                                                ( 3u)
#define FUNC_RMU                                                ( 4u)
#define FUNC_GPIO                                               ( 5u)
/* Communication */
#define FUNC_USART                                              (12u)
#define FUNC_LEUART                                             (13u)

#define FUNC_VERIFY                                              FUNC_BURTC


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

#include "cmu.h"
#include "burtc.h"
#include "gpio.h"
#include "usart.h"
#include "leuart.h"


#endif
