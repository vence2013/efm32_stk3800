#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "tc.h"
#include "drv.h"
#include <stdio.h>


#if (TESTCASE_ENABLE)

void tc_emu_info( void )
{
	u32 reg;

	printf("EMU Configuration\r\n");
	{
		reg = EMU->CTRL;

		printf("\tEM2/3\t%s%s\r\n",
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
		printf("\tEM4\t%s%s%s%s\r\n",
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
		printf("\tBakcup Power Domain\t%s%s%s%s\r\n",
				(reg&0X01) ? "Enable, " : "Disable, ",
				(reg&0X02) ? "Status Export, " : "",
				(reg&0X04) ? "Enable BOD calibration mode, " : "",
				(reg&0X08) ? "Enable brown out detection on BU_VIN when in backup mode, " : "");
	}
}

#if (TESTCASE_MODULE_SEL == TESTCASE_MODULE_EMU)

/***************************************************************************//**
 * @brief RTC Interrupt Handler
 ******************************************************************************/
void BURTC_IRQHandler(void)
{
	/*   Interrupt source: compare match */
	if ( BURTC_IntGet() & BURTC_IF_COMP0 )
	{
		BURTC_IntClear( BURTC_IF_COMP0 );

		Drv_led_toggle( LED0 );
	}
}

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

/* watch current of different low power mode */
void tc_emu_lowpower_mode( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

	em4Init.lockConfig = true;
	EMU_EM4Init(&em4Init);

	usart_setup();
	burtc_setup();
	tc_burtc_info();

	while(1)
	{
		printf("Enter EM0\r\n");
		delay_ms( 10000 );
		printf("Enter EM1\r\n");
		EMU_EnterEM1();
		printf("Enter EM2\r\n");
		EMU_EnterEM2( false );
		printf("Enter EM3\r\n");
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
		EMU_EnterEM3( false );
		printf("Enter EM4\r\n");
		EMU_EnterEM4();
	}
}

void tc_emu_em2block( void )
{
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

	em4Init.lockConfig = true;
	EMU_EM4Init(&em4Init);

	usart_setup();
	burtc_setup();
	printf("---- EM2 Block ----\r\n");
	EMU_EM2Block();

	while(1)
	{
		printf("Enter EM0\r\n");
		delay_ms( 10000 );
		printf("Enter EM1\r\n");
		EMU_EnterEM1();
		printf("Enter EM2\r\n");
		EMU_EnterEM2( false );
		printf("Enter EM3\r\n");
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);
		EMU_EnterEM3( false );
		printf("Enter EM4\r\n");
		EMU_EnterEM4();
	}
}

/* use HFXO as clock source */
void tc_emu_save_restore( void )
{
	CMU_HFXOInit_TypeDef initHFXO = CMU_HFXOINIT_DEFAULT;

	burtc_setup();

	while(1)
	{
		usart_setup();
		printf("init or wakeup without restore ----\r\n");
		tc_cmu_info();
		delay_ms( 10000 );

		CMU_HFXOInit(&initHFXO);
		CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
		CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
		usart_setup();
		printf("switch to HFXO ----\r\n");
		tc_cmu_info();

		EMU_EnterEM2( true );
		usart_setup();
		printf("wakeup with restore ----\r\n");
		tc_cmu_info();

		EMU_EnterEM2( false );
	}
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

/***************************************************************************//**
 * @brief EMU Interrupt Handler
 ******************************************************************************/
void EMU_IRQHandler(void)
{
	if (EMU_IntGet() & EMU_IF_BURDY)
	{
		EMU_IntClear( EMU_IF_BURDY );

		Drv_led_toggle( LED1 );
	}
}

/* do not run under debug */
void tc_emu_babckup_domain( void )
{
	usart_setup();
	tc_emu_info();

	budSetup();

	EMU_IntEnable( EMU_IF_BURDY );
	/* Enable Backup Power Domain interrupts */
	NVIC_ClearPendingIRQ( EMU_IRQn );
	NVIC_EnableIRQ( EMU_IRQn );

	delay_ms( 1000 );
	tc_emu_info();
}
#endif

#endif
