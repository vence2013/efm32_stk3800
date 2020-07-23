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
#define TESTCASE_MODULE_BURTC                                    (3u)
#define TESTCASE_MODULE_EMU                                      (4u)
#define TESTCASE_MODULE_WATCHDOG                                 (5u)
#define TESTCASE_MODULE_RMU                                      (6u)
#define TESTCASE_MODULE_CMU_EMU_RMU_BURTC                        (7u)
#define TESTCASE_MODULE_LCD                                      (8u)
#define TESTCASE_MODULE_RTC                                      (9u)
#define TESTCASE_MODULE_MSC                                     (10u)
#define TESTCASE_MODULE_SEL                                      TESTCASE_MODULE_MSC


/* Structure Definition -----------------------------------------------------*/


/* Variable Declaration -----------------------------------------------------*/


/* Exported Function --------------------------------------------------------*/

void tc_blinky( void );

void tc_cmu_info( void );
/* TestCases */
void tc_cmu_info_display( void );
void tc_cmu_control( void );
void tc_cmu_hfclk_switch_to_hfxo( void );

void tc_burtc_info( void );
/* TestCases */
void tc_burtc_seconds( void );
void tc_burtc_lfrco_wrap( void );
void tc_burtc_lowpower( void );

void tc_emu_info( void );
/* TestCases */
void tc_emu_lowpower_mode( void );
void tc_emu_em2block( void );
void tc_emu_save_restore( void );
void tc_emu_babckup_domain( void );

void tc_wdog_info( void );
/* TestCases */
void ts_wdog_feed( void );

u32 tc_rmu_info( void );
/* TestCases */
void tc_rmu_reset_cause( void );
void tc_rmu_reset_system( void );
void tc_rmu_wakeup_from_em4( void );
void tc_rmu_backup_mode( void );

void tc_ecrb_retention_register( void );
void tc_ecrb_timestamp( void );


/* TestCases */
void tc_lcd_display( void );
void tc_lcd_segment_enable( void );
void tc_lcd_mux( void );
void tc_lcd_config_bias( void );
void tc_lcd_config_constrast( void );
void tc_lcd_frame_rate( void );


void tc_rtc_interrupt( void );

void tc_msc_read_write( void );

#endif
