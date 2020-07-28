#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "cfg.h"


void emu_info( void )
{
	u32 reg;

	{
		reg = EMU->CTRL;

		printf("EM2/3 Configuration:\r\n\t%s%s\r\n",
				(reg&0X01) ? "Full Voltage Regulator Control, " : "Reduced Voltage Regulator Control, ",
				(reg&0X02) ? "EM2 Block, " : "");
	}

	{
		u8 osc[16];

		memset(osc, 0, 16);
		reg = EMU->EM4CONF;
		switch ((reg>>2)&0X03)
		{
		case 0: sprintf((char *)osc, "ULFRCO"); break;
		case 1: sprintf((char *)osc, "LFRCO"); break;
		case 2: sprintf((char *)osc, "LFXO"); break;
		}
		printf("EM4 Configuration:\r\n\t%s%s%s%s\r\n",
				(reg&0X01) ? "Voltage Regulator Enable, " : "",
				(reg&0X02) ? "Backup RTC EM4 wakeup Enable, " : "", osc,
				(reg&0X10) ? "Disable Reset from backup bod in EM4, " : "");
	}

	{
		u8 probe[16];

		memset(probe, 0, 16);
		reg = EMU->BUCTRL;
		switch ((reg>>6)&0X03)
		{
		case 0: sprintf((char *)probe, "Disable"); break;
		case 1: sprintf((char *)probe, "VDDDREG"); break;
		case 2: sprintf((char *)probe, "BUIN"); break;
		case 3: sprintf((char *)probe, "BUOUT"); break;
		}
		printf("Bakcup Power Domain:\r\n\t%s%s%s%s\r\n",
				(reg&0X01) ? "Enable, " : "Disable, ",
				(reg&0X02) ? "Status Export, " : "",
				(reg&0X04) ? "Enable BOD calibration mode, " : "",
				(reg&0X08) ? "Enable brown out detection on BU_VIN when in backup mode, " : "");
	}
}

#if (FUNC_VERIFY == FUNC_EMU)

/***************************************************************************//**
 * @brief RTC Interrupt Handler
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_IntClear( BURTC_IF_COMP0 );

		led_toggle( LED0 );
	}
}

/* 10s interrupt
 *   ULFRCO as source, 1 kHz, counter wrap
 * */
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
	burtcInit.compare0Top  = true;
	BURTC_CompareSet( 0, 10000 ); /* 10s */

	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ(BURTC_IRQn);
	BURTC_IntEnable( BURTC_IF_COMP0 );

	BURTC_Init( &burtcInit );
}

/* Observe the current difference in different power modes */
void emu_lowpower_modes( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

	em4Init.lockConfig = true;
	EMU_EM4Init(&em4Init);

	burtc_setup();

	led_setup();
	printf_setup();
	printf("\r\n Low Power Test \r\n");
	delay1k( 3000 );

	while(1)
	{
		printf("EM1\r\n");
		delay1k( 10 );
		EMU_EnterEM1();

		printf("EM2\r\n");
		delay1k( 10 );
		EMU_EnterEM2( false );

		printf("EM3\r\n");
		delay1k( 10 );
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
		EMU_EnterEM3( false );

		printf("EM4\r\n");
		delay1k( 10 );
		EMU_EnterEM4();

		printf("EM0\r\n");
		delay1k( 10000 );
	}
}


void emu_em2block( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

	em4Init.lockConfig = true;
	EMU_EM4Init(&em4Init);

	/* Comment this out to see difference */
	EMU_EM2Block();

	printf_setup();
	rmu_info();
	emu_info();

	delay1k( 5000 );
	printf("\r\n EM4 Block Test \r\n");
	EMU_EnterEM4();
}

void emu_clock_restore( void )
{
	CMU_HFXOInit_TypeDef initHFXO = CMU_HFXOINIT_DEFAULT;

	CMU_HFXOInit(&initHFXO);
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	burtc_setup();

	printf_setup();
	cmu_info();
	delay1k( 10 );

	/* Switch between true and false */
	EMU_EnterEM2( false );

	printf_setup();
	printf("\r\nAfter wakeup from EM2\r\n");
	cmu_info();
}


/***************************************************************************//**
 * @brief EMU Interrupt Handler
 ******************************************************************************/
void EMU_IRQHandler(void)
{
	if (EMU_IntGet() & EMU_IF_BURDY)
	{
		EMU_IntClear( EMU_IF_BURDY );

		led_toggle( LED0 );
	}
}

void bud_setup(void)
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
	bupdInit.inactivePower = emuPower_MainBU;
	EMU_BUPDInit( &bupdInit );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}

/* Do not run under debug !
 * */
void emu_babckup_domain( void )
{
	u32 ret;

	led_setup();
	printf_setup();
	ret = rmu_info();

	bud_setup();
	burtc_setup();

	EMU_IntEnable( EMU_IF_BURDY );
	/* Enable Backup Power Domain interrupts */
	NVIC_ClearPendingIRQ( EMU_IRQn );
	NVIC_EnableIRQ( EMU_IRQn );

	emu_info();
	if (ret & 0X0008) /* External Pin Reset */
	{
		BURTC_RetRegSet(1, 0X12345678);
	}
	else
	{
		ret = BURTC_RetRegGet( 1 );

		BURTC_RetRegSet(1, ret+1);
		printf("BURTC Register:%08X.\r\n", ret);
	}
}

#endif
