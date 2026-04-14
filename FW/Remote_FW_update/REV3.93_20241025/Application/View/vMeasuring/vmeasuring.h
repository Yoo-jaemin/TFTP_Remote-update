#ifndef   __VMEASURING_H__
#define   __VMEASURING_H__


#ifdef __VMEASURING_C__
	#define VMEASURING_EXT
#else
	#define VMEASURING_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "measuring.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VMEASURING_EXT void vMeasuring( void );
VMEASURING_EXT void vMeasuring_DrawBase( void );
VMEASURING_EXT char *vStrGasConc( U16 Dec, float Gas );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VMEASURING_H__
