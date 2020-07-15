#include "tc.h"
#include "drv.h"
#include "utils.h"


#if (TESTCASE_ENABLE)
#if (TESTCASE_MODULE_BLINKY == TESTCASE_MODULE_SEL)

void tc_blinky( void )
{
	Drv_led_setup();

	while(1)
	{
		Drv_led_toggle( LED0 );
		delay_ms( 1000 );
	}
}

#endif
#endif
