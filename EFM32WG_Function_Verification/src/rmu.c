#include "em_rmu.h"
#include "cfg.h"


u32 rmu_info( void )
{
	u32 resetCause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	printf("Reset Cause: 0X%08X\r\n", resetCause);
	printf("\t%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\r\n",
			(resetCause&0X0001) ? "Power On Reset, " : "",
			(resetCause&0X0002) ? "Brown Out Detector Unregulated Domain Reset, " : "",
			(resetCause&0X0004) ? "Brown Out Detector Regulated Domain Reset, " : "",
			(resetCause&0X0008) ? "External Pin Reset, " : "",
			(resetCause&0X0010) ? "Watchdog Reset, " : "",
			(resetCause&0X0020) ? "LOCKUP Reset, " : "",
			(resetCause&0X0040) ? "System Request Reset, " : "",
			(resetCause&0X0080) ? "EM4 Reset, " : "",
			(resetCause&0X0100) ? "EM4 Wake-up Reset, " : "",
			(resetCause&0X0200) ? "AVDD0 Bod Rese, " : "",
			(resetCause&0X0400) ? "AVDD1 Bod Reset, " : "",
			(resetCause&0X0800) ? "Backup Brown Out Detector VDD_DREG, " : "",
			(resetCause&0X1000) ? "Backup Brown Out Detector BU_VIN, " : "",
			(resetCause&0X2000) ? "Backup Brown Out Detector Unregulated Domain, " : "",
			(resetCause&0X4000) ? "Backup Brown Out Detector Regulated Domain, " : "",
			(resetCause&0X8000) ? "Backup mode reset, " : "");

	return resetCause;
}

#if (FUNC_VERIFY == FUNC_RMU)

void rmu_reset_by_por( void )
{
	printf_setup();
	rmu_info();

	while (1)
	{
		;
	}
}

/* [如何进行软复位操作](https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2016/07/17/_-D6yU) */
void rmu_reset_by_system( void )
{
	printf_setup();
	rmu_info();

	led_setup();
	led_ctrl(LED_ALL, LED_ALL);
	delay1k( 100 );
	led_ctrl(LED_ALL, 0);

	delay1k( 5000 );
	NVIC_SystemReset();
}

#endif
