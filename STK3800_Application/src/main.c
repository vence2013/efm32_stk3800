#include "em_chip.h"
#include "rtc_clock.h"


int main(void)
{
  /* Chip errata */
  CHIP_Init();

  rtc_clock_task();

  /* Infinite loop */
  while (1) {
  }
}
