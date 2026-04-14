#define __USART1_C__
    #include "usart1.h"
#undef  __USART1_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "usart.h"
#include "data_log.h"
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
UART_HandleTypeDef		USART1_Handle;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	if( ch == '\n' ){
		ch = '\r';
        HAL_UART_Transmit( &USART1_Handle, (uint8_t *)&ch, 1, 0xFFFF );
        ch = '\n';
	}
	
    HAL_UART_Transmit( &USART1_Handle, (uint8_t *)&ch, 1, 0xFFFF );
	
    return ch;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool USART1_Init( void )
{
	bool	err = false;
	
	USART1_Handle.Instance          = USART1;
	USART1_Handle.Init.BaudRate     = 921600;
	USART1_Handle.Init.WordLength   = UART_WORDLENGTH_8B;
	USART1_Handle.Init.StopBits     = UART_STOPBITS_1;
	USART1_Handle.Init.Parity       = UART_PARITY_NONE;
	USART1_Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	USART1_Handle.Init.Mode         = UART_MODE_TX_RX;
	USART1_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	
	if( HAL_UART_Init( &USART1_Handle ) != HAL_OK ){ err = true; }
	
	return err;
}
