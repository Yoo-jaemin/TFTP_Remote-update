#define __USART3_C__
    #include "usart3.h"
#undef  __USART3_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "modbus.h"
#include "option.h"
#include "eeprom.h"
/* End Include */

/* Start Define */
#define USART3_RX_FIFO_SIZE			( 4 )
#define USART3_RX_FIFO_MASK			( USART3_RX_FIFO_SIZE - 1 )
#define USART3_RX_BUFF_SIZE			( 255 )
//#define RX_PACKET_PRINT
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
/* End Function */

/* Start Variable */
static U8   USART3_RxBuf;
static U8   USART3_RxFifo[USART3_RX_FIFO_SIZE][USART3_RX_BUFF_SIZE];

#ifdef RX_PACKET_PRINT
static U8   USART3_RxFifoLen[USART3_RX_FIFO_SIZE];
#endif

static U8   USART3_RxCnt;
static U8   USART3_RxFifoHp;
static U8   USART3_RxFifoTp;
static U8   USART3_RxPacketCheckTick;
/* End Variable */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* USART3 init function */
void USART3_Init( void )
{
    GPIO_Init();
	
    USART3_Handle.Instance			= USART3;
    USART3_Handle.Init.BaudRate		= 115200;
    USART3_Handle.Init.WordLength	= UART_WORDLENGTH_8B;
    USART3_Handle.Init.StopBits		= UART_STOPBITS_1;
    USART3_Handle.Init.Parity		= UART_PARITY_NONE;
    USART3_Handle.Init.Mode			= UART_MODE_TX_RX;
    USART3_Handle.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
    USART3_Handle.Init.OverSampling	= UART_OVERSAMPLING_16;
      
    if( HAL_UART_Init( &USART3_Handle ) != HAL_OK ){ Error_Handler(); }
	                  
	USART3_RxCnt = 0;
	USART3_RxFifoHp = 0;
	USART3_RxFifoTp = 0;
	USART3_RxPacketCheckTick = 0;
    
    if( HAL_UART_Receive_IT( &USART3_Handle, (uint8_t *)&USART3_RxBuf, 1 ) != HAL_OK ){ Error_Handler(); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStruct = { 0 };
        
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_USART3_PARTIAL();
    
    __HAL_RCC_USART3_CLK_ENABLE();
    
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */                       
    GPIO_InitStruct.Pin		= GPIO_PIN_10;
    GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull	= GPIO_PULLUP;
    GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_HIGH;
	
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    GPIO_InitStruct.Pin		= GPIO_PIN_11;
    GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
	
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
    
    HAL_NVIC_SetPriority( USART3_IRQn, 3, 0 );
    HAL_NVIC_EnableIRQ( USART3_IRQn );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    U8	bData;
    
    if( huart->Instance == USART3 ){
	
        bData = USART3_RxBuf;
        
        if(HAL_UART_Receive_IT(&USART3_Handle, (uint8_t *)&USART3_RxBuf, 1) != HAL_OK)
        {
            Error_Handler();
        }
		
        if( USART3_RxCnt >= USART3_RX_BUFF_SIZE ){
			USART3_RxCnt = 0;
		}
		
		USART3_RxFifo[USART3_RxFifoHp & USART3_RX_FIFO_MASK][USART3_RxCnt] = bData;
		USART3_RxCnt++;
		
		USART3_RxPacketCheckTick = 0x8F; // 1.6 ms
	}             
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if( huart->Instance == USART3 ){
        TP_USART_L();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USART3_Rx_Packet_Check( void )
{
	if( USART3_RxPacketCheckTick & 0x80 ){
		if( USART3_RxPacketCheckTick == 0x80 ){
			USART3_RxPacketCheckTick = 0;  
    
            TP_USART_H();
			
#ifdef RX_PACKET_PRINT
			USART3_RxFifoLen[USART3_RxFifoHp & USART3_RX_FIFO_MASK] = USART3_RxCnt;
#endif
			
			USART3_RxFifoHp++;
			USART3_RxCnt = 0;
		}
		else {
			USART3_RxPacketCheckTick--;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USART3_Rx_Handler( void )
{
	U8	Hp = USART3_RxFifoHp & USART3_RX_FIFO_MASK;
	U8	Tp = USART3_RxFifoTp & USART3_RX_FIFO_MASK;
	
#ifdef RX_PACKET_PRINT
	U8	i;
#endif
	
	if( Hp != Tp ){
		USART3_RxFifoTp++;
		
#ifdef RX_PACKET_PRINT
        for( i=0; i<USART3_RxFifoLen[Tp]; i++ ){
            dprintf( "0x%02X, ", USART3_RxFifo[Tp][i] );
        }
        dprintf( "\n" );
#endif
		
		if(( USART3_RxFifo[Tp][1] == READ_HOLDING_REGISTERS ) || ( USART3_RxFifo[Tp][1] == READ_INPUT_REGISTERS )){
			
			Modbus_TimeoutTick = 0x8000 + EepHandle.Sram[EEP_SYS_MODBUS_TIMEOUT_TICK_100uS];
			Modbus_TimeoutFlag = false;
		}
		MODBUS_Packet_Decoder_Slave( &USART3_RxFifo[Tp][0] );
		
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USART3_Packet_Send( U8 *pData, U16 Len )
{
	if( HAL_UART_Transmit_IT( &USART3_Handle, (uint8_t *)pData, Len ) != HAL_OK ){
        Error_Handler();
    }
}


