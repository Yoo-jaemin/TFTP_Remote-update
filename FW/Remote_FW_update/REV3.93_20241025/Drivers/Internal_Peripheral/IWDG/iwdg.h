#ifndef   __IWDG_H__
#define   __IWDG_H__


#ifdef __IWDG_C__
	#define IWDG_EXT
#else
	#define IWDG_EXT extern
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
IWDG_EXT bool IWDG_Init( void );
IWDG_EXT void IWDG_Task( void );
IWDG_EXT void IWDG_Reload_Counter_Set( U8 sec );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __IWDG_H__
