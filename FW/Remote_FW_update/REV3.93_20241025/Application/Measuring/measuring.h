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
#include "sensor_list.h"
#include "ct_gas_list.h"
#include "ir_gas_list.h"
#include "model_list.h"
#include "flow_control.h"
/* End Include */

/* Start Define */
#define MEASURING_LPF_BUF_SIZE				( 64 )
#define MEASURING_LPF_BUF_MASK				( MEASURING_LPF_BUF_SIZE - 1 )
#define MEASURING_FILTER_BUF_SIZE			( 16 )
#define MEASURING_FILTER_BUF_MASK			( MEASURING_FILTER_BUF_SIZE - 1 )
#define MEASURING_GAS_PERCENTAGE_MAX		( 9999.f )
#define MEASURING_GAS_SENSOR_MAX			( 6 )
#define MEASURING_GAS_OUTPUT_MAX			( 2 )
/* End Define */

/* Start Enum */
enum {
	MEASURING_METHOD_IR,
	MEASURING_METHOD_CT
};

enum {
	MEASURING_UNIT_PPB,
	MEASURING_UNIT_PPM,
	MEASURING_UNIT_LEL,
	MEASURING_UNIT_VOL,
	MEASURING_UNIT_MAX
};

typedef enum {
	MEASURING_GAS_FUNC_CT_OUT,
	MEASURING_GAS_FUNC_CT_OUT_WITH_REF,
	MEASURING_GAS_FUNC_CT_INT,
	MEASURING_GAS_FUNC_CT_REF,
	MEASURING_GAS_FUNC_IR_OUT,
	MEASURING_GAS_FUNC_IR_OUT_WITH_REF,
	MEASURING_GAS_FUNC_IR_INT,
	MEASURING_GAS_FUNC_IR_REF
}Measuring_GasFunctype;

typedef enum {
	MEASURING_SEN_COMB_CTG_SING,
	MEASURING_SEN_COMB_CTG_DUAL,
	MEASURING_SEN_COMB_CVT_DUAL,
	MEASURING_SEN_COMB_CVT_QUAD,
	MEASURING_SEN_COMB_CTG_SING_CVT_DUAL,
	MEASURING_SEN_COMB_CTG_SING_CVT_QUAD,
	MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL,
	MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD
}Measuring_SenCombType;
/* End Enum */

/* Start Struct */
typedef struct {
	float						SysSenTemp;
	float						SysMcuTemp;
	float						BatSenTemp;
	float						BatSenJper; // As Percentage of REGN, JEITA_Tn
	
	float						Psen;
	float						FlowRate;
	float						VSUP;
	float						P48;
	float						P24;
	float						VBACKUP;
	
	float						IBUS;
	float						IBAT;
	float						VBUS;
	float						VBAT;
	                                       
	float						CtgSenHumi;
	float						CtgSenTemp;
	float						CtgMcuTemp;
	
	CT_RawDvSumType				CtgRawDvSum;
	CT_PeakDvType				CtgPeakDv;
	
	float						CtgAVDD;
	float						CtgDVDD;
	float						CtgVCB;
	float						CtgVZERO[CARTRIDGE_SEN_CH_MAX];
	float						CtgVBIAS[CARTRIDGE_SEN_CH_MAX];
	float						CtgICB[CARTRIDGE_SEN_CH_MAX];
	
	float						CvtSenTemp;
	float						CvtMcuTemp;
	
	float						CvtSrcPwr;
	
	float						CvtHiPeak[CAVITY_SEN_CH_MAX];
	float						CvtLoPeak[CAVITY_SEN_CH_MAX];
	float						CvtOcAvrg[CAVITY_SEN_CH_MAX];
	U16							CvtHiTime[CAVITY_SEN_CH_MAX];
	U16							CvtLoTime[CAVITY_SEN_CH_MAX];
	U16							CvtRawDvMax[CAVITY_SEN_CH_MAX];
	
	float						nDet[MEASURING_GAS_SENSOR_MAX]; // Sensor Output Voltage
	float						nRat[MEASURING_GAS_SENSOR_MAX]; // Base zero to normalized detection ratio
	float						nPer[MEASURING_GAS_SENSOR_MAX]; // Percentage gas
	float						iPer[MEASURING_GAS_SENSOR_MAX]; // Percentage gas for interference detection or without reference
}Measuring_DataType;

