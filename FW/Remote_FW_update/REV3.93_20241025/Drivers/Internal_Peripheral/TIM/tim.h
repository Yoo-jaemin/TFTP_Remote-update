#ifndef   __TIM_H__
#define   __TIM_H__


#ifdef __TIM_C__
	#define TIM_EXT
#else
	#define TIM_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define APB1_TIMER_CLOCKS	( 84000000.f )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
TIM_EXT bool TIM_Init( void );
TIM_EXT void HAL_TIM_MspPostInit( TIM_HandleTypeDef* pTim );
/* End Function */

/* Start Variable */ 
/* End Variable */


#endif // __TIM_H__
