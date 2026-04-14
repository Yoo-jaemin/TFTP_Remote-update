#ifndef   __FUNCTION_MODE_H__
#define   __FUNCTION_MODE_H__


#ifdef __FUNCTION_MODE_C__
	#define FUNCTION_MODE_EXT
#else
	#define FUNCTION_MODE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "key.h"
#include "vlibrary.h"
#include "rtc.h"
#include "measuring.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {               
	FUNC_LEVEL_1,
	FUNC_LEVEL_2,
	FUNC_LEVEL_3,
	FUNC_LEVEL_4,
	FUNC_LEVEL_5,
	FUNC_LEVEL_MAX
}Function_LevelType;

enum {
	FUNC_ITEM_LV1_MEASURING,
	FUNC_ITEM_LV1_FLOW_SET,
	FUNC_ITEM_LV1_INTERFACE,
	FUNC_ITEM_LV1_SYSTEM,
	FUNC_ITEM_LV1_TEST,
	FUNC_ITEM_LV1_FACTORY,
	FUNC_ITEM_LV1_MAX
};
/* End Enum */

/* Start Struct */
typedef union {
    U8	Byte;
    struct{
        bool	IsEntered	: 1; // 1
        bool	Flow		: 1; // 2
        bool	Test		: 1; // 3
        bool	Calibration	: 1; // 4
        U8		Reserved	: 4; // 5~8
    }Bit;
}Func_State;

typedef struct {
	U8 Lv1;
	U8 Lv2;
	U8 Lv3;
	U8 Lv4;
	U8 Lv5;
}Func_ItemType;

typedef struct {
	U16						QuitSec;
	U8						QuitTmr;
	U8						ViewTmr;
	bool					ViewEvt;
	bool					TaskEvt;
	Function_LevelType		Level;
	Func_ItemType			Item;
	KEY_DataType			KeyData;
	KEY_DetectionType		KeyType;
	bool					IsApproved;
	vMessageBoxType			*pMsgBox;
	bool					ViewBlk;
	Func_State				State;
	float					mA[MEASURING_GAS_OUTPUT_MAX];
}Func_HandleType;
/* End Struct */

/* Start Function */
FUNCTION_MODE_EXT void Enter_Function_Mode( void );
FUNCTION_MODE_EXT void Function_Mode_Task( void );
FUNCTION_MODE_EXT void Function_Mode_Timer( void );
FUNCTION_MODE_EXT void MoveFuncTop( void );
FUNCTION_MODE_EXT void Func_fSum( float lmt, float inc, float *pf );
FUNCTION_MODE_EXT void Func_iSum( S16 lmt, S16 inc, S16 *pf );
FUNCTION_MODE_EXT void Func_uInc( U16 lmt, U16 inc, U16 *pw );
FUNCTION_MODE_EXT void Func_uDec( U16 lmt, U16 dec, U16 *pw );
/* End Function */

/* Start Variable */
FUNCTION_MODE_EXT Func_HandleType	FuncHandle;
/* End Variable */


#endif // __FUNCTION_MODE_H__


