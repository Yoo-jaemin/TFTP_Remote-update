#ifndef   __CT_H__
#define   __CT_H__

#ifdef __CT_C__
	#define CT_EXT
#else
	#define CT_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h" 
#include "cartridge.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	U16		LPTIA_LPF[CARTRIDGE_SEN_CH_MAX];
	U16		VZERO[CARTRIDGE_SEN_CH_MAX];
	U16		VBIAS[CARTRIDGE_SEN_CH_MAX];
}CT_RawDvSumType;

typedef struct {
	U16		LPTIA_LPF[CARTRIDGE_SEN_CH_MAX];
	U16		VZERO[CARTRIDGE_SEN_CH_MAX];
	U16		VBIAS[CARTRIDGE_SEN_CH_MAX];
}CT_PeakDvType;

typedef struct {            
	CT_RawDvSumType			RawDvSum;
	CT_PeakDvType			PeakDv;
	
	ufloat					SenHumi;
	ufloat					SenTemp;
	ufloat					McuTemp;
	
	ufloat					AVDD;
	ufloat					DVDD;
	ufloat					VCB;
	
	ufloat					LPTIA_LPF[CARTRIDGE_SEN_CH_MAX];
	ufloat					VZERO[CARTRIDGE_SEN_CH_MAX];
	ufloat					VBIAS[CARTRIDGE_SEN_CH_MAX];
	ufloat					ICB[CARTRIDGE_SEN_CH_MAX];
}CT_Raw_DataType;

typedef struct {
	U16						*pReceiveBuf;
    CT_Raw_DataType			*pRaw;
}CT_Raw_CalculationType;
/* End Struct */

/* Start Function */
CT_EXT void CT_Calculation( CT_Raw_CalculationType *pCt );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __CT_H__
