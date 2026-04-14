#define __USART_C__
    #include "usart.h"
#undef  __USART_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "Sensor_Upload.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static HAL_StatusTypeDef USART1_Init( UART_HandleTypeDef *pUsart );
static HAL_StatusTypeDef USART3_Init( UART_HandleTypeDef *pUsart );
/* End Function */

/* Start Variable */
//static U8 USART3_RxBuf;
/* End Variable */
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	
    if(ch == '\n')
    {
		//LogHandle.UsedLF = true;
        ch = '\r';
        HAL_UART_Transmit( &USART1_Handle, (uint8_t *)&ch, 1, 0xFFFF );
        ch = '\n';
    }

    HAL_UART_Transmit( &USART1_Handle, (uint8_t *)&ch, 1, 0xFFFF );
	
    return ch;
}

bool USART_Init( void )
{
	bool	err = false;
	
	if( USART1_Init( &USART1_Handle ) != HAL_OK ){ err = true; }
	if( USART3_Init( &USART3_Handle ) != HAL_OK ){ err = true; }
	
	if(HAL_UART_Receive_IT(&USART3_Handle, (uint8_t *)&Packet_Handle.ucBuff, 1) != HAL_OK)
    {
       	Error_Handler();
		err = true;
    }
	
	return err;
}

void USART_DeInit(void)
{
  	bool bErr_flag = FALSE;	
  
  	if(HAL_UART_DeInit(&USART3_Handle) != HAL_OK) bErr_flag = TRUE;
	HAL_UART_MspDeInit(&USART3_Handle);
	
	if(HAL_UART_DeInit(&USART1_Handle) != HAL_OK) bErr_flag = TRUE;
	HAL_UART_MspDeInit(&USART1_Handle);
	  
	if(bErr_flag == TRUE) Error_Handler();
}


static HAL_StatusTypeDef USART1_Init( UART_HandleTypeDef *pUsart )
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


static HAL_StatusTypeDef USART3_Init( UART_HandleTypeDef *pUsart )
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


