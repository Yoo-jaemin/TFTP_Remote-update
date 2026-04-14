#ifndef   __MODBUS_RTU_MASTER_H__
#define   __MODBUS_RTU_MASTER_H__


#ifdef __MODBUS_RTU_MASTER_C__
	#define MODBUS_RTU_MASTER_EXT
#else
	#define MODBUS_RTU_MASTER_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "eeprom.h" 
#include "usart.h" 
/* End Include */

/* Start Define */
#define MODBUS_RTU_MASTER_TX_PACKET_SIZE				( 256 )
#define MODBUS_RTU_MASTER_RX_FIFO_SIZE					( 4 )
#define MODBUS_RTU_MASTER_RX_FIFO_MASK					( MODBUS_RTU_MASTER_RX_FIFO_SIZE - 1 )
#define MODBUS_RTU_MASTER_RX_BUFF_SIZE					( 256 )
#define MODBUS_RTU_MASTER_TIMEOUT_INTERVAL				( 0x8000 + 12 - 1 ) // 40msec. * 12 = 480 msec.
#define MODBUS_RTU_MASTER_SLAVE_DEVICE_MAX				( 2 )
/* End Define */

/* Start Enum */
enum {
	READ_COIL_STATUS = 1,
	READ_INPUT_STATUS,
	READ_HOLDING_REGISTERS,
	READ_INPUT_REGISTERS,
	WRITE_SINGLE_COIL,
	WRITE_SINGLE_REGISTER,
	WRITE_MULTIPLE_COILS = 15,
	WRITE_MULTIPLE_REGISTERS
};

typedef enum {
	MODBUS_RTU_MASTER_OK,
	MODBUS_RTU_MASTER_BUSY,
	MODBUS_RTU_MASTER_ADDRESS_MISMATCH,
	MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE,
	MODBUS_RTU_MASTER_CRC_ERROR,
	MODBUS_RTU_MASTER_TIMEOUT
}MODBUS_MasterStatusType;
/* End Enum */

/* Start Struct */
typedef struct {
	MODBUS_MasterStatusType			Status;
	U8								SlaveAddress;
	U8								Function;
	U16								StartAddress;
	U16								RequestCount;
	U8								ByteCount;
	U16								*pReceiveBuf;
	U16								*pRequestBuf;
	U16								TimeoutTick;
	
	// Below are usart parameter
	UART_HandleTypeDef				*pUart;
	uint8_t							RxIntBuf;
	U16   							RxIntCnt;
	U8   							RxFifoHp;
	U8   							RxFifoTp;
	U8   							RxFifo[MODBUS_RTU_MASTER_RX_FIFO_SIZE][MODBUS_RTU_MASTER_RX_BUFF_SIZE];
	U16   							RxFifoLen[MODBUS_RTU_MASTER_RX_FIFO_SIZE];
	U8   							PacketCheckTick;
	U8								*pRxPacket;                                    
	uint8_t							TxPacket[MODBUS_RTU_MASTER_TX_PACKET_SIZE];
	uint16_t						TxLength;
	bool							Is485Mode; 
	GPIO_TypeDef					*TxEnGpioPort;
	uint16_t						TxEnPin;
}MODBUS_MasterHandleType;
/* End Struct */

/* Start Function */
MODBUS_RTU_MASTER_EXT bool MODBUS_RTU_Init_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Rx_Interrupt_Callback_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Tx_Interrupt_Callback_Master( MODBUS_MasterHandleType *pMaster );                       
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Rx_Packet_Check_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Rx_Handler_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Timeout_Check_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Packet_Decoder_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Write_Register_Packet_Send_Master( MODBUS_MasterHandleType *pMaster );
MODBUS_RTU_MASTER_EXT void MODBUS_RTU_Read_Register_Packet_Send_Master( MODBUS_MasterHandleType *pMaster );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __MODBUS_H__
