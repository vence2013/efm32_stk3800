#include "em_emu.h"
#include "em_cmu.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "cfg.h"


#if (FUNC_VERIFY == FUNC_LCD)

void lcd_toggle_all( void )
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
		delay1k( 3000 );

		SegmentLCD_AllOff();
		delay1k( 3000 );
	}
}


void lcd_bias_constrast( void )
{
	LCD_Init_TypeDef lcdInit = LCD_INIT_DEFAULT;
	u8 tCnt = 0;

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
	CMU_ClockEnable(cmuClock_LCD, true);

	lcdInit.mux  = lcdMuxOctaplex;
	LCD_Init(&lcdInit);

	/* Enable all display segments */
	LCD_SEGMENTS_ENABLE();

	led_setup();
	printf_setup();

	while(1)
	{
		u8 bias, contrast;

		bias     = tCnt / 32;
		contrast = tCnt % 32;
		tCnt = (tCnt + 1) % 128;

		LCD_ContrastSet( contrast );
		if (contrast == 0)
		{
			switch (bias)
			{
			case 0: LCD_BiasSet( lcdBiasStatic ); break;
			case 1: LCD_BiasSet( lcdBiasOneHalf ); break;
			case 2: LCD_BiasSet( lcdBiasOneThird ); break;
			case 3: LCD_BiasSet( lcdBiasOneFourth ); break;
			}
		}
		printf("Bias:%d, Contrast:%d.\r\n", bias, contrast);

		led_ctrl(LED0, LED0);
		SegmentLCD_AllOn();
		delay1k( 1000 );

		led_ctrl(LED0, 0);
		SegmentLCD_AllOff();
		delay1k( 1000 );
	}
}

void lcd_display( void )
{
	SegmentLCD_Init( false );
	LCD_BiasSet( lcdBiasOneFourth );
	LCD_ContrastSet( 20 );

	while(1)
	{
		u8 i;

		SegmentLCD_LowerNumber( 1234567 );
		delay1k( 3000 );
		SegmentLCD_Number( 1234 );
		delay1k( 3000 );

		for (i = 0; i < 8; i++)
		{
			SegmentLCD_ARing(i, 1);
			delay1k( 1000 );
		}

		for (i = 0; i < 5; i++)
		{
			SegmentLCD_Battery( i );
			delay1k( 1000 );
		}

		for (i = 0; i < 5; i++)
		{
			SegmentLCD_EnergyMode(i, 1);
			delay1k( 1000 );
		}

		for (i = 0; i < 17; i++)
		{
			SegmentLCD_Symbol(LCD_SYMBOL_GECKO + i, 1);
			delay1k( 1000 );
		}

		SegmentLCD_AllOff();
		delay1k( 3000 );
	}
}

#endif
