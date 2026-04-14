#define __TASK_C__
    #include "task.h"
#undef  __TASK_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */     
#include "task_sync_timer.h"
#include "micro_sd.h"
#include "led.h" 
#include "measuring.h"
#include "rs485.h"      
#include "poe.h"
#include "relay.h"
#include "self_test.h"
#include "normal_mode.h"
#include "allbase_mode.h"
#include "password_mode.h"
#include "function_mode.h"
#include "flow_control.h"
#include "func_flowset.h"
#include "vstatusbar.h"
#include "option.h"
#include "iwdg.h"
#include "power_control.h"

#include "tftp_example.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Common_Task( void );
static void TmSync_Common_Task( void );
static void Normal_Common_Task( void );
/* End Function */

/* Start Variable */
static void (*SystemState_Task[SYSTEM_STATE_MAX])(void) =
{
	NULL,					// SYSTEM_STATE_INIT   
	NULL,					// SYSTEM_STATE_CONFIG
	Self_Test_Task,
	Normal_Mode_Task,
	AllBase_Mode_Task,
	Password_Mode_Task,
	Function_Mode_Task
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Task_Init( void )
{
	bool	err = false;
	
	TaskSyncTimer_Init();
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Main_Task( void )
{
	Common_Task();
	
	if( PwrCtrl.KeyScan.Item.IsPressed == false ){
		if( SystemState_Task[SystemState] != NULL ){
			SystemState_Task[SystemState]();
		}
	}
	
	IWDG_Task();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Common_Task( void )
{
	if( TaskSyncTimer.Evt.Word ){ TmSync_Common_Task(); }
	else						{ Normal_Common_Task(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void TmSync_Common_Task( void )
{
	if( TaskSyncTimer.Evt.Bit._01_MSEC == true ){
		TaskSyncTimer.Evt.Bit._01_MSEC = false;
		
		MODBUS_RTU_Rx_Packet_Check_Master( &SensingProcess.Modbus );
		if( (SystemOption.Item.IsBatMode == false) && (SystemOption.Item.IsPoeMode == false) ){
			RS485_Rx_Packet_Check();
		}
	}
	
	if( TaskSyncTimer.Evt.Bit._04_MSEC == true ){
		TaskSyncTimer.Evt.Bit._04_MSEC = false;
	}
	
	if( TaskSyncTimer.Evt.Bit._08_MSEC_1 == true ){
		TaskSyncTimer.Evt.Bit._08_MSEC_1 = false;
		
		KEY_Scan();
		if( KeyScan.Data != NoKey ){
			Measuring_Maintenance_Quit_Time_Refresh();
		}
	}
	
	if( TaskSyncTimer.Evt.Bit._08_MSEC_2 == true ){
		TaskSyncTimer.Evt.Bit._08_MSEC_2 = false;
		
		Flow_Control();
		FlowSet_Handler();
		vStBar.DrawEvt = true;
	}
	
	if( TaskSyncTimer.Evt.Bit._40_MSEC_1 == true ){
		TaskSyncTimer.Evt.Bit._40_MSEC_1 = false;
		
		MODBUS_RTU_Timeout_Check_Master( &SensingProcess.Modbus );
	}
	
	if( TaskSyncTimer.Evt.Bit._40_MSEC_2 == true ){
		TaskSyncTimer.Evt.Bit._40_MSEC_2 = false;
		
		Power_Control_Task();
	}
	
	if( TaskSyncTimer.Evt.Bit._40_MSEC_3 == true ){
		TaskSyncTimer.Evt.Bit._40_MSEC_3 = false;
		
		Password_Mode_Timer();
		Function_Mode_Timer();
		AllBaseHandle.TaskEvt = true;
	}
	
	if( TaskSyncTimer.Evt.Bit._01_SEC == true ){
		TaskSyncTimer.Evt.Bit._01_SEC = false;
		
		SelfHandle.TaskEvt = true;
		NormHandle.TaskEvt = true;
		Relay_Blink_Timer();
		Measuring_Maintenance_Auto_Quit_Handler();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Normal_Common_Task( void )
{
	FW_Update_task();
        
        Measuring_Task();
	LED_Task();
	
	if( SystemOption.Item.IsBatMode == false ){
		Relay_Task();
		if( SystemOption.Item.IsUniMode == true ){
			POE_Task();
			RS485_Task();
			mA_Output_Task();
		}
		else {
			if( SystemOption.Item.IsPoeMode == true ){
				POE_Task();
			}
			else {
				RS485_Task();
				mA_Output_Task();
			}
		}
	}
	
	EEPROM_Task();
	MicroSD_Insertion_Check();
}