typedef struct {
	float						CtgSenHumi;
	float						CtgSenTemp;
	
	float						CvtSenTemp;
	float						SysSenTemp;
	
	float						CvtSrcPwr;
	
	float						CvtHiPeak[CAVITY_SEN_CH_MAX];
	float						CvtLoPeak[CAVITY_SEN_CH_MAX];
	float						CvtOcAvrg[CAVITY_SEN_CH_MAX];
	U16							CvtHiTime[CAVITY_SEN_CH_MAX];
	U16							CvtLoTime[CAVITY_SEN_CH_MAX];
	U16							CvtRawDvMax[CAVITY_SEN_CH_MAX];
	U16							CvtRawDvMaxSum[CAVITY_SEN_CH_MAX];
	U16							CvtOcAvrgSum[CAVITY_SEN_CH_MAX];
	
	float						nDet[MEASURING_GAS_SENSOR_MAX]; // Sensor Output Voltage
	float						nPer[MEASURING_GAS_SENSOR_MAX]; // Percentage gas
}Measuring_DevType;

typedef union {
    U16	Word;
    struct{
		U8		Maintenance_QuitMin	:  8;
		bool	Maintenance			:  1;
		bool	Engineering			:  1;
        U16		Reserved			:  6;
    }Item;
}Measuring_ModeType;

typedef union {
    U16	Word;
    struct{
		bool	GasSimulation		:  1;
		bool	RemoteTest_CH1		:  1;
		bool	RemoteTest_CH2		:  1;
		bool	WebTest_CH1			:  1;
		bool	WebTest_CH2			:  1;
        U16		Reserved			: 11;
    }Item;
}Measuring_TestType;

typedef struct {
	U16							Ctg[CARTRIDGE_SEN_CH_MAX];
	U16							Cvt;
}Measuring_ConfNoType;

typedef struct {
	Measuring_SenCombType		SenComb;
	Measuring_ConfNoType		SetNo;
	
	U8							iRefCtg;
	U8							iRefCvt;
	U8							iOut[MEASURING_GAS_OUTPUT_MAX];
	
	Measuring_GasFunctype		Func[MEASURING_GAS_SENSOR_MAX];
	
	U16							CtgModel[CARTRIDGE_SEN_CH_MAX];
	U16							CvtModel;
	
	U16							SenNo[MEASURING_GAS_SENSOR_MAX];
	U16							GasNo[MEASURING_GAS_SENSOR_MAX];
	float						iSpanScale[MEASURING_GAS_SENSOR_MAX];
	
	U8							OutMax;
	
	bool						IsAndMode;
}Measuring_ConfType;

typedef struct {
	float	Min;
	float	Typ;
	float	Max;
}Measuring_CT_Sensitivity;

typedef struct {
	Cartridge_TiaType			TIA;
	U16							TargetOffs; // mV
	U16							TargetGain; // mV
	U16							TargetBias; // mV
	U16							Method;
	U16							SenNo;
	U16							GasNo;
	U16							Dp;
	U16							HighScale;
	U16							Unit;
	U16							MaintenanceGas;
	U16							CrossScale;
	float						StableVolt;
	U16							StableTime;
	ufloat						mZero;
	ufloat						mZeroTemp;
	ufloat						mSpan;
	ufloat						mSpanTemp;
	U16							SpanGas;
	ufloat						bZero;
	ufloat						bZeroTemp;
	ufloat						bSpan;
	ufloat						bSpanTemp;
	float						rZero;
	float						rSpan;
	U16							GasName[6];
	bool						bZeroUse;
	U16							bZeroPer;
	U16							bZeroSec;
	U16							bZeroTmr;
	U16							bZeroCnt;
	U16							ZeroSkipPer;
	U16							SpanSkipPer;
	U16							OutDelayPer;
	U16							OutDelaySec;
	U32							OutDelayTmr;
	Measuring_GasFunctype		Func;
	U8							Msd; // Most Significant Digit
	
	// Below are the parameters for IR calculation.
	float						a;
	float						n;
	float						Alpha;
	float						Beta;
}Measuring_GasInfo;

