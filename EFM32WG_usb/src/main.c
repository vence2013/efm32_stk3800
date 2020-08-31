#include "em_device.h"
#include "em_chip.h"
#include "bsp_trace.h"
#include "hid_keyboard.h"
#include "cdc_usb2serial.h"
#include "uvd_led.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	//hid_setup();
	//cdc_setup();
	uvd_setup();

	/* Infinite loop */
	while (1) {
	}
}
