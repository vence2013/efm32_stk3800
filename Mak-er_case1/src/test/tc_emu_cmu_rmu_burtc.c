#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "segmentlcd.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>


#if (TESTCASE_ENABLE)

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_CMU_EMU_RMU_BURTC)

void burtc_setup( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	/* use ULFRCO as source, only 2 frequency : 2kHz when /1, 1kHz other value */
	burtcInit.mode         = burtcModeEM4;
	burtcInit.clkDiv       = burtcClkDiv_2;
	BURTC_CompareSet( 0, 10000 ); /* 10s */

	BURTC_Init( &burtcInit );
}

void budSetup(void)
{
	/* Assign default TypeDefs */
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;

	/* Unlock configuration */
	EMU_EM4Lock( false );

	/*Setup EM4 configuration structure */
	em4Init.lockConfig = true;
	EMU_EM4Init( &em4Init );

	/* Initialize EM4 and Backup Power Domain with init structs */
	EMU_BUPDInit( &bupdInit );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}

void lcd_setup( void )
{
	SegmentLCD_Init( false );
	LCD_BiasSet( lcdBiasOneFourth );
	LCD_ContrastSet( 15 );
}

/* USART may supply power, stop enter backup power domain */
void tc_ecrb_retention_register( void )
{
	u32 tmp;

	lcd_setup();
	budSetup();
	burtc_setup();

	tmp = BURTC_RetRegGet( 10 );
	SegmentLCD_LowerHex( tmp );
	tmp = (tmp > 0X8000) ? 0 : (tmp +1);
	BURTC_RetRegSet(10, tmp);

	EMU_EnterEM2( false );
}


/* 备用电源域的测试应使用USB电源或电池电源，
 *     不能使用调试电源，同时USART接口可能会为设备供电。 这些操作可能会出现
 *     1. 无法进行备用模式；或者
 *     2. 复位原因的标志异常
 * 从而导致内存保持或TIMESTAMP异常。
 */
void tc_ecrb_timestamp( void )
{
	u32 resetcause = RMU->RSTCAUSE;
	RMU_ResetCauseClear();

	lcd_setup();
	budSetup();

	if ( (resetcause & RMU_RSTCAUSE_BUMODERST) && !(resetcause & RMU_RSTCAUSE_BUBODBUVIN) && !(resetcause & RMU_RSTCAUSE_PORST) )
	{
		SegmentLCD_UnsignedHex( BURTC_TimestampGet() );
		BURTC_StatusClear();
	}
	else
	{
		burtc_setup();
	}

	while(1)
	{
		SegmentLCD_LowerHex( BURTC_CounterGet() );
		delay_ms( 1000 );
	}
}

/* burtc: timestamp, ram retain
 * burtc running in backup power domain
 * */
#endif

#endif
