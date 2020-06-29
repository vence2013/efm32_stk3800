#include "em_cmu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "clock.h"
#include "segmentlcd.h"
#include "app.h"
#include "drv.h"


/***************************************************************************//**
 * @brief RTC Interrupt Handler,
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_CompareSet( 0, BURTC_CompareGet(0) + COUNTS_PER_SEC );
		BURTC_IntClear( BURTC_IF_COMP0 );

		Drv_led_toggle( LED0 );
	}
}

/******************************************************************************
 * @brief   Configure backup RTC
 *****************************************************************************/
void burtc_setup(void)
{
	/* Create burtcInit struct and fill with default values */
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	CMU_ClockEnable(cmuClock_CORELE, true);
    /* Start LFXO and wait until it is stable */
    CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	BURTC_CompareSet( 0, COUNTS_PER_SEC );

	burtcInit.mode = burtcModeEM4;
	burtcInit.clkSel = burtcClkSelLFXO;
	burtcInit.clkDiv = burtcClkDiv_128;
	/* Initialize BURTC with burtcInit struct */
	BURTC_Init( &burtcInit );

	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );
}

/******************************************************************************
 * @brief
 *****************************************************************************/
void App_rtc_setup( void )
{
	struct tm initialCalendar;

	burtc_setup();

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

/******************************************************************************
 * @brief
 *****************************************************************************/
void App_rtc_display( void )
{
	struct tm calendar;
	time_t currentTime;
	char displayStringBuf[6];

    currentTime = time( NULL );
    calendar = * localtime( &currentTime );

    /* Make string from calendar */
    displayStringBuf[0] = 0x30 + (calendar.tm_hour / 10);
    displayStringBuf[1] = 0x30 + (calendar.tm_hour % 10);;
    displayStringBuf[2] = 0x30 + (calendar.tm_min / 10);
    displayStringBuf[3] = 0x30 + (calendar.tm_min % 10);
    displayStringBuf[4] = 0x30 + (calendar.tm_sec / 10);
    displayStringBuf[5] = 0x30 + (calendar.tm_sec % 10);

    SegmentLCD_Write(displayStringBuf);
    SegmentLCD_Symbol(LCD_SYMBOL_COL3, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_COL5, 1);
}
