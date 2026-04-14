#define __USART6_C__
    #include "usart6.h"
#undef  __USART6_C__

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

HAL_StatusTypeDef USART6_Init( USARTx_HandleType *pUsart )
{
    pUsart->Handle.Instance				= USART6;
    pUsart->Handle.Init.BaudRate		= pUsart->Baud;
    pUsart->Handle.Init.WordLength		= pUsart->Data;
    pUsart->Handle.Init.StopBits		= pUsart->Stop;
    pUsart->Handle.Init.Parity			= pUsart->Parity;
    pUsart->Handle.Init.Mode			= UART_MODE_TX_RX;
    pUsart->Handle.Init.HwFlowCtl		= UART_HWCONTROL_NONE;
    pUsart->Handle.Init.OverSampling	= UART_OVERSAMPLING_16;
      
    return HAL_UART_Init( &pUsart->Handle );
}
 