#define __USART1_C__
    #include "usart1.h"
#undef  __USART1_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */    
#include "usart.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef USART1_Init( UART_HandleTypeDef *pUsart )
{
	pUsart->Instance          = USART1;
	pUsart->Init.BaudRate     = 921600;
	pUsart->Init.WordLength   = UART_WORDLENGTH_8B;
	pUsart->Init.StopBits     = UART_STOPBITS_1;
	pUsart->Init.Parity       = UART_PARITY_NONE;
	pUsart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	pUsart->Init.Mode         = UART_MODE_TX_RX;
	pUsart->Init.OverSampling = UART_OVERSAMPLING_16;
	  
	return HAL_UART_Init( pUsart );
}
