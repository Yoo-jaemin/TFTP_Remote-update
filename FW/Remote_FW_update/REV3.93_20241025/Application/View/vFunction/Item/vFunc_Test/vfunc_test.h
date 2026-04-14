#ifndef   __VFUNC_TEST_H__
#define   __VFUNC_TEST_H__


#ifdef __VFUNC_TEST_C__
	#define VFUNC_TEST_EXT
#else
	#define VFUNC_TEST_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "function_mode.h"
#include "func_test.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VFUNC_TEST_EXT void vFunc_Test( Func_HandleType *pFunc, Test_SetType *pSet );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VFUNC_TEST_H__
