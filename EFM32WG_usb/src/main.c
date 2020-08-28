#include "em_device.h"
#include "em_chip.h"
#include "bsp_trace.h"
#include "hid.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	hid_setup();

	/* Infinite loop */
	while (1) {
	}
}
