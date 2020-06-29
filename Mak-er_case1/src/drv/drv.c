#include "em_emu.h"
#include "em_rmu.h"
#include "drv.h"
#include "segmentlcd.h"


/***************************************************************************//**
 * @brief Set up backup domain.
 ******************************************************************************/
void bud_setup(void)
{
	/* Assign default TypeDefs */
	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;

	/*Setup EM4 configuration structure */
	em4Init.lockConfig = true;
	em4Init.osc = emuEM4Osc_LFXO;
	em4Init.buRtcWakeup = true;
	em4Init.vreg = true;

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

	/* Unlock configuration */
	EMU_EM4Lock( false );

	/* Initialize EM4 and Backup Power Domain with init structs */
	EMU_BUPDInit( &bupdInit );
	EMU_EM4Init( &em4Init );

	/* Release reset for backup domain */
	RMU_ResetControl( rmuResetBU, false );

	/* Lock configuration */
	EMU_EM4Lock( true );
}


/******************************************************************************
 * @brief
 *****************************************************************************/
void Drv_setup( void )
{
	//bud_setup();

	SegmentLCD_Init( false );
	LCD_BiasSet( lcdBiasOneFourth );
	LCD_ContrastSet( 15 );
}
