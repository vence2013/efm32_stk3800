#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "segmentlcd.h"
#include "clock.h"
#include "rtc_clock.h"
#include "rtc_clock_driver.h"
#include "cfg.h"
#include <stdint.h>


#if (APP_RTC_CLOCK_ENABLE)

struct tm calendar;

/* Declare variables for time keeping */
static uint32_t  burtcOverflowIntervalRem;


void rtc_clock_backup(void);

/***************************************************************************//**
 * @brief RTC Interrupt Handler, invoke callback if defined.
 *        The interrupt table is in assembly startup file startup_efm32gg.s
 *        Do critical tasks in interrupt handler. Other tasks are handled in main
 *        while loop.
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	/*   Increment compare value and
	 *   update TFT display            */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_CompareSet( 0, BURTC_CompareGet(0) + COUNTS_BETWEEN_UPDATE );

		BURTC_IntClear( BURTC_IF_COMP0 );
	}

	/* Interrupt source: counter overflow */
	/*   Increase overflow counter
	 *   and backup calendar              */
	if ( BURTC_IntGet() & BURTC_IF_OF )
	{
		clockOverflow();
		rtc_clock_backup();

		BURTC_IntClear( BURTC_IF_OF );
	}
}

/***************************************************************************//**
 * @brief  Show current time on TFT display
 *
 ******************************************************************************/
void rtc_clock_display(void)
{
	time_t    currentTime;
	u8 displayStringBuf[6];

	currentTime = time( NULL );
	calendar = * localtime( &currentTime );

	/* Make string from calendar */
	displayStringBuf[0] = 0x30 + (calendar.tm_hour / 10);
	displayStringBuf[1] = 0x30 + (calendar.tm_hour % 10);;
	displayStringBuf[2] = 0x30 + (calendar.tm_min / 10);
	displayStringBuf[3] = 0x30 + (calendar.tm_min % 10);
	displayStringBuf[4] = 0x30 + (calendar.tm_sec / 10);
	displayStringBuf[5] = 0x30 + (calendar.tm_sec % 10);

	/* Print time on LCD */
	SegmentLCD_Write( (char *)displayStringBuf );
	SegmentLCD_Symbol(LCD_SYMBOL_COL3, 1);
	SegmentLCD_Symbol(LCD_SYMBOL_COL5, 1);
}

/***************************************************************************//**
 * @brief  Backup CALENDAR to retention registers
 *
 *   RET[0].REG : number of BURTC overflows
 *   RET[1].REG : epoch offset
 *
 ******************************************************************************/
void rtc_clock_backup(void)
{
	/* Write overflow counter to retention memory */
	BURTC_RetRegSet( 0, clockGetOverflowCounter() );

	/* Write local epoch offset to retention memory */
	BURTC_RetRegSet( 1, clockGetStartTime() );
}

/***************************************************************************//**
 * @brief  Restore CALENDAR from retention registers
 *
 ******************************************************************************/
void rtc_clock_restore( void )
{
	uint32_t burtcStart, burtcCount, nextUpdate;
	uint32_t burtcTimestamp, burtcOverflowCounter;

	/* Store current BURTC value for consistency in display output within this function */
	burtcCount = BURTC_CounterGet();
	/* Timestamp is BURTC value at time of main power loss */
	burtcTimestamp = BURTC_TimestampGet();
	/* Read overflow counter from retention memory */
	burtcOverflowCounter = BURTC_RetRegGet( 0 );

	/* Check for overflow while in backup mode
	   Assume that overflow interval >> backup source capacity
	   i.e. that overflow has only occured once during main power loss */
	if ( burtcCount < burtcTimestamp )
	{
		burtcOverflowCounter++;
	}

	/* Restore epoch offset from retention memory */
	clockSetStartTime( BURTC_RetRegGet( 1 ) );

	/* Restore clock overflow counter */
	clockSetOverflowCounter(burtcOverflowCounter);

	/* Calculate start point for current BURTC count cycle
	 If (COUNTS_BETWEEN_UPDATE/burtcOverflowInterval) is not an integer,
	 BURTC value at first update is different between each count cycle */
	burtcStart = (burtcOverflowCounter * (COUNTS_BETWEEN_UPDATE - burtcOverflowIntervalRem)) % COUNTS_BETWEEN_UPDATE;

	/*  Calculate next update compare value
	  Add 1 extra UPDATE_INTERVAL to be sure that counter doesn't
	  pass COMP value before interrupts are enabled */
	nextUpdate = burtcStart + ((burtcCount / COUNTS_BETWEEN_UPDATE) +1 ) * COUNTS_BETWEEN_UPDATE ;
	BURTC_CompareSet( 0, nextUpdate );
}

void clock_setup( void )
{
	struct tm initialCalendar;

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
}

void rtc_clock_setup( void )
{
	u32 resetcause = RMU_ResetCauseGet();

	RMU_ResetCauseClear();

#if (APP_RTC_CLOCK_DEBUG)
	printf_setup();
#endif

	bud_setup();
	lcd_setup();
	clock_setup();

	/* Compute overflow interval (integer) and remainder */
	burtcOverflowIntervalRem = ((uint64_t)UINT32_MAX+1)%COUNTS_BETWEEN_UPDATE;

	rmu_info( resetcause );

	if ((resetcause & RMU_RSTCAUSE_BUMODERST) && !(resetcause & (RMU_RSTCAUSE_BUBODBUVIN | RMU_RSTCAUSE_PORST)))
	{
		/* Restore time from backup RTC + retention memory and print backup info*/
		rtc_clock_restore();

	    /* Reset timestamp */
	    BURTC_StatusClear();
	}
	else
	{
		/* Start LFXO and wait until it is stable */
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

		/* Setup BURTC */
		burtc_setup();

		rtc_clock_backup();
		rtc_clock_display();
	}

	/* Shut of LFRCO (enabled by SegmentLCD_Init in clockAppInit) and select LFXO as clock for LFA */
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_OscillatorEnable(cmuOsc_LFRCO, false , false);

	/* Enable BURTC interrupts */
	NVIC_ClearPendingIRQ( BURTC_IRQn );
	NVIC_EnableIRQ( BURTC_IRQn );
}

void rtc_clock_task( void )
{
	rtc_clock_setup();

	/* ---------- Eternal while loop ---------- */
	while (1)
	{
		/* Sleep while waiting for interrupt */
		EMU_EnterEM2( true );

		rtc_clock_display();
	}
}

#endif
