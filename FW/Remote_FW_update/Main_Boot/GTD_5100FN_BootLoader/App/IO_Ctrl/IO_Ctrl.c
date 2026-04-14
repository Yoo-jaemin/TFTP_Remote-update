#define __IO_CTRL_C__
    #include "IO_Ctrl.h"
#undef  __IO_CTRL_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gpio.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Key_Read(U8 ucKey);
/* End Function */

/* Start Variable */

/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Key_Check(void)
{
  	if(TECT_FUNC_READ() == SWITCH_ON){ 
		Key.uCheck_timer[nFUNC]++;
		if(Key.uCheck_timer[nFUNC] > KEYCHECK_TIME) Key.Check.Bit.bRead_Func = TRUE;
	}
	else {
	  	Key.uCheck_timer[nFUNC] = 0;
		Key.Check.Bit.bRead_Func = FALSE;
	}
	
	if(TECT_RST_READ() == SWITCH_ON){ 
		Key.uCheck_timer[nRST]++;
		if(Key.uCheck_timer[nRST] > KEYCHECK_TIME) Key.Check.Bit.bRead_RST = TRUE;
  	}
	else {
	  	Key.uCheck_timer[nRST] = 0;
		Key.Check.Bit.bRead_RST = FALSE;
	}
 	if(TECT_UP_READ() == SWITCH_ON){ 
		Key.uCheck_timer[nUP]++;
		if(Key.uCheck_timer[nUP] > KEYCHECK_TIME) Key.Check.Bit.bRead_Up = TRUE;
	}
	else {
	  	Key.uCheck_timer[nUP] = 0;
		Key.Check.Bit.bRead_Up = FALSE;
	}
	
	if(TECT_DOWN_READ() == SWITCH_ON){ 
		Key.uCheck_timer[nDOWN]++;
		if(Key.uCheck_timer[nDOWN] > KEYCHECK_TIME) Key.Check.Bit.bRead_Down = TRUE;
  	}
	else {
	  	Key.uCheck_timer[nDOWN] = 0;
		Key.Check.Bit.bRead_Down = FALSE;
	}
}

void Key_Operation(MainStatus* Status)
{
  	U8 ucOperateKey;
  	static U8 ucOld_OperateKey = 0x10;
		
	Key_Read(Key.Check.All);
	
	ucOperateKey = Key.Check.All & 0xF0;
	
	if( (ucOld_OperateKey == ucOperateKey) || ( Status->Bit.bBoot_mode != TRUE)) return;
		
	if(Key.Check.Bit.bOperate_Func == TRUE) Key.Setting.Bit.bFucn = TRUE;
	
	if(Key.Check.Bit.bOperate_Up == TRUE) {
	  	if((Key.Setting.ucPosition & 0x0F) > nMAIN_UPLOAD) {
			Key.Setting.ucPosition--;
			if(Key.Setting.ucPosition == nIR_SENSOR_UPLOAD){
				if( Status->Bit.bIR_Check != TRUE) Key.Setting.ucPosition--;
			}
			if(Key.Setting.ucPosition == nCT_SENSOR_UPLOAD){
				if( Status->Bit.bCT_Check != TRUE) Key.Setting.ucPosition--;
			}
	  	}
	}
	
	if(Key.Check.Bit.bOperate_Down == TRUE) {
	  	if((Key.Setting.ucPosition & 0x0F) < nUI_MENU_MAX-1) {
			Key.Setting.ucPosition++;
			if(Key.Setting.ucPosition == nCT_SENSOR_UPLOAD){
				if( Status->Bit.bCT_Check != TRUE) Key.Setting.ucPosition++;
			}
			if(Key.Setting.ucPosition == nIR_SENSOR_UPLOAD){
				if( Status->Bit.bIR_Check != TRUE) Key.Setting.ucPosition++;
			}
		}
	}
	
	if(Key.Check.Bit.bOperate_RST == TRUE) Key.Setting.Bit.bRST = TRUE;
	
	ucOld_OperateKey = ucOperateKey;
}


static void Key_Read(U8 ucKey)
{	
  	U8 ucReadKey = 0;
	static U8 ucOld_ReadKey = 0;

	ucReadKey = ucKey & 0x0F;
	
	if(ucOld_ReadKey == ucReadKey) return;
		
	
	if((ucReadKey & RST_KEY_ON) == RST_KEY_ON) LED_RST_ON();
	else {
	  	LED_RST_OFF();
		Key.Check.Bit.bOperate_RST = FALSE;
	}
	
	
	if((ucReadKey & FUCN_KEY_ON) == FUCN_KEY_ON) LED_FUNC_ON();
	else {
	  	LED_FUNC_OFF();
		Key.Check.Bit.bOperate_Func = FALSE;
	}
		
	if((ucReadKey & UP_KEY_ON) == UP_KEY_ON) LED_UP_ON();
	else {
	  	LED_UP_OFF();
		Key.Check.Bit.bOperate_Up = FALSE;
	}	
	
	if((ucReadKey & DOWN_KEY_ON) == DOWN_KEY_ON) LED_DOWN_ON();
	  	
	else {
	  	LED_DOWN_OFF();
		Key.Check.Bit.bOperate_Down = FALSE;
	}	
	
	
	switch(ucReadKey)
	{	
		case FUCN_KEY_ON: Key.Check.Bit.bOperate_Func = TRUE; break;
		case RST_KEY_ON: Key.Check.Bit.bOperate_RST = TRUE; break;
		case UP_KEY_ON:	Key.Check.Bit.bOperate_Up = TRUE; break;
		case DOWN_KEY_ON: Key.Check.Bit.bOperate_Down = TRUE; break;
		default:
		  //printf("\r\n Read Key Err");
		  break;
	}
	
	ucOld_ReadKey = ucReadKey;
}