typedef struct {
	bool						IsPresent;
	Measuring_GasInfo			Inf;
	float						nPerMin;
	float						nPerMax;
	float						iPerMin;
	float						iPerMax;
	float						Per;
	S16							Act;
	S16							Min;
	S16							Max;
	S16							Res;
	S16							Dsp;
	S16							Out;
	S16							Set;
}Measuring_GasType;

typedef struct {
	bool						_1st_Pre;
	U16							_1st_Dly;
	bool						_2nd_Pre;
	U16							_2nd_Dly;
}Measuring_AlarmDetection;

typedef struct {
	bool						IsUnderCheckEnabled;
	bool						_1stLatchEnabled;
	bool						_1stTypeIncrease;
	S16							_1stLevel;
	U16							_1stDead;
	U16							_1stDelaySec;
	bool						_2ndLatchEnabled;
	bool						_2ndTypeIncrease;
	S16							_2ndLevel;
	U16							_2ndDead;
	U16							_2ndDelaySec;
	Measuring_AlarmDetection	Det;
}Measuring_AlarmInfo;

typedef union {
    U8  Byte;
    struct{
        bool	a1st			: 1;
        bool	a2nd			: 1;
        bool	aOvr			: 1;
        bool	tUdr			: 1;
        U8		Reserved		: 4;
    }Bit;
}Measuring_AlarmItem;

typedef struct {
	Measuring_AlarmInfo			Inf;
	Measuring_AlarmItem			Item; 
	bool						Latched;
}Measuring_AlarmType;

typedef union {
    U32  Word;
    struct{
        bool	SystemPwr			:  1; // 0x00000001
        bool	FlowRate			:  1; // 0x00000002
        bool	InterfGas_1			:  1; // 0x00000004
        bool	InterfGas_2			:  1; // 0x00000008
		
        bool	CtgWarmUp			:  1; // 0x00000010
        bool	CtgTemp				:  1; // 0x00000020
        bool	CtgExcCurrent		:  1; // 0x00000040
        bool	CtgRandomNoise		:  1; // 0x00000080
        bool	CtgNotStable		:  1; // 0x00000100
        bool	CtgDeviation		:  1; // 0x00000200
        bool	CtgExcSignal		:  1; // 0x00000400
        bool	CtgDacError			:  1; // 0x00000800
		
        bool	CvtWarmUp			:  1; // 0x00001000
        bool	CvtTemp				:  1; // 0x00002000
        bool	CvtSrcPwr			:  1; // 0x00004000
        bool	CvtOffsetDrift		:  1; // 0x00008000
        bool	CvtRandomNoise		:  1; // 0x00010000
        bool	CvtNotStable		:  1; // 0x00020000
        bool	CvtDeviation		:  1; // 0x00040000
        bool	CvtExcSignal		:  1; // 0x00080000
        bool	CvtOptPath			:  1; // 0x00100000
        bool	CvtOffsInstability	:  1; // 0x00200000
		
        U16		Reserved			: 10; // 0xFFC00000
    }Bit;
}Measuring_FilterItem;

typedef union {
    U16  Word;
    struct{
        U16		Cnt					: 15;
        bool	IsCh1Higher			:  1;
    }Item;
}Gas_DeviationTmrType;

