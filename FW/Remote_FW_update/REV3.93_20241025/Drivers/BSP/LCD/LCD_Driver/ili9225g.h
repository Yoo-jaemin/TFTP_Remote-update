#ifndef   __ILI9225G_H__
#define   __ILI9225G_H__


#ifdef __ILI9225G_C__
	#define ILI9225G_EXT
#else
	#define ILI9225G_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h" 
/* End Include */

/* Start Define */
#define ILI9225G_ID						( 0x9226 )
#define ILI9225G_LCD_PIXEL_WIDTH		( (uint16_t)220 )
#define ILI9225G_LCD_PIXEL_HEIGHT		( (uint16_t)176 )
#define ILI9225G_TOTAL_PIXEL_COUNT		( ILI9225G_LCD_PIXEL_WIDTH * ILI9225G_LCD_PIXEL_HEIGHT )
   
enum {
	LCD_REG_00H = 0x00,
	LCD_REG_01H = 0x01,
	LCD_REG_02H = 0x02,
	LCD_REG_03H = 0x03,
	LCD_REG_07H = 0x07,
	LCD_REG_08H = 0x08,
	LCD_REG_0BH = 0x0B,
	LCD_REG_0CH = 0x0C,
	LCD_REG_0FH = 0x0F,
	LCD_REG_10H = 0x10,
	LCD_REG_11H = 0x11,
	LCD_REG_12H = 0x12,
	LCD_REG_13H = 0x13,
	LCD_REG_14H = 0x14,
	LCD_REG_20H = 0x20,
	LCD_REG_21H = 0x21,
	LCD_REG_22H = 0x22,
	LCD_REG_30H = 0x30,
	LCD_REG_31H = 0x31,
	LCD_REG_32H = 0x32,
	LCD_REG_33H = 0x33,
	LCD_REG_34H = 0x34,
	LCD_REG_35H = 0x35,
	LCD_REG_HEA = 0x36,
	LCD_REG_HSA = 0x37,
	LCD_REG_VEA = 0x38,
	LCD_REG_VSA = 0x39,
	LCD_REG_50H = 0x50,
	LCD_REG_51H = 0x51,
	LCD_REG_52H = 0x52,
	LCD_REG_53H = 0x53,
	LCD_REG_54H = 0x54,
	LCD_REG_55H = 0x55,
	LCD_REG_56H = 0x56,
	LCD_REG_57H = 0x57,
	LCD_REG_58H = 0x58,
	LCD_REG_59H = 0x59,
	LCD_REG_60H = 0x60,
	LCD_REG_61H = 0x61,
	LCD_REG_62H = 0x62,
	LCD_REG_63H = 0x63,
	LCD_REG_65H = 0x65,
	LCD_REG_C7H = 0xC7,
	LCD_REG_D0H = 0xD0,
	LCD_REG_EBH = 0xEB,
	LCD_REG_ECH = 0xEC
#if 0
	LCD_REG_80H = 0x80,
	LCD_REG_81H = 0x81,
	LCD_REG_82H = 0x82,
	LCD_REG_83H = 0x83,
	LCD_REG_84H = 0x84,
	LCD_REG_85H = 0x85,
	LCD_REG_86H = 0x86,
	LCD_REG_87H = 0x87,
	LCD_REG_88H = 0x88,
	LCD_REG_89H = 0x89,
	LCD_REG_8BH = 0x8B,
	LCD_REG_8CH = 0x8C,
	LCD_REG_8DH = 0x8D,
	LCD_REG_8FH = 0x8F,
	LCD_REG_90H = 0x90,
	LCD_REG_91H = 0x91,
	LCD_REG_92H = 0x92,
	LCD_REG_93H = 0x93,
	LCD_REG_94H = 0x94,
	LCD_REG_95H = 0x95,
	LCD_REG_96H = 0x96,
	LCD_REG_97H = 0x97,
	LCD_REG_98H = 0x98,
	LCD_REG_99H = 0x99,
	LCD_REG_9AH = 0x9A,
	LCD_REG_9DH = 0x9D,
	LCD_REG_C0H = 0xC0,
	LCD_REG_C1H = 0xC1,
	LCD_REG_C7H = 0xC7,
	LCD_REG_D0H = 0xD0,
	LCD_REG_E5H = 0xE5,
	LCD_REG_EBH = 0xEB,
	LCD_REG_ECH = 0xEC
#endif
};
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
    U16  Word;
    struct{    
        U8	Lo;
        U8	Hi;
    }Byte;
}Gamma_DataType;

typedef struct {
	Gamma_DataType	Set[10];
	Gamma_DataType	Out[10];
}Gamma_SetType;
/* End Struct */

/* Start Function */
ILI9225G_EXT void ILI9225G_Init(void);
ILI9225G_EXT uint16_t ILI9225G_ReadID(void);
ILI9225G_EXT void ILI9225G_WriteReg(uint8_t LCDReg, uint16_t LCDRegValue);
ILI9225G_EXT uint16_t ILI9225G_ReadReg(uint8_t LCDReg);

ILI9225G_EXT void ILI9225G_DisplayOn(void);
ILI9225G_EXT void ILI9225G_DisplayOff(void);
ILI9225G_EXT void ILI9225G_SetCursor(uint16_t Xpos, uint16_t Ypos);
ILI9225G_EXT void ILI9225G_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
ILI9225G_EXT uint16_t ILI9225G_ReadPixel(uint16_t Xpos, uint16_t Ypos);

ILI9225G_EXT void ILI9225G_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
ILI9225G_EXT void ILI9225G_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
ILI9225G_EXT void ILI9225G_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
ILI9225G_EXT void ILI9225G_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata);

ILI9225G_EXT void ILI9225G_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);

ILI9225G_EXT uint16_t ILI9225G_GetLcdPixelWidth(void);
ILI9225G_EXT uint16_t ILI9225G_GetLcdPixelHeight(void);

ILI9225G_EXT void ILI9225G_IO_WriteData(uint16_t Data); 
ILI9225G_EXT void ILI9225G_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
ILI9225G_EXT void ILI9225G_IO_WriteRGB(uint16_t Data); 
ILI9225G_EXT void ILI9225G_IO_WriteMultipleRGB(uint8_t *pData, uint32_t Size);
ILI9225G_EXT void ILI9225G_IO_WriteReg(uint8_t Reg);
ILI9225G_EXT uint16_t ILI9225G_IO_ReadData(uint16_t Reg);
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __ILI9225G_H__



