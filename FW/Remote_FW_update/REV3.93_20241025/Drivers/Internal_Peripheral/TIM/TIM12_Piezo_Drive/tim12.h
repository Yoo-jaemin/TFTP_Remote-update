#ifndef   __TIM12_H__
#define   __TIM12_H__


#ifdef __TIM12_C__
	#define TIM12_EXT
#else
	#define TIM12_EXT extern
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
TIM12_EXT bool TIM12_Init( TIM_HandleTypeDef *pTim );
TIM12_EXT void TIM12_Freq_Set( float Freq, float Duty );
TIM12_EXT void TIM12_Duty_Set( float Duty );
/* End Function */

/* Start Variable */ 
TIM12_EXT TIM_HandleTypeDef	TIM12_Handle;
/* End Variable */


#endif // __TIM12_H__
