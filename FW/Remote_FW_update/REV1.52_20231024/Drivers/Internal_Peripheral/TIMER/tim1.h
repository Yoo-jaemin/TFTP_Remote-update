#ifndef   __TIM1_H__
#define   __TIM1_H__


#ifdef __TIM1_C__
	#define TIM1_EXT
#else
	#define TIM1_EXT extern
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
TIM1_EXT void TIM1_Init(void);
/* End Function */

/* Start Variable */ 
TIM1_EXT TIM_HandleTypeDef	TIM1_Handle;
/* End Variable */


#endif // __TIM1_H__
