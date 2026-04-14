#define __VPOWER_OFF_C__
    #include "vpower_off.h"
#undef  __VPOWER_OFF_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gdi.h"
#include "vmeasuring.h"
#include "vlibrary.h"
#include "self_test.h"
#include "normal_mode.h"
#include "allbase_mode.h"
#include "vpassword_mode.h"
#include "function_mode.h"
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
#if 0
static vPoint	vp[30];
static vLine	vl[30];
#endif
vMessageBoxType			MsgBox;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vPowerOff( Power_CtrlType *pCtrl )
{
	char		str[8];
	
	sprintf( str, " - %1u - ", PwrCtrl.KeyScan.Item.CountDown );
	
	if( PwrCtrl.KeyScan.Item.CountDown == 3 ){
		MsgBox.ClearTitle = true;
		MsgBox.ClearMsg = true;
	}
	
	MsgBox.Title.pStr = "POWER OFF";
	MsgBox.Title.tClr = RGB.WHITE;
	MsgBox.Msg[0].pStr = str;
	MsgBox.Msg[0].tClr = RGB.YELLOW;
	
	MsgBox.LineColor = RGB.DARKRED;
	MsgBox.TboxColor = RGB.DARKRED;
	MsgBox.BackColor = 0x0800;
	MsgBox.Width = 130;                     
	MsgBox.MsgLine = 1;
	MsgBox.OutLine = 1;
	
	vMessageBox( &MsgBox );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vPowerOff_Low_Battery( void )
{
	GDI_FillRectColor( 30, 57, 160, 62, RGB.RED );
	GDI_DrawRectColor( 29, 56, 162, 64, RGB.WHITE );
	GDI_DrawRectColor( 33, 60, 154, 56, RGB.WHITE );
	GDI_SetFont( &Font11x16 );
	GDI_SetBackColor( RGB.RED );
	GDI_StringColor( 110, 80, GDI_STR_ALIGN_C, RGB.WHITE, "LOW BATTERY" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vPowerClr( void )
{
	GDI_FillRectColor( 45, 52, 130, 72, RGB.BLACK );
	
	switch( SystemState ){
		case SYSTEM_STATE_SELF_TEST:
			SelfHandle.TaskEvt = true;
			break;
			
		case SYSTEM_STATE_NORMAL:
			vMeasuring_DrawBase();
			NormHandle.TaskEvt = true;
			break;
			
		case SYSTEM_STATE_ALLBASE:
			AllBaseHandle.ViewEvt = true;
			break;
			
		case SYSTEM_STATE_PASSWORD:
			PwHandle.pMsgBox->ClearTitle = true;
			PwHandle.pMsgBox->ClearMsg = true;
			vPassword_Mode( &PwHandle );
			break;
			
		case SYSTEM_STATE_FUNCTION:
			FuncHandle.ViewEvt = true;
			break;
	}
}

