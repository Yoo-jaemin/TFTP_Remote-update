#define __ILI9225G_C__
    #include "ili9225g.h"
#undef  __ILI9225G_C__

/* Start Include */
#include "fsmc.h"
/* End Include */

/* Start Define */
#define DEFAULT_BACKLIGHT_BRITENESS     ( 0x255 )
/* End Define */

/* Start Enum*/
/* End Enum*/

/* Start Struct*/
/* End Struct*/

/* Start Function */
/* End Function */

/* Start Variable */
static uint16_t		RGB_ReMap[2];
static uint16_t		ArrayRGB[ILI9225G_LCD_PIXEL_WIDTH];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_Init(void)
{
	ILI9225G_WriteReg( LCD_REG_D0H, 0x0003 );
	ILI9225G_WriteReg( LCD_REG_EBH, 0x0B00 );
	ILI9225G_WriteReg( LCD_REG_ECH, 0x004F );
	ILI9225G_WriteReg( LCD_REG_C7H, 0x030F );
	
	ILI9225G_WriteReg( LCD_REG_01H, 0x021C );
	ILI9225G_WriteReg( LCD_REG_02H, 0x0100 );
	ILI9225G_WriteReg( LCD_REG_03H, 0x1038 );
	ILI9225G_WriteReg( LCD_REG_08H, 0x0808 );
	ILI9225G_WriteReg( LCD_REG_0FH, 0x0901 );
	
	HAL_Delay( 10 );
	
	ILI9225G_WriteReg( LCD_REG_10H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_11H, 0x1B41 );
	
	HAL_Delay( 120 );
	
	ILI9225G_WriteReg( LCD_REG_12H, 0x200E );
	ILI9225G_WriteReg( LCD_REG_13H, 0x0052 );
	ILI9225G_WriteReg( LCD_REG_14H, 0x5763 );
	
	ILI9225G_WriteReg( LCD_REG_30H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_31H, 0x00DB );
	ILI9225G_WriteReg( LCD_REG_32H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_33H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_34H, 0x00DB );
	ILI9225G_WriteReg( LCD_REG_35H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_HEA, 0x00AF );
	ILI9225G_WriteReg( LCD_REG_HSA, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_VEA, 0x00DB );
	ILI9225G_WriteReg( LCD_REG_VSA, 0x0000 );
	
	/* Adjust the Gamma Curve (ILI9225G) ----------------------------------------*/
	ILI9225G_WriteReg( LCD_REG_50H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_51H, 0x010C );//0x0808 );
	ILI9225G_WriteReg( LCD_REG_52H, 0x0A01 );
	ILI9225G_WriteReg( LCD_REG_53H, 0x0401 );
	ILI9225G_WriteReg( LCD_REG_54H, 0x020A );
	ILI9225G_WriteReg( LCD_REG_55H, 0x0B00 );
	ILI9225G_WriteReg( LCD_REG_56H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_57H, 0x0104 );
	ILI9225G_WriteReg( LCD_REG_58H, 0x0E05 );
	ILI9225G_WriteReg( LCD_REG_59H, 0x050E );
	
	ILI9225G_WriteReg( LCD_REG_20H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_21H, 0x0000 );
	ILI9225G_WriteReg( LCD_REG_07H, 0x1017 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_DisplayOn(void)
{
	/* Power On sequence -------------------------------------------------------*/
	ILI9225G_DisplayOff();
	
	HAL_Delay( 1000 );
	ILI9225G_WriteReg( LCD_REG_10H, 0x0200 ); /* Standby Mode Off */
	ILI9225G_WriteReg( LCD_REG_11H, 0x1041 ); /* Automatic booster start */
	
	HAL_Delay( 1000 );
	ILI9225G_WriteReg( LCD_REG_12H, 0x2012 ); /* Step-Up Circuit(2): AVDD = 2xVCI, VCL = -1xVCI, VGH = 6xVCI, VGL = -6xVCI */
	ILI9225G_WriteReg( LCD_REG_13H, 0x0009 ); /* Gamma voltage = 5.5[V] */
	ILI9225G_WriteReg( LCD_REG_14H, 0xCF08 ); /* VCOMG = 1, VCOM = VCOMH - GND */
	
	/* Display On */
	ILI9225G_WriteReg( LCD_REG_07H, 0x1017 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_DisplayOff(void)
{
	/* Power Off sequence ------------------------------------------------------*/
	ILI9225G_WriteReg( LCD_REG_10H, 0x0001 ); /* Standby Mode On */
	ILI9225G_WriteReg( LCD_REG_11H, 0x0000 ); /* Automatic booster stop */
	ILI9225G_WriteReg( LCD_REG_12H, 0x0000 ); /* Step-Up Circuit(5): AVDD = 2xVCI, VCL = -1xVCI, VGH = 7xVCI, VGL = -7xVCI */
	ILI9225G_WriteReg( LCD_REG_13H, 0x0000 ); /* Gamma voltage = 4.1[V] */
	ILI9225G_WriteReg( LCD_REG_14H, 0x0000 ); /* VCOMG = 1, VCOM = VCOMH - GND */
	
	/* Display Off */
	ILI9225G_WriteReg( LCD_REG_07H, 0x0000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t ILI9225G_GetLcdPixelWidth(void)
{
	return ILI9225G_LCD_PIXEL_WIDTH;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t ILI9225G_GetLcdPixelHeight(void)
{
	return ILI9225G_LCD_PIXEL_HEIGHT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t ILI9225G_ReadID(void)
{
	return ( ILI9225G_ReadReg( 0x00 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	ILI9225G_WriteReg( LCD_REG_20H, ILI9225G_LCD_PIXEL_HEIGHT - Ypos - 1 );
	ILI9225G_WriteReg( LCD_REG_21H, Xpos );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{                    
	/* Set Cursor */
	ILI9225G_SetCursor(Xpos, Ypos); 
	
	/* Prepare to write GRAM */
	ILI9225G_IO_WriteReg( LCD_REG_22H );
	                          
	/* Write 16-bit GRAM Reg */
	ILI9225G_IO_WriteMultipleRGB( (uint8_t*)&RGBCode, 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t ILI9225G_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
	/* Set Cursor */
	ILI9225G_SetCursor(Xpos, Ypos);
	
	/* Read 16-bit Reg */
	return (ILI9225G_IO_ReadData(LCD_REG_22H));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_WriteReg(uint8_t LCDReg, uint16_t LCDRegValue)
{
	ILI9225G_IO_WriteReg(LCDReg);
	
	/* Write 16-bit GRAM Reg */
	ILI9225G_IO_WriteMultipleData((uint8_t*)&LCDRegValue, 2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t ILI9225G_ReadReg(uint8_t LCDReg)
{ 
	/* Read 16-bit Reg */
	return (ILI9225G_IO_ReadData(LCDReg));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{    
	/* Horizontal GRAM Start Address */
	ILI9225G_WriteReg(LCD_REG_HSA, (ILI9225G_LCD_PIXEL_HEIGHT - 1 - Ypos - Height + 1));
	/* Horizontal GRAM End Address */
	ILI9225G_WriteReg(LCD_REG_HEA, (ILI9225G_LCD_PIXEL_HEIGHT - 1 - Ypos));
	
	/* Vertical GRAM Start Address */
	ILI9225G_WriteReg(LCD_REG_VSA, (Xpos));
	/* Vertical GRAM End Address */
	ILI9225G_WriteReg(LCD_REG_VEA, (Xpos + Width - 1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
	uint16_t	i;
	
	ILI9225G_WriteReg( LCD_REG_03H, 0x1328 );
	
	/* Set Cursor */
	ILI9225G_SetCursor( Xpos, Ypos );
	
	/* Prepare to write GRAM */
	ILI9225G_IO_WriteReg( LCD_REG_22H );
	
	/* Sent a complete line */
	for( i=0; i<Length; i++ ){ ArrayRGB[i] = RGBCode; }
	                  
	/* Write 16-bit GRAM Reg */
	ILI9225G_IO_WriteMultipleRGB( (uint8_t*)ArrayRGB, Length * 2 );
	
	ILI9225G_WriteReg( LCD_REG_03H, 0x1338 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
	uint16_t	i;
	
	/* set GRAM write direction and BGR = 1 */
	/* I/D=00 (Horizontal : increment, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	ILI9225G_WriteReg( LCD_REG_03H, 0x1320 );
	
	/* Set Cursor */
	ILI9225G_SetCursor( Xpos, Ypos );
	
	/* Prepare to write GRAM */
	ILI9225G_IO_WriteReg( LCD_REG_22H );
	
	/* Sent a complete line */
	for( i=0; i<Length; i++ ){ ArrayRGB[i] = RGBCode; }  
	                             
	/* Write 16-bit GRAM Reg */
	ILI9225G_IO_WriteMultipleRGB( (uint8_t*)ArrayRGB, Length * 2 ); 
	
	/* set GRAM write direction and BGR = 0 */
	/* I/D=00 (Horizontal : increment, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	ILI9225G_WriteReg( LCD_REG_03H, 0x1338 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
	uint32_t index = 0, size = 0;
	
	/* Read bitmap size */
	size = *(volatile uint16_t *) (pbmp + 2);
	size |= (*(volatile uint16_t *) (pbmp + 4)) << 16;
	
	/* Get bitmap data address offset */
	index = *(volatile uint16_t *) (pbmp + 10);
	index |= (*(volatile uint16_t *) (pbmp + 12)) << 16;
	size = (size - index)/2;
	pbmp += index;
	
	/* Set GRAM write direction and BGR = 0 */
	/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	ILI9225G_WriteReg( LCD_REG_03H, 0x1338 );
	
	/* Set Cursor */
	ILI9225G_SetCursor(Xpos, Ypos);  
	
	/* Prepare to write GRAM */
	ILI9225G_IO_WriteReg(LCD_REG_22H);
	
	ILI9225G_IO_WriteMultipleRGB((uint8_t*)pbmp, size*2);
	
	/* Set GRAM write direction and BGR = 1 */
	/* I/D = 01 (Horizontal : increment, Vertical : decrement) */
	/* AM = 1 (address is updated in vertical writing direction) */
	ILI9225G_WriteReg( LCD_REG_03H, 0x1338 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata)
{
	uint32_t size = 0;
	
	size = (Xsize * Ysize);
	
	ILI9225G_WriteReg( LCD_REG_03H, 0x1328 );
	
	/* Set Cursor */
	ILI9225G_SetCursor(Xpos, Ypos);
	
	/* Prepare to write GRAM */
	ILI9225G_IO_WriteReg(LCD_REG_22H);
	
	ILI9225G_IO_WriteMultipleRGB((uint8_t*)pdata, size*2);
	
	ILI9225G_WriteReg( LCD_REG_03H, 0x1338 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_IO_WriteData(uint16_t Data) 
{          
	/* Write 16-bit Reg */
	FSMC_SramBank1_WriteData( Data );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_IO_WriteMultipleData(uint8_t *pData, uint32_t Size)
{
	uint32_t counter;
	uint16_t *ptr = (uint16_t *)pData;
	
	
	for( counter=0; counter<Size; counter+=2 )
	{                                         
		/* Write 16-bit Reg */
		FSMC_SramBank1_WriteData( *ptr );
		ptr++;       
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_IO_WriteRGB(uint16_t Data) 
{                          
	RGB_ReMap[0] = ( (Data >> 14 & 0x0003) );
	RGB_ReMap[1] = ( (Data <<  2 & 0xE000) | (Data << 1 & 0x1FFE) | (Data & 0x0001) );
	
	/* Write 16-bit Reg */
	FSMC_SramBank1_WriteData( RGB_ReMap[0] );
	FSMC_SramBank1_WriteData( RGB_ReMap[1] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_IO_WriteMultipleRGB(uint8_t *pData, uint32_t Size)
{
	uint32_t	counter;
	uint16_t	*ptr = (uint16_t *)pData;

	for( counter=0; counter<Size; counter+=2 )
	{
		RGB_ReMap[0] = ( (*ptr >> 14 & 0x0003) );
		RGB_ReMap[1] = ( (*ptr <<  2 & 0xE000) | (*ptr << 1 & 0x1FFE) | (*ptr & 0x0001) );
		
		/* Write 16-bit Reg */
		FSMC_SramBank1_WriteData( RGB_ReMap[0] );
		FSMC_SramBank1_WriteData( RGB_ReMap[1] );
		ptr++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ILI9225G_IO_WriteReg(uint8_t Reg) 
{
	/* Write 16-bit Index, then Write Reg */
	FSMC_SramBank1_WriteReg(Reg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t ILI9225G_IO_ReadData(uint16_t Reg)
{
	FSMC_SramBank1_WriteReg(Reg);
	
	/* Read 16-bit Reg */  
	return FSMC_SramBank1_ReadData();
}


