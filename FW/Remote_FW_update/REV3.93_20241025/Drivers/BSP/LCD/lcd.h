#ifndef   __LCD_H__
#define   __LCD_H__


#ifdef __LCD_C__
	#define LCD_EXT
#else
	#define LCD_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h" 
#include "ili9225g.h" 
/* End Include */

/* Start Define */
#define LCD_BRIGHTNESS_EEP_PREFIX		( 0x8000 )
/* End Define */

/* Start Enum */ 
/* End Enum */

/* Start Struct */  
typedef struct
{
	void		(*BL_Init)(void);
	void		(*BL_On)(void);
	void		(*BL_Off)(void);
	bool		(*Init)(void);
	void		(*Brightness_Set)(U16);
	void		(*DisplayOn)(void);
	void		(*DisplayOff)(void);
	void		(*SetCursor)(uint16_t, uint16_t);
	void		(*WritePixel)(uint16_t, uint16_t, uint16_t);
	uint16_t	(*ReadPixel)(uint16_t, uint16_t);
	
	/* Optimized operation */
	void		(*SetDisplayWindow)(uint16_t, uint16_t, uint16_t, uint16_t);
	void		(*DrawHLine)(uint16_t, uint16_t, uint16_t, uint16_t);
	void		(*DrawVLine)(uint16_t, uint16_t, uint16_t, uint16_t);
	
	uint16_t	(*GetLcdPixelWidth)(void);
	uint16_t	(*GetLcdPixelHeight)(void);
	void		(*DrawBitmap)(uint16_t, uint16_t, uint8_t*);
	void		(*DrawRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);
	
	U16			BrightnessLevel;
}LCD_DriveType;
/* End Struct */

/* Start Function */
//
/* End Function */

/* Start Variable */
extern LCD_DriveType		LcdDrive;
/* End Variable */


 
#endif // __LCD_H__