typedef struct {
	Measuring_FilterItem		Buf[MEASURING_FILTER_BUF_SIZE];
	Measuring_FilterItem		Res;
	U8							Idx;
	Flow_ControlType			*pFlow;
	U16							CtgTempErrTmr;
	U16							CtgDacErrCnt;
	U16							CtgDacErrTmr;
	Gas_DeviationTmrType		CtgGasDevTmr;
	U16							CtgExcCurrentTmr[CARTRIDGE_SEN_CH_MAX];
	
	U16							CvtTempErrTmr;
	U16							SysPwrErrTmr;
	U16							SrcPwrErrTmr;
	U16							CvtOffsDriftTmr[CAVITY_SEN_CH_MAX];
	U16							CvtOptPthErrTmr[CAVITY_SEN_CH_MAX];
	U16							CvtOffsInstabilityTmr;
	
	U16							ExcSignalTmr[MEASURING_GAS_SENSOR_MAX];
	U16							NotStableTmr[MEASURING_GAS_SENSOR_MAX];
	U16							NotStableTmrMax[MEASURING_GAS_SENSOR_MAX];
	
	U32							PendingTmr;
	U16							NotStableDecTime;
}Measuring_FilterType;

typedef struct {
	bool						WarmUpEnd[MEASURING_GAS_SENSOR_MAX];
	U16							WarmUpTmr[MEASURING_GAS_SENSOR_MAX];
	bool						NotStable;
	U16							StableTmr;
}Measuring_bZeroType;

typedef struct {
	SensingProcess_HandleType	*pSen;
	float						IrSenLpfWeight;
	float						OtherLpfWeight;
	float						CtSenLpfWeight;
	Measuring_DataType			Raw;
	Measuring_DataType			Lpf[MEASURING_LPF_BUF_SIZE];
	U8							LpfIndex;
	Measuring_DataType			*pPrv;
	Measuring_DataType			*pCur;
	Measuring_DevType			Dev;
	Measuring_ModeType			Mode;
	Measuring_TestType			Test;
	Measuring_ConfType			Conf;
	Measuring_GasType			Gas[MEASURING_GAS_SENSOR_MAX];
	Measuring_AlarmType			Alr[MEASURING_GAS_SENSOR_MAX];
	Measuring_GasType			*pGas[MEASURING_GAS_OUTPUT_MAX];
	Measuring_AlarmType			*pAlr[MEASURING_GAS_OUTPUT_MAX];
	Measuring_FilterType		Filter;
	Measuring_bZeroType			bZero;
}Measuring_HandleType;
/* End Struct */

/* Start Function */
MEASURING_EXT void Measuring_Config( void );
MEASURING_EXT bool Measuring_Channel_Config_Load( Measuring_ConfType *pConf );
MEASURING_EXT void Measuring_Output_Channel_Mapping( void );
MEASURING_EXT void Measuring_Gas_Function_Load( void );
MEASURING_EXT void Measuring_Gas_MSD_Load( Measuring_GasInfo *pInf );
MEASURING_EXT void Measuring_Default_Gas_Name_Load( Measuring_GasInfo *pInf );
MEASURING_EXT void Measuring_Default_Gas_Info_Load( Measuring_GasInfo *pGas, Measuring_AlarmInfo *pAlr );
MEASURING_EXT void Measuring_Gas_Name_Update( U8 i );
MEASURING_EXT void Measuring_Gas_Info_Update( U8 i );
MEASURING_EXT void Measuring_Task( void );
MEASURING_EXT void Measuring_Maintenance_Auto_Quit_Handler( void );
MEASURING_EXT void Measuring_Maintenance_Quit_Time_Refresh( void );
MEASURING_EXT float Measuring_Gas_Response_Calculation( U8 i, U16 SpanGas, float det );
MEASURING_EXT S16 Measuring_CT_Sensor_Bias_Load( Measuring_GasInfo *pGas );
/* End Function */

/* Start Variable */
MEASURING_EXT Measuring_HandleType		MeasHandle;
/* End Variable */


#endif // __MEASURING_H__


