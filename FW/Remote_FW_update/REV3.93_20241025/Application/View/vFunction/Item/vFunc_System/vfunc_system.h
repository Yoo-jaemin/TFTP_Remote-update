#ifndef   __VFUNC_SYSTEM_H__
#define   __VFUNC_SYSTEM_H__


#ifdef __VFUNC_SYSTEM_C__
	#define VFUNC_SYSTEM_EXT
#else
	#define VFUNC_SYSTEM_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "function_mode.h"
#include "func_system.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VFUNC_SYSTEM_EXT void vFunc_System( Func_HandleType *pFunc, System_SetType *pSet );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VFUNC_SYSTEM_H__
