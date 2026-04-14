#define __SYSTEM_INIT_C__
    #include "system_init.h"
#undef  __SYSTEM_INIT_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "usart1.h"
#include "option.h"
#include "power_control.h"
#include "i2c.h"
#include "eeprom.h"
#include "mA_output.h"
#include "relay.h"
#include "fsmc.h"
#include "gdi.h"
#include "bor.h"
#include "nand_flash.h"
#include "version.h"
#include "usart.h"
#include "rs485.h"
#include "poe.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "bsp_sd.h"
#include "key.h"
#include "led.h"
#include "mcp46x1.h"
#include "flow_control.h"
#include "buzzer.h"
#include "sensing_process.h"
#include "task.h"
#include "vboot.h"
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
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

System_InitType System_Init( void )
{                                                      
	System_InitType		seq = (System_InitType)0;
	bool				err = false;
	
	while( seq != SYSTEM_INIT_OK ){
		
		switch( seq ){
			case SYSTEM_INIT_DEBUG_USART:	err = USART1_Init();			break;
			case SYSTEM_INIT_OPT_LOAD:		err = System_Opt_Load();		break;
			case SYSTEM_INIT_I2C:			err = I2C_Init();				break;
			case SYSTEM_INIT_EEPROM:		err = EEPROM_Init();			break;
			case SYSTEM_INIT_POWER_CONTROL:	err = Power_Control_Init();		break;
			case SYSTEM_INIT_mA_OUTPUT:		err = mA_Output_Init();			break;
			case SYSTEM_INIT_RELAY:			err = Relay_Init();				break;
			case SYSTEM_INIT_FSMC:			err = FSMC_Init();				break;
			case SYSTEM_INIT_GDI:			err = GDI_Init();				break;
			case SYSTEM_INIT_BOR:			err = BOR_Init();				break;
			case SYSTEM_INIT_NAND:			err = Nand_Flash_Init();		break; 
			case SYSTEM_INIT_VER_LOAD:		err = System_Ver_Load();		break;
			case SYSTEM_INIT_USART:			err = USART_Init();				break;
			case SYSTEM_INIT_RS485:			err = RS485_Init();				break;
			case SYSTEM_INIT_POE:			err = POE_Init();				break; 
			case SYSTEM_INIT_ADC:			err = iADC_Init();				break;
			case SYSTEM_INIT_RTC:			err = RTC_Init();				break;
			case SYSTEM_INIT_TIM:			err = TIM_Init();				break;
			case SYSTEM_INIT_SD:			err = BSP_SD_Init();			break;
			case SYSTEM_INIT_KEY:			err = KEY_Init();				break;
			case SYSTEM_INIT_LED:			err = LED_Init();				break;
			case SYSTEM_INIT_PSEN_DPM:		err = MCP46X1_Init();			break;
			case SYSTEM_INIT_FLOW_CONTROL:	err = Flow_Control_Init();		break;
			case SYSTEM_INIT_BUZZER:		err = Buzzer_Init();			break;
			case SYSTEM_INIT_SENSING:		err = Sensing_Process_Init();	break;
			case SYSTEM_INIT_TASK:			err = Task_Init();				break;
		}
		
		if( seq < SYSTEM_INIT_GDI ){
			if( err == true ){
				FSMC_Init();
				GDI_Init();
				vSysInit( seq, err );
			}
		}
		else {
			vSysInit( seq, err );
		}
		
		if( err == false )	{ seq++; }
		else				{ break; }
		
	}
	
	return seq;
}


