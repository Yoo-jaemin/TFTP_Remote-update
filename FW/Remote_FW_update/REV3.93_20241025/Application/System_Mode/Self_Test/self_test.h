#ifndef   __SELF_TEST_H__
#define   __SELF_TEST_H__


#ifdef __SELF_TEST_C__
	#define SELF_TEST_EXT
#else
	#define SELF_TEST_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	SELF_TEST_STATE_PREPARING,
	SELF_TEST_STATE_SENSOR_CHECK,
	SELF_TEST_STATE_IR_SRC_CHECK,
	SELF_TEST_STATE_FLOW_CHECK,
	SELF_TEST_STATE_WARM_UP,
	SELF_TEST_STATE_SUCCESS,
	SELF_TEST_STATE_PREPARING_FAILURE,
	SELF_TEST_STATE_SENSOR_FAILURE,
	SELF_TEST_STATE_IR_SRC_FAILURE,
	SELF_TEST_STATE_FLOW_FAILURE
}SelfTest_State;

typedef union {
    U32	Word;
    struct{
		bool	CtgRawDvSum_LPTIA_LPF_0	: 1; // 0x00000001
		bool	CtgRawDvSum_LPTIA_LPF_1	: 1; // 0x00000002
		bool	CtgRawDvSum_VZERO_0		: 1; // 0x00000004
		bool	CtgRawDvSum_VZERO_1		: 1; // 0x00000008
		bool	CtgRawDvSum_VBIAS_0		: 1; // 0x00000010
		bool	CtgRawDvSum_VBIAS_1		: 1; // 0x00000020
		bool	nDet_0					: 1; // 0x00000040
		bool	nDet_1					: 1; // 0x00000080
		bool	CvtOcAvrg_0				: 1; // 0x00000100
		bool	CvtOcAvrg_1				: 1; // 0x00000200
		bool	CvtOcAvrg_2				: 1; // 0x00000400
		bool	CvtOcAvrg_3				: 1; // 0x00000800
		bool	CvtHiTime_0				: 1; // 0x00001000
		bool	CvtHiTime_1				: 1; // 0x00002000
		bool	CvtHiTime_2				: 1; // 0x00004000
		bool	CvtHiTime_3				: 1; // 0x00008000
		bool	CvtLoTime_0				: 1; // 0x00010000
		bool	CvtLoTime_1				: 1; // 0x00020000
		bool	CvtLoTime_2				: 1; // 0x00040000
		bool	CvtLoTime_3				: 1; // 0x00080000
		bool	CvtRawDvMaxSum_0		: 1; // 0x00100000
		bool	CvtRawDvMaxSum_1		: 1; // 0x00200000
		bool	CvtRawDvMaxSum_2		: 1; // 0x00400000
		bool	CvtRawDvMaxSum_3		: 1; // 0x00800000
		bool	nDet_2					: 1; // 0x01000000
		bool	nDet_3					: 1; // 0x02000000
		bool	nDet_4					: 1; // 0x04000000
		bool	nDet_5					: 1; // 0x08000000
		U8		Reserved				: 4; // 0xF0000000
    }Bit;
}SelfTest_Item;
/* End Enum */

/* Start Struct */
typedef struct {
	SelfTest_State				State;
	SelfTest_Item				Item;
	bool						TaskEvt;
	U16							Timeout;
	U16							WarmUpSec;
	bool						IsWarmUpZeroEnabled;
}SelfTest_HandleType;
/* End Struct */

/* Start Function */
SELF_TEST_EXT void Enter_Self_Test_Mode( void );
SELF_TEST_EXT void Self_Test_Task( void );
/* End Function */

/* Start Variable */
SELF_TEST_EXT SelfTest_HandleType	SelfHandle;
/* End Variable */


#endif // __SELF_TEST_H__
