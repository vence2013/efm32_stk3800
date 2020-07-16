#include "em_chip.h"
#include "bsp_trace.h"
#include "em_emu.h"
#include "hal.h"
#include "app.h"
#include "tc.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	tc_cmu_hfclk_switch_to_hfxo();

	Drv_setup();

	App_rtc_setup();

	/* Infinite loop */
	while (1) {
	    EMU_EnterEM2( true );

	    App_rtc_display();
	}
}
