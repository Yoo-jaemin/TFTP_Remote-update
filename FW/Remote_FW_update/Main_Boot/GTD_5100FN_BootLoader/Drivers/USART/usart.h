#ifndef   __USART_H__
#define   __USART_H__
                 

#ifdef __USART_C__
	#define USART_EXT
#else
	#define USART_EXT extern
#endif
                         

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	USART_PORT_DEBUG,
	USART_PORT_SENSOR_MODULE,
	USART_PORT_RS485
}USART_PortType;
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
USART_EXT bool USART_Init( void );
USART_EXT void USART_DeInit(void);
USART_EXT void USART3_Packet_Send( U8 *pData, U16 Len );
USART_EXT void USART3_Packet_Send_IT( U8 *pData, U16 Len );
/* End Function */

/* Start Variable */ 
USART_EXT UART_HandleTypeDef			USART1_Handle;
USART_EXT UART_HandleTypeDef			USART3_Handle;
/* End Variable */


#endif // __USART_H__
