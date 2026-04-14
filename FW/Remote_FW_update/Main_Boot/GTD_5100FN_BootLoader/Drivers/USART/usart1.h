#ifndef   __USART1_H__
#define   __USART1_H__


#ifdef __USART1_C__
	#define USART1_EXT
#else
	#define USART1_EXT extern
#endif
                         
/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
USART1_EXT HAL_StatusTypeDef USART1_Init( UART_HandleTypeDef *pUsart );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __USART1_H__
