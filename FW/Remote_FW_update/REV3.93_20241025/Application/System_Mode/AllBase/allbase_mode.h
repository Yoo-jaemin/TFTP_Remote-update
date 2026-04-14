#ifndef   __ALLBASE_MODE_H__
#define   __ALLBASE_MODE_H__


#ifdef __ALLBASE_MODE_C__
	#define ALLBASE_MODE_EXT
#else
	#define ALLBASE_MODE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "vlibrary.h"
#include "measuring.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	ALLBASE_STATE_READY,
	ALLBASE_STATE_PROGRESS,
	ALLBASE_STATE_COMPLETE
}AllBase_State;
/* End Enum */

/* Start Struct */
typedef struct {
	Measuring_HandleType	*pMeas;
	vMessageBoxType			*pMsgBox;
	AllBase_State			State;
	bool					TaskEvt;
	U16						QuitTmr;
	U8						ProgressTmr;
	U8						ViewTmr;
	bool					ViewEvt;
	bool					IsApproved;
}AllBase_HandleType;
/* End Struct */

/* Start Function */
ALLBASE_MODE_EXT void Enter_AllBase_Mode( void );
ALLBASE_MODE_EXT void AllBase_Mode_Task( void );
/* End Function */

/* Start Variable */
ALLBASE_MODE_EXT AllBase_HandleType		AllBaseHandle;
/* End Variable */


#endif // __ALLBASE_MODE_H__
