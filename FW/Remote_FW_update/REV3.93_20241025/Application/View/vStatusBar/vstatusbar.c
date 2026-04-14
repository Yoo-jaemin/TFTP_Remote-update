#define __VSTATUSBAR_C__
    #include "vstatusbar.h"
#undef  __VSTATUSBAR_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vlibrary.h"
#include "vimage.h"
#include "flow_control.h"
#include "rtc.h"
#include "micro_sd.h"
#include "rs485.h"
#include "normal_mode.h"
#include "option.h"
#include "power_control.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void vFlowRate( float Flow );
static void vStatusMessage( void );
static void vPower( float Power );
static void vTemperature( float Temp );
static void vCalendar( void );
/* End Function */

/* Start Variable */
static U16				Conlon_Color;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vStatusBar( void )
{
	Measuring_FilterItem	ft;
	
	if( SystemOption.Item.IsBatMode == true ){ // Battery Status Icon
		
		vStBar.BatCur.Item.Level		= PwrCtrl.BatLevel;
		vStBar.BatCur.Item.IsPresent	= PwrCtrl.BQ.Status.Item.VBUS_Present;
		
		if( vStBar.BatPrv.Byte != vStBar.BatCur.Byte ){
			vStBar.BatPrv.Byte  = vStBar.BatCur.Byte;
			
			vDrawImg.Bat( 170, 2, vStBar.BatCur.Item.Level, vStBar.BatCur.Item.IsPresent );
		}
	}
	else { // Com Status Icon
		if( vStBar.ComIconOut != vStBar.ComIconSet ){
			vStBar.ComIconOut  = vStBar.ComIconSet;
			vDrawImg.ComStatus( 163, 0, vStBar.ComIconOut );
		}
	}
	
	if( vStBar.DrawEvt == true ){ // 8 msec
		vStBar.DrawEvt = false;
		
		// Com Status Timer
		if( vStBar.ComIconTmr ){
			if( vStBar.ComIconTmr == COM_ICON_BLINK_INTERVAL ){
				vStBar.ComIconSet = true;
			}
			else if	( vStBar.ComIconTmr == (COM_ICON_BLINK_INTERVAL >> 1) ){
				vStBar.ComIconSet = false;
			}
			vStBar.ComIconTmr--;
		}
	
		RTC_Get_Calendar();
		
		// Sys Status Icon
		if( uSD_Handle.Scan.IsChanged ){
			uSD_Handle.Scan.IsChanged--;
			
			if( uSD_Handle.Scan.IsPresent == SD_PRESENT ){
				vStBar.SysIconSet = SYSTEM_STATUS_ICON_SD_PRESENT;
			}
			else {
				vStBar.SysIconSet = SYSTEM_STATUS_ICON_SD_NOT_PRESENT;
			}
		}
		else {
			ft = MeasHandle.Filter.Res;
#if 0
			ft.Bit.InterfGas_1 = false;
			ft.Bit.InterfGas_2 = false;
#endif
			if( ft.Word ){
				vStBar.SysIconSet = SYSTEM_STATUS_ICON_FILTER;
			}
#if 0
			else if( MeasHandle.Filter.Res.Bit.InterfGas_1 || MeasHandle.Filter.Res.Bit.InterfGas_2 ){
				vStBar.SysIconSet = SYSTEM_STATUS_ICON_INTGAS;
			}
#endif
			else {
				vStBar.SysIconSet = SYSTEM_STATUS_ICON_NORMAL;
			}
		}
		if( vStBar.SysIconOut != vStBar.SysIconSet ){
			vStBar.SysIconOut  = vStBar.SysIconSet;
			
			switch( vStBar.SysIconOut ){
				case SYSTEM_STATUS_ICON_SD_PRESENT		: vDrawImg.iSD		( 194, 0 ); break;
				case SYSTEM_STATUS_ICON_SD_NOT_PRESENT	: vDrawImg.nSD		( 194, 0 ); break;
				case SYSTEM_STATUS_ICON_FILTER			: vDrawImg.Filter	( 194, 0 ); break;
				case SYSTEM_STATUS_ICON_INTGAS			: vDrawImg.iGas		( 194, 0 ); break;
				case SYSTEM_STATUS_ICON_NORMAL			: vDrawImg.Normal	( 194, 0 ); break;
			}
		}
		
		// Flow Rate
		vDrawImg.Flow( 0, 0, MeasHandle.pCur->FlowRate );
		vStBar.FlowTmr--;
		if( vStBar.FlowTmr == 0 ){
			vStBar.FlowTmr =  5;
			vFlowRate( MeasHandle.pCur->FlowRate );
		}
		
		vStBar.InfoTmr--;
		if( vStBar.InfoTmr == 0 ){
			vStBar.InfoTmr = 125;
		
			// System Status Message
			vStatusMessage();
			
			// Power & Temperature
			vPower( MeasHandle.pCur->VSUP );
			vTemperature( MeasHandle.pCur->SysSenTemp );
		}
	}
	
	vCalendar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vFlowRate( float Flow )
{
	char		str[18];
	uint16_t	Color;
	U16			os = 0;
	
	if( SystemOption.Item.IsBatMode == true ){ os = 3; }
	
	if( Flow < 0.f ){ Flow = 0.f; }
	
	if		( Flow <   80.f ){ strcpy( str, "FLOW: Undr mL/min" ); }
	else if	( Flow > 1200.f ){ strcpy( str, "FLOW: Over mL/min" ); }
	else { sprintf( str, "FLOW: %4.0f mL/min", Flow ); }
	
	vGradientBar( os+39, 17, 118, 11, Flow / FLOW_CTRL_MAX * 100.f );
	
	if( MeasHandle.Filter.Res.Bit.FlowRate == true ){ Color = RGB.YELLOW; }
	else { Color = RGB.WHITE; }
	
	GDI_SetFont( &Font07x12 );
	GDI_SetBackColor( RGB.BLACK );
	GDI_StringColor( os+157, 3, GDI_STR_ALIGN_R, Color, str );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vStatusMessage( void )
{
	U8		DebugMode = 0;
	char	str[22] = "STATUS IS NOT DEFINED";
	U16		rgb;
	
	// Status Message
	if( NormHandle.KeyLock == true  ){
		rgb = RGB.SPRINGGREEN;
		strcpy( str, "LOCK MODE            " );
	}
	else {
		rgb = RGB.YELLOW;
		if( MeasHandle.Mode.Item.Maintenance == true ){
			if( MeasHandle.Test.Item.RemoteTest_CH1 | MeasHandle.Test.Item.RemoteTest_CH2 ){
				strcpy( str, "MT & REMOTE TEST MODE" );
			}
			else if( MeasHandle.Test.Item.WebTest_CH1 | MeasHandle.Test.Item.WebTest_CH2 ){
				strcpy( str, "MT & WEB TEST MODE   " );
			}
			else if( MeasHandle.Mode.Item.Engineering == true ){
				strcpy( str, "MT & ENGINEERING MODE" );
			}
			else {
				strcpy( str, "MAINTENANCE MODE     " );
			}
		}
		else {
			if( MeasHandle.Test.Item.RemoteTest_CH1 | MeasHandle.Test.Item.RemoteTest_CH2 ){
				strcpy( str, "REMOTE TEST MODE     " );
			}
			else if( MeasHandle.Test.Item.WebTest_CH1 | MeasHandle.Test.Item.WebTest_CH2 ){
				strcpy( str, "WEB TEST MODE        " );
			}
			else if( MeasHandle.Mode.Item.Engineering == true ){
				strcpy( str, "ENGINEERING MODE     " );
			}
			else {
#ifdef _DEBUG_BUILD
				DebugMode |= 1;
#endif
				if( MeasHandle.pSen->pCtg->Error.Bit.DebugMode == true ){ DebugMode |= 2; }
				if( MeasHandle.pSen->pCvt->Error.Bit.DebugMode == true ){ DebugMode |= 4; }
				switch( DebugMode ){
					case 0: strcpy( str, "NORMAL OPERATION MODE" ); rgb = RGB.LIGHTSKYBLUE; break;
					case 1: strcpy( str, "DEBUG [ MAIN ]       " ); break;
					case 2: strcpy( str, "DEBUG [ CT ]         " ); break;
					case 3: strcpy( str, "DEBUG [ MAIN & CT ]  " ); break;
					case 4: strcpy( str, "DEBUG [ IR ]         " ); break;
					case 5: strcpy( str, "DEBUG [ MAIN & IR ]  " ); break;
					case 6: strcpy( str, "DEBUG [ CT & IR ]    " ); break;
					case 7: strcpy( str, "DEBUG [ ALL ]        " ); break;
				}
			}
		}
	}
	GDI_SetFont( &Font07x12 );
	GDI_SetBackColor( RGB.BLACK );                                                       
	GDI_StringColor( 3, 150, GDI_STR_ALIGN_L, rgb, str );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vPower( float Power )
{
	char		str[10];
	
	GDI_SetFont( &Font07x12 );
	GDI_SetBackColor( RGB.BLACK );
	sprintf( str, "P. %4.1f V", Power );
	
	GDI_StringColor(  3, 163, GDI_STR_ALIGN_L, RGB.WHITE, str );
	GDI_StringColor( 66, 163, GDI_STR_ALIGN_L, RGB.DARKGRAY, " | " );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vTemperature( float Temp )
{
	char		str[10];
	
	GDI_SetFont( &Font07x12 );
	GDI_SetBackColor( RGB.BLACK );
	sprintf( str, "T.%5.1f`C", Temp );
	
	GDI_StringColor( 87, 163, GDI_STR_ALIGN_L, RGB.GOLD, str );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vCalendar( void )
{
	uint8_t		AP = 'A';
	char		StrHour[6];
	char		StrMinute[3];
	char		StrDate[9];
	
	GDI_SetFont( &Font07x12 );
	GDI_SetBackColor( RGB.BLACK );
	
	if( SysCalendar.SecondEvt == true ){
		SysCalendar.SecondEvt = false;
		
		if( Conlon_Color == RGB.WHITE ){
			Conlon_Color = RGB.LIGHTSLATEGRAY;
		}
		else {
			Conlon_Color = RGB.WHITE;
		}
		GDI_StringColor( 203, 150, GDI_STR_ALIGN_R, Conlon_Color, ":" );
	}
	
	if( SysCalendar.MinuteEvt == true ){
		SysCalendar.MinuteEvt = false;
		
		if( SysCalendar.Time24.Hours > 11 ){ AP = 'P'; }
	
		sprintf( StrHour, "%cM %02u", AP, SysCalendar.Time12.Hours );
		sprintf( StrMinute, "%02u", SysCalendar.Time12.Minutes );
		sprintf( StrDate, "%02u/%02u/%02u", SysCalendar.Date.Year, SysCalendar.Date.Month, SysCalendar.Date.Date );
		
		GDI_SetTextColor( RGB.WHITE );
		GDI_String( 196, 150, GDI_STR_ALIGN_R, StrHour );
		GDI_String( 217, 150, GDI_STR_ALIGN_R, StrMinute );         
		GDI_String( 217, 163, GDI_STR_ALIGN_R, StrDate );
	}
}


