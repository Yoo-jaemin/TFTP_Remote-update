#define __RS485_C__
    #include "rs485.h"
#undef  __RS485_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "modbus_map.h"
#include "eeprom.h"
#include "vstatusbar.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void RS485_Packet_Send( void );
/* End Function */

/* Start Variable */
static const U32	Baud_Type[7]		= { 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
static const U8		Packet_Iterval[7]	= { 0x8F, 0x88, 0x84, 0x83, 0x83, 0x83, 0x83 };
/* End Variable */     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RS485_Init( void )
{
	bool	err		= false;
	U32		Baud	= 9600;
	U32		Data	= UART_WORDLENGTH_8B;
	U32		Parity	= UART_PARITY_NONE;
	U32		Stop	= UART_STOPBITS_1;
	
	if( ((SystemOption.Item.IsBatMode == false) && (SystemOption.Item.IsPoeMode == false)) ||
		 (SystemOption.Item.IsUniMode ==  true) )
	{
	
		RS485_Handle.Conf.UnitID	= EepHandle.Sram[SYSTEM_EEP_MODBUS_UNIT_ID];
		RS485_Handle.Conf.Baud		= EepHandle.Sram[SYSTEM_EEP_RS485_BAUD_RATE];
		RS485_Handle.Conf.Type		= EepHandle.Sram[SYSTEM_EEP_RS485_DATA_TYPE];
		
		Baud = Baud_Type[RS485_Handle.Conf.Baud];
		
		switch( RS485_Handle.Conf.Type ){
			case  0: Data = UART_WORDLENGTH_8B, Parity = UART_PARITY_NONE, Stop = UART_STOPBITS_1; break;
			case  1: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_ODD , Stop = UART_STOPBITS_1; break;
			case  2: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_EVEN, Stop = UART_STOPBITS_1; break;
			case  3: Data = UART_WORDLENGTH_8B, Parity = UART_PARITY_NONE, Stop = UART_STOPBITS_2; break;
			case  4: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_ODD , Stop = UART_STOPBITS_2; break;
			case  5: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_EVEN, Stop = UART_STOPBITS_2; break;
		}
		
		RS485_Handle.Uart.Instance			= USART6;
		 
		RS485_Handle.Uart.Init.BaudRate		= Baud;
		RS485_Handle.Uart.Init.WordLength	= Data;
		RS485_Handle.Uart.Init.StopBits		= Stop;
		RS485_Handle.Uart.Init.Parity		= Parity;
		RS485_Handle.Uart.Init.Mode			= UART_MODE_TX_RX;
		RS485_Handle.Uart.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
		RS485_Handle.Uart.Init.OverSampling	= UART_OVERSAMPLING_16;
		
		RS485_Handle.Modbus.pUnitID			= &RS485_Handle.Conf.UnitID;
		RS485_Handle.Modbus.pTxBuf			= &RS485_Handle.TxBuf[0];
		RS485_Handle.Modbus.pTxLen			= &RS485_Handle.TxLen;
		
		RS485_Handle.RxIntCnt				= 0;
		RS485_Handle.RxFifoHp				= 0;
		RS485_Handle.RxFifoTp				= 0;
		RS485_Handle.PacketCheckTick		= 0;
		RS485_Handle.PacketCheckTime		= Packet_Iterval[RS485_Handle.Conf.Baud];
		
		RS485_Handle.TxEnGpioPort			= GPIOG;
		RS485_Handle.TxEnPin				= GPIO_PIN_2;
		
		GPIO_Init();
		MODBUS_RTU_Init_Slave( &RS485_Handle.Modbus );
		
		if( HAL_UART_Init( &RS485_Handle.Uart ) != HAL_OK ){ err = true; }
		if( HAL_UART_Receive_IT( &RS485_Handle.Uart, &RS485_Handle.RxIntBuf, 1 ) != HAL_OK ){ err = true; }
	}
	
	return err;
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RS485_ReInit( void )
{
	U32		Baud	= 9600;
	U32		Data	= UART_WORDLENGTH_8B;
	U32		Parity	= UART_PARITY_NONE;
	U32		Stop	= UART_STOPBITS_1;
	
	HAL_GPIO_WritePin( RS485_Handle.TxEnGpioPort, RS485_Handle.TxEnPin, GPIO_PIN_SET );
	
	HAL_UART_DeInit( &RS485_Handle.Uart );
	
	Baud = Baud_Type[RS485_Handle.Conf.Baud];
	
	switch( RS485_Handle.Conf.Type ){
		case  0: Data = UART_WORDLENGTH_8B, Parity = UART_PARITY_NONE, Stop = UART_STOPBITS_1; break;
		case  1: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_ODD , Stop = UART_STOPBITS_1; break;
		case  2: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_EVEN, Stop = UART_STOPBITS_1; break;
		case  3: Data = UART_WORDLENGTH_8B, Parity = UART_PARITY_NONE, Stop = UART_STOPBITS_2; break;
		case  4: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_ODD , Stop = UART_STOPBITS_2; break;
		case  5: Data = UART_WORDLENGTH_9B, Parity = UART_PARITY_EVEN, Stop = UART_STOPBITS_2; break;
	}
	
	RS485_Handle.Uart.Instance			= USART6;
	 
    RS485_Handle.Uart.Init.BaudRate		= Baud;
    RS485_Handle.Uart.Init.WordLength	= Data;
    RS485_Handle.Uart.Init.StopBits		= Stop;
    RS485_Handle.Uart.Init.Parity		= Parity;
    RS485_Handle.Uart.Init.Mode			= UART_MODE_TX_RX;
    RS485_Handle.Uart.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
    RS485_Handle.Uart.Init.OverSampling	= UART_OVERSAMPLING_16;
	
	RS485_Handle.Modbus.pUnitID			= &RS485_Handle.Conf.UnitID;
	RS485_Handle.Modbus.pTxBuf			= &RS485_Handle.TxBuf[0];
	RS485_Handle.Modbus.pTxLen			= &RS485_Handle.TxLen;
	
	RS485_Handle.RxIntCnt				= 0;
	RS485_Handle.RxFifoHp				= 0;
	RS485_Handle.RxFifoTp				= 0;
	RS485_Handle.PacketCheckTick		= 0;
	RS485_Handle.PacketCheckTime		= Packet_Iterval[RS485_Handle.Conf.Baud];
	
	MODBUS_RTU_Init_Slave( &RS485_Handle.Modbus );
    
	HAL_UART_Init( &RS485_Handle.Uart );
	HAL_UART_Receive_IT( &RS485_Handle.Uart, &RS485_Handle.RxIntBuf, 1 );
	
	CLEAR_BIT( RS485_Handle.Uart.Instance->CR1, USART_CR1_TXEIE );
	
	HAL_GPIO_WritePin( RS485_Handle.TxEnGpioPort, RS485_Handle.TxEnPin, GPIO_PIN_RESET );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RS485_Task( void )
{
	U8		Hp = RS485_Handle.RxFifoHp & RS485_RX_FIFO_MASK;
	U8		Tp = RS485_Handle.RxFifoTp & RS485_RX_FIFO_MASK;
	
	if( Hp != Tp ){
		RS485_Handle.Modbus.pRxBuf = &RS485_Handle.RxFifo[Tp][0];
		RS485_Handle.Modbus.pRxLen = &RS485_Handle.RxFifoLen[Tp];
		MODBUS_RTU_Packet_Decoder_Slave( &RS485_Handle.Modbus );
		
		if( RS485_Handle.TxLen ){
			RS485_Packet_Send();
			if( !vStBar.ComIconTmr ){ vStBar.ComIconTmr = COM_ICON_BLINK_INTERVAL; }
		}
		
		RS485_Handle.RxFifoTp++;
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RS485_Rx_Interrupt_Callback( void )
{
    U8	bData;
	
	bData = RS485_Handle.RxIntBuf;
	
	if( HAL_UART_Receive_IT( &RS485_Handle.Uart, &RS485_Handle.RxIntBuf, 1 ) != HAL_OK ){
		Error_Handler();
	}
	
	if( RS485_Handle.RxIntCnt > (RS485_RX_BUFF_SIZE - 1) ){
		RS485_Handle.RxIntCnt = 0;
	}
	
	RS485_Handle.RxFifo[RS485_Handle.RxFifoHp & RS485_RX_FIFO_MASK][RS485_Handle.RxIntCnt] = bData;
	RS485_Handle.RxIntCnt++;
	
	RS485_Handle.PacketCheckTick = RS485_Handle.PacketCheckTime;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RS485_Tx_Interrupt_Callback( void )
{
	HAL_GPIO_WritePin( RS485_Handle.TxEnGpioPort, RS485_Handle.TxEnPin, GPIO_PIN_RESET );
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RS485_Rx_Packet_Check( void )
{
	if( RS485_Handle.PacketCheckTick & 0x80 ){
		if( RS485_Handle.PacketCheckTick == 0x80 ){
			RS485_Handle.PacketCheckTick = 0;  
			
			RS485_Handle.RxFifoLen[RS485_Handle.RxFifoHp & RS485_RX_FIFO_MASK] = RS485_Handle.RxIntCnt;
			
			RS485_Handle.RxFifoHp++;
			RS485_Handle.RxIntCnt = 0;
		}
		else {
			RS485_Handle.PacketCheckTick--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{    
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
    
  __HAL_RCC_GPIOG_CLK_ENABLE();
    
    GPIO_InitStructure.Pin		= RS485_Handle.TxEnPin;
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_NOPULL;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_WritePin( RS485_Handle.TxEnGpioPort, RS485_Handle.TxEnPin, GPIO_PIN_RESET );
	
    HAL_GPIO_Init( RS485_Handle.TxEnGpioPort, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void RS485_Packet_Send( void )
{
	HAL_GPIO_WritePin( RS485_Handle.TxEnGpioPort, RS485_Handle.TxEnPin, GPIO_PIN_SET );
	
	if( HAL_UART_Transmit_IT( &RS485_Handle.Uart, RS485_Handle.TxBuf, RS485_Handle.TxLen ) != HAL_OK ){
		Error_Handler();
	}
}


