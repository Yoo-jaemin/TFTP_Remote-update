#define __VMEASURING_C__
    #include "vmeasuring.h"
#undef  __VMEASURING_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vlibrary.h"
#include "vimage.h"
/* End Include */

/* Start Define */
#define AL1		( 0 )
#define AL2		( 1 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	bool						Draw;
	uint16_t					Back;
	uint16_t					GasName;
	uint16_t					HiScale;
	uint16_t					AlrInfo;
	uint16_t					GasConc;
}vMeasuring_ColorType;
/* End Struct */

/* Start Function */
static char *vStrHighScale( U8 i );
static char *vStrAlarmInfo( U8 i, U8 No );
static char *vStrMeasGas( U8 i );
static void vMeasuring_ColorSet( Measuring_HandleType *pMeas );
static void vMeasuring_SetBackColor_1CH( Measuring_HandleType *pMeas );
static void vMeasuring_SetBackColor_2CH( Measuring_HandleType *pMeas );
static void vMeasuring_BLK( void );
static void vMeasuring_1CH( Measuring_HandleType *pMeas );
static void vMeasuring_2CH( Measuring_HandleType *pMeas );
/* End Function */

/* Start Variable */
#if 0
static vPoint	vp[30];
static vLine	vl[30];
#endif

static char						*pMainUnit[MEASURING_UNIT_MAX] = { "PPB", "PPM", "LEL", "VOL" };
static char						*pAuxiUnit[MEASURING_UNIT_MAX] = { " ", " ", "%", "%" };
static vMeasuring_ColorType		vColor[MEASURING_GAS_OUTPUT_MAX];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vMeasuring( void )
{
	vMeasuring_ColorSet( &MeasHandle );
	
	switch( MeasHandle.Conf.OutMax ){
		case  1: vMeasuring_1CH( &MeasHandle );	break;
		case  2: vMeasuring_2CH( &MeasHandle );	break;
		default: vMeasuring_BLK();				break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        
void vMeasuring_DrawBase( void )
{
	vColor[0].Back = RGB.BLACK;
	vColor[0].GasName = RGB.YELLOW;
	vColor[0].HiScale = RGB.WHITE;
	vColor[0].AlrInfo = RGB.SPRINGGREEN;
	vColor[0].GasConc = RGB.WHITE;
	vColor[0].Draw = true;
	
	vColor[1].Back = RGB.BLACK;
	vColor[1].GasName = RGB.YELLOW;
	vColor[1].HiScale = RGB.WHITE;
	vColor[1].AlrInfo = RGB.SPRINGGREEN;
	vColor[1].GasConc = RGB.WHITE;
	vColor[1].Draw = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *vStrGasConc( U16 Dec, float Gas )
{                             
	static char	Ret[6];
	
	switch( Dec ){
		case 0: sprintf( Ret, "%5.0f", Gas * 1.000f ); break;
		case 1: sprintf( Ret, "%5.1f", Gas * 0.100f ); break;
		case 2: sprintf( Ret, "%5.2f", Gas * 0.010f ); break;
		case 3: sprintf( Ret, "%5.3f", Gas * 0.001f ); break;
	}
	
	return Ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        
static char *vStrHighScale( U8 i )
{
	static char		Ret[11];
	
	sprintf( Ret, "H-S:%5s ", vStrGasConc( MeasHandle.pGas[i]->Inf.Dp, MeasHandle.pGas[i]->Inf.HighScale ) );
	
	return Ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        
static char *vStrAlarmInfo( U8 i, U8 No )
{
	static char		Ret[11];
	S16				Level;
	
	if( No == AL1 ) { Level = MeasHandle.pAlr[i]->Inf._1stLevel; }
	else			{ Level = MeasHandle.pAlr[i]->Inf._2ndLevel; }
	
	sprintf( Ret, "AL%u:%5s", No+1, vStrGasConc( MeasHandle.pGas[i]->Inf.Dp, Level ) );
	
	return Ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char *vStrMeasGas( U8 i )
{
	static char			Ret[6];
	
	if( SystemError.Code[i] ){
		sprintf( Ret, "E-%02u", SystemError.Code[i] );
	}
	else {
		if( MeasHandle.Mode.Item.Engineering == true ){
			switch( MeasHandle.pGas[i]->Inf.Dp ){
				case 0: sprintf( Ret, "%4.0f", MeasHandle.pGas[i]->Res * 1.000f ); break;
				case 1: sprintf( Ret, "%5.1f", MeasHandle.pGas[i]->Res * 0.100f ); break;
				case 2: sprintf( Ret, "%5.2f", MeasHandle.pGas[i]->Res * 0.010f ); break;
				case 3: sprintf( Ret, "%5.3f", MeasHandle.pGas[i]->Res * 0.001f ); break;
			}
		}
		else {
			if( MeasHandle.pAlr[i]->Item.Bit.aOvr ){ sprintf( Ret, "OVER" ); }
			else {
				switch( MeasHandle.pGas[i]->Inf.Dp ){
					case 0: sprintf( Ret, "%4.0f", MeasHandle.pGas[i]->Dsp * 1.000f ); break;
					case 1: sprintf( Ret, "%5.1f", MeasHandle.pGas[i]->Dsp * 0.100f ); break;
					case 2: sprintf( Ret, "%5.2f", MeasHandle.pGas[i]->Dsp * 0.010f ); break;
					case 3: sprintf( Ret, "%5.3f", MeasHandle.pGas[i]->Dsp * 0.001f ); break;
				}
			}
		}
	}
	
	return Ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        
static void vMeasuring_ColorSet( Measuring_HandleType *pMeas )
{
	U8		i;
	
	for( i=0; i<pMeas->Conf.OutMax; i++ ){
		if( SystemError.Code[i] ){
			if( vColor[i].Back != RGB.YELLOW ){
				vColor[i].Draw = true;
				vColor[i].Back = RGB.YELLOW;
				vColor[i].GasName = RGB.BLACK;
				vColor[i].HiScale = RGB.MEDIUMBLUE;
				vColor[i].AlrInfo = RGB.TEAL;
				vColor[i].GasConc = RGB.MEDIUMBLUE;
			}
		}
		else {
			if( (pMeas->pAlr[i]->Item.Bit.a1st == true) || (pMeas->pAlr[i]->Item.Bit.a2nd == true) ){
				if( vColor[i].Back != RGB.RED ){
					vColor[i].Draw = true;
					vColor[i].Back = RGB.RED;
					vColor[i].GasName = RGB.YELLOW;
					vColor[i].HiScale = RGB.WHITE;
					vColor[i].AlrInfo = RGB.SPRINGGREEN;
					vColor[i].GasConc = RGB.WHITE;
				}
			}
			else {
				if( vColor[i].Back != RGB.BLACK ){
					vColor[i].Draw = true;
					vColor[i].Back = RGB.BLACK;
					vColor[i].GasName = RGB.YELLOW;
					vColor[i].HiScale = RGB.WHITE;
					vColor[i].AlrInfo = RGB.SPRINGGREEN;
					vColor[i].GasConc = RGB.WHITE;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_SetBackColor_1CH( Measuring_HandleType *pMeas )
{
	if( vColor[0].Draw == true ){
		vColor[0].Draw = false;
		
		GDI_SetTextColor( vColor[0].Back );
		GDI_FillRect ( 0,  35, 220, 109 );
		GDI_SetTextColor( RGB.DARKGRAY );
		GDI_DrawHLine( 0,  34, 220 );
		GDI_DrawHLine( 0, 144, 220 );
	}
}
                             
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_SetBackColor_2CH( Measuring_HandleType *pMeas )
{
	bool	DrawCenterLine = false;
	
	if( vColor[0].Draw == true ){
		vColor[0].Draw = false;
		
		GDI_SetTextColor( vColor[0].Back );
		GDI_FillRect ( 0,  35, 220, 54 );
		GDI_SetTextColor( RGB.DARKGRAY );
		GDI_DrawHLine( 0,  34, 220 );
		GDI_DrawHLine( 0,  89, 220 );
		DrawCenterLine = true;
	}
	if( vColor[1].Draw == true ){
		vColor[1].Draw = false;
		
		GDI_SetTextColor( vColor[1].Back );
		GDI_FillRect ( 0,  90, 220, 54 );
		GDI_SetTextColor( RGB.DARKGRAY );
		if( DrawCenterLine == false ){ GDI_DrawHLine( 0, 89, 220 ); }
		GDI_DrawHLine( 0, 144, 220 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_BLK( void )
{
	GDI_SetTextColor( RGB.BLACK );
	GDI_FillRect ( 0,  35, 220, 109 );
	
	GDI_SetTextColor( RGB.DARKGRAY );
	GDI_DrawHLine( 0,  34, 220 );
	GDI_DrawHLine( 0, 144, 220 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
static void vMeasuring_1CH( Measuring_HandleType *pMeas )
{
	uint16_t	rgb[2];
	
	vMeasuring_SetBackColor_1CH( pMeas );
	
	GDI_SetBackColor( vColor[0].Back );
	
	GDI_SetFont( &Font07x12 );
	GDI_StringColor( 3, 45, GDI_STR_ALIGN_L, vColor[0].GasName, (char *)pMeas->pGas[0]->Inf.GasName );
	GDI_StringColor( 3, 57, GDI_STR_ALIGN_L, vColor[0].HiScale, vStrHighScale( 0 ) );
	GDI_StringColor( 3, 69, GDI_STR_ALIGN_L, vColor[0].AlrInfo, vStrAlarmInfo( 0, AL1 ) );
	GDI_StringColor( 3, 81, GDI_STR_ALIGN_L, vColor[0].AlrInfo, vStrAlarmInfo( 0, AL2 ) );
	
	if( vColor[0].Back == RGB.RED ){
		rgb[AL1] = RGB.WHITE;
		rgb[AL2] = RGB.WHITE;
	}
	else {
		if( pMeas->pAlr[0]->Inf._1stTypeIncrease == true ){ rgb[AL1] = RGB.RED; }
		else { rgb[AL1] = RGB.BLUE; }
		if( pMeas->pAlr[0]->Inf._2ndTypeIncrease == true ){ rgb[AL2] = RGB.RED; }
		else { rgb[AL2] = RGB.BLUE; }
	}
	vDrawImg.AlarmArrow( pMeas->pAlr[0]->Inf._1stTypeIncrease, 66, 68, rgb[AL1] );
	vDrawImg.AlarmArrow( pMeas->pAlr[0]->Inf._2ndTypeIncrease, 66, 80, rgb[AL2] );
	
	GDI_SetFont( &Font29x46 );
	GDI_StringColor( 194,  92, GDI_STR_ALIGN_R, vColor[0].GasConc, vStrMeasGas( 0 ) );
	GDI_SetFont( &Font11x16 );
	GDI_StringColor( 216, 107, GDI_STR_ALIGN_R, vColor[0].GasConc, (char *)pAuxiUnit[pMeas->pGas[0]->Inf.Unit] );
	GDI_SetFont( &Font07x12 );
	GDI_StringColor( 216, 122, GDI_STR_ALIGN_R, vColor[0].GasConc, (char *)pMainUnit[pMeas->pGas[0]->Inf.Unit] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_2CH( Measuring_HandleType *pMeas )
{
	U16			Yos;
	U8			i;
	uint16_t	rgb[2];
	
	vMeasuring_SetBackColor_2CH( pMeas );
	
	for( i=0; i<2; i++ ){
		
		Yos = i * 55;
		
		GDI_SetBackColor( vColor[i].Back );
		
		GDI_SetFont( &Font07x12 );
		GDI_StringColor( 3, 40+Yos, GDI_STR_ALIGN_L, vColor[i].GasName, (char *)pMeas->pGas[i]->Inf.GasName );
		GDI_StringColor( 3, 51+Yos, GDI_STR_ALIGN_L, vColor[i].HiScale, vStrHighScale( i ) );
		GDI_StringColor( 3, 62+Yos, GDI_STR_ALIGN_L, vColor[i].AlrInfo, vStrAlarmInfo( i, AL1 ) );
		GDI_StringColor( 3, 73+Yos, GDI_STR_ALIGN_L, vColor[i].AlrInfo, vStrAlarmInfo( i, AL2 ) );
		
		if( vColor[i].Back == RGB.RED ){
			rgb[AL1] = RGB.WHITE;
			rgb[AL2] = RGB.WHITE;
		}
		else {
			if( pMeas->pAlr[i]->Inf._1stTypeIncrease == true ){ rgb[AL1] = RGB.RED; }
			else { rgb[AL1] = RGB.BLUE; }
			if( pMeas->pAlr[i]->Inf._2ndTypeIncrease == true ){ rgb[AL2] = RGB.RED; }
			else { rgb[AL2] = RGB.BLUE; }
		}
		vDrawImg.AlarmArrow( pMeas->pAlr[i]->Inf._1stTypeIncrease, 66, 62+Yos, rgb[AL1] );
		vDrawImg.AlarmArrow( pMeas->pAlr[i]->Inf._2ndTypeIncrease, 66, 73+Yos, rgb[AL2] );
		
		GDI_SetFont( &Font29x46 );
		GDI_StringColor( 197, 43+Yos, GDI_STR_ALIGN_R, vColor[i].GasConc, vStrMeasGas( i ) );
		GDI_SetFont( &Font11x16 );
		GDI_StringColor( 217, 58+Yos, GDI_STR_ALIGN_R, vColor[i].GasConc, (char *)pAuxiUnit[pMeas->pGas[i]->Inf.Unit] );
		GDI_SetFont( &Font07x12 );
		GDI_StringColor( 217, 73+Yos, GDI_STR_ALIGN_R, vColor[i].GasConc, (char *)pMainUnit[pMeas->pGas[i]->Inf.Unit] );
	}
}
