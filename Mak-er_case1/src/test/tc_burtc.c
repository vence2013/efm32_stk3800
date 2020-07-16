#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>


#if (TESTCASE_ENABLE)

void tc_burtc_info( void )
{
	u32 reg;

	printf("----- BURTC information -----\r\n");

	{
		u8 mode[16], lpcom[16], presc[16], clksel[16];

		printf("Configuration\r\n");

		reg = BURTC->CTRL;
		memset(mode, 0, 16);
		memset(lpcom, 0, 16);
		memset(presc, 0, 16);
		memset(clksel, 0, 16);

		switch(reg&0X03)
		{
		case 0: sprintf((char *)mode, "Disable"); break;
		case 1: sprintf((char *)mode, "EM2EN"); break;
		case 2: sprintf((char *)mode, "EM3EN"); break;
		case 3: sprintf((char *)mode, "EM4EN"); break;
		}

		switch ((reg>>5)&0X07)
		{
		case 0: sprintf((char *)lpcom, "IGN0LSB"); break;
		case 1: sprintf((char *)lpcom, "IGN1LSB"); break;
		case 2: sprintf((char *)lpcom, "IGN2LSB"); break;
		case 3: sprintf((char *)lpcom, "IGN3LSB"); break;
		case 4: sprintf((char *)lpcom, "IGN4LSB"); break;
		case 5: sprintf((char *)lpcom, "IGN5LSB"); break;
		case 6: sprintf((char *)lpcom, "IGN6LSB"); break;
		case 7: sprintf((char *)lpcom, "IGN7LSB"); break;
		}

		switch ((reg>>8)&0X07)
		{
		case 0: sprintf((char *)presc, "DIV1"); break;
		case 1: sprintf((char *)presc, "DIV2"); break;
		case 2: sprintf((char *)presc, "DIV4"); break;
		case 3: sprintf((char *)presc, "DIV8"); break;
		case 4: sprintf((char *)presc, "DIV16"); break;
		case 5: sprintf((char *)presc, "DIV32"); break;
		case 6: sprintf((char *)presc, "DIV64"); break;
		case 7: sprintf((char *)presc, "DIV128"); break;
		}

		switch ((reg>>12)&0X03)
		{
		case 0: sprintf((char *)clksel, "NONE"); break;
		case 1: sprintf((char *)clksel, "LFRCO"); break;
		case 2: sprintf((char *)clksel, "LFXO"); break;
		case 3: sprintf((char *)clksel, "ULFRCO"); break;
		}

		printf("\tClock Source:%s, Prescaler:%s, Lowpower compare:%s, mode:%s.\r\n",
				clksel, presc, lpcom, mode);
		printf("\t%s%s.\r\n",
				(reg&0X10) ? "Wrap when CNT=COMP0, " : "",
				(reg&0X4000) ? "Timestamp Enable, " : "");
	}

	{
		u8 lpmode[16];

		memset(lpmode, 0, 16);
		switch(BURTC->LPMODE)
		{
		case 0: sprintf((char *)lpmode, "Disabled"); break;
		case 1: sprintf((char *)lpmode, "Always Enabled"); break;
		case 2: sprintf((char *)lpmode, "Enabled in backup mode"); break;
		}

		printf("\tLowpower mode:%s.\r\n", lpmode);
	}
	printf("Data\r\n\tFrequency:%ld, Compare:%ld, Counter:%ld.\r\n",
			BURTC_ClockFreqGet(), BURTC_CompareGet( 0 ), BURTC_CounterGet());

	{
		reg = BURTC_Status();

		printf("Status: %s%s%s.\r\n",
				(reg&0X01) ? "in lowpower mode" : "",
				(reg&0X02) ? "timestamp stored" : "",
				(reg&0X04) ? "RAM write error" : "");
	}
}

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_EMU)

static u8 burtc_int = 0;

#if 0
/* counter for compare
 * 1000: ULFRCO 1 kHz
 */
#define COUNTS_FOR_COMPARE                                    (1000u)

/***************************************************************************//**
 * @brief RTC Interrupt Handler
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_IntClear( BURTC_IF_COMP0 );
		BURTC_CompareSet( 0, BURTC_CompareGet(0) + COUNTS_FOR_COMPARE );

		burtc_int = 1;
		Drv_led_toggle( LED0 );
	}
}

/* interrupt 1s; printf second count */
void tc_burtc_seconds( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	/* use ULFRCO as source, only 2 frequency : 2kHz when /1, 1kHz other value */
	burtcInit.clkDiv       = burtcClkDiv_2;
	BURTC_CompareSet( 0, COUNTS_FOR_COMPARE ); /* initial compare value */

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );

	BURTC_Init( &burtcInit );

	usart_setup();

	while(1)
	{
		if (burtc_int)
		{
			burtc_int = 0;

			tc_burtc_info();
		}
	}
}

#endif

#if 1
/* about 4.8mA */

/* counter for compare
 * 256: LFRCO 32.768 kHz / 128
 */
#define COUNTS_FOR_COMPARE                                     (256u)

/***************************************************************************//**
 * @brief RTC Interrupt Handler
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_IntClear( BURTC_IF_COMP0 );

		burtc_int = 1;
		Drv_led_toggle( LED0 );
	}
}

void tc_burtc_lfrco_wrap( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* Starting LFRCO and waiting until it is stable */
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	/* use ULFRCO as source, only 2 frequency : 2kHz when /1, 1kHz other value */
	burtcInit.clkSel       = burtcClkSelLFRCO;
	burtcInit.clkDiv       = burtcClkDiv_128;
	burtcInit.compare0Top  = true;
	BURTC_CompareSet( 0, COUNTS_FOR_COMPARE ); /* initial compare value */

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );

	BURTC_Init( &burtcInit );

	usart_setup();

	while(1)
	{
		if (burtc_int)
		{
			burtc_int = 0;

			tc_burtc_info();
		}
	}
}


/* sames the low power mode of burtc increase the frequency of interrupt */
void tc_burtc_lowpower( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);

	/* Starting LFRCO and waiting until it is stable */
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	/* use ULFRCO as source, only 2 frequency : 2kHz when /1, 1kHz other value */
	burtcInit.clkSel       = burtcClkSelLFRCO;
	burtcInit.clkDiv       = burtcClkDiv_128;
	burtcInit.compare0Top  = true;
	burtcInit.lowPowerComp = 2;
	burtcInit.lowPowerMode = burtcLPEnable;
	BURTC_CompareSet( 0, COUNTS_FOR_COMPARE ); /* initial compare value */

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );

	BURTC_Init( &burtcInit );

	usart_setup();

	while(1)
	{
		if (burtc_int)
		{
			burtc_int = 0;

			tc_burtc_info();
		}
	}
}
#endif

#endif

#endif
