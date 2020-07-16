#ifndef __TC_H__
#define __TC_H__


/* Include File -------------------------------------------------------------*/

#include "typedef.h"
#include "drv.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>


/* Macro Definition ---------------------------------------------------------*/

#define TESTCASE_ENABLE                                          (1u)
#define TESTCASE_MODULE_NONE                                     (0u)
#define TESTCASE_MODULE_BLINKY                                   (1u)
#define TESTCASE_MODULE_CMU                                      (2u)
#define TESTCASE_MODULE_SEL                                      TESTCASE_MODULE_CMU


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void tc_blinky( void );
void tc_cmu_info( void );
void tc_cmu_info_display( void );
void tc_cmu_control( void );
void tc_cmu_hfclk_switch_to_hfxo( void );


#endif
