#ifndef   __PUMP_H__
#define   __PUMP_H__


#ifdef __PUMP_C__
	#define PUMP_EXT
#else
	#define PUMP_EXT extern
#endif 

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
#define PUMP_CTRL_FREQ_MIN			( 60.0f ) // Hz
#define PUMP_CTRL_FREQ_MAX			( 60.0f ) // Hz
#define PUMP_CTRL_DUTY_MIN			(  1.0f ) // %
#define PUMP_CTRL_DUTY_MAX			( 36.0f ) // %
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	bool				IsEnabled;    
	float				SetFreq;
	float				OutFreq;    
	float				SetDuty;
	float				OutDuty;
}Pump_DriveType;
/* End Struct */

/* Start Function */
PUMP_EXT bool Pump_Init( void );
PUMP_EXT void Pump_On( void );
PUMP_EXT void Pump_Off( void );
PUMP_EXT void Pump_Drive( float Ctrl );
/* End Function */

/* Start Variable */
PUMP_EXT Pump_DriveType			PumpDrive; 
/* End Variable */


#endif // __PUMP_H__


