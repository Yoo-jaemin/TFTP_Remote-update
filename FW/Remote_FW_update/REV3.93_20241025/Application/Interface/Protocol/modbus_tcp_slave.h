#ifndef   __MODBUS_TCP_SLAVE_H__
#define   __MODBUS_TCP_SLAVE_H__


#ifdef __MODBUS_TCP_SLAVE_C__
	#define MODBUS_TCP_SLAVE_EXT
#else
	#define MODBUS_TCP_SLAVE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "eeprom.h"
#include "modbus_map.h"
/* End Include */

/* Start Define */
#define MODBUS_TCP_PORT							( 502 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct { 
	Modbus_Bit_Type			*pReg_0x; 
	Modbus_Bit_Type			*pReg_1x; 
	U16						*pReg_3x;
	U16						*pReg_4x;
	U8   					*pRxBuf;
	U16   					*pRxLen;
	U8   					*pTxBuf;
	U16   					*pTxLen;
}MODBUS_TCP_SlaveHandleType;
/* End Struct */

/* Start Function */
MODBUS_TCP_SLAVE_EXT void MODBUS_TCP_Init_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
MODBUS_TCP_SLAVE_EXT void MODBUS_TCP_Packet_Decoder_Slave( MODBUS_TCP_SlaveHandleType *pSlave );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __MODBUS_TCP_SLAVE_H__
