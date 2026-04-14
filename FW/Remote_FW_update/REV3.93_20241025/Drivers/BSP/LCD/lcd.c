#define __LCD_C__
    #include "lcd.h"
#undef  __LCD_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "fsmc.h"
#include "tim4.h"
#include "eeprom.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static bool LCD_Init( void );
static void LCD_BL_On( void );
static void LCD_BL_Off( void );
static void LCD_Brightness_Set( U16 Level );
static void LCD_BL_Init( void );
static void LCD_IO_Reset( void );
/* End Function */

/* Start Variable */
LCD_DriveType	LcdDrive = 
{
	LCD_BL_Init,
	LCD_BL_On,
	LCD_BL_Off,
	LCD_Init,
	LCD_Brightness_Set,
	ILI9225G_DisplayOn,
	ILI9225G_DisplayOff,
	ILI9225G_SetCursor,
	ILI9225G_WritePixel,
	ILI9225G_ReadPixel,
	ILI9225G_SetDisplayWindow,
	ILI9225G_DrawHLine,
	ILI9225G_DrawVLine,
	ILI9225G_GetLcdPixelWidth,
	ILI9225G_GetLcdPixelHeight,
	ILI9225G_DrawBitmap,
	ILI9225G_DrawRGBImage,  
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	
    __HAL_RCC_GPIOF_CLK_ENABLE();
	
	/**TFT GPIO Configuration    
	PF15	------> TFT nRESET Pin
	*/
    
    GPIO_InitStructure.Pin		= GPIO_PIN_15;
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_PULLUP;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_WritePin( GPIOF, GPIO_PIN_15, GPIO_PIN_SET );
	
    HAL_GPIO_Init( GPIOF, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool LCD_Init( void )
{
	bool	err = false;
	U16		i;
	
	if( TIM4_Init( &TIM4_Handle ) != HAL_OK ){ err = true; }
	
	LCD_BL_Off();
	
	GPIO_Init();
	LCD_IO_Reset();
	
	if( ILI9225G_ReadID() == ILI9225G_ID ){
		ILI9225G_Init();
	}
	
	if( EepHandle.Sram[SYSTEM_EEP_LCD_BRIGHTNESS_LEVEL] & LCD_BRIGHTNESS_EEP_PREFIX ){
		LcdDrive.BrightnessLevel = EepHandle.Sram[SYSTEM_EEP_LCD_BRIGHTNESS_LEVEL] & ~LCD_BRIGHTNESS_EEP_PREFIX;
	}
	else {
		EEPROM_Update( EepHandle.pDefault+SYSTEM_EEP_LCD_BRIGHTNESS_LEVEL, SYSTEM_EEP_LCD_BRIGHTNESS_LEVEL, 1 );
		LcdDrive.BrightnessLevel = 50;
	}
	
	LCD_Brightness_Set( LcdDrive.BrightnessLevel );
	
	for( i=0; i<ILI9225G_GetLcdPixelHeight(); i++ ){
    	ILI9225G_DrawHLine( 0, 0, i, ILI9225G_GetLcdPixelWidth() );
	}
	
	LCD_BL_On();
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_BL_On( void )
{
	HAL_TIM_PWM_Start( &TIM4_Handle, TIM_CHANNEL_3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_BL_Off( void )
{
	HAL_TIM_PWM_Stop( &TIM4_Handle, TIM_CHANNEL_3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_Brightness_Set( U16 Level )
{
	float	Duty = (float)Level * 0.95f + 5.f;
	
	TIM4_Duty_Set( Duty );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_BL_Init( void )
{
	HAL_TIM_PWM_Stop ( &TIM4_Handle, TIM_CHANNEL_3 );
	HAL_Delay( 5 );
	HAL_TIM_PWM_Start( &TIM4_Handle, TIM_CHANNEL_3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_IO_Reset( void )
{
	HAL_GPIO_WritePin( GPIOF, GPIO_PIN_15, GPIO_PIN_SET   );
	HAL_Delay(  10 );
	HAL_GPIO_WritePin( GPIOF, GPIO_PIN_15, GPIO_PIN_RESET );
	HAL_Delay(  10 );
	HAL_GPIO_WritePin( GPIOF, GPIO_PIN_15, GPIO_PIN_SET   );
	HAL_Delay( 120 );
}


