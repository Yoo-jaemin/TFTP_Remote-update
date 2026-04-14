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
/* End Define */

/* Start Enum */ 
/* End Enum */

/* Start Struct */  
typedef struct
{
  	void		(*DeInit)(void);
	void		(*Io_Init)(void);
	void		(*BL_Init)(void);
    void        (*BL_Off)(void);
	void		(*Init)(void);
	uint16_t	(*ReadID)(void);
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
}LCD_DriveType;
/* End Struct */

/* Start Function */
//
/* End Function */

/* Start Variable */
extern LCD_DriveType		LcdDrive;
/* End Variable */


 
#endif // __LCD_H__
