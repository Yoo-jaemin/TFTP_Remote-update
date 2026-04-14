#ifndef   __DATA_LOG_H__
#define   __DATA_LOG_H__


#ifdef __DATA_LOG_C__
	#define DATA_LOG_EXT
#else
	#define DATA_LOG_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "measuring.h"
#include "rtc.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	DATA_LOG_MODE_OFF,
	DATA_LOG_MODE_DEBUG_PRINT,
	DATA_LOG_MODE_MAX
}DataLog_ModeType;
/* End Enum */

/* Start Struct */
typedef struct {
	DataLog_ModeType				Mode;
	U16								Period;
	U8								Period_Cnt;
	RTC_SystemCalendarType			*pCalendar;
}DataLog_HandleType;
/* End Struct */

/* Start Function */
DATA_LOG_EXT void Data_Logger_Config( void );
DATA_LOG_EXT void Data_Logger( Measuring_HandleType *pMeas );
/* End Function */

/* Start Variable */
DATA_LOG_EXT DataLog_HandleType		LogHandle;
/* End Variable */


#endif // __DATA_LOG_H__
