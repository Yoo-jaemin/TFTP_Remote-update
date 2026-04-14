#define __USART_C__
    #include "usart.h"
#undef  __USART_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "usart3.h"            
#include "modbus_rtu_master.h"
#include "modbus_rtu_slave.h" 
#include "sensing_process.h"  
#include "rs485.h"  
#include "option.h"
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

bool USART_Init( void )
{
	bool	err = false;
	
	if( USART3_Init( &USART3_Handle ) != HAL_OK ){ err = true; }
	
	return err;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_UART_MspInit( UART_HandleTypeDef* pUsart )
{
	GPIO_TypeDef		*GpioPort;
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
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
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
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
	
	if		( pUsart->Instance == USART1 ){ UsartPort = USART_PORT_DEBUG;			}
	else if	( pUsart->Instance == USART3 ){ UsartPort = USART_PORT_SENSOR_MODULE;	}
	else if	( pUsart->Instance == USART6 ){ UsartPort = USART_PORT_RS485;			}
	
	switch( UsartPort ){
		case USART_PORT_DEBUG: 
			break;
			
		case USART_PORT_SENSOR_MODULE:
        	MODBUS_RTU_Rx_Interrupt_Callback_Master( &SensingProcess.Modbus );
			break;
			
		case USART_PORT_RS485:
        	RS485_Rx_Interrupt_Callback();
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
			MODBUS_RTU_Tx_Interrupt_Callback_Master( &SensingProcess.Modbus );
			break;
			
		case USART_PORT_RS485:
			RS485_Tx_Interrupt_Callback();
			break;
	}
}



