#define __LCD_C__
    #include "lcd.h"
#undef  __LCD_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "fsmc.h"  
#include "tim4.h"  
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void LCD_DeInit(void);
static void GPIO_Init( void );
static void LCD_IO_Init( void );
static void LCD_BL_Init( void );
static void LCD_IO_Reset( void );
static void LCD_BL_Off( void );
/* End Function */

/* Start Variable */
LCD_DriveType	LcdDrive = 
{
  	LCD_DeInit,
	LCD_IO_Init,
	LCD_BL_Init,
    LCD_BL_Off,
	ILI9225G_Init,
	ILI9225G_ReadID,
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

static void LCD_DeInit(void)
{
  	ILI9225G_DisplayOff();
  	
  	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_15);
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
}

static void LCD_IO_Init( void )
{
	GPIO_Init();
	LCD_IO_Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure = { 0 };
	
    __HAL_RCC_GPIOF_CLK_ENABLE();
	
	/**TFT GPIO Configuration    
	PF15	------> TFT nRESET Pin
	*/
    
    GPIO_InitStructure.Pin		= GPIO_PIN_15;
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_PULLUP;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
	
    HAL_GPIO_Init( GPIOF, &GPIO_InitStructure );
	
	HAL_GPIO_WritePin( GPIOF, GPIO_PIN_15, GPIO_PIN_SET );
	
	/**TFT GPIO Configuration    
	PB8     ------> BLU Enable Pin
	*/
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStructure.Pin			= GPIO_PIN_8 | GPIO_PIN_9; //PB9 -- BackLight PWM
	GPIO_InitStructure.Mode			= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull			= GPIO_PULLUP;
	GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );
	
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_8, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_9, GPIO_PIN_RESET );
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_BL_Init( void )
{
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_8, GPIO_PIN_RESET );
	HAL_Delay( 5 );
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_8, GPIO_PIN_SET );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void LCD_BL_Off( void )
{
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_8, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_9, GPIO_PIN_RESET );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
