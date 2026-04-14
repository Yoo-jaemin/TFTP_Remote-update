#define __TIM4_C__
    #include "tim4.h"
#undef  __TIM4_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "tim.h"
/* End Include */

/* Start Define */ 
#define TIM4_FREQUENCY_100KHz		( 100000.f * 2 )
#define TIM4_FREQUENCY				( TIM4_FREQUENCY_100KHz )
#define TIM4_PRESCALER_VALUE		( (uint32_t)( SystemCoreClock / TIM4_FREQUENCY ) - 1 )
#define TIM4_PERIOD_VALUE			( (uint32_t)( 10.f - 1 ) )       
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */
                 
bool TIM4_Init( TIM_HandleTypeDef *pTim )
{
	bool	err = false;
	
	TIM_ClockConfigTypeDef				sClockSourceConfig;
	TIM_MasterConfigTypeDef				sMasterConfig;
	TIM_OC_InitTypeDef					sConfigOC;
	
	pTim->Instance						= TIM4;
	pTim->Init.Prescaler				= TIM4_PRESCALER_VALUE;
	pTim->Init.CounterMode				= TIM_COUNTERMODE_UP;
	pTim->Init.Period					= TIM4_PERIOD_VALUE;
	pTim->Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	pTim->Init.RepetitionCounter		= 0;
	
	if( HAL_TIM_Base_Init( pTim ) != HAL_OK ){ err = true; }
	
	sClockSourceConfig.ClockSource		= TIM_CLOCKSOURCE_INTERNAL;
	if( HAL_TIM_ConfigClockSource( pTim, &sClockSourceConfig ) != HAL_OK ){ err = true; }
	
	if( HAL_TIM_PWM_Init( pTim ) != HAL_OK ){ err = true; }
	
	sMasterConfig.MasterOutputTrigger	= TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode		= TIM_MASTERSLAVEMODE_DISABLE;
	if( HAL_TIMEx_MasterConfigSynchronization( pTim, &sMasterConfig ) != HAL_OK ){ err = true; }
	
	sConfigOC.OCMode					= TIM_OCMODE_PWM1;
	sConfigOC.Pulse						= TIM4_PERIOD_VALUE / 2;
	sConfigOC.OCPolarity				= TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode				= TIM_OCFAST_DISABLE;
	if( HAL_TIM_PWM_ConfigChannel( pTim, &sConfigOC, TIM_CHANNEL_3 ) != HAL_OK ){ err = true; }
	
	HAL_TIM_MspPostInit( pTim );
	
	HAL_TIM_PWM_Start( pTim, TIM_CHANNEL_3 );
	
	return err;
}    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM4_Freq_Set( float Freq )
{                             
    TIM4_Handle.Instance->ARR = ( (uint32_t)(100000.f / Freq - 1.f) );
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM4_Duty_Set( float Duty )
{
	uint32_t	Res = ( (uint32_t)( (float)TIM4_Handle.Instance->ARR * Duty * 0.01f - 1.f ) );
	
    TIM4_Handle.Instance->CCR3 = Res;
}



