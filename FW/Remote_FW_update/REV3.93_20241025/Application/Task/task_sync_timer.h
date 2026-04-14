#ifndef   __TASK_SYNC_TIMER_H__
#define   __TASK_SYNC_TIMER_H__


#ifdef __TASK_SYNC_TIMER_C__
	#define TASK_SYNC_TIMER_EXT
#else
	#define TASK_SYNC_TIMER_EXT extern
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
TASK_SYNC_TIMER_EXT void TaskSyncTimer_Init( void );
TASK_SYNC_TIMER_EXT void TaskSyncTimer_Handler( void );
/* End Function */

/* Start Variable */
TASK_SYNC_TIMER_EXT TaskSyncTimer_HandleType		TaskSyncTimer;
/* End Variable */


#endif // __TASK_SYNC_TIMER_H__
