#ifndef   __VALLBASE_MODE_H__
#define   __VALLBASE_MODE_H__


#ifdef __VALLBASE_MODE_C__
	#define VALLBASE_MODE_EXT
#else
	#define VALLBASE_MODE_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "allbase_mode.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VALLBASE_MODE_EXT void vAllBase_Mode( AllBase_HandleType *pHandle );
VALLBASE_MODE_EXT void vAllBase_Clr( void );
/* End Function */

/* Start Variable */                                   
/* End Variable */


#endif // __VALLBASE_MODE_H__
