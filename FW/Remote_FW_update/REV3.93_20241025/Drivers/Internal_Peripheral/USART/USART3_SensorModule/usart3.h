#ifndef   __USART3_H__
#define   __USART3_H__
                 

#ifdef __USART3_C__
	#define USART3_EXT
#else
	#define USART3_EXT extern
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
USART3_EXT HAL_StatusTypeDef USART3_Init( UART_HandleTypeDef *pUsart );
/* End Function */

/* Start Variable */                         
/* End Variable */


#endif // __USART3_H__
