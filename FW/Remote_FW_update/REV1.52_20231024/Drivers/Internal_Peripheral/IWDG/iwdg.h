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
IWDG_EXT void MX_IWDG_Init(void);
/* End Function */

/* Start Variable */
IWDG_EXT IWDG_HandleTypeDef IWDG_Handle;
/* End Variable */


#endif // __IWDG_H__
