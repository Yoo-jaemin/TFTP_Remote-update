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
#include "modbus_rtu_master.h"
#include "cartridge.h"
#include "cavity.h"
#include "ct.h"
#include "ir.h"
#include "adc.h"
#include "power_supply.h"
#include "pressure.h"
#include "temperature.h"
/* End Include */

/* Start Define */  
#define CAVITY_PORT										( GPIO_PIN_RESET )
#define CARTRIDGE_USART_PORT							( GPIO_PIN_SET )
#define SENSOR_USART_PORT_SELECTION( port )				( HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, port ) )

#define SEN_EEP_HANDLE_FIFO_SIZE						( 32 )
#define SEN_EEP_HANDLE_FIFO_MASK						( SEN_EEP_HANDLE_FIFO_SIZE - 1 )
/* End Define */

/* Start Enum */
typedef enum {
	SENSING_PROCESS_CTG_SENS_DATA_REQUEST,
	SENSING_PROCESS_CTG_SENS_DATA_RECEIVE,
	SENSING_PROCESS_CTG_EEP_WRITE_REQUEST,
	SENSING_PROCESS_CTG_EEP_WRITE_RECEIVE,
	SENSING_PROCESS_CTG_EEP_RELOAD,
	SENSING_PROCESS_CVT_SENS_DATA_REQUEST,
	SENSING_PROCESS_CVT_SENS_DATA_RECEIVE,
	SENSING_PROCESS_CVT_EEP_WRITE_REQUEST,
	SENSING_PROCESS_CVT_EEP_WRITE_RECEIVE,
	SENSING_PROCESS_CVT_EEP_RELOAD,
	SENSING_PROCESS_iADC_CONVERSION_START,
	SENSING_PROCESS_iADC_CONVERSION_CHECK,
	SENSING_PROCESS_BAT_CHARGER_ADCs_READ
}SensingProcess_StateType;
/* End Enum */

/* Start Struct */
typedef union {
    U8  Byte;
    struct{
        bool		CT				: 1;
        bool		IR				: 1;
        bool		iADC			: 1;
        bool		BatCharger		: 1;
        U8			Reserved		: 4;
    }Item;
}SensingProcess_SyncType;

typedef struct {
	CT_Raw_DataType						CT;
	IR_Raw_DataType						IR;
	PowerSupply_DataType				Power;
	Pressure_DataType					Pressure;
	Temperature_DataType				Temp;
}SensingProcess_DataType;

typedef struct {
	CT_Raw_CalculationType				CT;
	IR_Raw_CalculationType				IR;
	PowerSupply_CalculationType			Power;
	Pressure_CalculationType			Pressure;
	Temperature_CalculationType			Temp;
}SensingProcess_CalculationType;

typedef struct {
	U8									Hp;
	U8									Tp;
	U8									Block[SEN_EEP_HANDLE_FIFO_SIZE];
	U16									wAddr[SEN_EEP_HANDLE_FIFO_SIZE];
	U16									wSize[SEN_EEP_HANDLE_FIFO_SIZE];
}SensingProcess_SensorEepHandleType;

typedef struct {
	SensingProcess_StateType			State;
	SensingProcess_SyncType				Sync;
	
	SensingProcess_DataType				Raw;
	SensingProcess_CalculationType		Calculation;
	
	MODBUS_MasterHandleType				Modbus;
	
	Cartridge_HandleType				*pCtg;
	SensingProcess_SensorEepHandleType	CtgEep;
	U16									CtgComErrCnt;
	bool								CtgWaveModeSet;
	bool								CtgWaveModeOut;
	
	Cavity_HandleType					*pCvt;
	SensingProcess_SensorEepHandleType	CvtEep;
	U16									CvtComErrCnt;
	bool								CvtWaveModeSet;
	bool								CvtWaveModeOut;
	
	iADC_HandleType						*piADC;
}SensingProcess_HandleType;
/* End Struct */

/* Start Function */
SENSING_PROCESS_EXT bool Sensing_Process_Init( void );
SENSING_PROCESS_EXT bool Sensing_Process( void );
SENSING_PROCESS_EXT void SenEEP_Write_CHn( U16 *pData, U8 iCh, U16 wAddr, U16 wSize );
SENSING_PROCESS_EXT void SenEEP_Write_Ctg( U16 *pData, U8 Block, U16 wAddr, U16 wSize );
SENSING_PROCESS_EXT void SenEEP_Write_Cvt( U16 *pData, U8 Block, U16 wAddr, U16 wSize );
/* End Function */

/* Start Variable */
SENSING_PROCESS_EXT SensingProcess_HandleType		SensingProcess;
/* End Variable */


#endif // __SENSING_PROCESS_H__
