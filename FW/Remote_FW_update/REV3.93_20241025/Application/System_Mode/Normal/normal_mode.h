#ifndef   __NORMAL_MODE_H__
#define   __NORMAL_MODE_H__


#ifdef __NORMAL_MODE_C__
	#define NORMAL_MODE_EXT
#else
	#define NORMAL_MODE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "measuring.h" 
#include "pump.h"
#include "option.h"
#include "relay.h"
#include "led.h"
#include "key.h"
#include "ma_output.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	Measuring_HandleType		*pMeas;
	Pump_DriveType				*pPump;
	System_OptionType			*pOption;
	Relay_DriveType				*pRly;
	LED_DriveType				*pLed;
	mA_HandleType				*pmA[MEASURING_GAS_OUTPUT_MAX];
	bool						TaskEvt;
	bool						KeyLock;
}NormalMode_HandleType;
/* End Struct */

/* Start Function */
NORMAL_MODE_EXT void Enter_Normal_Mode( void );
NORMAL_MODE_EXT void Normal_Mode_Task( void );
/* End Function */

/* Start Variable */
NORMAL_MODE_EXT NormalMode_HandleType	NormHandle;
/* End Variable */


#endif // __NORMAL_MODE_H__
