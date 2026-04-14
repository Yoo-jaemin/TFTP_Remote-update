#ifndef   __TEMPERATURE_H__
#define   __TEMPERATURE_H__


#ifdef __TEMPERATURE_C__
	#define TEMPERATURE_EXT
#else
	#define TEMPERATURE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "lmt84.h" 
#include "bq2579x.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {            
	float						LmtTemp;
	float						McuTemp;
	float						BatTemp;
	float						BatJper;
}Temperature_DataType;

typedef struct {
	U16							*pLmtTemp_ADC;
	U16							*pMcuTemp_ADC;
	BQ2579x_ADC_Data			*pCharger;
    Temperature_DataType		*pRaw;
}Temperature_CalculationType;
/* End Struct */

/* Start Function */
TEMPERATURE_EXT void Temperature_Calculation( Temperature_CalculationType *pTemperature );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __TEMPERATURE_H__
