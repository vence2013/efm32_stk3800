#include "em_chip.h"
#include "bsp_trace.h"
#include "cfg.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	/* Function Verification */
	gpio_em4_wakup();

	/* Infinite loop */
	while (1) {
	}
}
