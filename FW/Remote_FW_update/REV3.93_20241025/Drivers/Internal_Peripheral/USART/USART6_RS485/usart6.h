#ifndef   __USART6_H__
#define   __USART6_H__
                 

#ifdef __USART6_C__
	#define USART6_EXT
#else
	#define USART6_EXT extern
#endif
                         
/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "usart.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
USART6_EXT HAL_StatusTypeDef USART6_Init( USARTx_HandleType *pUsart );
/* End Function */

/* Start Variable */                              
/* End Variable */


#endif // __USART6_H__
