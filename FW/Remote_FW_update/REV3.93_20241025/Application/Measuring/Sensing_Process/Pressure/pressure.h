#ifndef   __PRESSURE_H__
#define   __PRESSURE_H__


#ifdef __PRESSURE_C__
	#define PRESSURE_EXT
#else
	#define PRESSURE_EXT extern
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

typedef struct {            
	float						AmpOut;
	float						AmpRef;
}Pressure_DataType;

typedef struct { 
	U16							*pAmpOut_ADC;
	U16							*pAmpRef_ADC;
    Pressure_DataType			*pRaw;
}Pressure_CalculationType;
/* End Struct */

/* Start Function */ 
PRESSURE_EXT void Pressure_Calculation( Pressure_CalculationType *pPressure );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __PRESSURE_H__
