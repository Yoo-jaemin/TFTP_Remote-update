#define __VIMAGE_C__
    #include "vimage.h"
	#include "vimage_com.h"
	#include "vimage_disk.h"
	#include "vimage_intro.h"
	#include "vimage_pump.h"
	#include "vimage_status.h"
	#include "vimage_bat.h"
#undef  __VIMAGE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gdi.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum*/
/* End Enum*/

/* Start Struct*/
typedef struct {
	const uint8_t				*pTable;
	uint16_t					x;
	uint16_t					y;
	uint16_t					w;
	uint16_t					h;
}vImage_InfoType;
/* End Struct*/

/* Start Function */
static void vDrawImage( vImage_InfoType *pImg );
static void vImg_AlarmArrow( bool IsIncrease, uint16_t x, uint16_t y, uint16_t color );
static void vImg_Disk( uint16_t x, uint16_t y );
static void vImg_Com_Status( uint16_t x, uint16_t y, bool IsGray );
static void vImg_Intro( uint16_t x, uint16_t y );
static void vImg_Flow( uint16_t x, uint16_t y, float flow );
static void vImg_iSD( uint16_t x, uint16_t y );
static void vImg_nSD( uint16_t x, uint16_t y );
static void vImg_Filter( uint16_t x, uint16_t y );
static void vImg_iGas( uint16_t x, uint16_t y );
static void vImg_Normal( uint16_t x, uint16_t y );
static void vImg_Bat( uint16_t x, uint16_t y, U8 Level, bool IsCharging );
/* End Function */

/* Start Variable */
vImage_DrawType	vDrawImg = 
{
	vImg_AlarmArrow,
	vImg_Disk,
	vImg_Com_Status,
	vImg_Intro,
	vImg_Flow,
	vImg_iSD,
	vImg_nSD,
	vImg_Filter,
	vImg_iGas,
	vImg_Normal,
	vImg_Bat
};

static U8	vFlowAniTick = 0;
static U8	vFlowAniTime = 0;
static U8	vFlowFrameNo = 0;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vDrawImage( vImage_InfoType *pImg )
{
	GDI_DrawRGBImage( pImg->x, pImg->y, pImg->w, pImg->h, (uint8_t *)pImg->pTable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_AlarmArrow( bool IsIncrease, uint16_t x, uint16_t y, uint16_t color )
{
	GDI_DrawPropTypeDef		FontBackup = DrawProp; 
	
	GDI_SetFont( &Fimg07x12 );
	
	if( IsIncrease == true ){
		GDI_StringColor( x, y, GDI_STR_ALIGN_L, color, "I" );
	}
	else {
		GDI_StringColor( x, y, GDI_STR_ALIGN_L, color, "D" );
	}
	
	GDI_SetTextColor(FontBackup.TextColor);
	GDI_SetBackColor(FontBackup.BackColor);
	GDI_SetFont(FontBackup.pFont);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Disk( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_Disk_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 27;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Com_Status( uint16_t x, uint16_t y, bool IsGray )
{
	vImage_InfoType		Img;
	
	if( IsGray == true ){
		Img.pTable = vImage_Com_Blue_Table;
	}
	else {
		Img.pTable = vImage_Com_Gray_Table;
	}
	
	Img.x = x;
	Img.y = y;
	Img.w = 26;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Intro( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_Intro_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 220;
	Img.h = 176;
	
	vDrawImage( &Img );
	
	HAL_Delay( 3000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Flow( uint16_t x, uint16_t y, float flow )
{
	vImage_InfoType		Img;
	const uint8_t		*pImg[4] = { vImage_Flow1_Table, vImage_Flow2_Table, vImage_Flow3_Table, vImage_Flow4_Table };
	bool				err = false;
	
	Img.x = x;
	Img.y = y;
	Img.w = 34;
	Img.h = 34;
	
	if		( flow >  749.f )	{ vFlowAniTime =  6; }
	else if	( flow >  249.f )	{ vFlowAniTime =  8; }
	else if	( flow >  149.f )	{ vFlowAniTime = 12; }
	else if	( flow >=  80.f )	{ vFlowAniTime = 30; }
	else						{ vFlowAniTime =  6, err = true; }
	
	if( vFlowAniTick ){
		vFlowAniTick--;
	}
	else {
		vFlowAniTick = vFlowAniTime;
		if( err == false ){
			vFlowFrameNo++;
		}
		Img.pTable = pImg[vFlowFrameNo & 3];
		vDrawImage( &Img );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_iSD( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_iSD_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 26;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_nSD( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_nSD_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 26;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Filter( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_Filter_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 26;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_iGas( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_iGas_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 26;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Normal( uint16_t x, uint16_t y )
{
	vImage_InfoType		Img;
	
	Img.pTable = vImage_Normal_Table;
	Img.x = x;
	Img.y = y;
	Img.w = 26;
	Img.h = 34;
	
	vDrawImage( &Img );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vImg_Bat( uint16_t x, uint16_t y, U8 Level, bool IsCharging )
{
	vImage_InfoType		Img;
	
	if( IsCharging == true ){
		if		( Level > 99 )	{ Img.pTable = vImage_Bat_CF_Table; }
		else if	( Level > 80 )	{ Img.pTable = vImage_Bat_C5_Table; }
		else if	( Level > 60 )	{ Img.pTable = vImage_Bat_C4_Table; }
		else if	( Level > 40 )	{ Img.pTable = vImage_Bat_C3_Table; }
		else if	( Level > 20 )	{ Img.pTable = vImage_Bat_C2_Table; }
		else					{ Img.pTable = vImage_Bat_C1_Table; }
	}
	else {
		if		( Level > 99 )	{ Img.pTable = vImage_Bat_NF_Table; }
		else if	( Level > 80 )	{ Img.pTable = vImage_Bat_N5_Table; }
		else if	( Level > 60 )	{ Img.pTable = vImage_Bat_N4_Table; }
		else if	( Level > 40 )	{ Img.pTable = vImage_Bat_N3_Table; }
		else if	( Level > 20 )	{ Img.pTable = vImage_Bat_N2_Table; }
		else if	( Level >  4 )	{ Img.pTable = vImage_Bat_N1_Table; }
		else					{ Img.pTable = vImage_Bat_N0_Table; }
	}
	
	Img.x = x;
	Img.y = y;
	Img.w = 16;
	Img.h = 27;
	
	vDrawImage( &Img );
}

