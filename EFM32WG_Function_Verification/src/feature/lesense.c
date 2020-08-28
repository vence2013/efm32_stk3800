#include "em_cmu.h"
#include "em_gpio.h"
#include "em_acmp.h"
#include "em_lesense.h"
#include "cfg.h"


#if (FUNC_VERIFY == FUNC_LESENSE)


/**************************************************************************//**
 * @brief LESENSE_IRQHandler
 * Interrupt Service Routine for LESENSE Interrupt Line
 *****************************************************************************/
void LESENSE_IRQHandler( void )
{
	u32 chs = LESENSE_IF_CH8 | LESENSE_IF_CH9 | LESENSE_IF_CH10 | LESENSE_IF_CH11;

	if (LESENSE_IntGet() & chs)
	{
		LESENSE_IntClear( chs );
	}
}

/* Warning : not work!
 * */
void lesense_touch( void )
{
	/* Select clock source for LFA clock. */
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);

	/* Enable CORELE clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);
	/* Enable clock for GPIO. */
	CMU_ClockEnable(cmuClock_GPIO, true);
	/* Enable clock for ACMP0. */
	CMU_ClockEnable(cmuClock_ACMP1, true);
	/* Enable clock for LESENSE. */
	CMU_ClockEnable(cmuClock_LESENSE, true); // 32.768 kHz / 8 ( max ) = 4096
	/* Enable clock divider for LESENSE. */
	CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_8);

	{
		/* GPIO */
		GPIO_PinModeSet(gpioPortC, 8, gpioModeDisabled, 0);
		GPIO_PinModeSet(gpioPortC, 9, gpioModeDisabled, 0);
		GPIO_PinModeSet(gpioPortC, 10, gpioModeDisabled, 0);
		GPIO_PinModeSet(gpioPortC, 11, gpioModeDisabled, 0);
	}

	{
		/* ACMP1 */
		ACMP_CapsenseInit_TypeDef acmpInit = ACMP_CAPSENSE_INIT_DEFAULT;

		acmpInit.fullBias = true;
		acmpInit.halfBias = true;
		acmpInit.biasProg = 0X05;
		acmpInit.resistor = acmpResistor0;
		acmpInit.vddLevel = 0x30;
		acmpInit.enable   = false;
		ACMP_CapsenseInit(ACMP1, &acmpInit);
	}

	{
		u8 i;

		/* LESENSE configuration structure */
		LESENSE_Init_TypeDef initLesense = LESENSE_INIT_DEFAULT;

		initLesense.coreCtrl.debugRun = false;
		initLesense.perCtrl.dacRef = lesenseDACRefBandGap;
		initLesense.perCtrl.acmp0Mode = lesenseACMPModeMux;
		initLesense.perCtrl.acmp1Mode = lesenseACMPModeMux;
		initLesense.perCtrl.warmupMode= lesenseWarmupModeNormal;
		initLesense.decCtrl.intMap = false;
		initLesense.decCtrl.prsChSel0 = false;
		initLesense.decCtrl.prsChSel1 = false;
		initLesense.decCtrl.prsChSel2 = false;
		initLesense.decCtrl.prsChSel3 = false;

		/* Initialize LESENSE interface _with_ RESET. */
		LESENSE_Init(&initLesense, true);

		/* Channel configuration */
		static const LESENSE_ChDesc_TypeDef initLesenseCh =
		{
			.enaScanCh     = true,
			.enaPin        = true,
			.enaInt        = true,
			.chPinExMode   = lesenseChPinExDis,
			.chPinIdleMode = lesenseChPinIdleDis,
			.useAltEx      = false,
			.shiftRes      = false,
			.invRes        = false,
			.storeCntRes   = true,
			.exClk         = lesenseClkLF,
			.sampleClk     = lesenseClkLF,
			.exTime        = 0x0,
			.sampleDelay   = 30,
			.measDelay     = 0x0,
			.acmpThres     = 0x0,                   // don't care, configured by ACMPInit
			.sampleMode    = lesenseSampleModeCounter,
			.intMode       = lesenseSetIntLevel,
			.cntThres      = 0x0,                   // Configured later by calibration function
			.compMode      = lesenseCompModeLess
		};

		/* Configure channels */
		for(i = 8; i <= 11; i++){
			LESENSE_ChannelConfig(&initLesenseCh, i);
		}

		/** Scan frequency for LESENSE, how often all the pads are scanned. */
		#define LESENSE_SCAN_FREQUENCY          5

		/* Set scan frequency */
		LESENSE_ScanFreqSet(0, LESENSE_SCAN_FREQUENCY);

		/* Set clock divisor for LF clock. */
		LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

		/* Enable interrupt in NVIC. */
		NVIC_EnableIRQ(LESENSE_IRQn);

		/* Start scan. */
		LESENSE_ScanStart();
	}
}

#endif
