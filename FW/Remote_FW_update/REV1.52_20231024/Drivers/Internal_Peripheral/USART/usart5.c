#define __USART5_C__
    #include "usart5.h"
#undef  __USART5_C__

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
static void GPIO_Init( void );
/* End Function */

/* Start Variable */
static UART_HandleTypeDef	UART5_Handle;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	
    if(ch == '\n')
    {
        ch = '\r';
        HAL_UART_Transmit( &UART5_Handle, (uint8_t *)&ch, 1, 0xFFFF );
        ch = '\n';
    }

    HAL_UART_Transmit( &UART5_Handle, (uint8_t *)&ch, 1, 0xFFFF );
	
    return ch;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART5_Init( void )
{
    GPIO_Init();
	
    UART5_Handle.Instance			= UART5;
    UART5_Handle.Init.BaudRate		= 921600;
    UART5_Handle.Init.WordLength	= UART_WORDLENGTH_8B;
    UART5_Handle.Init.StopBits		= UART_STOPBITS_1;
    UART5_Handle.Init.Parity		= UART_PARITY_NONE;
    UART5_Handle.Init.Mode			= UART_MODE_TX_RX;
    UART5_Handle.Init.HwFlowCtl		= UART_HWCONTROL_NONE;
    UART5_Handle.Init.OverSampling	= UART_OVERSAMPLING_16;
	
    if( HAL_UART_Init( &UART5_Handle ) != HAL_OK ){ Error_Handler(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStruct = { 0 };
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	__HAL_RCC_UART5_CLK_ENABLE();
	
	/**UART5 GPIO Configuration    
	PC12     ------> UART5_TX
	PD2     ------> UART5_RX 
	*/            
	GPIO_InitStruct.Pin		= GPIO_PIN_12;
	GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull	= GPIO_PULLUP;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Pin		= GPIO_PIN_2;
	GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
	HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );
}


