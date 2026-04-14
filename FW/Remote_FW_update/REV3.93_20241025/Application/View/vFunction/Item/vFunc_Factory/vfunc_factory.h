#ifndef   __VFUNC_FACTORY_H__
#define   __VFUNC_FACTORY_H__


#ifdef __VFUNC_FACTORY_C__
	#define VFUNC_FACTORY_EXT
#else
	#define VFUNC_FACTORY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "func_factory.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VFUNC_FACTORY_EXT void vFunc_Factory( Func_HandleType *pFunc, Factory_SetType *pSet );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VFUNC_FACTORY_H__
