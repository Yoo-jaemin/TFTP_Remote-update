#ifndef   __HS300x_H__
#define   __HS300x_H__


#ifdef __HS300x_C__
	#define HS300x_EXT
#else
	#define HS300x_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define HS300x_Humi_Calculation( Humi )		( (float)Humi / 16383.f * 100.f )
#define HS300x_Temp_Calculation( Temp )		( (float)Temp / 16383.f * 165.f - 40.f )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	U8					RxBuf[4];
	uuword				Humi;
	uuword				Temp;
	bool				CmdMR;
	bool				CmdDF;
}HS300x_HandleType;
/* End Struct */

/* Start Function */
HS300x_EXT void HS300x_Task( void );
/* End Function */

/* Start Variable */
HS300x_EXT HS300x_HandleType	HS300x;
/* End Variable */


#endif // __HS300x_H__
