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

/* Start Define */
#define LED_Set()		DioSetPin( pADI_GPIO1, PIN1 )
#define LED_Clr()		DioClrPin( pADI_GPIO1, PIN1 )
#define LED_Tog()		DioTglPin( pADI_GPIO1, PIN1 )
/* End Define */

/* Start Enum */
enum {
    AL1 = 0,
    AL2,
    TRB,
	ALL
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
LED_EXT void LED_Init( void );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __LED_H__
