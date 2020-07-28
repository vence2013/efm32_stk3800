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
#define FUNC_USART                                              (21u)
#define FUNC_LEUART                                             (22u)
/* Auxiliary */
#define FUNC_WDOG                                               (40u)
#define FUNC_VERIFY                                             FUNC_RMU


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

#include "cmu.h"
#include "burtc.h"
#include "emu.h"
#include "rmu.h"
#include "gpio.h"
#include "usart.h"
#include "leuart.h"


#endif
