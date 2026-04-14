#ifndef   __FUNC_FACTORY_H__
#define   __FUNC_FACTORY_H__


#ifdef __FUNC_FACTORY_C__
	#define FUNC_FACTORY_EXT
#else
	#define FUNC_FACTORY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "function_mode.h"
#include "measuring.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	FACTORY_CAL_STATUS_READY,
	FACTORY_CAL_STATUS_INJECT_GAS,
	FACTORY_CAL_STATUS_DECISION,
	FACTORY_CAL_STATUS_FINISHED,
	FACTORY_CAL_STATUS_PACKET_SEND_GAIN,
	FACTORY_CAL_STATUS_PROGRESS_GAIN,
	FACTORY_CAL_STATUS_FINISHED_GAIN
}FactoryCal_Status;

typedef enum {
	FACTORY_CAL_RESULT_SUCCESS,
	FACTORY_CAL_RESULT_TIMEOUT,
	FACTORY_CAL_RESULT_LO_RESPONSE,
	FACTORY_CAL_RESULT_HI_RESPONSE,
	FACTORY_CAL_RESULT_OUTPUT_REVERSE
}FactoryCal_Result;
/* End Enum */

/* Start Struct */
typedef struct {
	FactoryCal_Status		Status;
	bool					Select[MEASURING_GAS_SENSOR_MAX];
	FactoryCal_Result		Result[MEASURING_GAS_SENSOR_MAX];
	U16						InjectionTmr;
	U16						Tick;
	U16						SpanMin;
	U16						SpanMax;
	U16						SpanGas;
	U8						SpanMsd;
	float					Volt[MEASURING_GAS_SENSOR_MAX];
	float					nA[CARTRIDGE_SEN_CH_MAX];
	float					Temp[MEASURING_GAS_SENSOR_MAX];
	bool					bZeroUse[MEASURING_GAS_SENSOR_MAX];
	U16						bZeroPer[MEASURING_GAS_SENSOR_MAX];
	U16						bZeroSec[MEASURING_GAS_SENSOR_MAX];
	U16						TargetOffs[MEASURING_GAS_SENSOR_MAX];
	U16						TargetGain[MEASURING_GAS_SENSOR_MAX];
	Cartridge_TiaType		TIA[CARTRIDGE_SEN_CH_MAX];
}FactoryCal_Type;

typedef union {
    U16	Word;
    struct{
		bool	mA_1		: 1; // 0x0001
		bool	mA_2		: 1; // 0x0002
		bool	Relay		: 1; // 0x0004
		bool	Flow		: 1; // 0x0008
		bool	S1_Set		: 1; // 0x0010
		bool	S2_Set		: 1; // 0x0020
		bool	S3_Set		: 1; // 0x0040
		bool	S4_Set		: 1; // 0x0080
		bool	S5_Set		: 1; // 0x0100
		bool	S6_Set		: 1; // 0x0200
		bool	S1_Cal		: 1; // 0x0400
		bool	S2_Cal		: 1; // 0x0800
		bool	S3_Cal		: 1; // 0x1000
		bool	S4_Cal		: 1; // 0x2000
		bool	S5_Cal		: 1; // 0x4000
		bool	S6_Cal		: 1; // 0x8000
    }Bit;
}FactoryInit_Item;

typedef struct {
	bool					IsConfirmMode;
	FactoryInit_Item		Set;
	FactoryInit_Item		Err;
}FactoryInit_Type;

typedef struct {
	Measuring_HandleType	*pMeas;
	Measuring_GasInfo		sGas[MEASURING_GAS_SENSOR_MAX];
	Measuring_AlarmInfo		sAlr[MEASURING_GAS_SENSOR_MAX];
	Measuring_ConfType		sConf;
	Measuring_ModeType		sMode;
	FactoryCal_Type			Cal;
	FactoryInit_Type		Init;
	U8						iWave;
	U16						NotStableDecTime;
	U16						sCtgSenType[CARTRIDGE_SEN_CH_MAX];
	U8						vScroll;
}Factory_SetType;
/* End Struct */

/* Start Function */
FUNC_FACTORY_EXT void Func_Factory( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __FUNC_FACTORY_H__
