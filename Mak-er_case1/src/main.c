#include "em_chip.h"
#include "bsp_trace.h"
#include "em_rmu.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_burtc.h"
#include "hal.h"
#include "utils.h"
#include "clock.h"
#include "clockApp_stk.h"
#include "dbg.h"


static uint32_t resetcause = 0;
static uint32_t burtcCountAtWakeup = 0;
struct tm initialCalendar;

/***************************************************************************//**
 * @brief Set up backup domain.
 ******************************************************************************/
void budSetup(void)
{
	/* Assign default TypeDefs */
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;

	/*Setup EM4 configuration structure */
	em4Init.lockConfig = true;
	em4Init.osc = emuEM4Osc_LFXO;
	em4Init.buRtcWakeup = true;
	em4Init.vreg = true;

	/* Setup Backup Power Domain configuration structure */
	bupdInit.probe = emuProbe_Disable;
	bupdInit.bodCal = false;
	bupdInit.statusPinEnable = false;
	bupdInit.resistor = emuRes_Res0;
	bupdInit.voutStrong = false;
	bupdInit.voutMed = false;
	bupdInit.voutWeak = false;
	bupdInit.inactivePower = emuPower_MainBU;
	bupdInit.activePower = emuPower_None;
	bupdInit.enable = true;

	/* Unlock configuration */
	EMU_EM4Lock( false );

	/* Initialize EM4 and Backup Power Domain with init structs */
	EMU_BUPDInit( &bupdInit );
	EMU_EM4Init( &em4Init );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}

/******************************************************************************
 * @brief   Configure backup RTC
 *****************************************************************************/
void burtcSetup(void)
{
	/* Create burtcInit struct and fill with default values */
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Set burtcInit to proper values for this application */
	/* To make this example easier to read, all fields are listed,
	even those which are equal to their default value */
	burtcInit.enable = true;
	burtcInit.mode = burtcModeEM4;
	burtcInit.debugRun = false;
	burtcInit.clkSel = burtcClkSelLFXO;
	burtcInit.clkDiv = burtcClkDiv_128;
	burtcInit.timeStamp = true;
	burtcInit.compare0Top = false;
	burtcInit.lowPowerMode = burtcLPDisable;

	/* Initialize BURTC with burtcInit struct */
	BURTC_Init( &burtcInit );

	BURTC_CompareSet( 0, COUNTS_BETWEEN_UPDATE );

	/* Enable BURTC interrupt on compare match and counter overflow */
	BURTC_IntEnable( BURTC_IF_COMP0 | BURTC_IF_OF );
}

int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	dbg_lcd_basic();

#if 0
	/* Read and clear RMU->RSTCAUSE as early as possible */
	resetcause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();
	/* Enable clock to low energy modules */
	CMU_ClockEnable(cmuClock_CORELE, true);
	/* Read Backup Real Time Counter value */
	burtcCountAtWakeup = BURTC_CounterGet();

	budSetup();

	/* Setting up a structure to initialize the calendar
		for January 1 2012 12:00:00
		The struct tm is declared in time.h
		More information for time.h library in http://en.wikipedia.org/wiki/Time.h */
	initialCalendar.tm_sec    = 0;    /* 0 seconds (0-60, 60 = leap second)*/
	initialCalendar.tm_min    = 0;   /* 0 minutes (0-59) */
	initialCalendar.tm_hour   = 12;   /* 12 hours (0-23) */
	initialCalendar.tm_mday   = 1;   /* 1st day of the month (1 - 31) */
	initialCalendar.tm_mon    = 0;   /* January (0 - 11, 0 = January) */
	initialCalendar.tm_year   = 112;  /* Year 2012 (year since 1900) */
	initialCalendar.tm_wday   = 0;    /* Sunday (0 - 6, 0 = Sunday) */
	initialCalendar.tm_yday   = 0;  /* 1st day of the year (0-365) */
	initialCalendar.tm_isdst  = -1;    /* Daylight saving time; enabled (>0), disabled (=0) or unknown (<0) */

	/* Set the calendar */
	clockInit(&initialCalendar);

	/* Initialize display ++ */
	clockAppInit();

	/* If waking from backup mode, restore time from retention registers */
	if ( (resetcause & RMU_RSTCAUSE_BUMODERST) && !(resetcause & RMU_RSTCAUSE_BUBODBUVIN) && !(resetcause & RMU_RSTCAUSE_PORST) )
	{
		/* Restore time from backup RTC + retention memory and print backup info*/
		clockAppRestore( burtcCountAtWakeup );

		/* Reset timestamp */
		BURTC_StatusClear();
	}
	/* If normal startup, initialize BURTC */
	else
	{
		/* Start LFXO and wait until it is stable */
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

		/* Setup BURTC */
		burtcSetup();

		/* Backup initial calendar (also to initialize retention registers) */
		clockAppBackup();

		/* Update display if necessary */
		clockAppDisplay();
	}

	/* Shut of LFRCO (enabled by SegmentLCD_Init in clockAppInit) and select LFXO as clock for LFA */
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_OscillatorEnable(cmuOsc_LFRCO, false , false);

	/* Enable BURTC interrupts */
	NVIC_ClearPendingIRQ( BURTC_IRQn );
	NVIC_EnableIRQ( BURTC_IRQn );
#endif
	/* Infinite loop */
	while (1) {
#if 0
	    /* Sleep while waiting for interrupt */
	    EMU_EnterEM2(true);

	    /* Update display if necessary */
	    clockAppDisplay();
#endif
	}
}
