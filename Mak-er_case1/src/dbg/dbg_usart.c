#include "dbg_usart.h"
#include "utils.h"


void dbg_usart_basic( void )
{
	u8 tCnt = 0;

	while (1)
	{
		usart_put_char( tCnt++ );

		delay_ms( 1000 );
	}
}
