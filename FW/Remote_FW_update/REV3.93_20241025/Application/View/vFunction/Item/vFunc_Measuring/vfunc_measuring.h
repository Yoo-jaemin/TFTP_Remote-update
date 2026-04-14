#ifndef   __VFUNC_MEASURING_H__
#define   __VFUNC_MEASURING_H__


#ifdef __VFUNC_MEASURING_C__
	#define VFUNC_MEASURING_EXT
#else
	#define VFUNC_MEASURING_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "function_mode.h"
#include "func_measuring.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VFUNC_MEASURING_EXT void vFunc_Measuring( Func_HandleType *pFunc, Measuring_SetType *pSet );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VFUNC_MEASURING_H__
