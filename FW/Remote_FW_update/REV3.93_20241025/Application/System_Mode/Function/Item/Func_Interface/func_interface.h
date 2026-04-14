#ifndef   __FUNC_INTERFACE_H__
#define   __FUNC_INTERFACE_H__


#ifdef __FUNC_INTERFACE_C__
	#define FUNC_INTERFACE_EXT
#else
	#define FUNC_INTERFACE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "function_mode.h"
#include "poe.h"
#include "relay.h"
#include "rs485.h"
#include "ma_output.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	Measuring_HandleType	*pMeas;
	POE_ConfigType			*pPoe;
	POE_ConfigType			sPoe;
	RS485_ConfigType		*p485;
	RS485_ConfigType		s485;
	mA_HandleType			*pmA[MEASURING_GAS_OUTPUT_MAX];
	mA_HandleType			smA[MEASURING_GAS_OUTPUT_MAX];
	Relay_DriveType			*pRly;
	Relay_DriveType			sRly;
}Interface_SetType;
/* End Struct */

/* Start Function */
FUNC_INTERFACE_EXT void Func_Interface( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __FUNC_INTERFACE_H__
