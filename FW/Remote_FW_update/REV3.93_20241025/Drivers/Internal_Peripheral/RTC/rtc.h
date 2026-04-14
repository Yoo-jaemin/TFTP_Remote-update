#ifndef   __RTC_H__
#define   __RTC_H__


#ifdef __RTC_C__
	#define RTC_EXT
#else
	#define RTC_EXT extern
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
	RTC_DateTypeDef			Date;
	RTC_TimeTypeDef			Time12;
	RTC_TimeTypeDef			Time24;
	bool					SecondEvt;
	bool					MinuteEvt;
}RTC_SystemCalendarType;
/* End Struct */

/* Start Function */
RTC_EXT bool RTC_Init( void );
RTC_EXT void RTC_Get_Calendar( void );
bool RTC_Set_Calendar( RTC_DateTypeDef *pDate, RTC_TimeTypeDef *pTime );
/* End Function */

/* Start Variable */
RTC_EXT RTC_HandleTypeDef			RtcHandle;
RTC_EXT	RTC_SystemCalendarType		SysCalendar;
/* End Variable */


#endif // __RTC_H__
