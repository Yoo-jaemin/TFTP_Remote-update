#ifndef   __TIM4_H__
#define   __TIM4_H__


#ifdef __TIM4_C__
	#define TIM4_EXT
#else
	#define TIM4_EXT extern
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
TIM4_EXT bool TIM4_Init( TIM_HandleTypeDef *pTim );
TIM4_EXT void TIM4_Freq_Set( float Freq, float Duty );
TIM4_EXT void TIM4_Duty_Set( float Duty );
/* End Function */

/* Start Variable */ 
TIM4_EXT TIM_HandleTypeDef	TIM4_Handle;
/* End Variable */


#endif // __TIM12_H__
