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
/* End Include */

/* Start Define */
#define IR_BUFF_SIZE					( 200 )
#define IR_FIFO_SIZE					( 8 )
#define IR_FIFO_MASK					( IR_FIFO_SIZE - 1 )
                                               
#define AMP_CAL_PTM_OFFS_DEC_LIMIT		(  80 ) // 2.1328125[V] ~ 2.2500000[V]
#define AMP_CAL_PTM_OFFS_INC_LIMIT		( 176 ) // 2.7500000[V] ~ 2.8671875[V]
#define AMP_CAL_PTM_GAIN_DEC_LIMIT		(   0 )
#define AMP_CAL_PTM_GAIN_INC_LIMIT		( 255 )
/* End Define */

/* Start Enum */
enum {
	IR_WL_1,
	IR_WL_2,
	IR_WL_3,
	IR_WL_4,
	IR_WL_MAX
}; 

enum {
	IR_LOG_OFF = 0,
	IR_LOG_SERIALSCOPE,
	IR_LOG_TERATERM_FULL,
	IR_LOG_TERATERM_HALF,
	IR_LOG_TERATERM_1SEC,
	IR_LOG_MODE_MAX = IR_LOG_TERATERM_1SEC
};

typedef enum {                                   
    AMP_CAL_IDLE = 0,
    AMP_CAL_OFFS_CHECK_1ST = 1,
    AMP_CAL_READY = 8,
    AMP_CAL_OFFS_DEC_1ST = 10,
    AMP_CAL_OFFS_INC_1ST = 20,
    AMP_CAL_GAIN_CHECK_1ST = 21,
    AMP_CAL_GAIN_DEC_1ST = 30,
    AMP_CAL_GAIN_INC_1ST = 40,
    AMP_CAL_OFFS_CHECK_2ND = 41,
    AMP_CAL_OFFS_DEC_2ND = 50,
    AMP_CAL_OFFS_INC_2ND = 60,
    AMP_CAL_GAIN_CHECK_2ND = 61,
    AMP_CAL_GAIN_DEC_2ND = 70,
    AMP_CAL_GAIN_INC_2ND = 80,
    AMP_CAL_SUCCESS = 81,
    AMP_CAL_FAILURE = 82
}AmpCal_StatusType;

typedef enum {         
    IR_PROCESS_IDLE = 0,
    IR_PROCESS_ADS868x_START_CH_SET = 2,
    IR_PROCESS_WAVE_SENSING,
    IR_PROCESS_ADS868x_TEMP_CH_SET,
    IR_PROCESS_SENSOR_TEMP_SENSING,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH0,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH1,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH2,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH3,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH4,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH5,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH6,
    IR_PROCESS_ADS868x_GAIN_CHECK_CH7,
    IR_PROCESS_ADS868x_INIT_AUTO_SEQ_EN,
    IR_PROCESS_ADS868x_INIT_CH_PWR_DOWN,
    IR_PROCESS_ADS868x_INIT_FEATURE_SELECT,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_0,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_1,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_2,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_3,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_4,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_5,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_6,
    IR_PROCESS_ADS868x_INIT_INPUT_RANGE_7
}IR_ProcessState;
/* End Enum */

/* Start Struct */
typedef struct {        
	bool				iADC_ConversionComplete;
    float				SrcPwrRaw;
	float				McuTempRaw;
	float				SenTempRaw;
	
	U16					IrRaw[IR_WL_MAX][IR_BUFF_SIZE];
	U16					RawDvMax[IR_WL_MAX]; 
	
    float				SrcPwr; // mW
	ufloat				McuTemp;
	ufloat				SenTemp;
	
	ufloat				HiPeak[IR_WL_MAX];
	ufloat				LoPeak[IR_WL_MAX];
	ufloat				OcPtoP[IR_WL_MAX];
	
	U16					HiTime[IR_WL_MAX];
	U16					LoTime[IR_WL_MAX];
	
	ufloat				HiAvrg[IR_WL_MAX];
	ufloat				LoAvrg[IR_WL_MAX];
	ufloat				AvPtoP[IR_WL_MAX];
	
	ufloat				OcAvrg[IR_WL_MAX];
	
	float				PeakDv[IR_WL_MAX]; // Amp. Calibration Only
	
	U8					Wave[IR_WL_MAX][IR_BUFF_SIZE];
}IR_DataType;

typedef struct {
    AmpCal_StatusType  	Status[IR_WL_MAX];
    U8          		Offs[IR_WL_MAX];
    U8          		Gain[IR_WL_MAX];
    U8          		ManualOffs[IR_WL_MAX];
    U8          		ManualGain[IR_WL_MAX];
    float				TargetGain[IR_WL_MAX];
    float				TargetOffs[IR_WL_MAX];
    U16         		RunFlag;
    U16         		ErrFlag;
}IR_AmpCalType;

typedef struct {        
	IR_ProcessState		State;
	U8					FifoHp;
	U8					FifoTp;
	U16					iWave;
	U16					iBuff;
	bool				TaskComplete;
	U16					LogMode;
	U8					LogHandleCnt;
	U8					ModbusWaveOutputChannel;
	U8					SrcFreq;
	U16					BufSize;
	U16					SrcPowerCheckTmr;
	U16					SrcPowerCheckSet;
}IR_ProcessType;

/* End Struct */

/* Start Function */
IR_EXT void IR_Init( void );
IR_EXT void IR_Process_Start( void );
IR_EXT void IR_Process_Timer( void );
IR_EXT void IR_Signal_Processing( void );
IR_EXT void IR_Task( void );
IR_EXT void IR_Source_PWM_Start( void );
IR_EXT void IR_Source_PWM_Stop( void );
/* End Function */

/* Start Variable */                               
IR_EXT U16						iADC[IR_FIFO_SIZE][2];
IR_EXT IR_DataType				IrData[IR_FIFO_SIZE];
IR_EXT IR_AmpCalType			IrAmpCal;
IR_EXT IR_ProcessType			IrProcess;
/* End Variable */


#endif // __IR_H__
