#include "em_chip.h"
#include "cfg.h"


int main(void)
{
  /* Chip errata */
  CHIP_Init();

  usb_hid_kbd_task();

  /* Infinite loop */
  while (1) {
  }
}
