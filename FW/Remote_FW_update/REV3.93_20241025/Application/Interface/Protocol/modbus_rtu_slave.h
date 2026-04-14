#ifndef   __MODBUS_RTU_SLAVE_H__
#define   __MODBUS_RTU_SLAVE_H__


#ifdef __MODBUS_RTU_SLAVE_C__
	#define MODBUS_RTU_SLAVE_EXT
#else
	#define MODBUS_RTU_SLAVE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "eeprom.h"
#include "modbus_map.h" 
#include "usart.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	MODBUS_RTU_SLAVE_OK,
	MODBUS_RTU_SLAVE_BUSY,
	MODBUS_RTU_SLAVE_CRC_ERROR,
	MODBUS_RTU_SLAVE_TIMEOUT
}MODBUS_SlaveStatusType;
/* End Enum */

/* Start Struct */
typedef struct {
	U16								Invalid;
	U16								Timeout;
}MODBUS_SlaveErrorType;

typedef struct {
	MODBUS_SlaveStatusType			Status;
	U16								*pUnitID; 
	Modbus_Bit_Type					*pReg_0x; 
	Modbus_Bit_Type					*pReg_1x; 
	U16								*pReg_3x;
	U16								*pReg_4x;
	U8								*pRxBuf;
	U16   							*pRxLen;                                    
	uint8_t							*pTxBuf;
	uint16_t						*pTxLen;
}MODBUS_RTU_SlaveHandleType;
/* End Struct */

/* Start Function */
MODBUS_RTU_SLAVE_EXT void MODBUS_RTU_Init_Slave( MODBUS_RTU_SlaveHandleType *pSlave );
MODBUS_RTU_SLAVE_EXT void MODBUS_RTU_Packet_Decoder_Slave( MODBUS_RTU_SlaveHandleType *pSlave );  
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __MODBUS_RTU_SLAVE_H__