void HAL_UART_MspInit( UART_HandleTypeDef* pUsart )
{
	GPIO_TypeDef		*GpioPort;
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_PortType		UsartPort;
	IRQn_Type			IRQn;
	uint32_t			Priority;
	
	if		( pUsart->Instance == USART1 ){ UsartPort = USART_PORT_DEBUG;			}
	else if	( pUsart->Instance == USART3 ){ UsartPort = USART_PORT_SENSOR_MODULE;	}
	else if	( pUsart->Instance == USART6 ){ UsartPort = USART_PORT_RS485;			}
	
	/* USART GPIO Configuration    
		PA9		------> USART1_TX
		PA10	------> USART1_RX    
		PB10	------> USART3_TX
		PB11	------> USART3_RX  
		PC6		------> USART6_TX
		PC7		------> USART6_RX   
	*/
	switch( UsartPort ){
		case USART_PORT_DEBUG:
    		__HAL_RCC_USART1_CLK_ENABLE();
			__HAL_RCC_GPIOA_CLK_ENABLE();
			GpioPort = GPIOA;
			GPIO_InitStructure.Pin = ( GPIO_PIN_9 | GPIO_PIN_10 );
			GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
			break;
			
		case USART_PORT_SENSOR_MODULE: 
    		__HAL_RCC_USART3_CLK_ENABLE();
			__HAL_RCC_GPIOB_CLK_ENABLE();
			GpioPort = GPIOB;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_10 | GPIO_PIN_11 );
			GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
			IRQn = USART3_IRQn;
			Priority = 3;
			break;
			
		case USART_PORT_RS485:
    		__HAL_RCC_USART6_CLK_ENABLE();
			__HAL_RCC_GPIOC_CLK_ENABLE();
			GpioPort = GPIOC;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_6 | GPIO_PIN_7 );
			GPIO_InitStructure.Alternate = GPIO_AF8_USART6;
			IRQn = USART6_IRQn;
			Priority = 6;
			break;
	}
	
	GPIO_InitStructure.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull		= GPIO_PULLUP;
	GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init( GpioPort, &GPIO_InitStructure );
	
	if( UsartPort != USART_PORT_DEBUG ){
    	HAL_NVIC_SetPriority( IRQn, Priority, 0 );
    	HAL_NVIC_EnableIRQ( IRQn );
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_UART_MspDeInit( UART_HandleTypeDef* pUsart )
{   
	GPIO_TypeDef		*GpioPort;
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_PortType		UsartPort;
	IRQn_Type			IRQn;
	
	
	if		( pUsart->Instance == USART1 ){ UsartPort = USART_PORT_DEBUG;			}
	else if	( pUsart->Instance == USART3 ){ UsartPort = USART_PORT_SENSOR_MODULE;	}
	else if	( pUsart->Instance == USART6 ){ UsartPort = USART_PORT_RS485;			}
	                         
	/* USART GPIO Configuration    
		PA9		------> USART1_TX
		PA10	------> USART1_RX    
		PB10	------> USART3_TX
		PB11	------> USART3_RX  
		PC6		------> USART6_TX
		PC7		------> USART6_RX   
	*/
	switch( UsartPort ){
		case USART_PORT_DEBUG:
    		__HAL_RCC_USART1_CLK_DISABLE();
			GpioPort = GPIOA;
			GPIO_InitStructure.Pin = ( GPIO_PIN_9 | GPIO_PIN_10 );
			break;
			
		case USART_PORT_SENSOR_MODULE: 
    		__HAL_RCC_USART3_CLK_DISABLE();
			GpioPort = GPIOB;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_10 | GPIO_PIN_11 );
			IRQn = USART3_IRQn;
			break;
			
		case USART_PORT_RS485:  
    		__HAL_RCC_USART6_CLK_DISABLE();
			GpioPort = GPIOC;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_6 | GPIO_PIN_7 );
			IRQn = USART6_IRQn;
			break;
	}
	
	HAL_GPIO_DeInit( GpioPort, GPIO_InitStructure.Pin );
	
	if( UsartPort != USART_PORT_DEBUG ){
		HAL_NVIC_DisableIRQ( IRQn );
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *pUsart )
{         
	USART_PortType		UsartPort;
	U8	bData;
	
	if		( pUsart->Instance == USART1 ){ UsartPort = USART_PORT_DEBUG;			}
	else if	( pUsart->Instance == USART3 ){ UsartPort = USART_PORT_SENSOR_MODULE;	}
	else if	( pUsart->Instance == USART6 ){ UsartPort = USART_PORT_RS485;			}
	
	switch( UsartPort ){
		case USART_PORT_DEBUG: 
			break;
			
		case USART_PORT_SENSOR_MODULE:
		  	
		  	bData = Packet_Handle.ucBuff;
			  
        	if(HAL_UART_Receive_IT(pUsart, (uint8_t *)&Packet_Handle.ucBuff, 1) != HAL_OK)
        	{
            	Error_Handler();
        	}
			
            //printf( "\r\n %02x", bData);
            
			if(bData == STX) {
		  		if(Packet_Handle.Flag.Bit.bSTX !=TRUE) {
					Packet_Handle.uTimeout = 0;
					Packet_Handle.uCount = 0;
					Packet_Handle.ucCRC_cnt = 0;
					Packet_Handle.uLen = 0;
			  		Packet_Handle.Flag.Bit.bSTX = TRUE;
					Packet_Handle.Flag.Bit.bETX = FALSE;
					Packet_Handle.Flag.Bit.bEOT = FALSE;
				}
			}
		
			if(Packet_Handle.Flag.Bit.bSTX != TRUE) break;
		
			if((Packet_Handle.Flag.Bit.bSTX == TRUE) && (Packet_Handle.Flag.Bit.bETX == TRUE)){
		  		if(Packet_Handle.ucCRC_cnt == CRC_SIZE) {
					if(bData == EOT) Packet_Handle.Flag.Bit.bEOT = TRUE;
				}
		  		Packet_Handle.ucCRC_cnt++;
			}
		

			if((bData == ETX) && (Packet_Handle.uCount == (Packet_Handle.uLen-1))) Packet_Handle.Flag.Bit.bETX = TRUE;
						
		
			Packet_Handle.ucRxBuf[Packet_Handle.ucHp & USART3_RX_FIFO_MASK][Packet_Handle.uCount] = bData;
			Packet_Handle.uCount++;
			
			if(Packet_Handle.uCount == 4){
				Packet_Handle.uLen = Packet_Handle.ucRxBuf[Packet_Handle.ucHp & USART3_RX_FIFO_MASK][2] << 8;
				Packet_Handle.uLen |= Packet_Handle.ucRxBuf[Packet_Handle.ucHp & USART3_RX_FIFO_MASK][3];
			}
		
			if(Packet_Handle.Flag.Bit.bEOT == TRUE){
			  	Packet_Handle.Flag.Bit.bSTX = FALSE;
				Packet_Handle.uReadSize[Packet_Handle.ucHp & USART3_RX_FIFO_MASK] = Packet_Handle.uCount;
				Packet_Handle.ucHp++;
			}
			
			if( Packet_Handle.uCount >= PACKET_BUFF_SIZE ){
				Packet_Handle.uCount = 0;
				Packet_Handle.Flag.Bit.bSTX = FALSE;
			}
			
			break;
			
		case USART_PORT_RS485:
        	//RS485_Rx_Interrupt_Callback();
			break;
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_UART_TxCpltCallback( UART_HandleTypeDef *pUsart )
{    
	USART_PortType		UsartPort;
	
	if		( pUsart->Instance == USART1 ){ UsartPort = USART_PORT_DEBUG;			}
	else if	( pUsart->Instance == USART3 ){ UsartPort = USART_PORT_SENSOR_MODULE;	}
	else if	( pUsart->Instance == USART6 ){ UsartPort = USART_PORT_RS485;			}
	               
	switch( UsartPort ){
		case USART_PORT_DEBUG: 
			break;
			
		case USART_PORT_SENSOR_MODULE:
			//MODBUS_RTU_Tx_Interrupt_Callback_Master( &SensingProcess.Modbus );
			break;
			
		case USART_PORT_RS485:
			//RS485_Tx_Interrupt_Callback();
			break;
	}
}

void USART3_Packet_Send( U8 *pData, U16 Len )
{
  	HAL_UART_Transmit( &USART3_Handle, (uint8_t *)pData, Len, 0xFFFF );
}

void USART3_Packet_Send_IT( U8 *pData, U16 Len )
{
  	if( HAL_UART_Transmit_IT( &USART3_Handle, (uint8_t *)pData, Len ) != HAL_OK ){
        Error_Handler();
    }
}
