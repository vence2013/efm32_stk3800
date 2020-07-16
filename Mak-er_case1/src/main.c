#include "em_chip.h"
#include "bsp_trace.h"
#include "tc.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	tc_burtc_lfrco_wrap();

	/* Infinite loop */
	while (1) {
	    ;
	}
}
