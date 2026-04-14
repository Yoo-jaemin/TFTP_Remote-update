#define __LED_C__
    #include "led.h"
#undef  __LED_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "btimer.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void LED_Gpio_Write( LED_GpioType *pLED, bool State );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool LED_Init( void )
{
	bool	err = false;
	
	GPIO_Init();
	
	LedDrive.Out.Byte = 0xFF;
	LedDrive.Set.Byte = 0x00;
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LED_Task( void )
{
	LED_Indicator		Compare;
	
	Compare.Byte = LedDrive.Out.Byte ^ LedDrive.Set.Byte;
	
	if( Compare.Byte ){
		LedDrive.Out.Byte = LedDrive.Set.Byte;
		
		if( Compare.Bit.Function	== true ){ LED_Gpio_Write( &LedDrive.Gpio.Function	, LedDrive.Out.Bit.Function	); }		
		if( Compare.Bit.Reset		== true ){ LED_Gpio_Write( &LedDrive.Gpio.Reset		, LedDrive.Out.Bit.Reset	); }
		if( Compare.Bit.Up			== true ){ LED_Gpio_Write( &LedDrive.Gpio.Up		, LedDrive.Out.Bit.Up		); }
		if( Compare.Bit.Down		== true ){ LED_Gpio_Write( &LedDrive.Gpio.Down		, LedDrive.Out.Bit.Down		); }
		if( Compare.Bit.AL1			== true ){ LED_Gpio_Write( &LedDrive.Gpio.AL1		, LedDrive.Out.Bit.AL1		); }
		if( Compare.Bit.AL2			== true ){ LED_Gpio_Write( &LedDrive.Gpio.AL2		, LedDrive.Out.Bit.AL2		); }
		if( Compare.Bit.TRB			== true ){ LED_Gpio_Write( &LedDrive.Gpio.TRB		, LedDrive.Out.Bit.TRB		); }
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	uint32_t			GpioE_All_Pin;
	uint32_t			GpioF_All_Pin;
	uint32_t			GpioG_All_Pin;
	
    /**BSP LED GPIO Configuration
    PE4      ------> FUNCTION
    PE5      ------> UP
	
    PF2      ------> RESET
    PF3      ------> DOWN
	
    PG12     ------> ALARM-1
    PG13     ------> TROUBLE
    PG14     ------> ALARM-2
    */
	
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
	
	LedDrive.Gpio.Function.Port	= GPIOE;
	LedDrive.Gpio.Up.Port		= GPIOE;
	
	LedDrive.Gpio.Reset.Port	= GPIOF;
	LedDrive.Gpio.Down.Port		= GPIOF;
	
	LedDrive.Gpio.AL1.Port		= GPIOG;
	LedDrive.Gpio.TRB.Port		= GPIOG;
	LedDrive.Gpio.AL2.Port		= GPIOG;
	
	LedDrive.Gpio.Function.Pin	= GPIO_PIN_4;
	LedDrive.Gpio.Up.Pin		= GPIO_PIN_5;
	
	LedDrive.Gpio.Reset.Pin		= GPIO_PIN_2;
	LedDrive.Gpio.Down.Pin		= GPIO_PIN_3;
	
	LedDrive.Gpio.AL1.Pin		= GPIO_PIN_12;
	LedDrive.Gpio.TRB.Pin		= GPIO_PIN_13;
	LedDrive.Gpio.AL2.Pin		= GPIO_PIN_14;
	
    GpioE_All_Pin = (LedDrive.Gpio.Function.Pin | LedDrive.Gpio.Up.Pin);
    GpioF_All_Pin = (LedDrive.Gpio.Reset.Pin | LedDrive.Gpio.Down.Pin);
    GpioG_All_Pin = (LedDrive.Gpio.AL1.Pin | LedDrive.Gpio.AL2.Pin | LedDrive.Gpio.TRB.Pin);
	
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_NOPULL;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_WritePin( GPIOE, GpioE_All_Pin, GPIO_PIN_SET );
	HAL_GPIO_WritePin( GPIOF, GpioF_All_Pin, GPIO_PIN_SET );
	HAL_GPIO_WritePin( GPIOG, GpioG_All_Pin, GPIO_PIN_SET );
	
    GPIO_InitStructure.Pin = GpioE_All_Pin;
    HAL_GPIO_Init( GPIOE, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = GpioF_All_Pin;
    HAL_GPIO_Init( GPIOF, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = GpioG_All_Pin;
    HAL_GPIO_Init( GPIOG, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LED_Gpio_Write( LED_GpioType *pLED, bool State )
{
	HAL_GPIO_WritePin( pLED->Port, pLED->Pin, (GPIO_PinState)(State ^ true) );
}


