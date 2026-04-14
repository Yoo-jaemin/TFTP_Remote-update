#define __PASSWORD_MODE_C__
    #include "password_mode.h"
#undef  __PASSWORD_MODE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vpassword_mode.h"
#include "eeprom.h"
#include "key.h"
#include "normal_mode.h"
#include "function_mode.h"
#include "self_test.h"
#include "buzzer.h"
/* End Include */

/* Start Define */
#define MODE_QUIT_TIME		(0x8000 + 250 - 1) // 40 msec * 250 = 10 sec
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Enter_Password_Mode( void )
{
	PwHandle.IsHideMode = true;
	PwHandle.SysPW = EepHandle.Sram[SYSTEM_EEP_PASSWORD];
	PwHandle.SetPW = 0;
	PwHandle.HideTmr = 0;
	PwHandle.QuitTmr = MODE_QUIT_TIME;
	PwHandle.PrvState = SystemState;
	PwHandle.pMsgBox = &vMsgBox;
	PwHandle.pMsgBox->ClearTitle = true;
	PwHandle.pMsgBox->ClearMsg = true;
	KeyScan.Tick[KEY_INDEX_UP  ].IsRepeatMode = true;
	KeyScan.Tick[KEY_INDEX_DOWN].IsRepeatMode = true;
	
	Buzzer_Stop();
	
	vPassword_Mode( &PwHandle );
	
	SystemState = SYSTEM_STATE_PASSWORD;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Password_Mode_Task( void )
{
	KEY_DataType	Key = KeyScan.Data;
	
	KeyScan.Data = NoKey;
	
	if( Key != NoKey ){
		
		PwHandle.QuitTmr = MODE_QUIT_TIME;
		
		switch( Key ){
			case Function:
				if( PwHandle.SetPW == PwHandle.SysPW ){
					PwHandle.HideTmr = 0;
					PwHandle.QuitTmr = 0;
					if( SelfHandle.IsWarmUpZeroEnabled == true ){
						Pump_On();
					}
					Enter_Function_Mode();
				}
				else {
					PwHandle.IsHideMode = false;
					PwHandle.IsMisMatch = true;
					PwHandle.HideTmr = 0x8000+24;
					vPassword_Mode( &PwHandle );
				}
				break;
				
			case Up:
				if( PwHandle.SetPW < 99 ){
					PwHandle.SetPW++;
				}
				PwHandle.IsHideMode = false;
				PwHandle.HideTmr = 0x8000+24;
				vPassword_Mode( &PwHandle );
				break;
				
			case Down:                         
				if( PwHandle.SetPW ){
					PwHandle.SetPW--;
				}
				PwHandle.IsHideMode = false;
				PwHandle.HideTmr = 0x8000+24;
				vPassword_Mode( &PwHandle );
				break;
				
			case Reset:
				PwHandle.HideTmr = 0;
				PwHandle.QuitTmr = 0;
				if( PwHandle.PrvState == SYSTEM_STATE_SELF_TEST ){ Enter_Self_Test_Mode(); }
				else { Enter_Normal_Mode(); }
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Password_Mode_Timer( void )
{
	if( PwHandle.HideTmr & 0x8000 ){
		if( PwHandle.HideTmr == 0x8000 ){
			PwHandle.HideTmr = 0;
			
			PwHandle.IsHideMode = true;
			
			vPassword_Mode( &PwHandle );
		}
		else {
			PwHandle.HideTmr--;
		}
	}
	
	if( PwHandle.QuitTmr & 0x8000 ){
		if( PwHandle.QuitTmr == 0x8000 ){
			PwHandle.QuitTmr = 0;
			
			if( PwHandle.PrvState == SYSTEM_STATE_SELF_TEST ){ Enter_Self_Test_Mode(); }
			else { Enter_Normal_Mode(); }
		}
		else {
			PwHandle.QuitTmr--;
		}
	}
}


