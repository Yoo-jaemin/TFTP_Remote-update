#define __TIM4_C__
    #include "tim4.h"
#undef  __TIM4_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "tim.h"
/* End Include */

/* Start Define */
#define TIM4_FREQUENCY				( 7000000.f )
#define TIM4_PRESCALER_VALUE		( (uint32_t)( APB1_TIMER_CLOCKS / TIM4_FREQUENCY ) - 1 )
#define TIM4_PWM_FREQ				( 7000.f )
#define TIM4_PERIOD_VALUE			( (uint32_t)( TIM4_FREQUENCY / TIM4_PWM_FREQ - 1 ) )
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
	
	TIM_ClockConfigTypeDef				sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef				sMasterConfig = {0};
	TIM_OC_InitTypeDef					sConfigOC = {0};
	
	pTim->Instance						= TIM4;
	pTim->Init.Prescaler				= TIM4_PRESCALER_VALUE;
	pTim->Init.CounterMode				= TIM_COUNTERMODE_UP;
	pTim->Init.Period					= TIM4_PERIOD_VALUE;
	pTim->Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	pTim->Init.AutoReloadPreload		= TIM_AUTORELOAD_PRELOAD_ENABLE;
	
	if( HAL_TIM_Base_Init( pTim ) != HAL_OK ){ err = true; }
	
	sClockSourceConfig.ClockSource		= TIM_CLOCKSOURCE_INTERNAL;
	if( HAL_TIM_ConfigClockSource( pTim, &sClockSourceConfig ) != HAL_OK ){ err = true; }
	
	if( HAL_TIM_PWM_Init( pTim ) != HAL_OK ){ err = true; }
	
	sMasterConfig.MasterOutputTrigger	= TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode		= TIM_MASTERSLAVEMODE_DISABLE;
	if ( HAL_TIMEx_MasterConfigSynchronization( pTim, &sMasterConfig ) != HAL_OK ){ err = true; }
	
	sConfigOC.OCMode					= TIM_OCMODE_PWM1;
	sConfigOC.Pulse						= 0;
	sConfigOC.OCPolarity				= TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode				= TIM_OCFAST_ENABLE;
	if( HAL_TIM_PWM_ConfigChannel( pTim, &sConfigOC, TIM_CHANNEL_3 ) != HAL_OK ){ err = true; }
	
	HAL_TIM_MspPostInit( pTim );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM4_Freq_Set( float Freq, float Duty )
{
	float	Period = TIM4_FREQUENCY / Freq;
	
    TIM4_Handle.Instance->ARR	= (uint32_t)(Period - 1.f);
    TIM4_Handle.Instance->CCR3	= (uint32_t)(Period * Duty * 0.01f);
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM4_Duty_Set( float Duty )
{
	uint32_t	Res = ( (uint32_t)( ((float)TIM4_Handle.Instance->ARR + 1.f) * Duty * 0.01f) );
	
    TIM4_Handle.Instance->CCR3 = Res;
}


