#ifndef   __GPT_H__
#define   __GPT_H__


#ifdef __GPT_C__
	#define GPT_EXT
#else
	#define GPT_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct { 
	U16					_01_MSEC;
	U16					_02_MSEC_1;
	U16					_02_MSEC_2;
	U16					_04_MSEC;
	U16					_10_MSEC;
	U16					_01_SEC;
}TaskSyncTimer_CntType;

typedef union {
    U16  Word;
    struct{
        bool			_01_MSEC	: 1;
        bool			_04_MSEC	: 1;
        bool			_08_MSEC_1	: 1;
        bool			_08_MSEC_2	: 1;
        bool			_40_MSEC_1	: 1;
        bool			_40_MSEC_2	: 1;
        bool			_40_MSEC_3	: 1;
        bool			_01_SEC		: 1;
        U8				Reserved	: 8;
    }Bit;
}TaskSyncTimer_EvtType;

typedef struct {
	TaskSyncTimer_CntType		Cnt; 
	TaskSyncTimer_EvtType		Evt;
}TaskSyncTimer_HandleType;
/* End Struct */

/* Start Function */
GPT_EXT void GPT_Init( void );
/* End Function */

/* Start Variable */
GPT_EXT TaskSyncTimer_HandleType	TaskSyncTimer;
/* End Variable */


#endif // __GPT_H__
