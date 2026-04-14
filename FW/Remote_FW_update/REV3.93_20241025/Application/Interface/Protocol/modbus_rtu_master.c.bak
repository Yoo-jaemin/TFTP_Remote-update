#define __MODBUS_C__
    #include "modbus_rtu_master.h"
#undef  __MODBUS_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */     
#include "usart.h"     
#include "crc16.h"  
/* End Include */

/* Start Define */                           
/* End Define */

/* Start Enum */        
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void MODBUS_RTU_Packet_Send_Master( MODBUS_MasterHandleType *pMaster );
static void MODBUS_RTU_Read_Register_Packet_Decoder_Master( MODBUS_MasterHandleType *pMaster );
static void MODBUS_RTU_Write_Register_Packet_Decoder_Master( MODBUS_MasterHandleType *pMaster );
/* End Function */

/* Start Variable */  
/* End Variable */    
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MODBUS_RTU_Init_Master( MODBUS_MasterHandleType *pMaster )
{                              
	bool	err = false;
	
	pMaster->Status				= MODBUS_RTU_MASTER_OK;
	
	// Below are usart parameter
	pMaster->pUart				= &USART3_Handle;
	pMaster->RxIntCnt			= 0;
	pMaster->RxFifoHp			= 0;
	pMaster->RxFifoTp			= 0;
	pMaster->PacketCheckTick	= 0;
	pMaster->Is485Mode			= false;
	
	if( HAL_UART_Receive_IT( pMaster->pUart, &pMaster->RxIntBuf, 1 ) != HAL_OK ){
		err = true;
	}
	
	return err;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Rx_Interrupt_Callback_Master( MODBUS_MasterHandleType *pMaster )
{
    U8	bData;

	bData = pMaster->RxIntBuf;
	
	if( HAL_UART_Receive_IT( pMaster->pUart, &pMaster->RxIntBuf, 1 ) != HAL_OK ){
		Error_Handler();
	}
	
	if( pMaster->RxIntCnt >= MODBUS_RTU_MASTER_RX_BUFF_SIZE ){
		pMaster->RxIntCnt  = 0;
	}
	
	pMaster->RxFifo[pMaster->RxFifoHp & MODBUS_RTU_MASTER_RX_FIFO_MASK][pMaster->RxIntCnt] = bData;
	pMaster->RxIntCnt++;
	
	pMaster->PacketCheckTick = 0x83; // 4 ms

}    
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Tx_Interrupt_Callback_Master( MODBUS_MasterHandleType *pMaster )
{
	if( pMaster->Is485Mode == true ){
		HAL_GPIO_WritePin( pMaster->TxEnGpioPort, pMaster->TxEnPin, GPIO_PIN_RESET );
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Rx_Packet_Check_Master( MODBUS_MasterHandleType *pMaster )
{
	if( pMaster->PacketCheckTick & 0x80 ){
		if( pMaster->PacketCheckTick == 0x80 ){
			pMaster->PacketCheckTick = 0;  
			
			pMaster->RxFifoLen[pMaster->RxFifoHp & MODBUS_RTU_MASTER_RX_FIFO_MASK] = pMaster->RxIntCnt;
			
			pMaster->RxFifoHp++;
			pMaster->RxIntCnt = 0;
		}
		else {
			pMaster->PacketCheckTick--;
		}
	}
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Rx_Handler_Master( MODBUS_MasterHandleType *pMaster )
{
	U8		Hp = pMaster->RxFifoHp & MODBUS_RTU_MASTER_RX_FIFO_MASK;
	U8		Tp = pMaster->RxFifoTp & MODBUS_RTU_MASTER_RX_FIFO_MASK;
	
	if( Hp != Tp ){
		
		pMaster->pRxPacket = &pMaster->RxFifo[Tp][0];
		pMaster->RxFifoTp++;
		
		MODBUS_RTU_Packet_Decoder_Master( pMaster );
		
		pMaster->TimeoutTick = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Timeout_Check_Master( MODBUS_MasterHandleType *pMaster )
{
	if( pMaster->TimeoutTick & 0x8000 ){
		if( pMaster->TimeoutTick == 0x8000 ){
			pMaster->TimeoutTick = 0;
			
			pMaster->Status = MODBUS_RTU_MASTER_TIMEOUT;
		}
		else {
			pMaster->TimeoutTick--;
		}
	}
}
                                                                     
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Packet_Decoder_Master( MODBUS_MasterHandleType *pMaster )
{
	if( pMaster->pRxPacket[0] != pMaster->SlaveAddress ){
		pMaster->Status = MODBUS_RTU_MASTER_ADDRESS_MISMATCH;
	}
	else {
		switch( pMaster->pRxPacket[1] ){
			case READ_INPUT_REGISTERS:
			case READ_HOLDING_REGISTERS:
				MODBUS_RTU_Read_Register_Packet_Decoder_Master( pMaster );
				break;
				
			case WRITE_SINGLE_REGISTER:
			case WRITE_MULTIPLE_REGISTERS:
				MODBUS_RTU_Write_Register_Packet_Decoder_Master( pMaster );
				break;
				
			default:
				pMaster->Status = MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE;
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Write_Register_Packet_Send_Master( MODBUS_MasterHandleType *pMaster )
{
	U16		wCRC;
	U8		i;
	
	pMaster->TxPacket[0] = pMaster->SlaveAddress;
	pMaster->TxPacket[1] = pMaster->Function;
	pMaster->TxPacket[2] = HI_BYTE( pMaster->StartAddress );
	pMaster->TxPacket[3] = LO_BYTE( pMaster->StartAddress );
	
	switch( pMaster->Function ){
		case WRITE_SINGLE_REGISTER:
			pMaster->TxPacket[4] = HI_BYTE( pMaster->pRequestBuf[0] );
			pMaster->TxPacket[5] = LO_BYTE( pMaster->pRequestBuf[0] );
			wCRC = CRC16( pMaster->TxPacket, 6 );
			pMaster->TxPacket[6] = LO_BYTE( wCRC );
			pMaster->TxPacket[7] = HI_BYTE( wCRC );
			pMaster->TxLength = 8;
			MODBUS_RTU_Packet_Send_Master( pMaster );
			break;
			
		case WRITE_MULTIPLE_REGISTERS:
			pMaster->TxPacket[4] = HI_BYTE( pMaster->RequestCount );
			pMaster->TxPacket[5] = LO_BYTE( pMaster->RequestCount );
			pMaster->TxPacket[6] = pMaster->ByteCount;
			for( i=0; i<pMaster->RequestCount; i++ ){
				pMaster->TxPacket[7+i*2] = HI_BYTE( pMaster->pRequestBuf[i] );
				pMaster->TxPacket[8+i*2] = LO_BYTE( pMaster->pRequestBuf[i] );
			}
			wCRC = CRC16( pMaster->TxPacket, 7+pMaster->ByteCount );
			pMaster->TxPacket[7+pMaster->ByteCount] = LO_BYTE( wCRC );
			pMaster->TxPacket[8+pMaster->ByteCount] = HI_BYTE( wCRC );
			pMaster->TxLength = 9 + pMaster->ByteCount;
			MODBUS_RTU_Packet_Send_Master( pMaster );
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_RTU_Read_Register_Packet_Send_Master( MODBUS_MasterHandleType *pMaster )
{
	U16		wCRC;
	
	pMaster->TxPacket[0] = pMaster->SlaveAddress;
	pMaster->TxPacket[1] = pMaster->Function;
	pMaster->TxPacket[2] = HI_BYTE( pMaster->StartAddress );
	pMaster->TxPacket[3] = LO_BYTE( pMaster->StartAddress );
	pMaster->TxPacket[4] = HI_BYTE( pMaster->RequestCount );
	pMaster->TxPacket[5] = LO_BYTE( pMaster->RequestCount );
	wCRC = CRC16( pMaster->TxPacket, 6 );
	pMaster->TxPacket[6] = LO_BYTE( wCRC );
	pMaster->TxPacket[7] = HI_BYTE( wCRC );
	pMaster->TxLength = 8;
	MODBUS_RTU_Packet_Send_Master( pMaster );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                           
static void MODBUS_RTU_Packet_Send_Master( MODBUS_MasterHandleType *pMaster )
{
	if( HAL_UART_Transmit_IT( pMaster->pUart, pMaster->TxPacket, pMaster->TxLength ) != HAL_OK ){
		Error_Handler();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_RTU_Read_Register_Packet_Decoder_Master( MODBUS_MasterHandleType *pMaster )
{
	U8		bc = pMaster->pRxPacket[2];
	U16		wCRC = CRC16( pMaster->pRxPacket, 3+bc );
	U16		i;
	
	if( ( LO_BYTE(wCRC) == pMaster->pRxPacket[3+bc] ) && ( HI_BYTE(wCRC) == pMaster->pRxPacket[4+bc] ) ){
		for( i=0, bc/=2; i<bc; i++ ){
			*pMaster->pReceiveBuf++ = ( ( (U16)pMaster->pRxPacket[3+i*2] ) << 8 ) | ( (U16)pMaster->pRxPacket[4+i*2] );
		}
		pMaster->Status = MODBUS_RTU_MASTER_OK;
	}
	else {
		pMaster->Status = MODBUS_RTU_MASTER_CRC_ERROR;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_RTU_Write_Register_Packet_Decoder_Master( MODBUS_MasterHandleType *pMaster )
{
	U16		wCRC = CRC16( pMaster->pRxPacket, 6 );
	
	if( ( LO_BYTE(wCRC) == pMaster->pRxPacket[6] ) && ( HI_BYTE(wCRC) == pMaster->pRxPacket[7] ) ){
		pMaster->Status = MODBUS_RTU_MASTER_OK;
	}
	else {
		pMaster->Status = MODBUS_RTU_MASTER_CRC_ERROR;
	}
}


