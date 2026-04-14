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
USART3_EXT void USART3_Init( void );
USART3_EXT void USART3_Rx_Packet_Check( void );
USART3_EXT void USART3_Rx_Handler( void );
USART3_EXT void USART3_Packet_Send( U8 *pData, U16 Len );
/* End Function */

/* Start Variable */
USART3_EXT UART_HandleTypeDef	USART3_Handle;
/* End Variable */


#endif // __USART3_H__
