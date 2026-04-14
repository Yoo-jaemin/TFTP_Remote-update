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
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static char *vStrMeasGas( Measuring_GasType *pGas, Measuring_AlarmItem Alarm );
static void vMeasuring_SetBackColor_1CH( Measuring_HandleType *pMeas );
static void vMeasuring_SetBackColor_2CH( Measuring_HandleType *pMeas );
static void vMeasuring_BLK( void );
static void vMeasuring_1CH( Measuring_HandleType *pMeas );
static void vMeasuring_2CH( Measuring_HandleType *pMeas );
/* End Function */

/* Start Variable */
static const char	*sp3330_GasName[] =
{
	"AUX-3330  ",
	"CH4       ",
	"LNG       "
};

static const char	*sp9440_GasName[] =
{                
	"AUX-9440  ", 
	"CH3F      "
};

static const char	*sp8000_GasName[] =
{
	"AUX-8000  ",
	"CH4       "
};

static const char	*sp4850_GasName[] =
{
	"AUX-4850  ",
	"COS       ",
	"COS       "
};

static const char	*spAuxiUnit[] =
{
	"  ",
	"  ",
	" %",
	" %"
};

static const char	*spMainUnit[] =
{
	"PPB",
	"PPM",
	"LEL",
	"VOL"
};

