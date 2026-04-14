#ifndef   __MEASURING_H__
#define   __MEASURING_H__


#ifdef __MEASURING_C__
	#define MEASURING_EXT
#else
	#define MEASURING_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "sensing_process.h"
/* End Include */

/* Start Define */  
/* End Define */

/* Start Enum */
enum {
	MEASURING_GAS_CH_1,
	MEASURING_GAS_CH_2,
	MEASURING_GAS_CH_MAX
};
/* End Enum */

/* Start Struct */
typedef struct {
	ADC_RawType				RawDvSum;
	ADC_RawType				PeakDv;
	//U16						PeakDv_LPTIA0_LPF;
	//U16						PeakDv_LPTIA1_LPF;
	
	ufloat					SenHumi;
	ufloat					SenTemp;
	ufloat					McuTemp;
	
	ufloat					AVDD;
	ufloat					DVDD;
	ufloat					VCB;
	
	ufloat					LPTIA0_LPF;
	ufloat					VZERO0;
	ufloat					VBIAS0;
	ufloat					ICB0;
	
	ufloat					LPTIA1_LPF;
	ufloat					VZERO1;
	ufloat					VBIAS1;
	ufloat					ICB1;
	
}Measuring_DataType;

typedef struct {
	SensingProcess_HandleType	*pSen;
	Measuring_DataType			Data;
}Measuring_HandleType;
/* End Struct */

/* Start Function */
MEASURING_EXT void Measuring_Init( void );
MEASURING_EXT void Measuring_Task( void );
/* End Function */

/* Start Variable */
MEASURING_EXT Measuring_HandleType		MeasHandle;
/* End Variable */


#endif // __MEASURING_H__
