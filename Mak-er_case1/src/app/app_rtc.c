#include "em_cmu.h"
#include "em_emu.h"
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
	burtcInit.timeStamp = true;
	burtcInit.compare0Top = false;
	burtcInit.lowPowerMode = burtcLPDisable;
	/* Initialize BURTC with burtcInit struct */
	BURTC_Init( &burtcInit );

	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );
}

/***************************************************************************//**
 * @brief Set up backup domain.
 ******************************************************************************/
void bud_setup(void)
{
	/* Assign default TypeDefs */
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;

	/* Unlock configuration */
	EMU_EM4Lock( false );

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

	/* Initialize EM4 and Backup Power Domain with init structs */
	EMU_BUPDInit( &bupdInit );

	/*Setup EM4 configuration structure */
	em4Init.lockConfig = true;
	em4Init.osc = cmuOsc_LFXO;
	em4Init.buRtcWakeup = true;
	em4Init.vreg = true;

	EMU_EM4Init( &em4Init );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}

/******************************************************************************
 * @brief
 *****************************************************************************/
void App_rtc_setup( void )
{
	struct tm initialCalendar;
	uint32_t resetcause = 0, burtcCountAtWakeup = 0;

	resetcause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	burtcCountAtWakeup = BURTC_CounterGet();

	bud_setup();

	initialCalendar.tm_sec    =  0;   /* 0 seconds (0-60, 60 = leap second)*/
	initialCalendar.tm_min    =  0;   /* 0 minutes (0-59) */
	initialCalendar.tm_hour   = 12;   /* 12 hours (0-23) */
	initialCalendar.tm_mday   = 30;   /* 1st day of the month (1 - 31) */
	initialCalendar.tm_mon    =  5;   /* January (0 - 11, 0 = January) */
	initialCalendar.tm_year   = 20;   /* Year 2012 (year since 1900) */
	initialCalendar.tm_wday   =  1;   /* Sunday (0 - 6, 0 = Sunday) */
	initialCalendar.tm_yday   =  0;   /* 1st day of the year (0-365) */
	initialCalendar.tm_isdst  = -1;    /* Daylight saving time; enabled (>0), disabled (=0) or unknown (<0) */

	/* Set the calendar */
	clockInit(&initialCalendar);

	/* If waking from backup mode, restore time from retention registers */
	if ( (resetcause & RMU_RSTCAUSE_BUMODERST) && !(resetcause & RMU_RSTCAUSE_BUBODBUVIN) && !(resetcause & RMU_RSTCAUSE_PORST) )
	{
		uint32_t burtcCount = BURTC_CounterGet();
		uint32_t  burtcTimestamp = BURTC_TimestampGet();

		Drv_led(LED_ALL, LED0);

		clockSetStartTime( BURTC_RetRegGet( 1 ) );
		BURTC_CompareSet( 0, burtcCount );
	    /* Reset timestamp */
	    BURTC_StatusClear();
	}
	/* If normal startup, initialize BURTC */
	else
	{
		Drv_led(LED_ALL, 0);
		burtc_setup();

		BURTC_RetRegSet( 1, clockGetStartTime() );
	}
	App_rtc_display();

	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_OscillatorEnable(cmuOsc_LFRCO, false , false);
}

/******************************************************************************
 * @brief
 *****************************************************************************/
void App_rtc_display( void )
{
	struct tm calendar;
	time_t currentTime;
	int date;
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

    date = (calendar.tm_mon + 1)*100 + calendar.tm_mday;
    SegmentLCD_Number( date );
    SegmentLCD_Write( displayStringBuf );
    SegmentLCD_Symbol(LCD_SYMBOL_COL3, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_COL5, 1);
}
