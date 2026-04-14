#ifndef   __LED_H__
#define   __LED_H__


#ifdef __LED_C__
	#define LED_EXT
#else
	#define LED_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Enum */
/* End Enum */

/* Start Define */
/* End Define */

/* Start Struct */
/* End Struct */

/* Start Function */
LED_EXT void BSP_LED_Init( void );
LED_EXT void LED_Task( void );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __LED_H__
