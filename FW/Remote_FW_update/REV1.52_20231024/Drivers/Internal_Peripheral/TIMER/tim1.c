#define __TIM1_C__
    #include "tim1.h"
#undef  __TIM1_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "ir.h"                
#include "btimer.h"
#include "option.h"
/* End Include */

/* Start Define */
#define APB1_TIMER_CLOCKS				( 72000000.f )
#define TIM1_FREQUENCY					( 100000.f )
#define TIM1_PRESCALER_VALUE			((uint32_t)( APB1_TIMER_CLOCKS / TIM1_FREQUENCY - 1 ))
#define TIM1_PERIOD_VALUE( Freq )		((uint32_t)( TIM1_FREQUENCY / Freq ))
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void HAL_TIM_MspPostInit( TIM_HandleTypeDef *timHandle );
/* End Function */

/* Start Variable */
/* End Variable */  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 
void TIM1_Init( void )
{
	TIM_ClockConfigTypeDef					sClockSourceConfig = { 0 };
    TIM_MasterConfigTypeDef					sMasterConfig = { 0 };
    TIM_OC_InitTypeDef						sConfigOC = { 0 };
    TIM_BreakDeadTimeConfigTypeDef			sBreakDeadTimeConfig = { 0 };
	float									fPWM;
	
	switch( SystemOption.DetType ){
		case 970: fPWM = 10.f; break;
		case 972: fPWM =  8.f; break;
		case 974: fPWM = 10.f; break;
		case 976: fPWM = 10.f; break;
	}
	
    TIM1_Handle.Instance					= TIM1;
    TIM1_Handle.Init.Prescaler				= TIM1_PRESCALER_VALUE;
    TIM1_Handle.Init.CounterMode			= TIM_COUNTERMODE_UP;
    TIM1_Handle.Init.Period					= TIM1_PERIOD_VALUE( fPWM );
    TIM1_Handle.Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
    TIM1_Handle.Init.RepetitionCounter		= 0;
    TIM1_Handle.Init.AutoReloadPreload		= TIM_AUTORELOAD_PRELOAD_DISABLE;
	
    if( HAL_TIM_Base_Init( &TIM1_Handle ) != HAL_OK ){ Error_Handler(); }
    
    sClockSourceConfig.ClockSource			= TIM_CLOCKSOURCE_INTERNAL;
	
    if ( HAL_TIM_ConfigClockSource( &TIM1_Handle, &sClockSourceConfig ) != HAL_OK ){ Error_Handler(); }
    
    if( HAL_TIM_PWM_Init( &TIM1_Handle ) != HAL_OK ){ Error_Handler(); }
    
    sMasterConfig.MasterOutputTrigger		= TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode			= TIM_MASTERSLAVEMODE_DISABLE;
	
    if( HAL_TIMEx_MasterConfigSynchronization( &TIM1_Handle, &sMasterConfig ) != HAL_OK ){ Error_Handler(); }
    
    sConfigOC.OCMode						= TIM_OCMODE_PWM1;
    sConfigOC.Pulse							= (TIM1_PERIOD_VALUE( fPWM ) + 1) / 2;
    sConfigOC.OCPolarity					= TIM_OCPOLARITY_LOW;
    sConfigOC.OCNPolarity					= TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode					= TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState					= TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState					= TIM_OCNIDLESTATE_RESET;
	
    if( HAL_TIM_PWM_ConfigChannel( &TIM1_Handle, &sConfigOC, TIM_CHANNEL_1 ) != HAL_OK ){ Error_Handler(); }
    
    sBreakDeadTimeConfig.OffStateRunMode	= TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode	= TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel			= TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime			= 0;
    sBreakDeadTimeConfig.BreakState			= TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity		= TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput	= TIM_AUTOMATICOUTPUT_DISABLE;
	
    if( HAL_TIMEx_ConfigBreakDeadTime( &TIM1_Handle, &sBreakDeadTimeConfig ) != HAL_OK ){ Error_Handler(); }
    
    HAL_TIM_MspPostInit( &TIM1_Handle );
}       

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void HAL_TIM_MspPostInit( TIM_HandleTypeDef *timHandle )
{
    GPIO_InitTypeDef	GPIO_InitStruct = { 0 };
    
    if( timHandle->Instance == TIM1 ){
		
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        /**TIM1 GPIO Configuration    
        PA8     ------> TIM1_CH1 
        */
        GPIO_InitStruct.Pin		= GPIO_PIN_8;
        GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull	= GPIO_NOPULL;
        GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_LOW;
		
        HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
    }
}       

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_Base_MspDeInit( TIM_HandleTypeDef *tim_baseHandle )
{
    if( tim_baseHandle->Instance==TIM1 ){
		
        __HAL_RCC_TIM1_CLK_DISABLE();
        
        HAL_NVIC_DisableIRQ( TIM1_CC_IRQn );
    }
}        

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_Base_MspInit( TIM_HandleTypeDef *tim_baseHandle )
{

    if( tim_baseHandle->Instance == TIM1 ){
		
        __HAL_RCC_TIM1_CLK_ENABLE();
        
        HAL_NVIC_SetPriority( TIM1_CC_IRQn, 1, 0 );
        HAL_NVIC_EnableIRQ( TIM1_CC_IRQn );
    }
}


