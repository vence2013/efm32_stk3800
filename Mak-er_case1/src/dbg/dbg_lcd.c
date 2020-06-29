#include "em_emu.h"
#include "em_cmu.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "dbg_lcd.h"
#include "utils.h"


void dbg_lcd_contrast( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;
	u8 level = 0;

	/* Basic clock setting */
	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux = lcdMuxOctaplex;
	LCD_Init(&lcdInit);

	/* Enable all segment */
	LCD_SegmentRangeEnable(lcdSegment12_15, true);
	LCD_SegmentRangeEnable(lcdSegment16_19, true);
	LCD_SegmentRangeEnable(lcdSegment28_31, true);
	LCD_SegmentRangeEnable(lcdSegment32_35, true);
	LCD_SegmentRangeEnable(lcdSegment36_39, true);

	/* Try different bias */
	LCD_BiasSet( lcdBiasOneFourth );

	while(1)
	{
		SegmentLCD_LowerNumber( level );
		LCD_ContrastSet( level );

		level = (level + 1) % 32;
		delay_ms( 3000 );
	}
}


/*
 * All Segment
 *   SegmentLCD_AllOff()/SegmentLCD_AllOn(), on/off
 * Main Section Display
 *   SegmentLCD_LowerHex(), hex number
 *   SegmentLCD_LowerNumber(), number, in range -9999999 to +9999999
 * Time & Numer Display ( right top section )
 *   SegmentLCD_Number(), in range -999 to +9999
 *   SegmentLCD_NumberOff(), off
 *   SegmentLCD_UnsignedHex(),
 * Symbles
 *   SegmentLCD_Symbol(),
 * 	 SegmentLCD_ARing(), range 0 - 7
 * 	 SegmentLCD_Battery(), 0 to 4 (0 turns all off)
 * 	 SegmentLCD_EnergyMode(), 0 to 4
 */
void dbg_lcd_basic( void )
{
	u8 tCnt;

	/* Initialize LCD controller without boost. */
	SegmentLCD_Init( false );

	SegmentLCD_AllOn();
	delay_ms( 1000 );
	SegmentLCD_AllOff();

#if 0
	for (tCnt = 0; tCnt <= 16; tCnt++)
	{
		SegmentLCD_Number(tCnt);
		SegmentLCD_Symbol((lcdSymbol)tCnt, 1);
		delay_ms( 1000 );
		SegmentLCD_Symbol((lcdSymbol)tCnt, 0);
	}
	SegmentLCD_NumberOff();


	for (tCnt = 0; tCnt <= 7; tCnt++)
	{
		SegmentLCD_Number(tCnt);
		SegmentLCD_ARing(tCnt, 1);
		delay_ms( 1000 );
		SegmentLCD_ARing(tCnt, 0);
	}
	SegmentLCD_NumberOff();


	for (tCnt = 0; tCnt <= 4; tCnt++)
	{
		SegmentLCD_Number(tCnt);
		SegmentLCD_Battery(tCnt);
		delay_ms( 1000 );
	}
	SegmentLCD_NumberOff();


	for (tCnt = 0; tCnt <= 4; tCnt++)
	{
		SegmentLCD_Number(tCnt);
		SegmentLCD_EnergyMode(tCnt, 1);
		delay_ms( 1000 );
		SegmentLCD_EnergyMode(tCnt, 0);
	}
	SegmentLCD_NumberOff();
#endif

	SegmentLCD_UnsignedHex(123);
	/* Write something to text field on LCD display. */
	SegmentLCD_Write( "1234567" );

	tCnt = 0;
	while(1)
	{
		SegmentLCD_UnsignedHex(tCnt++);

		delay_ms( 1000 );
	}
}
