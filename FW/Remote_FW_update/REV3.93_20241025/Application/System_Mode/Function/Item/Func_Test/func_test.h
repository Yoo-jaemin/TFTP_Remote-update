#ifndef   __FUNC_TEST_H__
#define   __FUNC_TEST_H__


#ifdef __FUNC_TEST_C__
	#define FUNC_TEST_EXT
#else
	#define FUNC_TEST_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "function_mode.h"
#include "measuring.h"
#include "relay.h"
#include "led.h"
#include "ma_output.h"
#include "flow_control.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	Measuring_HandleType		*pMeas;
	mA_HandleType				*pmA[MEASURING_GAS_OUTPUT_MAX];
	Relay_DriveType				*pRelay;
	LED_DriveType				*pLed;
	Flow_ControlType			*pFlow;
	bool						With_mA;
	bool						With_Relay;
	bool						With_Buz;
	bool						With_Led;
	bool						With_Pump;
	bool						Trouble[MEASURING_GAS_OUTPUT_MAX];
	uint16_t					TestColor;
}Test_SetType;
/* End Struct */

/* Start Function */
FUNC_TEST_EXT void Func_Test( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __FUNC_TEST_H__


