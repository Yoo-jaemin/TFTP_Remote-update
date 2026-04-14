#ifndef   __AT24C32_H__
#define   __AT24C32_H__


#ifdef __AT24C32_C__
	#define AT24C32_EXT
#else
	#define AT24C32_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
AT24C32_EXT void AT24C32_Init( void );
AT24C32_EXT void AT24C32_WriteWord( U16 wAddr, U16 wData );
AT24C32_EXT U16 AT24C32_ReadWord( U16 wAddr );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __AT24C32_H__
