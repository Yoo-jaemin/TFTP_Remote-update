#ifndef   __WDT_H__
#define   __WDT_H__


#ifdef __WDT_C__
	#define WDT_EXT
#else
	#define WDT_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
WDT_EXT void WDT_Init( void );
WDT_EXT void WDT_Refresh( void );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __WDT_H__
