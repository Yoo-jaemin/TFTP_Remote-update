#ifndef   __IR_H__
#define   __IR_H__

#ifdef __IR_C__
	#define IR_EXT
#else
	#define IR_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "cavity.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {            
    float						SrcPwr; // mW
	ufloat						McuTemp;
	ufloat						SenTemp;
	
	ufloat						HiPeak[CAVITY_SEN_CH_MAX];
	ufloat						LoPeak[CAVITY_SEN_CH_MAX];
	ufloat						OcPtoP[CAVITY_SEN_CH_MAX];
	
	ufloat						HiAvrg[CAVITY_SEN_CH_MAX];
	ufloat						LoAvrg[CAVITY_SEN_CH_MAX];
	ufloat						AvPtoP[CAVITY_SEN_CH_MAX];
	
	ufloat						OcAvrg[CAVITY_SEN_CH_MAX];
	
	U16							HiTime[CAVITY_SEN_CH_MAX]; // msec
	U16							LoTime[CAVITY_SEN_CH_MAX]; // msec
	U16							RawDvMax[CAVITY_SEN_CH_MAX];
}IR_Raw_DataType;

typedef struct {
	U16							*pReceiveBuf;
    IR_Raw_DataType				*pRaw;
}IR_Raw_CalculationType;
/* End Struct */

/* Start Function */
IR_EXT void IR_Raw_Calculation( IR_Raw_CalculationType *pIr );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __IR_H__
