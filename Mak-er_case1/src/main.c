#include "em_chip.h"
#include "bsp_trace.h"
#include "hal.h"
#include "dbg.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	Drv_setup();

	App_rtc_setup();

	/* Infinite loop */
	while (1) {
	    EMU_EnterEM2( false );

	    App_rtc_display();
	}
}
