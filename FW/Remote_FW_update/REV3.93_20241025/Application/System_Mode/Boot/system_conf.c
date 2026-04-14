#define __SYSTEM_CONF_C__
    #include "system_conf.h"
#undef  __SYSTEM_CONF_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vboot.h"
#include "ma_output.h"
#include "micro_sd.h"
#include "pump.h"
#include "measuring.h"
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

void System_Config( void )
{
	System_ConfType	seq = (System_ConfType)0;
	
	while( seq <= SYSTEM_CONFIG_END ){
		
		switch( seq ){
			case SYSTEM_CONFIG_CAVITY:
				SENSOR_USART_PORT_SELECTION( CAVITY_PORT );
				Cavity_Config( &SensingProcess.Modbus );
				break;
				
			case SYSTEM_CONFIG_CARTRIDGE:
				SENSOR_USART_PORT_SELECTION( CARTRIDGE_USART_PORT );
				Cartridge_Config( &SensingProcess.Modbus );
				break;
				
			case SYSTEM_CONFIG_FW_VER:
				Measuring_Config();
				break;
				
			case SYSTEM_CONFIG_OPTION:
				uSD_Config();
				break;
		}
		
		vSysConf( seq++ ); 
	}
}



