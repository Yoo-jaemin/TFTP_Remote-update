#ifndef   __FUNC_SYSTEM_H__
#define   __FUNC_SYSTEM_H__


#ifdef __FUNC_SYSTEM_C__
	#define FUNC_SYSTEM_EXT
#else
	#define FUNC_SYSTEM_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "function_mode.h"
#include "buzzer.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	RTC_DateTypeDef			*pDate;
	RTC_TimeTypeDef			*pTime;
	RTC_DateTypeDef			sDate;
	RTC_TimeTypeDef			sTime;
	U16						*pPW;
	U16						sPW;
	U16						*pBrightness;
	U16						sBrightness;
	U16						*pBuzVolume;
	U16						sBuzVolume;
}System_SetType;
/* End Struct */

/* Start Function */
FUNC_SYSTEM_EXT void Func_System( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __FUNC_SYSTEM_H__
