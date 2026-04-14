#define __VLIBRARY_C__
    #include "vlibrary.h"
#undef  __VLIBRARY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
const char					*spBlankMsg = "                  ";
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vMessageBox( vMessageBoxType *pBox )
{
	vArea			va	= { (GDI_GetXSize() - pBox->Width) / 2, 2, pBox->Width, 172 };
	GDI_Point		pt	= { (GDI_GetXSize() / 2), 10 };
	GDI_Point		pm	= { pt.X, pt.Y + 35 };
	U8				lo	= ( (GDI_MESSAGE_BOX_LINE_MAX * 10 - pBox->OutLine * 10) );
	U8				mo	= ( (GDI_MESSAGE_BOX_LINE_MAX * 10 - pBox->MsgLine * 10) );
	uint16_t		fo	= 20;
	U8				i;
	U8				CurLen;
	
	GDI_SetFont( &Font11x16 );
	
	GDI_DrawRectColor( va.x  , va.y  +lo, va.w  , va.h  -lo*2, pBox->LineColor );
	GDI_DrawRectColor( va.x+1, va.y+1+lo, va.w-2, va.h-2-lo*2, pBox->LineColor );
	GDI_DrawHLine( va.x+2, 29+lo, va.w-4 );
	GDI_DrawHLine( va.x+2, 30+lo, va.w-4 );
	
	CurLen = strlen( pBox->Title.pStr );
	if( (pBox->ClearTitle == true) ||
		(pBox->Title.PrvLen != CurLen) ||
		(pBox->PrvWidth != pBox->Width) )
	{
		GDI_FillRectColor( va.x+2, va.y+2+lo, va.w-4, 25, pBox->TboxColor );
	}
	pBox->Title.PrvLen = CurLen;
	
	GDI_SetBackColor( pBox->TboxColor );
	GDI_StringColor( pt.X, pt.Y+lo, GDI_STR_ALIGN_C, pBox->Title.tClr, pBox->Title.pStr );
	pBox->Title.tClr = RGB.WHITE;
	
	if( (pBox->ClearMsg == true) ||
		(pBox->PrvMsgLine != pBox->MsgLine) ||
		(pBox->PrvOutLine != pBox->OutLine) ||
		(pBox->PrvWidth != pBox->Width) )
	{
		GDI_FillRectColor( va.x+2, 31+lo, va.w-4, 141-lo*2, pBox->BackColor );
	}
	
	for( i=0, GDI_SetBackColor( pBox->BackColor ); i<GDI_MESSAGE_BOX_LINE_MAX; i++ ){
		
		if( i<pBox->MsgLine ){
			CurLen = strlen( pBox->Msg[i].pStr );
			if( pBox->Msg[i].PrvLen > CurLen ){
				GDI_FillRectColor( va.x+3, pm.Y+(i*fo)+mo, va.w-6, 16, pBox->BackColor );
			}
			GDI_StringColor( pm.X, pm.Y+(i*fo)+mo, GDI_STR_ALIGN_C, pBox->Msg[i].tClr, pBox->Msg[i].pStr );
			pBox->Msg[i].PrvLen = CurLen;
		}
		
		pBox->Msg[i].tClr = RGB.WHITE;
		pBox->Msg[i].pStr = (char *)spBlankMsg;
	}
		                                
	pBox->LineColor		= RGB.DARKSLATEGRAY;
	pBox->TboxColor		= RGB.DARKSLATEGRAY;
	pBox->BackColor		= RGB.BLACK;
	
	pBox->ClearTitle	= false;
	pBox->ClearMsg		= false;
	pBox->PrvWidth		= pBox->Width;
	pBox->PrvMsgLine	= pBox->MsgLine;
	pBox->PrvOutLine	= pBox->OutLine;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vMessageBox_Parameter_Init( vMessageBoxType *pBox )
{
	pBox->LineColor		= RGB.DARKSLATEGRAY;
	pBox->TboxColor		= RGB.DARKSLATEGRAY;
	pBox->BackColor		= RGB.BLACK;
	pBox->Title.tClr	= RGB.WHITE;
	pBox->Title.pStr	= (char *)spBlankMsg;
	pBox->Title.PrvLen	= 19;
	pBox->Msg[0].tClr	= RGB.WHITE;
	pBox->Msg[1].tClr	= RGB.WHITE;
	pBox->Msg[2].tClr	= RGB.WHITE;
	pBox->Msg[3].tClr	= RGB.WHITE;
	pBox->Msg[4].tClr	= RGB.WHITE;
	pBox->Msg[5].tClr	= RGB.WHITE;
	pBox->Msg[0].pStr	= (char *)spBlankMsg;
	pBox->Msg[1].pStr	= (char *)spBlankMsg;
	pBox->Msg[2].pStr	= (char *)spBlankMsg;
	pBox->Msg[3].pStr	= (char *)spBlankMsg;
	pBox->Msg[4].pStr	= (char *)spBlankMsg;
	pBox->Msg[5].pStr	= (char *)spBlankMsg;
	pBox->Msg[0].PrvLen	= 19;
	pBox->Msg[1].PrvLen	= 19;
	pBox->Msg[2].PrvLen	= 19;
	pBox->Msg[3].PrvLen	= 19;
	pBox->Msg[4].PrvLen	= 19;
	pBox->Msg[5].PrvLen	= 19;
	pBox->ClearTitle	= true;
	pBox->ClearMsg		= true;
	pBox->Width			= 216;
	pBox->MsgLine		= 1;
	pBox->OutLine		= 2;
	pBox->PrvWidth		= 216;
	pBox->PrvMsgLine	= 1;
	pBox->PrvOutLine	= 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vGradientBar( int16_t x, int16_t y, int16_t w, int16_t h, float Percent )
{                    
	vArea			FillArea;
	uint16_t		FillWidth;
	uint16_t		HalfWidth;
	uint16_t		FillColor;
	uint16_t		r, g, b;
	uint16_t		i;
	float			rScale, gScale, bScale;
	
	FillArea.x	= (x + 3);
	FillArea.y	= (y + 3);
	FillArea.w	= (w - 6);
	FillArea.h	= (h - 6);
	FillWidth	= (uint16_t)(FillArea.w * Percent * 0.01f );
	HalfWidth	= (FillArea.w >> 1);
	rScale		= (HalfWidth / 31.f);
	gScale		= (HalfWidth / 63.f);
	bScale		= (HalfWidth / 31.f);
	
	GDI_SetTextColor( RGB.BLACK );
	GDI_DrawRect( x+1, y+1, w-2, h-2 );
	GDI_DrawRect( x+2, y+2, w-4, h-4 );
	
	GDI_SetTextColor( RGB.WHITE );
	GDI_DrawRect( x, y, w, h );
	
	for( i=0; i<FillArea.w; i++ ){
		
		if( i < FillWidth ){
			
			if( i < HalfWidth ){
				r = 0;
				g = (RGB.GREEN - ((uint16_t)((i + 1.f) / gScale) <<  5 & RGB.GREEN));
				b = (RGB.BLACK + ((uint16_t)((i + 1.f) / bScale)       & RGB.BLUE ));
			}
			else {
				r = (RGB.BLACK + ((uint16_t)((i - HalfWidth + 1.f) / rScale) << 11 & RGB.RED ));
				g = 0;
				b = (RGB.BLUE  - ((uint16_t)((i - HalfWidth + 1.f) / bScale)       & RGB.BLUE));
			}
		}
		else {
			r = 0;
			g = 0;
			b = 0;
		}
		
		FillColor = r + g + b;
		
		GDI_SetTextColor( FillColor );
		GDI_DrawVLine( FillArea.x+i, FillArea.y, FillArea.h );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vProgressBar( int16_t x, int16_t y, int16_t w, int16_t h, float Percent )
{                    
	vArea			FillArea;
	uint16_t		FillWidth;
	uint16_t		FillColor;
	uint16_t		i;
	
	FillArea.x	= (x + 2);
	FillArea.y	= (y + 2);
	FillArea.w	= (w - 4);
	FillArea.h	= (h - 4);
	FillWidth	= (uint16_t)(FillArea.w * Percent * 0.01f );
	
	GDI_SetTextColor( RGB.BLACK );
	GDI_DrawRect( x+1, y+1, w-2, h-2 );
	
	GDI_SetTextColor( RGB.WHITE );
	GDI_DrawRect( x, y, w, h );
	
	for( i=0; i<FillArea.w; i++ ){
		
		if( i < FillWidth )	{ FillColor = RGB.YELLOW;	}
		else				{ FillColor = RGB.BLACK;	}
		
		GDI_SetTextColor( FillColor );
		GDI_DrawVLine( FillArea.x+i, FillArea.y, FillArea.h );
	}
}
