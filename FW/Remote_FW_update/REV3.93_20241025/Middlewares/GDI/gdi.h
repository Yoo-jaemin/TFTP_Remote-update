#ifndef   __GDI_H__
#define   __GDI_H__


#ifdef __GDI_C__
	#define GDI_EXT
#else
	#define GDI_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "fonts.h"
/* End Include */

/* Start Define */
#define LCD_DEFAULT_FONT					Font11x16
/* End Define */

/* Start Enum */
enum {
	GDI_OK,
	GDI_ERROR,
	GDI_TIMEOUT
};

typedef enum {
	GDI_STR_ALIGN_C		= 0x01,    /* Center mode */
	GDI_STR_ALIGN_R		= 0x02,    /* Right mode  */
	GDI_STR_ALIGN_L		= 0x03     /* Left mode   */
}GDI_LineModeType;
/* End Enum */

/* Start Struct */
typedef struct {
  uint32_t					TextColor;
  uint32_t					BackColor;
  sFONT						*pFont;
}GDI_DrawPropTypeDef;

typedef struct {
  int16_t		X;
  int16_t		Y;
}GDI_Point, *pPoint;

typedef struct {
	uint16_t	RED;
	uint16_t	CRIMSON;
	uint16_t	FIREBRICK;
	uint16_t	MAROON;
	uint16_t	DARKRED;
	uint16_t	BROWN;
	uint16_t	SIENNA;
	uint16_t	SADDLEBROWN;
	uint16_t	INDIANRED;
	uint16_t	ROSYBROWN;
	uint16_t	LIGHTCORAL;
	uint16_t	SALMON;
	uint16_t	DARKSALMON;
	uint16_t	CORAL;
	uint16_t	TOMATO;
	uint16_t	SANDYBROWN;
	uint16_t	LIGHTSALMON;
	uint16_t	PERU;
	uint16_t	CHOCOLATE;
	uint16_t	ORANGERED;
	uint16_t	ORANGE;
	uint16_t	DARKORANGE;
	uint16_t	TAN;
	uint16_t	PEACHPUFF;
	uint16_t	BISQUE;
	uint16_t	MOCCASIN;
	uint16_t	NAVAJOWHITE;
	uint16_t	WHEAT;
	uint16_t	BURLYWOOD;
	uint16_t	DARKGOLDENROD;
	uint16_t	GOLDENROD;
	uint16_t	GOLD;
	uint16_t	YELLOW;
	uint16_t	LIGHTGOLDENRODYELLOW;
	uint16_t	PALEGOLDENROD;
	uint16_t	KHAKI;
	uint16_t	DARKKHAKI;
	uint16_t	LAWNGREEN;
	uint16_t	GREENYELLOW;
	uint16_t	CHARTREUSE;
	uint16_t	LIME;
	uint16_t	LIMEGREEN;
	uint16_t	YELLOWGREEN;
	uint16_t	OLIVE;
	uint16_t	OLIVEDRAB;
	uint16_t	DARKOLIVEGREEN;
	uint16_t	FORESTGREEN;
	uint16_t	DARKGREEN;
	uint16_t	GREEN;
	uint16_t	SEAGREEN;
	uint16_t	MEDIUMSEAGREEN;
	uint16_t	DARKSEAGREEN;
	uint16_t	LIGHTGREEN;
	uint16_t	PALEGREEN;
	uint16_t	SPRINGGREEN;
	uint16_t	MEDIUMSPRINGGREEN;
	uint16_t	TEAL;
	uint16_t	DARKCYAN;
	uint16_t	LIGHTSEAGREEN;
	uint16_t	MEDIUMAQUAMARINE;
	uint16_t	CADETBLUE;
	uint16_t	STEELBLUE;
	uint16_t	AQUAMARINE;
	uint16_t	POWDERBLUE;
	uint16_t	PALETURQUOISE;
	uint16_t	LIGHTBLUE;
	uint16_t	LIGHTSTEELBLUE;
	uint16_t	SKYBLUE;
	uint16_t	LIGHTSKYBLUE;
	uint16_t	MEDIUMTURQUOISE;
	uint16_t	TURQUOISE;
	uint16_t	DARKTURQUOISE;
	uint16_t	AQUA;
	uint16_t	CYAN;
	uint16_t	DEEPSKYBLUE;
	uint16_t	DODGERBLUE;
	uint16_t	CORNFLOWERBLUE;
	uint16_t	ROYALBLUE;
	uint16_t	BLUE;
	uint16_t	MEDIUMBLUE;
	uint16_t	NAVY;
	uint16_t	DARKBLUE;
	uint16_t	MIDNIGHTBLUE;
	uint16_t	DARKSLATEBLUE;
	uint16_t	SLATEBLUE;
	uint16_t	MEDIUMSLATEBLUE;
	uint16_t	MEDIUMPURPLE;
	uint16_t	DARKORCHID;
	uint16_t	DARKVIOLET;
	uint16_t	BLUEVIOLET;
	uint16_t	MEDIUMORCHID;
	uint16_t	PLUM;
	uint16_t	LAVENDER;
	uint16_t	THISTLE;
	uint16_t	ORCHID;
	uint16_t	VIOLET;
	uint16_t	INDIGO;
	uint16_t	DARKMAGENTA;
	uint16_t	PURPLE;
	uint16_t	MEDIUMVIOLETRED;
	uint16_t	DEEPPINK;
	uint16_t	FUCHSIA;
	uint16_t	MAGENTA;
	uint16_t	HOTPINK;
	uint16_t	PALEVIOLETRED;
	uint16_t	LIGHTPINK;
	uint16_t	PINK;
	uint16_t	MISTYROSE;
	uint16_t	BLANCHEDALMOND;
	uint16_t	LIGHTYELLOW;
	uint16_t	CORNSILK;
	uint16_t	ANTIQUEWHITE;
	uint16_t	PAPAYAWHIP;
	uint16_t	LEMONCHIFFON;
	uint16_t	BEIGE;
	uint16_t	LINEN;
	uint16_t	OLDLACE;
	uint16_t	LIGHTCYAN;
	uint16_t	ALICEBLUE;
	uint16_t	WHITESMOKE;
	uint16_t	LAVENDERBLUSH;
	uint16_t	FLORALWHITE;
	uint16_t	MINTCREAM;
	uint16_t	GHOSTWHITE;
	uint16_t	HONEYDEW;
	uint16_t	SEASHELL;
	uint16_t	IVORY;
	uint16_t	AZURE;
	uint16_t	SNOW;
	uint16_t	WHITE;
	uint16_t	GAINSBORO;
	uint16_t	LIGHTGRAY;
	uint16_t	SILVER;
	uint16_t	DARKGRAY;
	uint16_t	LIGHTSLATEGRAY;
	uint16_t	SLATEGRAY;
	uint16_t	GRAY;
	uint16_t	DIMGRAY;
	uint16_t	DARKSLATEGRAY;
	uint16_t	SELECTED;
	uint16_t	BLACK;
}GDI_ColorType;
/* End Struct */

