#define __FUNCTION_MODE_C__
    #include "function_mode.h"
#undef  __FUNCTION_MODE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */          
#include "vfunction_mode.h"
#include "normal_mode.h"
#include "pump.h"
#include "func_measuring.h"
#include "func_flowset.h"
#include "func_interface.h"
#include "func_system.h"
#include "func_test.h"
#include "func_factory.h"
#include "measuring.h"
#include "ma_output.h"
#include "option.h"
#include "iwdg.h"
#include "lcd.h"
#include "buzzer.h"
/* End Include */

/* Start Define */
#define FUNCTION_MODE_QUIT_TIME		( 0x80 + 25 - 1 ) // 40 msec * 25 = 1000 msec = 1 sec
#define FUNCTION_MODE_VIEW_TIME		( 0x80 +  5 - 1 ) // 40 msec *  5 =  200 msec
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Function_Quit_Timer( void );
static void KeyProcess( void );
/* End Function */

/* Start Variable */
static void (*FuncItem[FUNC_ITEM_LV1_MAX])(Func_HandleType *) =
{
	Func_Measuring,
	Func_FlowSet,
	Func_Interface,
	Func_System,
	Func_Test,
	Func_Factory
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Enter_Function_Mode( void )
{
	U8		i;
	
	IWDG_Reload_Counter_Set( 10 );
	
	RelayDrive.AL1.Set = false;
	RelayDrive.AL2.Set = false;
	RelayDrive.TRB.Set = false;
	
	LedDrive.Set.Bit.AL1 = false;
	LedDrive.Set.Bit.AL2 = false;
	LedDrive.Set.Bit.TRB = false;
	
	MeasHandle.pAlr[0]->Latched = false;
	MeasHandle.pAlr[1]->Latched = false;
	
	MeasHandle.pAlr[0]->Item.Byte = 0;
	MeasHandle.pAlr[1]->Item.Byte = 0;
	
	if( SystemError.Code[0] == 30 ){ SystemError.Code[0] = 0; }
	if( SystemError.Code[1] == 30 ){ SystemError.Code[1] = 0; }
	
	FuncHandle.QuitSec = 60;
	FuncHandle.QuitTmr = FUNCTION_MODE_QUIT_TIME;
	FuncHandle.ViewTmr = FUNCTION_MODE_VIEW_TIME;
	FuncHandle.ViewEvt = true;
	
	FuncHandle.Item.Lv1 = 0;
	FuncHandle.Item.Lv2 = 0;
	FuncHandle.Item.Lv3 = 0;
	FuncHandle.Item.Lv4 = 0;
	FuncHandle.Item.Lv5 = 0;
	
	GDI_Clear( RGB.BLACK );
	
	LcdDrive.BL_Init();
	LcdDrive.Brightness_Set( LcdDrive.BrightnessLevel );
	
	FuncHandle.pMsgBox				= &vMsgBox;
	FuncHandle.pMsgBox->ClearTitle	= true;
	FuncHandle.pMsgBox->ClearMsg	= true;
	FuncHandle.pMsgBox->Width		= 216;
	FuncHandle.pMsgBox->MsgLine		= 5;
	FuncHandle.pMsgBox->OutLine		= 6;
	
	vFuncItemSet( &FuncHandle );
	
	FuncHandle.Level = FUNC_LEVEL_1;
	
	FuncHandle.State.Bit.IsEntered = true;
	
	FuncHandle.mA[0] = 3000;
	FuncHandle.mA[1] = 3000;
	
	if( MeasHandle.Mode.Item.Maintenance == true ){
		mA_Handle[0].Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1];
		mA_Handle[1].Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2];
	}
	else {
		mA_Handle[0].Set = FuncHandle.mA[0];
		mA_Handle[1].Set = FuncHandle.mA[1];
	}
	
	for( i=0; i<KEY_INDEX_MAX; i++ ){
		KeyScan.Tick[i].Long = KEY_SCAN_MSEC( 1000 );
	}
	
	KeyScan.Tick[KEY_INDEX_UP  ].IsRepeatMode = true;
	KeyScan.Tick[KEY_INDEX_DOWN].IsRepeatMode = true;
	
	Buzzer_Stop();
	
	SystemState = SYSTEM_STATE_FUNCTION;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Function_Mode_Task( void )
{
	FuncHandle.KeyData = KeyScan.Data;
	FuncHandle.KeyType = KeyScan.Type;
	
	KeyScan.Data = NoKey;
	
	if( FuncHandle.TaskEvt == true ){ RTC_Get_Calendar(); }
	
	if( FuncHandle.KeyData != NoKey ){
		FuncHandle.ViewEvt = true;
		FuncHandle.ViewTmr = FUNCTION_MODE_VIEW_TIME;
		if( (FuncHandle.KeyData == Up) || (FuncHandle.KeyData == Down) ){
			FuncHandle.ViewBlk = false;
			FuncHandle.ViewTmr += 10;
		}
	}
	
	if( FuncHandle.Level == FUNC_LEVEL_1 ){
		KeyProcess();
		vFuncItemSet( &FuncHandle );
	}
	else {
		FuncItem[FuncHandle.Item.Lv1]( &FuncHandle );
	}
	
	Function_Quit_Timer();
	
	FuncHandle.ViewEvt = false;
	FuncHandle.TaskEvt = false;
	FuncHandle.KeyData = NoKey;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Function_Mode_Timer( void )
{
	if( SystemState != SYSTEM_STATE_FUNCTION ){ return; }
	
	FuncHandle.TaskEvt = true;
	
	if( FuncHandle.QuitTmr & 0x80 ){
		if( FuncHandle.QuitTmr == 0x80 ){
			FuncHandle.QuitTmr = FUNCTION_MODE_QUIT_TIME;
			
			FuncHandle.QuitSec--;
			if( FuncHandle.QuitSec == 0 ){
				FuncHandle.QuitTmr = 0;
				FuncHandle.ViewTmr = 0;
				FuncHandle.State.Byte = 0;
				FuncHandle.ViewEvt = false;
				FuncHandle.TaskEvt = false;
				FuncHandle.KeyData = NoKey;
				Enter_Normal_Mode();
			}
		}
		else {
			FuncHandle.QuitTmr--;
		}
	}
	
	if( FuncHandle.ViewTmr & 0x80 ){
		if( FuncHandle.ViewTmr == 0x80 ){
			FuncHandle.ViewTmr = FUNCTION_MODE_VIEW_TIME;
			
			FuncHandle.ViewEvt  = true;
			FuncHandle.ViewBlk ^= true;
		}
		else {
			FuncHandle.ViewTmr--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveFuncTop( void )
{
	vFuncItemSet( &FuncHandle );
	
	FuncHandle.ViewEvt = false;
	FuncHandle.Level = FUNC_LEVEL_1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_fSum( float lmt, float inc, float *pf )
{
	float	sum = *pf + inc;
	
	if( inc < 0.f ){
		if( sum < lmt )	{ *pf = lmt; }
		else			{ *pf = sum; }
	}
	else {
		if( sum > lmt )	{ *pf = lmt; }
		else			{ *pf = sum; }
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_iSum( S16 lmt, S16 inc, S16 *pf )
{
	S16 sum = *pf + inc;
	
	if( inc < 0 ){
		if( sum < lmt )	{ *pf = lmt; }
		else			{ *pf = sum; }
	}
	else {
		if( sum > lmt )	{ *pf = lmt; }
		else			{ *pf = sum; }
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_uInc( U16 lmt, U16 inc, U16 *pw )
{
	U32	sum = *pw + inc;
	
	if( sum > lmt )	{ *pw = lmt; }
	else			{ *pw = sum; }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_uDec( U16 lmt, U16 dec, U16 *pw )
{
	U16	sum;
	
	if( dec > *pw )	{ sum = 0; }
	else			{ sum = *pw - dec; }
	
	if( sum < lmt )	{ *pw = lmt; }
	else			{ *pw = sum; }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Function_Quit_Timer( void )
{
#ifdef _DEBUG_BUILD
	U16 Second = 3600;
#else
	U16 Second = 60;
#endif
	
	if( (SystemState == SYSTEM_STATE_FUNCTION) && (FuncHandle.KeyData != NoKey) ){
		
		if( FuncHandle.Level > FUNC_LEVEL_2 ){
			
			switch( FuncHandle.Item.Lv1 ){
				case FUNC_ITEM_LV1_FLOW_SET:
					switch( FuncHandle.Item.Lv2 ){
						case 0: // Calibration
							Second = 3600;
							break;
					}
					break;
					
				case FUNC_ITEM_LV1_INTERFACE:
					switch( FuncHandle.Item.Lv2 ){
						case 3: // mA Calibration
						case 4: // mA Zero Offset
							Second = 3600;
							break;
					}
					break;
					
				case FUNC_ITEM_LV1_TEST:
					switch( FuncHandle.Item.Lv2 ){
						case 4: // Maximum Power Test
							Second = 18000;
							break;
							
						default:
							Second = 3600;
							break;
					}
					break;
					
				case FUNC_ITEM_LV1_FACTORY:
					Second = 3600;
					break;
			}
		}
		FuncHandle.QuitSec = Second;
		FuncHandle.QuitTmr = FUNCTION_MODE_QUIT_TIME;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( void )
{
	switch( FuncHandle.KeyData ){
		case Function:
			if( FuncItem[FuncHandle.Item.Lv1] != NULL ){
				FuncHandle.Item.Lv2 = 0;
				FuncHandle.Item.Lv3 = 0;
				FuncHandle.Item.Lv4 = 0;
				FuncHandle.Item.Lv5 = 0;
				FuncHandle.KeyData = NoKey;
				FuncHandle.IsApproved = false;
				FuncItem[FuncHandle.Item.Lv1]( &FuncHandle );
				FuncHandle.ViewEvt = false;
				FuncHandle.Level = FUNC_LEVEL_2;
			}
			break;
			
		case Up:
			if( FuncHandle.Item.Lv1 ){
				if( (SystemOption.Item.IsBatMode == true) && (FuncHandle.Item.Lv1 == FUNC_ITEM_LV1_SYSTEM) ){
					FuncHandle.Item.Lv1 = FUNC_ITEM_LV1_FLOW_SET;
				}
				else {
					FuncHandle.Item.Lv1--;
				}
			}
			break;
			
		case Down:
			if( FuncHandle.Item.Lv1 < (FUNC_ITEM_LV1_MAX - 1) ){
				if( (SystemOption.Item.IsBatMode == true) && (FuncHandle.Item.Lv1 == FUNC_ITEM_LV1_FLOW_SET) ){
					FuncHandle.Item.Lv1  = FUNC_ITEM_LV1_SYSTEM;
				}
				else {
					FuncHandle.Item.Lv1++;
				}
			}
			break;
			
		case Reset:
			FuncHandle.QuitSec = 0;
			FuncHandle.QuitTmr = 0;
			FuncHandle.ViewTmr = 0;
			FuncHandle.State.Byte = 0;
			Enter_Normal_Mode();
			FuncHandle.ViewEvt = false;
			break;
	}
}


