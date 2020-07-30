#include "rtc_clock.h"
#include "rtc_clock_driver.h"
#include "cfg.h"


#if (APP_RTC_CLOCK_ENABLE)

void app_rtc_clock_setup( void )
{
#if (APP_RTC_CLOCK_DEBUG)
	printf_setup();
#endif
}

void app_rtc_clock_task( void )
{
	u8 tCnt = 0;

	app_rtc_clock_setup();

	while (1)
	{
		printf("Counter:%d.\r\n", tCnt);

		tCnt++;
		delay( 1000000 );
	}
}

#endif
