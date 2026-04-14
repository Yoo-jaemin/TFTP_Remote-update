#define __LED_C__
    #include "led.h"
#undef  __LED_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "btimer.h"
#include "ir.h"
/* End Include */

/* Start Define */
#define LED_GRN_PIN                     GPIO_PIN_6
#define LED_GRN_GPIO_PORT               GPIOC
#define LED_GRN_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_GRN_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define LED_RED_PIN                     GPIO_PIN_7
#define LED_RED_GPIO_PORT               GPIOC
#define LED_RED_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_RED_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define LED_ORG_PIN                     (GPIO_PIN_6|GPIO_PIN_7)
#define LED_ORG_GPIO_PORT               GPIOC
#define LED_ORG_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_ORG_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
/* End Define */

/* Start Enum */
enum {
    GRN = 0,
    RED,
    ORG
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void BSP_LED_On( U8 Color );
static void BSP_LED_Off( U8 Color );
static void BSP_LED_Toggle( U8 Color );
/* End Function */

/* Start Variable */
/* End Variable */   

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BSP_LED_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStruct = { 0 };
    
    LED_GRN_GPIO_CLK_ENABLE();
    
    GPIO_InitStruct.Pin    = LED_GRN_PIN;
    GPIO_InitStruct.Mode   = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init( LED_GRN_GPIO_PORT, &GPIO_InitStruct );
    BSP_LED_On(GRN); 
    
    LED_RED_GPIO_CLK_ENABLE();
    
    GPIO_InitStruct.Pin    = LED_RED_PIN;
    
    HAL_GPIO_Init( LED_RED_GPIO_PORT, &GPIO_InitStruct );
    BSP_LED_Off(RED); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LED_Task( void )
{
    switch( SystemState ){
        case SYSTEM_STATE_WARM_UP:
            if( sTimerFlag[TMR_LED_BLINK] ){
                sTimerFlag[TMR_LED_BLINK] = 0;
                
                BSP_LED_Toggle(ORG);
            }
            break;
            
        case SYSTEM_STATE_NORMAL:
            if( IrProcess.TaskComplete ){
                IrProcess.TaskComplete = false;
                
                BSP_LED_Toggle(GRN);
            }
            if( SystemErrorBuf.Word != SystemError.Word ){
				SystemErrorBuf.Word  = SystemError.Word;
				
				if( SystemError.Word )	{ BSP_LED_On (RED); }
            	else					{ BSP_LED_Off(RED); }
            }
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void BSP_LED_On( U8 Color )
{
    switch( Color ){
        case GRN: HAL_GPIO_WritePin( LED_GRN_GPIO_PORT, LED_GRN_PIN, GPIO_PIN_SET ); break;
        case RED: HAL_GPIO_WritePin( LED_RED_GPIO_PORT, LED_RED_PIN, GPIO_PIN_SET ); break;
        case ORG: HAL_GPIO_WritePin( LED_ORG_GPIO_PORT, LED_ORG_PIN, GPIO_PIN_SET ); break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void BSP_LED_Off( U8 Color )
{
    switch( Color ){
        case GRN: HAL_GPIO_WritePin( LED_GRN_GPIO_PORT, LED_GRN_PIN, GPIO_PIN_RESET ); break;
        case RED: HAL_GPIO_WritePin( LED_RED_GPIO_PORT, LED_RED_PIN, GPIO_PIN_RESET ); break;
        case ORG: HAL_GPIO_WritePin( LED_ORG_GPIO_PORT, LED_ORG_PIN, GPIO_PIN_RESET ); break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void BSP_LED_Toggle( U8 Color )
{
    switch( Color ){
        case GRN: HAL_GPIO_TogglePin( LED_GRN_GPIO_PORT, LED_GRN_PIN ); break;
        case RED: HAL_GPIO_TogglePin( LED_RED_GPIO_PORT, LED_RED_PIN ); break;
        case ORG: HAL_GPIO_TogglePin( LED_ORG_GPIO_PORT, LED_ORG_PIN ); break;
    }
}
