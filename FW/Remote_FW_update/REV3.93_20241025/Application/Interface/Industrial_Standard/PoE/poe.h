#ifndef   __POE_H__
#define   __POE_H__


#ifdef __POE_C__
	#define POE_EXT
#else
	#define POE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "modbus_tcp_slave.h"
/* End Include */

/* Start Define */
//#define USE_DHCP       /* enable DHCP, if disabled static address is used */
#define POE_TX_BUFF_SIZE			(208)
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	uint8_t							MC[6];
	uint8_t							IP[4];
	uint8_t							NM[4];
	uint8_t							GW[4];
}POE_ConfigType;

typedef struct {
	POE_ConfigType					Conf;
	struct netif					NetIf;
	MODBUS_TCP_SlaveHandleType		Modbus;
	
	U8								TxBuf[POE_TX_BUFF_SIZE];
	U16								TxLen;
}POE_HandleType;
/* End Struct */

/* Start Function */
POE_EXT bool POE_Init( void );
POE_EXT void POE_ReInit( void );
POE_EXT void POE_Task( void );
/* End Function */

/* Start Variable */
POE_EXT POE_HandleType				PoeHandle;
/* End Variable */


#endif // __POE_H__
