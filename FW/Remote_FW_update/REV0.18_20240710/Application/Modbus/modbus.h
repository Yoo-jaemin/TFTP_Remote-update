#ifndef   __MODBUS_H__
#define   __MODBUS_H__


#ifdef __MODBUS_C__
	#define MODBUS_EXT
#else
	#define MODBUS_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "modbus_rtu_slave.h"
/* End Include */

/* Start Define */
#define MODBUS_RX_FIFO_SIZE			( 4 )
#define MODBUS_RX_FIFO_MASK			( MODBUS_RX_FIFO_SIZE - 1 )
#define MODBUS_TX_BUFF_SIZE			( 256 )
#define MODBUS_RX_BUFF_SIZE			( 256 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
    U8	Byte;
    struct{
        U8		Cnt					: 7;
        bool	IsEnabled			: 1;
    }Item;
}MODBUS_PacketDetType;

typedef struct {
	U16								UnitID;
	U32								Baud;
}MODBUS_ConfigType;

typedef struct {
	MODBUS_ConfigType				Conf;
	MODBUS_RTU_SlaveHandleType		Slave;
	
	U16   							iRxBuf;
	U8   							RxFifoHp;
	U8   							RxFifoTp;
	U8   							RxFifo[MODBUS_RX_FIFO_SIZE][MODBUS_RX_BUFF_SIZE];
	U16   							RxFifoLen[MODBUS_RX_FIFO_SIZE];
	MODBUS_PacketDetType			PacketDet;
	U8								TxBuf[MODBUS_TX_BUFF_SIZE];
	U8								*pTxBuf;
	U16								TxLen;
}MODBUS_HandleType;
/* End Struct */

/* Start Function */
MODBUS_EXT bool MODBUS_Init( void );
MODBUS_EXT void MODBUS_ReInit( void );
MODBUS_EXT void MODBUS_Task( void );
MODBUS_EXT void MODBUS_Rx_Interrupt_Callback( void );
MODBUS_EXT void MODBUS_Tx_Interrupt_Callback( void );
MODBUS_EXT void MODBUS_Rx_Packet_Check( void );
/* End Function */

/* Start Variable */
#pragma location = "never_retained_ram"
MODBUS_EXT MODBUS_HandleType		ModbusHandle;
/* End Variable */


#endif // __MODBUS_H__
