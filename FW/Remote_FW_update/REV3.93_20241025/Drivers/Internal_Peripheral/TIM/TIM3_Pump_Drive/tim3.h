#ifndef   __TIM3_H__
#define   __TIM3_H__


#ifdef __TIM3_C__
	#define TIM3_EXT
#else
	#define TIM3_EXT extern
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
TIM3_EXT bool TIM3_Init( TIM_HandleTypeDef *pTim );
TIM3_EXT void TIM3_Freq_Set( float Freq, float Duty );
TIM3_EXT void TIM3_Duty_Set( float Duty );
/* End Function */

/* Start Variable */ 
TIM3_EXT TIM_HandleTypeDef	TIM3_Handle;
/* End Variable */


#endif // __TIM3_H__
