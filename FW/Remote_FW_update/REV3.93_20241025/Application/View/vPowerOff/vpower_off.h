#ifndef   __VPOWER_OFF_H__
#define   __VPOWER_OFF_H__


#ifdef __VPOWER_OFF_C__
	#define VPOWER_OFF_EXT
#else
	#define VPOWER_OFF_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "power_control.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VPOWER_OFF_EXT void vPowerOff( Power_CtrlType *pCtrl );
VPOWER_OFF_EXT void vPowerOff_Low_Battery( void );
VPOWER_OFF_EXT void vPowerClr( void );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __VPOWER_OFF_H__
