#define __USART3_C__
    #include "usart3.h"
#undef  __USART3_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
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

HAL_StatusTypeDef USART3_Init( UART_HandleTypeDef *pUsart )
{
    pUsart->Instance			= USART3;
    pUsart->Init.BaudRate		= 115200;
    pUsart->Init.WordLength		= UART_WORDLENGTH_8B;
    pUsart->Init.StopBits		= UART_STOPBITS_1;
    pUsart->Init.Parity			= UART_PARITY_NONE;
    pUsart->Init.Mode			= UART_MODE_TX_RX;
    pUsart->Init.HwFlowCtl		= UART_HWCONTROL_NONE;
    pUsart->Init.OverSampling	= UART_OVERSAMPLING_16;
      
    return HAL_UART_Init( pUsart );
}
