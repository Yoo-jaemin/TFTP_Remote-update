#ifndef   __CAVITY_H__
#define   __CAVITY_H__


#ifdef __CAVITY_C__
	#define CAVITY_EXT
#else
	#define CAVITY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "sensor_eep_map.h"
#include "modbus_rtu_master.h"
/* End Include */

/* Start Define */
#define CAVITY_READ_REGISTER_SIZE		( 128 )
#define CAVITY_MODBUS_ADDRESS			( 1 )
#define CAVITY_SENSOR_OFFSET			( 2.5f )
#define CAVITY_BUF_SIZE					( 200 )
/* End Define */

/* Start Enum */
enum {
	CAVITY_SEN_CH_1,
	CAVITY_SEN_CH_2,
	CAVITY_SEN_CH_3,
	CAVITY_SEN_CH_4,
	CAVITY_SEN_CH_MAX
};

enum {
	CAVITY_SEN_TYPE_LMM_274_X025,
	CAVITY_SEN_TYPE_LMM_274_X027,
	CAVITY_SEN_TYPE_LIM_272_CH,
	CAVITY_SEN_TYPE_LIM_272_DH,
	CAVITY_SEN_TYPE_LIM_272_IH,
	CAVITY_SEN_TYPE_LIM_272_X018,
	CAVITY_SEN_TYPE_LIM_272_X021,
	CAVITY_SEN_TYPE_LIM_272_X054,
	CAVITY_SEN_TYPE_LIM_272_X040,
	CAVITY_SEN_TYPE_LIM_272_X044,
	CAVITY_SEN_TYPE_LMM_274_X033,
	CAVITY_SEN_TYPE_LMM_274_X040,
	CAVITY_SEN_TYPE_LMM_274_X042,
	CAVITY_SEN_TYPE_LMM_274_X044,
	CAVITY_SEN_TYPE_LIM_272_X127,
	CAVITY_SEN_TYPE_MAX
};

enum {
	CAVITY_INF_DET_TYPE,
	CAVITY_INF_HW_VER,
	CAVITY_INF_FW_VER,
	CAVITY_INF_RESERVED_1,
	CAVITY_INF_RESERVED_2,
	CAVITY_INF_BUF_SIZE,
	CAVITY_INF_SRC_FREQ,
	CAVITY_INF_SEN_TYPE,
	CAVITY_INF_CHANNELS,
	CAVITY_INF_SIZE
};

enum {
	CAVITY_EEP_BLK_SYS,
	CAVITY_EEP_BLK_CH1,
	CAVITY_EEP_BLK_CH2,
	CAVITY_EEP_BLK_CH3,
	CAVITY_EEP_BLK_CH4,
	CAVITY_EEP_BLK_MAX
};

enum {
	CAVITY_AMP_CAL_CH1 = BIT0,
	CAVITY_AMP_CAL_CH2 = BIT1,
	CAVITY_AMP_CAL_CH3 = BIT2,
	CAVITY_AMP_CAL_CH4 = BIT3,
	CAVITY_AMP_CAL_ALL = ( CAVITY_AMP_CAL_CH1 | CAVITY_AMP_CAL_CH2 | CAVITY_AMP_CAL_CH3 | CAVITY_AMP_CAL_CH4 ),
};
/* End Enum */

/* Start Struct */
typedef union {
    U16  Word;
    struct{
		bool	DebugMode	: 1; // 1
        bool	Reserved_1	: 1; // 2
        bool	Reserved_2	: 1; // 3
        bool	eADC		: 1; // 4
        bool	Reserved_3	: 1; // 5
        bool	Eeprom_CRC	: 1; // 6
        bool	AT24C32		: 1; // 7
        bool	Driver		: 1; // 8
        bool	TPL0102		: 1; // 9
        bool	Init		: 1; // 10
        U8		Reserved_4	: 6;
    }Bit;
}CavityErrorType;

typedef struct {
	bool						IsPresent;
	U16							State;
	CavityErrorType				Error;
	U16							AmpCalRunFlag;
	U16							AmpCalErrFlag;
	U8							AmpCalStatus[CAVITY_SEN_CH_MAX];
	U8							PtmOffs[CAVITY_SEN_CH_MAX];
	U8							PtmGain[CAVITY_SEN_CH_MAX];
	U8							iWaveSet;
	U8							iWaveOut;
	U8							Wave[CAVITY_SEN_CH_MAX][CAVITY_BUF_SIZE];
	U16							ReceiveBuf[CAVITY_READ_REGISTER_SIZE];
	U16							Inf[CAVITY_INF_SIZE];
	U16							Eep[CAVITY_EEP_BLK_MAX][SENSOR_EEP_BLK_SIZE];
	U16							SequenceNo;
}Cavity_HandleType;
/* End Struct */

/* Start Function */
CAVITY_EXT void Cavity_Config( MODBUS_MasterHandleType *pModbus );
CAVITY_EXT void Cavity_Info_Read_Packet_Send( MODBUS_MasterHandleType *pModbus );
CAVITY_EXT void Cavity_Data_Read_Packet_Send( MODBUS_MasterHandleType *pModbus );
CAVITY_EXT void Cavity_Wave_Read_Packet_Send( MODBUS_MasterHandleType *pModbus );
CAVITY_EXT void Cavity_Eep_Read_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block, U8 wAddr, U8 wSize );
CAVITY_EXT void Cavity_Eep_Write_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block, U8 wAddr, U8 wSize );
CAVITY_EXT void Cavity_Eep_Init_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block );
CAVITY_EXT bool Cavity_Packet_Sequence_Check( MODBUS_MasterHandleType *pModbus );
/* End Function */

/* Start Variable */
CAVITY_EXT Cavity_HandleType	CavityHandle;
/* End Variable */


#endif // __CAVITY_H__


