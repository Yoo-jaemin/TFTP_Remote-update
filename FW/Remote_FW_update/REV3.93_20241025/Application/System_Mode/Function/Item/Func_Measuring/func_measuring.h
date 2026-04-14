#ifndef   __FUNC_MEASURING_H__
#define   __FUNC_MEASURING_H__


#ifdef __FUNC_MEASURING_C__
	#define FUNC_MEASURING_EXT
#else
	#define FUNC_MEASURING_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "function_mode.h"
#include "measuring.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	Measuring_HandleType	*pMeas;
	U8						sCh;
	Measuring_GasInfo		sGas[MEASURING_GAS_OUTPUT_MAX];
	Measuring_AlarmInfo		sAlr[MEASURING_GAS_OUTPUT_MAX];
	char					GasName[12];
	bool					IsSelected[MEASURING_GAS_OUTPUT_MAX];
	bool					IsConfirmMode;
}Measuring_SetType;
/* End Struct */

/* Start Function */
FUNC_MEASURING_EXT void Func_Measuring( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __FUNC_MEASURING_H__
