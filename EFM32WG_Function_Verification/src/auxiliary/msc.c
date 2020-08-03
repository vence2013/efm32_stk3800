#include "em_cmu.h"
#include "em_msc.h"
#include "cfg.h"



void msc_info( void )
{
	u32 ret;

	{
		ret = MSC->TIMEBASE;

		printf("MSC Timebase: %08X\r\n\tperiod:%s, base:%d\r\n", ret,
				(ret&0X10000) ? "5us" : "1us", ret&0X3F);
		printf("AUXHFRCO frequency:%ld.\r\n", CMU_ClockFreqGet(cmuClock_AUX));
	}

	{
		u8 mode[16], busstrategy[32];

		memset(mode, 0, sizeof(mode));
		memset(busstrategy, 0, sizeof(busstrategy));

		ret = MSC->READCTRL;
		printf("MSC Read Configuration: %08X\r\n", ret);

		switch((ret>>0)&0X07)
		{
		case 0: sprintf((char *)mode, "WS0, "); break;
		case 1: sprintf((char *)mode, "WS1, "); break;
		case 2: sprintf((char *)mode, "WS0SCBTP, "); break;
		case 3: sprintf((char *)mode, "WS1SCBTP, "); break;
		case 4: sprintf((char *)mode, "WS2, "); break;
		case 5: sprintf((char *)mode, "WS2SCBTP, "); break;
		}
		switch((ret>>16)&0X03)
		{
		case 0: sprintf((char *)busstrategy, "CPU with low latency, "); break;
		case 1: sprintf((char *)busstrategy, "DMA with low latency, "); break;
		case 3: sprintf((char *)busstrategy, "DMAEM1 with low latency, "); break;
		}

		printf("\t%s%s%s%s%s%s%s\r\n", mode,
				(ret&0X08) ? "Internal Flash Cache Disable, " : "",
				(ret&0X10) ? "Automatic Invalidate Disable, " : "",
				(ret&0X20) ? "Interrupt Context Cache Disable, " : "",
				(ret&0X40) ? "External Bus Interface Cache Disable, " : "",
				(ret&0X80) ? "RAM Cache Enable, " : "", busstrategy);
	}

	{
		ret = MSC->WRITECTRL;
		printf("MSC Write Configuration: %08X\r\n", ret);

		printf("\t%s%s\r\n",
				(ret&0X01) ? "Enable, " : "Disable, ",
				(ret&0X02) ? "Abort Page Erase on Interrupt, " : "");
	}

	{
		ret = MSC->STATUS;
		printf("MSC Status: %08X\r\n", ret);

		printf("\t%s%s%s%s%s%s%s\r\n",
				(ret&MSC_STATUS_BUSY) ? "Erase/Write Busy, " : "",
				(ret&MSC_STATUS_LOCKED) ? "Access Locked, " : "",
				(ret&MSC_STATUS_INVADDR) ? "Invalid Write Address or Erase Page, " : "",
				(ret&MSC_STATUS_WDATAREADY) ? "WDATA Write Ready, " : "",
				(ret&MSC_STATUS_WORDTIMEOUT) ? "Flash Write Word Timeout, " : "",
				(ret&MSC_STATUS_ERASEABORTED) ? "The Current Flash Erase Operation Aborted, " : "",
				(ret&MSC_STATUS_PCRUNNING) ? "Performance Counters Running, " : "");
	}
}

#if (FUNC_VERIFY == FUNC_MSC)

void flash_display(u32 address, u32 length)
{
	u32 i;

	printf("Flash Data at 0X%08X, length:%d.", address, length);
	for (i=0; i<length; i++)
	{
		if (i%16 == 0)
		{
			printf("\r\n\t");
		}
		printf("%08X, ", *(u32 *)(address + i*4));
	}
	printf("\r\n");
}

/**************************************************************************//**
 * @brief MSC IRQ handler.
 *****************************************************************************/
void MSC_IRQHandler(void)
{
	if (MSC_IntGet() & MSC_IF_ERASE)
	{
		MSC_IntClear( MSC_IF_ERASE );
		led_ctrl(LED0, 0);
	}
}

#define ADDR_TEST                                        (0X00010000)

void msc_read_write( void )
{
	uint32_t userData[2];
	uint32_t count = 0;

	MSC_Init();
	/* Enabling Interrupt from BURTC */
	NVIC_EnableIRQ( MSC_IRQn );
	MSC_IntEnable( MSC_IF_ERASE );

	led_setup();
	printf_setup();
	msc_info();

	while(1)
	{
		printf("\r\n\r\n\r\nCounter:%ld, Flash at 0X%08X.\r\n", count, ADDR_TEST);

		led_ctrl(LED0, LED0);
		/* Before write, must erase! */
		MSC_ErasePage( (uint32_t *)ADDR_TEST );
		flash_display(ADDR_TEST, 16);

		while (MSC->STATUS&MSC_STATUS_BUSY) ;
		userData[0] = 0X2000 + count;
		userData[1] = 0X10000000 + count;
		MSC_WriteWord((uint32_t *)ADDR_TEST, userData, sizeof(userData));
		MSC_WriteWord((uint32_t *)(ADDR_TEST+16), (uint32_t *)&count, sizeof(count));

		flash_display(ADDR_TEST, 16);

		count++;
		delay1k( 5000 );
	}
}

#endif
