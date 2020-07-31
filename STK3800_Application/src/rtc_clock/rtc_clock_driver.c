#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "segmentlcd.h"
#include "clock.h"
#include "rtc_clock_driver.h"


#if (APP_RTC_CLOCK_ENABLE)

void bud_setup( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;

	/* Unlock configuration */
	EMU_EM4Lock( false );

	/* Initialize EM4 and Backup Power Domain with init structs */
	em4Init.lockConfig = true;
	em4Init.osc = emuEM4Osc_LFXO;
	EMU_EM4Init( &em4Init );

	bupdInit.inactivePower = emuPower_MainBU;
	EMU_BUPDInit( &bupdInit );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}

void lcd_setup( void )
{
	/* Initialize LCD without voltage boost */
	SegmentLCD_Init(false);
	LCD_ContrastSet( 20 );

	/* Turn on EFM32 logo and Gecko*/
	SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);
	SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
}

void burtc_setup( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	burtcInit.mode   = burtcModeEM4;
	burtcInit.clkSel = burtcClkSelLFXO;
	burtcInit.clkDiv = burtcClkDiv_128;
	BURTC_Init( &burtcInit );

	BURTC_CompareSet( 0, COUNTS_BETWEEN_UPDATE );

	/* Enable BURTC interrupt on compare match and counter overflow */
	BURTC_IntEnable( BURTC_IF_COMP0 | BURTC_IF_OF );
}


#endif
