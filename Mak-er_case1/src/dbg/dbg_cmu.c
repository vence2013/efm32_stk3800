#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "dbg.h"
#include "drv.h"
#include "utils.h"
#include "pindef.h"
#include <stdio.h>
#include <string.h>


u8 readyEvent = 0;

/**************************************************************************//**
 * @brief CMU IRQ handler.
 *****************************************************************************/
void CMU_IRQHandler(void)
{
	u32 flags = CMU_IntGet();

	if (CMU_IFC_HFXORDY & flags)
	{
		/* Clear interrupt flag */
		CMU_IntClear(CMU_IFC_HFXORDY);
		readyEvent = 1;
	}
	if (CMU_IFC_LFXORDY & flags)
	{
		/* Clear interrupt flag */
		CMU_IntClear(CMU_IFC_LFXORDY);
		readyEvent = 1;
	}
}


void cmu_status( void )
{
	u32 status;
	u8 buf[64];

	status = CMU->STATUS;

	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "CMU status\r\n");
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "HFRCO>\t\tEns:%d, Rdy:%d, Sel:%d;\r\n", (status&0X01 ? 1 : 0), (status&0X02 ? 1 : 0), (status&0X400 ? 1 : 0));
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "HFXO>\t\tEns:%d, Rdy:%d, Sel:%d;\r\n", (status&0X04 ? 1 : 0), (status&0X08 ? 1 : 0), (status&0X800 ? 1 : 0));
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "AUXHFRCO>\tEns:%d, Rdy:%d;\r\n", (status&0X10 ? 1 : 0), (status&0X20 ? 1 : 0));
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "LFRCO>\t\tEns:%d, Rdy:%d, Sel:%d;\r\n", (status&0X040 ? 1 : 0), (status&0X080 ? 1 : 0), (status&0X1000 ? 1 : 0));
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	sprintf((char *)buf, "LFXO>\t\tEns:%d, Rdy:%d, Sel:%d;\r\n", (status&0X100 ? 1 : 0), (status&0X200 ? 1 : 0), (status&0X2000 ? 1 : 0));
	usart_put_data(buf, strlen((char *)buf));
}

void cmu_hfrc_info( void )
{
	CMU_HFRCOBand_TypeDef band;
	u32 delay;
	u8 buf[64];

	band = CMU_HFRCOBandGet();
	memset(buf, 0, sizeof(buf));
	switch (band)
	{
	case cmuHFRCOBand_1MHz:
		sprintf((char *)buf, "\t\nHFRCO band: 1MHz.");
		break;
	case cmuHFRCOBand_7MHz:
		sprintf((char *)buf, "\t\nHFRCO band: 7MHz.");
		break;
	case cmuHFRCOBand_11MHz:
		sprintf((char *)buf, "\t\nHFRCO band: 11MHz.");
		break;
	case cmuHFRCOBand_14MHz:
		sprintf((char *)buf, "\t\nHFRCO band: 14MHz.");
		break;
	case cmuHFRCOBand_21MHz:
		sprintf((char *)buf, "\t\nHFRCO band: 21MHz.");
		break;
	case cmuHFRCOBand_28MHz:
		sprintf((char *)buf, "\t\nHFRCO band: 28MHz.");
		break;
	}
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	delay = CMU_HFRCOStartupDelayGet();
	sprintf((char *)buf, "\t\tHFRCO start-up delay: %d.\r\n", delay);
	usart_put_data(buf, strlen((char *)buf));
}

void cmu_hf_info( void )
{
	CMU_Select_TypeDef sel;
	u32 freq;
	u8 buf[64];

	sel = CMU_ClockSelectGet( cmuClock_HF );
	switch (sel)
	{
	case cmuSelect_Disabled:
		sprintf((char *)buf, "\t\nClock selector disabled.");
		break;
	case cmuSelect_LFXO:
		sprintf((char *)buf, "\t\nLow-frequency crystal oscillator.");
		break;
	case cmuSelect_LFRCO:
		sprintf((char *)buf, "\t\nLow-frequency RC oscillator.");
		break;
	case cmuSelect_HFXO:
		sprintf((char *)buf, "\t\nHigh-frequency crystal oscillator.");
		break;
	case cmuSelect_HFRCO:
		sprintf((char *)buf, "\t\nHigh-frequency RC oscillator.");
		break;
	default:
		return ;
	}
	usart_put_data(buf, strlen((char *)buf));

	memset(buf, 0, sizeof(buf));
	freq = CMU_ClockFreqGet( cmuClock_HF );
	sprintf((char *)buf, "\t\tHF frequency:%d.\r\n", freq);
	usart_put_data(buf, strlen((char *)buf));
}

void cmu_hfxo_start( void )
{
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

	readyEvent = 0;

	CMU_HFXOInit(&hfxoInit);

	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

	CMU_IntEnable( CMU_IFC_HFXORDY );
	NVIC_ClearPendingIRQ(CMU_IRQn);
	NVIC_EnableIRQ(CMU_IRQn);

	while(!readyEvent)
	{
		delay_ms( 1 );
	}
	cmu_status();
}

void cmu_lfxo_start( void )
{
	CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

	readyEvent = 0;

	CMU_LFXOInit(&lfxoInit);

	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	CMU_IntEnable( CMU_IFC_LFXORDY );
	NVIC_ClearPendingIRQ(CMU_IRQn);
	NVIC_EnableIRQ(CMU_IRQn);

	while(!readyEvent)
	{
		delay_ms( 1 );
	}
	cmu_status();
}


void cmu_hfrco_update( void )
{
	delay_ms( 100 );
	CMU_HFRCOBandSet( cmuHFRCOBand_28MHz );

	usart_setup();
	cmu_hfrc_info();
	cmu_hf_info();
}

void cmu_switch_hfxo( void )
{
	delay_ms( 100 );
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	usart_setup();
	cmu_hf_info();
}

/**********************************************************
 * @brief Interrupt handler for right side push button.
 **********************************************************/
void GPIO_EVEN_IRQHandler(void)
{
	GPIO->IFC = GPIO->IF;

	/* Toggle LED ON/OFF */
	Drv_led_toggle( LED0 );
}

/* running not in debug */
void cmu_clock_at_wakeup( void )
{
	/* Enable GPIO clock */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Configure push button 0 as input and enable interrupt  */
	GPIO_PinModeSet(PB_Port, PB1_Pin, gpioModeInputPullFilter, 1);
	GPIO_ExtIntConfig(PB_Port, PB1_Pin, PB1_Pin, false, true, true);
	/* Enable interrupt in core for even and odd GPIO interrupts */
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	cmu_hf_info();
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	delay_ms( 1000 );
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	delay_ms( 1000 );
	usart_setup();
	cmu_hf_info();
	delay_ms( 1000 );

	EMU_EnterEM2( true );

	delay_ms( 1000 );
	usart_setup();
	cmu_hf_info();
}

void dbg_cmu( void )
{
	cmu_status();

	cmu_clock_at_wakeup();

	while(1)
	{
		;
	}
}
