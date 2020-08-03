#include "em_cmu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "cfg.h"


void burtc_info( void )
{
	u32 reg;

	{
		u8 mode[16], lpcom[16], presc[16], clksel[16], lpmode[16];

		memset(mode, 0, 16);
		memset(lpcom, 0, 16);
		memset(presc, 0, 16);
		memset(clksel, 0, 16);
		memset(lpmode, 0, 16);

		reg = BURTC->CTRL;
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

		printf("BURTC Clock Source:%s, Prescale:%s, Frequency:%ld, Compare:%ld.\r\n",
				clksel, presc, BURTC_ClockFreqGet(), BURTC_CompareGet( 0 ));
		printf("\tmode:%s, %s%s.\r\n", mode,
				(reg&0X10) ? "Wrap when CNT=COMP0, " : "",
				(reg&0X4000) ? "Timestamp Enable, " : "");

		switch(BURTC->LPMODE)
		{
		case 0: sprintf((char *)lpmode, "Disabled"); break;
		case 1: sprintf((char *)lpmode, "Always Enabled"); break;
		case 2: sprintf((char *)lpmode, "Enabled in backup mode"); break;
		}
		printf("\tlow power mode:%s, low power compare:%s.\r\n", lpmode, lpcom);
	}

	{
		reg = BURTC_Status();

		printf("BURTC Counter:%ld, Status:\r\n\t%s%s%s\r\n",
				BURTC_CounterGet(),
				(reg&0X01) ? "in lowpower mode" : "",
				(reg&0X02) ? "timestamp stored" : "",
				(reg&0X04) ? "RAM write error" : "");
	}
}

#if (FUNC_VERIFY == FUNC_BURTC)

#define COUNTER_WRAP                                             (0u)
#define ONE_SECOND_BY_LFRCO_DIV_128                            (256u)


/***************************************************************************//**
 * @brief RTC Interrupt Handler
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_IntClear( BURTC_IF_COMP0 );
#if (!COUNTER_WRAP)
		BURTC_CompareSet( 0, BURTC_CompareGet(0) + ONE_SECOND_BY_LFRCO_DIV_128 );
#endif
		led_toggle( LED0 );
	}
}

void burtc_interrupt_every_seconds( void )
{
	BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

	/* Enable Low energy clocking module clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);
	/* Starting LFRCO and waiting until it is stable */
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

	/* Enable access to BURTC registers */
	RMU_ResetControl(rmuResetBU, rmuResetModeClear);

	/* LFRCO 32.768 kHz
	 *   LFRCO 32.768 kHz / 128 = 256, this means 1 second
	 * */
	burtcInit.clkSel       = burtcClkSelLFRCO;
	burtcInit.clkDiv       = burtcClkDiv_128;
#if (COUNTER_WRAP)
	burtcInit.compare0Top  = true;
#endif
	BURTC_CompareSet( 0, ONE_SECOND_BY_LFRCO_DIV_128 ); /* initial compare value */

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );

	BURTC_Init( &burtcInit );

	printf_setup();
	burtc_info();

	while(1)
	{
		;
	}
}

#endif
