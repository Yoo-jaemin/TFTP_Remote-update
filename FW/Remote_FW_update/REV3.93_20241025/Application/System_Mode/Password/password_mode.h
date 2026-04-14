#ifndef   __PASSWORD_MODE_H__
#define   __PASSWORD_MODE_H__


#ifdef __PASSWORD_MODE_C__
	#define PASSWORD_MODE_EXT
#else
	#define PASSWORD_MODE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "vlibrary.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	bool				IsHideMode;
	bool				IsMisMatch;
	U16					SysPW;
	U16					SetPW;
	U16					HideTmr;
	U16					QuitTmr;
	System_StateType	PrvState;
	vMessageBoxType		*pMsgBox;
}Password_HandleType;
/* End Struct */

/* Start Function */
PASSWORD_MODE_EXT void Enter_Password_Mode( void );
PASSWORD_MODE_EXT void Password_Mode_Task( void );
PASSWORD_MODE_EXT void Password_Mode_Timer( void );
/* End Function */

/* Start Variable */
PASSWORD_MODE_EXT Password_HandleType		PwHandle;
/* End Variable */


#endif // __PASSWORD_MODE_H__
