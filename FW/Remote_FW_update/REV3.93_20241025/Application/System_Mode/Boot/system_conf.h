#ifndef   __SYSTEM_CONF_H__
#define   __SYSTEM_CONF_H__


#ifdef __SYSTEM_CONF_C__
	#define SYSTEM_CONF_EXT
#else
	#define SYSTEM_CONF_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef enum{
    SYSTEM_CONFIG_CAVITY,
    SYSTEM_CONFIG_CARTRIDGE,
    SYSTEM_CONFIG_FW_VER,
    SYSTEM_CONFIG_OPTION,
    SYSTEM_CONFIG_END
}System_ConfType;
/* End Struct */

/* Start Function */
SYSTEM_CONF_EXT void System_Config( void );
/* End Function */

/* Start Variable */                                   
/* End Variable */


#endif // __SYSTEM_CONF_H__
