#ifndef   __POWER_SUPPLY_H__
#define   __POWER_SUPPLY_H__


#ifdef __POWER_SUPPLY_C__
	#define POWER_SUPPLY_EXT
#else
	#define POWER_SUPPLY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "bq2579x.h"
/* End Include */

/* Start Define */
#define VSUP_RP					(  24000.f ) // ohm
#define VSUP_RN					(   1000.f ) // ohm
#define VSUP_MONITOR_SLOPE		(( VSUP_RP + VSUP_RN ) / VSUP_RN * iADC_LSB * 0.001f )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {            
	float						VSUP;
	float						P48;
	float						P24;
	float						VBACKUP;
	float						IBUS;
	float						IBAT;
	float						VBUS;
	float						VBAT;
	float						VSYS;
}PowerSupply_DataType;

typedef struct {
	U16							*pVSUP_N_ADC;
	U16							*pVSUP_P_ADC;
	U16							*pVBAT_ADC;
	BQ2579x_ADC_Data			*pCharger;
    PowerSupply_DataType		*pRaw;
}PowerSupply_CalculationType;
/* End Struct */

/* Start Function */
POWER_SUPPLY_EXT void PowerSupply_Calculation( PowerSupply_CalculationType *pPower );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __POWER_SUPPLY_H__
