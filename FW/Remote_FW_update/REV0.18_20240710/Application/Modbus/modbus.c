#define __MODBUS_C__
    #include "modbus.h"
#undef  __MODBUS_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "modbus_map.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void MODBUS_Packet_Send( void );
/* End Function */

/* Start Variable */
/* End Variable */     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MODBUS_Init( void )
{
	bool	err		= false;
	
	ModbusHandle.Conf.UnitID				= 0;
	
	ModbusHandle.Slave.pUnitID				= &ModbusHandle.Conf.UnitID;
	ModbusHandle.Slave.pTxBuf				= &ModbusHandle.TxBuf[0];
	ModbusHandle.Slave.pTxLen				= &ModbusHandle.TxLen;
	
	ModbusHandle.iRxBuf						= 0;
	ModbusHandle.RxFifoHp					= 0;
	ModbusHandle.RxFifoTp					= 0;
	ModbusHandle.PacketDet.Item.Cnt			= 0;
	ModbusHandle.PacketDet.Item.IsEnabled	= false;
	
	MODBUS_RTU_Init_Slave( &ModbusHandle.Slave );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Task( void )
{
	U8		Hp = ModbusHandle.RxFifoHp & MODBUS_RX_FIFO_MASK;
	U8		Tp = ModbusHandle.RxFifoTp & MODBUS_RX_FIFO_MASK;
	
	if( Hp != Tp ){
		
		ModbusHandle.RxFifoTp++;
		
		ModbusHandle.Slave.pRxBuf = &ModbusHandle.RxFifo[Tp][0];
		ModbusHandle.Slave.pRxLen = &ModbusHandle.RxFifoLen[Tp];
		
		MODBUS_RTU_Packet_Decoder_Slave( &ModbusHandle.Slave );
		
		if( ModbusHandle.TxLen ){
			MODBUS_Packet_Send();
		}
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Rx_Interrupt_Callback( void )
{
	if( ModbusHandle.iRxBuf > (MODBUS_RX_BUFF_SIZE - 1) ){
		ModbusHandle.iRxBuf = 0;
	}
	
	ModbusHandle.RxFifo[ModbusHandle.RxFifoHp & MODBUS_RX_FIFO_MASK][ModbusHandle.iRxBuf] = pADI_UART0->COMRX & 0xFF;
	ModbusHandle.iRxBuf++;
	
	ModbusHandle.PacketDet.Item.Cnt = 1;
	ModbusHandle.PacketDet.Item.IsEnabled = true;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Tx_Interrupt_Callback( void )
{	
	if( ModbusHandle.TxLen ){
		ModbusHandle.TxLen--;
		
		if( pADI_UART0->COMLSR & BITM_UART_COMLSR_THRE ){ pADI_UART0->COMTX = *ModbusHandle.pTxBuf++; }
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Rx_Packet_Check( void )
{
	if( ModbusHandle.PacketDet.Item.IsEnabled == true ){
		if( ModbusHandle.PacketDet.Item.Cnt ){
			ModbusHandle.PacketDet.Item.Cnt--;
		}
		else {
			ModbusHandle.PacketDet.Item.IsEnabled = false;
			ModbusHandle.RxFifoLen[ModbusHandle.RxFifoHp & MODBUS_RX_FIFO_MASK] = ModbusHandle.iRxBuf;
			ModbusHandle.RxFifoHp++;
			ModbusHandle.iRxBuf = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_Packet_Send( void )
{
	ModbusHandle.pTxBuf = ModbusHandle.TxBuf;
	ModbusHandle.TxLen -= 1;
	
	UrtTx( pADI_UART0, *ModbusHandle.pTxBuf++ );
}


