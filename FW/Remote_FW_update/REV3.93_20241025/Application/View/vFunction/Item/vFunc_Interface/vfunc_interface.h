#ifndef   __VFUNC_INTERFACE_H__
#define   __VFUNC_INTERFACE_H__


#ifdef __VFUNC_INTERFACE_C__
	#define VFUNC_INTERFACE_EXT
#else
	#define VFUNC_INTERFACE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "function_mode.h"
#include "func_interface.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VFUNC_INTERFACE_EXT void vFunc_Poe( Func_HandleType *pFunc, Interface_SetType *pSet );
/* End Function */

/* Start Variable */
VFUNC_INTERFACE_EXT bool	vInterface_BackUpdate;
/* End Variable */


#endif // __VFUNC_INTERFACE_H__
