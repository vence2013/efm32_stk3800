#include "em_cmu.h"
#include "em_lesense.h"
#include "lesense.h"


#if (FUNC_VERIFY == FUNC_LESENSE)

void lesense_lightsense( void )
{
	/* Select clock source for LFA clock. */
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);

	/* Enable CORELE clock. */
	CMU_ClockEnable(cmuClock_CORELE, true);
	/* Enable clock for GPIO. */
	CMU_ClockEnable(cmuClock_GPIO, true);
	/* Enable clock for ACMP0. */
	CMU_ClockEnable(cmuClock_ACMP0, true);
	/* Enable clock for LESENSE. */
	CMU_ClockEnable(cmuClock_LESENSE, true); // 32.768 kHz / 8 ( max ) = 4096
	/* Enable clock divider for LESENSE. */
	CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_8);
}

#endif
