#define __TIM12_C__
    #include "tim12.h"
#undef  __TIM12_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "tim.h"
/* End Include */

/* Start Define */
#define TIM12_FREQUENCY				( 12000000.f )
#define TIM12_PRESCALER_VALUE		( (uint32_t)( APB1_TIMER_CLOCKS / TIM12_FREQUENCY ) - 1 )
#define TIM12_PWM_FREQ				( 3200.f )
#define TIM12_PERIOD_VALUE			( (uint32_t)( TIM12_FREQUENCY / TIM12_PWM_FREQ - 1 ) )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */
                 
bool TIM12_Init( TIM_HandleTypeDef *pTim )
{
	bool	err = false;
	
	TIM_ClockConfigTypeDef				sClockSourceConfig = {0};
	TIM_OC_InitTypeDef					sConfigOC = {0};
	
	pTim->Instance						= TIM12;
	pTim->Init.Prescaler				= TIM12_PRESCALER_VALUE;
	pTim->Init.CounterMode				= TIM_COUNTERMODE_UP;
	pTim->Init.Period					= TIM12_PERIOD_VALUE;
	pTim->Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	pTim->Init.RepetitionCounter		= 0;
	pTim->Init.AutoReloadPreload		= TIM_AUTORELOAD_PRELOAD_ENABLE;
	
	if( HAL_TIM_Base_Init( pTim ) != HAL_OK ){ err = true; }
	
	sClockSourceConfig.ClockSource		= TIM_CLOCKSOURCE_INTERNAL;
	if( HAL_TIM_ConfigClockSource( pTim, &sClockSourceConfig ) != HAL_OK ){ err = true; }
	
	if( HAL_TIM_PWM_Init( pTim ) != HAL_OK ){ err = true; }
	
	sConfigOC.OCMode					= TIM_OCMODE_PWM1;
	sConfigOC.Pulse						= 0;
	sConfigOC.OCPolarity				= TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode				= TIM_OCFAST_ENABLE;
	if( HAL_TIM_PWM_ConfigChannel( pTim, &sConfigOC, TIM_CHANNEL_2 ) != HAL_OK ){ err = true; }
	
	HAL_TIM_MspPostInit( pTim );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM12_Freq_Set( float Freq, float Duty )
{
	float	Period = TIM12_FREQUENCY / Freq;
	
    TIM12_Handle.Instance->ARR	= (uint32_t)(Period - 1.f);
    TIM12_Handle.Instance->CCR2	= (uint32_t)(Period * Duty * 0.01f);
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM12_Duty_Set( float Duty )
{
	uint32_t	Res = ( (uint32_t)( ((float)TIM12_Handle.Instance->ARR + 1.f) * Duty * 0.01f) );
	
    TIM12_Handle.Instance->CCR2 = Res;
}


