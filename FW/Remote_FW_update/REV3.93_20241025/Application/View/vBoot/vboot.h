#ifndef   __VBOOT_H__
#define   __VBOOT_H__


#ifdef __VBOOT_C__
	#define VBOOT_EXT
#else
	#define VBOOT_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "system_init.h"
#include "system_conf.h" 
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VBOOT_EXT void vSysInit( System_InitType seq, bool err );
VBOOT_EXT void vSysConf( System_ConfType seq );
/* End Function */

/* Start Variable */ 
/* End Variable */



#endif // __VBOOT_H__
