#include "em_emu.h"
#include "em_cmu.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "utils.h"
#include "tc.h"


#if (TESTCASE_ENABLE)

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_LCD)

/* No significant difference in current */
void tc_lcd_frame_rate( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;

	usart_setup();

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux  = lcdMuxOctaplex;
	LCD_Init(&lcdInit);

	/* Enable all display segments */
	LCD_SEGMENTS_ENABLE();

	SegmentLCD_AllOn();

	while(1)
	{
		printf("FDIV for LCD:0\r\n");
		CMU_ClockEnable(cmuClock_LCD, false);
		CMU_LCDClkFDIVSet( 0 );
		CMU_ClockEnable(cmuClock_LCD, true);
		delay_ms( 5000 );

		printf("FDIV for LCD:7\r\n");
		CMU_ClockEnable(cmuClock_LCD, false);
		CMU_LCDClkFDIVSet( 7 );
		CMU_ClockEnable(cmuClock_LCD, true);
		delay_ms( 5000 );
	}
}

void tc_lcd_config_constrast( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;
	u8 tCnt = 0, bias = 0;

	usart_setup();

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux  = lcdMuxOctaplex;
	lcdInit.bias = lcdBiasOneThird;
	LCD_Init(&lcdInit);

	/* Enable all display segments */
	LCD_SEGMENTS_ENABLE();

	while(1)
	{
		LCD_ContrastSet( tCnt );
		if (tCnt == 0)
		{
			bias = (bias + 1) % 4;

			switch (bias)
			{
			case 0: LCD_BiasSet( lcdBiasStatic ); break;
			case 1: LCD_BiasSet( lcdBiasOneHalf ); break;
			case 2: LCD_BiasSet( lcdBiasOneThird ); break;
			case 3: LCD_BiasSet( lcdBiasOneFourth ); break;
			}
		}
		printf("Bias:%d, Contrast:%d.\r\n", bias, tCnt);

		tCnt += 4;
		if (tCnt >= 32)
		{
			tCnt = 0;
		}

		SegmentLCD_AllOn();
		delay_ms( 3000 );

		SegmentLCD_AllOff();
		delay_ms( 3000 );
	}
}

void tc_lcd_config_bias( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;
	u8 tCnt = 0;

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux  = lcdMuxOctaplex;
	lcdInit.bias = lcdBiasOneFourth;
	LCD_Init(&lcdInit);

	/* Enable all display segments */
	LCD_SEGMENTS_ENABLE();

	while(1)
	{
		switch (tCnt)
		{
		case 0: LCD_BiasSet( lcdBiasStatic ); break;
		case 1: LCD_BiasSet( lcdBiasOneHalf ); break;
		case 2: LCD_BiasSet( lcdBiasOneThird ); break;
		case 3: LCD_BiasSet( lcdBiasOneFourth ); break;
		}

		SegmentLCD_AllOn();
		delay_ms( 5000 );

		SegmentLCD_AllOff();
		delay_ms( 5000 );

		tCnt = (tCnt + 1) % 5;
	}
}

void tc_lcd_mux( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;
	u8 tCnt = 0;

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	while(1)
	{
		switch (tCnt)
		{
		case 0:
			lcdInit.mux  = lcdMuxStatic;
			lcdInit.bias = lcdBiasStatic;
			break;
		case 1:
			lcdInit.mux  = lcdMuxDuplex;
			lcdInit.bias = lcdBiasOneHalf;
			break;
		case 2:
			lcdInit.mux  = lcdMuxTriplex;
			lcdInit.bias = lcdBiasOneThird;
			break;
		case 3:
			lcdInit.mux  = lcdMuxSextaplex;
			lcdInit.bias = lcdBiasOneFourth;
			break;
		case 4:
			lcdInit.mux  = lcdMuxOctaplex;
			lcdInit.bias = lcdBiasOneFourth;
			break;
		}

		LCD_Init(&lcdInit);
		/* Enable all display segments */
		LCD_SEGMENTS_ENABLE();
		SegmentLCD_AllOn();

		tCnt = (tCnt + 1) % 5;
		delay_ms( 5000 );
	}
}

void tc_lcd_segment_enable( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;
	u8 tCnt = 0;

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux  = lcdMuxOctaplex;
	lcdInit.bias = lcdBiasOneFourth;
	LCD_Init(&lcdInit);

	SegmentLCD_AllOn();
	while(1)
	{
		switch (tCnt)
		{
		case 0: LCD_SegmentRangeEnable(lcdSegment12_15, true); break;
		case 1:	LCD_SegmentRangeEnable(lcdSegment16_19, true); break;
		case 2: LCD_SegmentRangeEnable(lcdSegment28_31, true); break;
		case 3: LCD_SegmentRangeEnable(lcdSegment32_35, true); break;
		case 4: LCD_SegmentRangeEnable(lcdSegment36_39, true); break;
		case 5:
			LCD_SegmentRangeEnable(lcdSegment12_15, false);
			LCD_SegmentRangeEnable(lcdSegment16_19, false);
			LCD_SegmentRangeEnable(lcdSegment28_31, false);
			LCD_SegmentRangeEnable(lcdSegment32_35, false);
			LCD_SegmentRangeEnable(lcdSegment36_39, false);
			break;
		}
		tCnt = (tCnt + 1) % 6;
		delay_ms( 5000 );
	}
}

/* all on & all off toggle
 *
 * Notice: current almost has no different!
 */
void tc_lcd_display( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux  = lcdMuxOctaplex;
	lcdInit.bias = lcdBiasOneFourth;
	LCD_Init(&lcdInit);

	/* Enable all display segments */
	LCD_SEGMENTS_ENABLE();

	while(1)
	{
		SegmentLCD_AllOn();
		delay_ms( 3000 );

		SegmentLCD_AllOff();
		delay_ms( 3000 );
	}
}

#endif

#endif
