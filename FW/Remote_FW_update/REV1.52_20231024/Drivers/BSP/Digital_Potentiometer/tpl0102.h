#ifndef   __TPL0102_H__
#define   __TPL0102_H__


#ifdef __TPL0102_C__
	#define TPL0102_EXT
#else
	#define TPL0102_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define TPL0102_DeviceAddress( addr )   	( 0xA0 | ( addr << 1 & 0x0E ))
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
TPL0102_EXT bool TPL0102_Init( void );
TPL0102_EXT void TPL0102_WiperUpdate( U16 DevAddr, U16 RegAddr, U8 Data );
TPL0102_EXT void TPL0102_WiperHandler( U16 DevAddr );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __TPL0102_H__
