#define __TIM3_C__
    #include "tim3.h"
#undef  __TIM3_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "tim.h"
/* End Include */

/* Start Define */        
#define TIM3_FREQUENCY				( 600000.f )
#define TIM3_PRESCALER_VALUE		( (uint32_t)( APB1_TIMER_CLOCKS / TIM3_FREQUENCY ) - 1 )
#define TIM3_PWM_FREQ				( 60.f )
#define TIM3_PERIOD_VALUE			( (uint32_t)( TIM3_FREQUENCY / (TIM3_PWM_FREQ * 2) ) )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */
                 
bool TIM3_Init( TIM_HandleTypeDef *pTim )
{
	bool	err = false;
	
	TIM_ClockConfigTypeDef				sClockSourceConfig;
	TIM_MasterConfigTypeDef				sMasterConfig;
	TIM_OC_InitTypeDef					sConfigOC;
	
	pTim->Instance						= TIM3;
	pTim->Init.Prescaler				= TIM3_PRESCALER_VALUE;
	pTim->Init.CounterMode				= TIM_COUNTERMODE_CENTERALIGNED1;
	pTim->Init.Period					= TIM3_PERIOD_VALUE;
	pTim->Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	pTim->Init.RepetitionCounter		= 0;
	pTim->Init.AutoReloadPreload		= TIM_AUTORELOAD_PRELOAD_ENABLE;
	
	if( HAL_TIM_Base_Init( pTim ) != HAL_OK ){ err = true; }
	
	sClockSourceConfig.ClockSource		= TIM_CLOCKSOURCE_INTERNAL;
	if( HAL_TIM_ConfigClockSource( pTim, &sClockSourceConfig ) != HAL_OK ){ err = true; }
	
	if( HAL_TIM_PWM_Init( pTim ) != HAL_OK ){ err = true; }
	
	sMasterConfig.MasterOutputTrigger	= TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode		= TIM_MASTERSLAVEMODE_DISABLE;
	if( HAL_TIMEx_MasterConfigSynchronization( pTim, &sMasterConfig ) != HAL_OK ){ err = true; }
	
	sConfigOC.OCMode					= TIM_OCMODE_PWM1;
	sConfigOC.Pulse						= 0;
	sConfigOC.OCPolarity				= TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode				= TIM_OCFAST_DISABLE;
	if( HAL_TIM_PWM_ConfigChannel( pTim, &sConfigOC, TIM_CHANNEL_2 ) != HAL_OK ){ err = true; }
	
	sConfigOC.Pulse						= TIM3_PERIOD_VALUE;
	sConfigOC.OCPolarity				= TIM_OCPOLARITY_HIGH;
	if( HAL_TIM_PWM_ConfigChannel( pTim, &sConfigOC, TIM_CHANNEL_3 ) != HAL_OK ){ err = true; }
	
	HAL_TIM_MspPostInit( pTim );
	
	return err;
}    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM3_Freq_Set( float Freq, float Duty )
{                             
	float	Period = (TIM3_FREQUENCY / (Freq * 2));
	
    TIM3_Handle.Instance->ARR  = (uint32_t)(Period - 1.f);
	TIM3_Handle.Instance->CCR2 = (uint32_t)(Period * Duty * 0.01f);
	TIM3_Handle.Instance->CCR3 = (uint32_t)(Period - TIM3_Handle.Instance->CCR2);
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM3_Duty_Set( float Duty )
{
	float	Period = TIM3_Handle.Instance->ARR + 1.f;
	
	TIM3_Handle.Instance->CCR2 = (uint32_t)(Period * Duty * 0.01f);
	TIM3_Handle.Instance->CCR3 = (uint32_t)(Period - TIM3_Handle.Instance->CCR2);
}


