#ifndef   __DAC8571_H__
#define   __DAC8571_H__


#ifdef __DAC8571_C__
	#define DAC8571_EXT
#else
	#define DAC8571_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define DAC8571_REF						( 2.5f )
#define DAC8571_RES						( 65536 )
#define DAC8571_LSB						( DAC8571_REF / DAC8571_RES )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
DAC8571_EXT void DAC8571_Write( U8 DeviceAddr, float fDAC );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __DAC8571_H__