static U8				vAlarm;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vMeasuring( Measuring_HandleType *pMeas )
{
	switch( pMeas->Ch.MainNo ){
		case  1: vMeasuring_1CH( pMeas );	break;
		case  2: vMeasuring_2CH( pMeas );	break;
		default: vMeasuring_BLK();			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *vStrMainUnit( Measuring_GasInfo *pInf )
{                             
	const	char		*pErr = "---";
	static	char		*pRet;
	
	if( pInf->Unit < MEASURING_UNIT_MAX )	{ pRet = (char *)spMainUnit[pInf->Unit];	}
	else									{ pRet = (char *)pErr;						}
	
	return pRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *vStrAuxiUnit( Measuring_GasInfo *pInf )
{                             
	const	char		*pErr = "---";
	static	char		*pRet;
	
	if( pInf->Unit < MEASURING_UNIT_MAX )	{ pRet = (char *)spAuxiUnit[pInf->Unit];	}
	else									{ pRet = (char *)pErr;						}
	
	return pRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *vStrGasName( Measuring_GasInfo *pInf )
{
	const	char	*pNone = "EMPTY     ";
	static	char	*pRet;
	
	switch( pInf->Method ){
		case MEASURING_METHOD_IR:
			switch( pInf->SenNo ){
				case IR_SENSOR_3330: pRet = (char *)sp3330_GasName[pInf->GasNo]; break;
				case IR_SENSOR_9440: pRet = (char *)sp9440_GasName[pInf->GasNo]; break;
				case IR_SENSOR_8000: pRet = (char *)sp8000_GasName[pInf->GasNo]; break;
				case IR_SENSOR_4850: pRet = (char *)sp4850_GasName[pInf->GasNo]; break;
			}
			break;
			
		default: pRet = (char *)pNone; break;
#if 0
		case MEASURING_METHOD_EC:
		case MEASURING_METHOD_CB:
		case MEASURING_METHOD_PID:
		case MEASURING_METHOD_SC:
		case MEASURING_METHOD_GALVANIC:
			break;
#endif
	}
	
	return pRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        
char *vStrHighScale( Measuring_GasInfo *pInf )
{
	static char		str[11];
	
	sprintf( str, "H-S:%5s ", vStrGasValue(pInf->Decimal, pInf->HighScale) );
	
	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        
char *vStrAlarmInfo( U8 Step, U16 Resolution, float Val )
{
	static char		str[11];
	
	sprintf( str, "AL%u:%5s", Step+1, vStrGasValue(Resolution, Val) );
	
	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *vStrGasValue( U16 Dec, float Gas )
{                             
	static char	str[6];
	
	switch( Dec ){
		case 0: sprintf( str, "%5.0f", Gas * 1.000f ); break;
		case 1: sprintf( str, "%5.1f", Gas * 0.100f ); break;
		case 2: sprintf( str, "%5.2f", Gas * 0.010f ); break;
		case 3: sprintf( str, "%5.3f", Gas * 0.001f ); break;
	}
	
	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char *vStrMeasGas( Measuring_GasType *pGas, Measuring_AlarmItem Alarm )
{                             
	static char		str[6];
	
	if( SystemError.Word ){
		sprintf( str, "E-01" );
	}
	else {
		if( MeasHandle.IsEngineerMode == true ){
			switch( pGas->Inf.Decimal ){
				case 0: sprintf( str, "%4.0f", pGas->Res * 1.000f ); break;
				case 1: sprintf( str, "%5.1f", pGas->Res * 0.100f ); break;
				case 2: sprintf( str, "%5.2f", pGas->Res * 0.010f ); break;
				case 3: sprintf( str, "%5.3f", pGas->Res * 0.001f ); break;
			}
		}
		else {
			if		( Alarm.Bit.aOvr ){ sprintf( str, "OVER" ); }
			else if	( Alarm.Bit.aUdr ){ sprintf( str, "UDER" ); }
			else {
				switch( pGas->Inf.Decimal ){
					case 0: sprintf( str, "%4.0f", pGas->Out * 1.000f ); break;
					case 1: sprintf( str, "%5.1f", pGas->Out * 0.100f ); break;
					case 2: sprintf( str, "%5.2f", pGas->Out * 0.010f ); break;
					case 3: sprintf( str, "%5.3f", pGas->Out * 0.001f ); break;
				}
			}
		}
	}
	
	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_BLK( void )
{
	GDI_SetTextColor( RGB.BLACK );
	GDI_FillRect ( 0,  35, 220, 105 );
	
	GDI_SetTextColor( RGB.GRAY );
	GDI_DrawHLine( 0,  34, 220 );
	GDI_DrawHLine( 0, 140, 220 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
static void vMeasuring_1CH( Measuring_HandleType *pMeas )
{
	vPoint					vpConc		= { 194,  38 };
	vPoint					vpMainUnit	= { 216,  68 };
	vPoint					vpAuxiUnit	= { 216,  53 };
	vPoint					vpGasName	= {   3,  44 };
	vPoint					vpHighScale	= {   3,  56 };
	vPoint					vpAlarmInfo	= {   3,  68 };
	U16						vpOs		= (  53 );
	
	U16						Dec			= pMeas->pGas[0]->Inf.Decimal;
	
	S16						aLv[2]		= { pMeas->Alr[0].Inf._1stLevel, pMeas->Alr[0].Inf._2ndLevel };
	U16						aTy[2]		= { pMeas->Alr[0].Inf._1stTypeIncrease, pMeas->Alr[0].Inf._2ndTypeIncrease };
	
	uint16_t				aColor[2];
	
	vMeasuring_SetBackColor_1CH( pMeas );
	
	GDI_SetBackColor( pMeas->Alr[0].View.Back );
	
	GDI_SetFont( &Font07x12 );
	GDI_SetTextColor( pMeas->Alr[0].View.GasName );
	GDI_String( vpGasName.x, vpGasName.y, GDI_STR_ALIGN_L, vStrGasName( &pMeas->pGas[0]->Inf ) );
	
	GDI_SetFont( &Font29x48 );
	GDI_SetTextColor( pMeas->Alr[0].View.Text );
	GDI_String( vpConc.x, vpConc.y+vpOs, GDI_STR_ALIGN_R, vStrMeasGas( pMeas->pGas[0], pMeas->Alr[0].Item) );
	
	GDI_SetFont( &Font11x16 );
	GDI_String( vpAuxiUnit.x, vpAuxiUnit.y+vpOs, GDI_STR_ALIGN_R, vStrAuxiUnit( &pMeas->pGas[0]->Inf ) );
	GDI_SetFont( &Font07x12 );
	GDI_String( vpMainUnit.x, vpMainUnit.y+vpOs, GDI_STR_ALIGN_R, vStrMainUnit( &pMeas->pGas[0]->Inf ) );
	
	GDI_String( vpHighScale.x, vpHighScale.y, GDI_STR_ALIGN_L, vStrHighScale( &pMeas->pGas[0]->Inf ) );
	
	GDI_SetTextColor( pMeas->Alr[0].View.Alarm );
	GDI_String( vpAlarmInfo.x, vpAlarmInfo.y   , GDI_STR_ALIGN_L, vStrAlarmInfo(0, Dec, aLv[0]) );
	GDI_String( vpAlarmInfo.x, vpAlarmInfo.y+12, GDI_STR_ALIGN_L, vStrAlarmInfo(1, Dec, aLv[1]) );
	
	if( pMeas->Alr[0].View.Back == RGB.RED ){
		aColor[0] = RGB.WHITE;
		aColor[1] = RGB.WHITE;
	}
	else {
		if( aTy[0] == true ){ aColor[0] = RGB.RED; }
		else { aColor[0] = RGB.BLUE; }
		if( aTy[1] == true ){ aColor[1] = RGB.RED; }
		else { aColor[1] = RGB.BLUE; }
	}
	vDrawImg.Arrow( aTy[0], vpAlarmInfo.x+63, vpAlarmInfo.y   , aColor[0] );
	vDrawImg.Arrow( aTy[1], vpAlarmInfo.x+63, vpAlarmInfo.y+12, aColor[1] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_2CH( Measuring_HandleType *pMeas )
{     
	vPoint					vpConc		=	{ 194,  38 };
	vPoint					vpMainUnit	=	{ 216,  68 };
	vPoint					vpAuxiUnit	=	{ 216,  53 };
	vPoint					vpGasName	=	{   3,  44 };
	vPoint					vpHighScale	=	{   3,  56 };
	vPoint					vpAlarmInfo	=	{   3,  68 };
	U16						vpOs		=	(  53 );
	
	U16						Dec[2]		=	{ pMeas->pGas[0]->Inf.Decimal, pMeas->pGas[1]->Inf.Decimal };
	
	S16						aLv[2][2]	=	{{ pMeas->Alr[0].Inf._1stLevel, pMeas->Alr[0].Inf._2ndLevel },
											 { pMeas->Alr[1].Inf._1stLevel, pMeas->Alr[1].Inf._2ndLevel }};
	U16						aTy[2][2]	=	{{ pMeas->Alr[0].Inf._1stTypeIncrease, pMeas->Alr[0].Inf._2ndTypeIncrease },
											 { pMeas->Alr[1].Inf._1stTypeIncrease, pMeas->Alr[1].Inf._2ndTypeIncrease }};
	uint16_t				aColor[2][2];	
	
	U8			i;
	
	vMeasuring_SetBackColor_2CH( pMeas );
	
	for( i=0; i<2; i++ ){
		
		GDI_SetBackColor( pMeas->Alr[i].View.Back );
		
		GDI_SetFont( &Font07x12 );
		GDI_SetTextColor( pMeas->Alr[i].View.GasName );
		GDI_String( vpGasName.x, vpGasName.y, GDI_STR_ALIGN_L, vStrGasName( &pMeas->pGas[i]->Inf ) );
		
		GDI_SetFont( &Font29x48 );
		GDI_SetTextColor( pMeas->Alr[i].View.Text );
		GDI_String( vpConc.x, vpConc.y, GDI_STR_ALIGN_R, vStrMeasGas( pMeas->pGas[i], pMeas->Alr[i].Item ) );
		                    
		GDI_SetFont( &Font11x16 );
		GDI_String( vpAuxiUnit.x, vpAuxiUnit.y, GDI_STR_ALIGN_R, vStrAuxiUnit( &pMeas->pGas[i]->Inf ) );
		GDI_SetFont( &Font07x12 );
		GDI_String( vpMainUnit.x, vpMainUnit.y, GDI_STR_ALIGN_R, vStrMainUnit( &pMeas->pGas[i]->Inf ) );
		
		GDI_String( vpHighScale.x, vpHighScale.y, GDI_STR_ALIGN_L, vStrHighScale( &pMeas->pGas[i]->Inf ) );
		
		GDI_SetTextColor( pMeas->Alr[i].View.Alarm );
		GDI_String( vpAlarmInfo.x, vpAlarmInfo.y, GDI_STR_ALIGN_L, vStrAlarmInfo(vAlarm, Dec[i], aLv[i][vAlarm]) );
		
		if( pMeas->Alr[i].View.Back == RGB.RED ){
			aColor[i][vAlarm] = RGB.WHITE;
		}
		else {
			if( aTy[i][vAlarm] == true ){ aColor[i][vAlarm] = RGB.RED; }
			else { aColor[i][vAlarm] = RGB.BLUE; }
		}
		
		vDrawImg.Arrow( aTy[i][vAlarm], vpAlarmInfo.x+63, vpAlarmInfo.y, aColor[i][vAlarm] );
		
		vpConc.y		+= vpOs;
		vpMainUnit.y	+= vpOs;
		vpAuxiUnit.y	+= vpOs;
		vpGasName.y		+= vpOs;
		vpHighScale.y	+= vpOs;
		vpAlarmInfo.y	+= vpOs;
	}
	
	vAlarm ^=1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_SetBackColor_1CH( Measuring_HandleType *pMeas )
{
	if( pMeas->Alr[0].View.Draw == true ){
		pMeas->Alr[0].View.Draw = false;
		
		GDI_SetTextColor( pMeas->Alr[0].View.Back );
		GDI_FillRect ( 0,  35, 220, 105 );
	}
		
	GDI_SetTextColor( RGB.GRAY );
	GDI_DrawHLine( 0,  34, 220 );
	GDI_DrawHLine( 0, 140, 220 );
	
}
                             
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vMeasuring_SetBackColor_2CH( Measuring_HandleType *pMeas )
{
	if( pMeas->Alr[0].View.Draw == true ){
		pMeas->Alr[0].View.Draw = false;
		
		GDI_SetTextColor( pMeas->Alr[0].View.Back );
		GDI_FillRect ( 0,  35, 220, 52 );
	}
	if( pMeas->Alr[1].View.Draw == true ){
		pMeas->Alr[1].View.Draw = false;
		
		GDI_SetTextColor( pMeas->Alr[1].View.Back );
		GDI_FillRect ( 0,  88, 220, 52 );
	}
	
	GDI_SetTextColor( RGB.GRAY );
	GDI_DrawHLine( 0,  34, 220 );
	GDI_DrawHLine( 0,  87, 220 );
	GDI_DrawHLine( 0, 140, 220 );
}
