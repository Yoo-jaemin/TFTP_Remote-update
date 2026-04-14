#ifndef   __MCP46X1_H__
#define   __MCP46X1_H__


#ifdef __MCP46X1_C__
	#define MCP46X1_EXT
#else
	#define MCP46X1_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
enum {
	MCP46x1_MEM_VOLATILE_WIPER_0,
	MCP46x1_MEM_VOLATILE_WIPER_1,
	MCP46x1_MEM_NON_VOLATILE_WIPER_0,
	MCP46x1_MEM_NON_VOLATILE_WIPER_1,
	MCP46x1_MEM_VOLATILE_TCON,
	MCP46x1_MEM_VOLATILE_STATUS,
	MCP46x1_MEM_EEPROM,
	MCP46x1_MEM_MAX = 15
};
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
	U16			Word;
	uint8_t		Byte[2];
}MCP46x1_WiperType;
/* End Struct */

/* Start Function */
MCP46X1_EXT bool MCP46X1_Init( void );
MCP46X1_EXT void MCP46X1_Wr_Wiper( U8 WiperAddress, U16 WiperValue );
MCP46X1_EXT bool MCP46X1_Rd_Wiper( U8 WiperAddress, MCP46x1_WiperType *pWiper );
/* End Function */

/* Start Variable */
MCP46X1_EXT MCP46x1_WiperType	MCP46x1_Wiper[2];      
/* End Variable */


#endif // __MCP46X1_H__
