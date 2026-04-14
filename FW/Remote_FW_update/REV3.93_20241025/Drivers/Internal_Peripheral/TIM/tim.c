#define __TIM_C__
    #include "tim.h"
#undef  __TIM_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "tim3.h"
#include "tim12.h"
#include "flow_control.h"
#include "buzzer.h"
#include "option.h"
/* End Include */

/* Start Define */        
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TIM_Init( void )
{
	bool	err = false;
	
	if( TIM3_Init( &TIM3_Handle ) != HAL_OK ){
		err = true;
	}
	
	if( SystemOption.Item.BuzPresent == true ){
		
		if( TIM12_Init( &TIM12_Handle ) != HAL_OK ){
			err = true;
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_Base_MspInit( TIM_HandleTypeDef* pTim )
{
	if( pTim->Instance == TIM3 ){
		
		__HAL_RCC_TIM3_CLK_ENABLE();
		
		HAL_NVIC_SetPriority( TIM3_IRQn, 5, 0 );
		HAL_NVIC_EnableIRQ( TIM3_IRQn );
	}
	else if( pTim->Instance == TIM4 ){
		
		__HAL_RCC_TIM4_CLK_ENABLE();
	}
	else if( pTim->Instance == TIM12 ){
		
		__HAL_RCC_TIM12_CLK_ENABLE();
		
		HAL_NVIC_SetPriority( TIM8_BRK_TIM12_IRQn, 7, 0 );
		HAL_NVIC_EnableIRQ( TIM8_BRK_TIM12_IRQn );
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_Base_MspDeInit( TIM_HandleTypeDef* pTim )
{
	if( pTim->Instance == TIM3 ){
		
		__HAL_RCC_TIM3_CLK_DISABLE();
		
		HAL_NVIC_DisableIRQ( TIM3_IRQn );
	}
	else if( pTim->Instance == TIM4 ){
		
		__HAL_RCC_TIM4_CLK_DISABLE();
	}
	else if( pTim->Instance == TIM12 ){
		
		__HAL_RCC_TIM12_CLK_DISABLE();
		
		HAL_NVIC_DisableIRQ( TIM8_BRK_TIM12_IRQn );
	}
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_PWM_MspInit( TIM_HandleTypeDef* pTim )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_PWM_MspDeInit( TIM_HandleTypeDef* pTim )
{
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *pTim )
{                  
	                  
	if( pTim->Instance == TIM3 ){
		Pump_Drive( FlowCtrl.P_Value );
	}
	else if( pTim->Instance == TIM12 ){
		Buzzer_Drive();
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_MspPostInit( TIM_HandleTypeDef* pTim )
{
	GPIO_InitTypeDef  GPIO_InitStructure = { 0 };
	
	if( pTim->Instance == TIM3 ){
             
		/**TIM3 GPIO Configuration    
		PB0     ------> TIM3_CH3	( Pump_Forward_Pin )
		PB5     ------> TIM3_CH2	( Pump_Reverse_Pin )
		*/                          
	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_InitStructure.Pin			= ( GPIO_PIN_0 | GPIO_PIN_5 );
		GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull			= GPIO_PULLUP;
		GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStructure.Alternate	= GPIO_AF2_TIM3;
		
		HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );
	}
	else if( pTim->Instance == TIM4 ){
             
		/**TIM3 GPIO Configuration    
		PB8    ------> TIM4_CH3	( LCD Backlight Enable-2 )
		*/                          
	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_InitStructure.Pin			= GPIO_PIN_8;
		GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull			= GPIO_PULLDOWN;
		GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Alternate	= GPIO_AF2_TIM4;
		
		HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );
	}
	else if( pTim->Instance == TIM12 ){
             
		/**TIM3 GPIO Configuration    
		PB15    ------> TIM12_CH2	( Buzzer_Drive )
		*/                          
	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_InitStructure.Pin			= GPIO_PIN_15;
		GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull			= GPIO_PULLDOWN;
		GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Alternate	= GPIO_AF9_TIM12;
		
		HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );
	}
}