/* Start Function */
GDI_EXT bool GDI_Init( void );
GDI_EXT uint32_t GDI_GetXSize(void);
GDI_EXT uint32_t GDI_GetYSize(void);
 
GDI_EXT uint16_t GDI_GetTextColor(void);
GDI_EXT uint16_t GDI_GetBackColor(void);
GDI_EXT void     GDI_SetTextColor(__IO uint16_t Color);
GDI_EXT void     GDI_SetBackColor(__IO uint16_t Color);
GDI_EXT void     GDI_SetFont(sFONT *fonts);
GDI_EXT sFONT    *GDI_GetFont(void);

GDI_EXT void     GDI_Clear(uint16_t Color);
GDI_EXT void     GDI_ClearStringLine(uint16_t Line);
GDI_EXT void GDI_StringAtLine( uint16_t Line, char *ptr );
GDI_EXT void GDI_StringColor( uint16_t Xpos, uint16_t Ypos, GDI_LineModeType Mode, uint16_t color, char *str );
GDI_EXT void GDI_String( uint16_t Xpos, uint16_t Ypos, GDI_LineModeType Mode, char *Text );
GDI_EXT void GDI_DisplayChar( uint16_t Xpos, uint16_t Ypos, uint8_t Ascii );

GDI_EXT uint16_t GDI_ReadPixel(uint16_t Xpos, uint16_t Ypos);
GDI_EXT void     GDI_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code);
GDI_EXT void     GDI_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
GDI_EXT void     GDI_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
GDI_EXT void GDI_DrawHLineColor( uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Color );
GDI_EXT void GDI_DrawVLineColor( uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Color );
GDI_EXT void     GDI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
GDI_EXT void GDI_DrawRectColor( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t Color );
GDI_EXT void GDI_DrawRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height );
GDI_EXT void     GDI_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
GDI_EXT void     GDI_DrawPolygon(pPoint Points, uint16_t PointCount);
GDI_EXT void     GDI_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
GDI_EXT void     GDI_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
GDI_EXT void     GDI_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pbmp);
GDI_EXT void GDI_FillRectColor( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t Color );
GDI_EXT void GDI_FillRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height );
GDI_EXT void     GDI_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
GDI_EXT void     GDI_FillPolygon(pPoint Points, uint16_t PointCount);
GDI_EXT void     GDI_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);

GDI_EXT void     GDI_DisplayOff(void);
GDI_EXT void     GDI_DisplayOn(void);
/* End Function */

/* Start Variable */ 
GDI_EXT GDI_DrawPropTypeDef		DrawProp;
GDI_EXT GDI_ColorType			RGB;
/* End Variable */

 
#endif // __GDI_H__
