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
VMEASURING_EXT void vMeasuring( Measuring_HandleType *pMeas );
VMEASURING_EXT char *vStrMainUnit( Measuring_GasInfo *pInf );
VMEASURING_EXT char *vStrAuxiUnit( Measuring_GasInfo *pInf );
VMEASURING_EXT char *vStrGasName( Measuring_GasInfo *pInf );
VMEASURING_EXT char *vStrHighScale( Measuring_GasInfo *pInf );
VMEASURING_EXT char *vStrAlarmInfo( U8 Step, U16 Resolution, float Val );
VMEASURING_EXT char *vStrGasValue( U16 Dec, float Gas );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VMEASURING_H__
