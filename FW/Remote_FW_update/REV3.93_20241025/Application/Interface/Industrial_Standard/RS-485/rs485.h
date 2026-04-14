#ifndef   __RS485_H__
#define   __RS485_H__


#ifdef __RS485_C__
	#define RS485_EXT
#else
	#define RS485_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "modbus_rtu_slave.h"
/* End Include */

/* Start Define */
#define RS485_RX_FIFO_SIZE			( 4 )
#define RS485_RX_FIFO_MASK			( RS485_RX_FIFO_SIZE - 1 )
#define RS485_TX_BUFF_SIZE			( 208 )
#define RS485_RX_BUFF_SIZE			( 208 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	U16								UnitID;
	U16								Baud;
	U16								Type;
}RS485_ConfigType;

typedef struct {
	RS485_ConfigType				Conf;
	UART_HandleTypeDef				Uart;
	MODBUS_RTU_SlaveHandleType		Modbus;
	
	uint8_t							RxIntBuf;
	U16   							RxIntCnt;
	U8   							RxFifoHp;
	U8   							RxFifoTp;
	U8   							RxFifo[RS485_RX_FIFO_SIZE][RS485_RX_BUFF_SIZE];
	U16   							RxFifoLen[RS485_RX_FIFO_SIZE];
	U8   							PacketCheckTick;
	U8   							PacketCheckTime;
	uint8_t							TxBuf[RS485_TX_BUFF_SIZE];
	uint16_t						TxLen;
	GPIO_TypeDef					*TxEnGpioPort;
	uint16_t						TxEnPin;
}RS485_HandleType;
/* End Struct */

/* Start Function */
RS485_EXT bool RS485_Init( void );
RS485_EXT void RS485_ReInit( void );
RS485_EXT void RS485_Task( void );
RS485_EXT void RS485_Rx_Interrupt_Callback( void );
RS485_EXT void RS485_Tx_Interrupt_Callback( void );
RS485_EXT void RS485_Rx_Packet_Check( void );
/* End Function */

/* Start Variable */ 
RS485_EXT RS485_HandleType			RS485_Handle;                                  
/* End Variable */


#endif // __RS485_H__
