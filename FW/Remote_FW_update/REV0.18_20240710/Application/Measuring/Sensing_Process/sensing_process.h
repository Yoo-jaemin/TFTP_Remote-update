#ifndef   __SENSING_PROCESS_H__
#define   __SENSING_PROCESS_H__


#ifdef __SENSING_PROCESS_C__
	#define SENSING_PROCESS_EXT
#else
	#define SENSING_PROCESS_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "dac.h"
#include "tia.h"
#include "adc.h"
#include "hs300x.h"
/* End Include */

/* Start Define */  
#define SENSING_ADC_BUFF_SIZE				( 100 )
#define SENSING_RAW_FIFO_SIZE				( 4 )
#define SENSING_RAW_FIFO_MASK				( SENSING_RAW_FIFO_SIZE - 1 )
/* End Define */

/* Start Enum */
enum {
	SENSING_GAS_CH_1,
	SENSING_GAS_CH_2,
	SENSING_GAS_CH_MAX
};

typedef enum {
	SENSING_STATE_IDLE,
	SENSING_STATE_HS300x_MEAS_REQUESTS,
	SENSING_STATE_ADC_CONVERSION_START,
	SENSING_STATE_LOAD_ADC,
	SENSING_STATE_LOAD_HS300x
}Sensing_StateType;
/* End Enum */

/* Start Struct */
typedef struct {
	U8						SenNo;
	LPDAC_ConfType			DAC;
	LPTIA_ConfType			TIA;
	U16						TargetGain;
}Sensing_ConfType;

typedef struct {
	ADC_RawType				ADC[SENSING_ADC_BUFF_SIZE];
	U16						HS300x_Humi;
	U16						HS300x_Temp;
}Sensing_RawType;

typedef struct {
	Sensing_ConfType		Conf[SENSING_GAS_CH_MAX];
	
	ADC_HandleType			*pADC;
	HS300x_HandleType		*pHS300x;
	
	Sensing_StateType		State;
	U8						Hp;
	U8						Tp;
	Sensing_RawType			Raw[SENSING_RAW_FIFO_SIZE];
	U8						ADC_RawIndex;
	U8						ADC_BuffSize;
	
	U16						Period; // msec
	U16						Period_Cnt;
	
	U16						DAC_Reset_Cnt;
}SensingProcess_HandleType;
/* End Struct */

/* Start Function */
SENSING_PROCESS_EXT void Sensing_Process_Init( void );
SENSING_PROCESS_EXT void Sensing_Process( void );
/* End Function */

/* Start Variable */
#pragma location = "never_retained_ram"
SENSING_PROCESS_EXT SensingProcess_HandleType		SensingProcess;
/* End Variable */


#endif // __SENSING_PROCESS_H__
