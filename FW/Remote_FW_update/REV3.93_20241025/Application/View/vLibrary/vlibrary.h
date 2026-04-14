#ifndef   __VLIBRARY_H__
#define   __VLIBRARY_H__


#ifdef __VLIBRARY_C__
	#define VLIBRARY_EXT
#else
	#define VLIBRARY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "gdi.h"  
/* End Include */

/* Start Define */
#define GDI_MESSAGE_BOX_LINE_MAX			( 6 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	int16_t		x;
	int16_t		y;
}vPoint;

typedef struct {
	int16_t		x;
	int16_t		y;
	int16_t		l;
}vLine;

typedef struct {
	int16_t		x;
	int16_t		y;
	int16_t		w;
	int16_t		h;
}vArea;

typedef struct {
	uint16_t				tClr;
	char					*pStr;
	U8						PrvLen;
}vMessageType;

typedef struct {
	uint16_t				LineColor;
	uint16_t				TboxColor;
	uint16_t				BackColor;
	vMessageType			Title;
	vMessageType			Msg[GDI_MESSAGE_BOX_LINE_MAX];
	bool					ClearTitle;
	bool					ClearMsg;
	uint16_t				Width;
	uint8_t					MsgLine;
	uint8_t					OutLine;
	uint16_t				PrvWidth;
	uint8_t					PrvMsgLine;
	uint8_t					PrvOutLine;
}vMessageBoxType;
/* End Struct */

/* Start Function */
VLIBRARY_EXT void vMessageBox( vMessageBoxType *pBox );
VLIBRARY_EXT void vMessageBox_Parameter_Init( vMessageBoxType *pBox );
VLIBRARY_EXT void vGradientBar( int16_t x, int16_t y, int16_t w, int16_t h, float Percent );
VLIBRARY_EXT void vProgressBar( int16_t x, int16_t y, int16_t w, int16_t h, float Percent );
/* End Function */

/* Start Variable */
VLIBRARY_EXT vMessageBoxType	vMsgBox;
/* End Variable */


#endif // __VLIBRARY_H__


