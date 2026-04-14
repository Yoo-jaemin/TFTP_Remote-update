#ifndef   __VSTATUSBAR_H__
#define   __VSTATUSBAR_H__


#ifdef __VSTATUSBAR_C__
	#define VSTATUSBAR_EXT
#else
	#define VSTATUSBAR_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "measuring.h"  
/* End Include */

/* Start Define */
#define COM_ICON_BLINK_INTERVAL		( 75 ) // 8 * 75 = 600 msec
/* End Define */

/* Start Enum */
typedef enum {
	//SENSING_PROCESS_READY,    
	SYSTEM_STATUS_ICON_SD_PRESENT,
	SYSTEM_STATUS_ICON_SD_NOT_PRESENT,
	SYSTEM_STATUS_ICON_FILTER,
	SYSTEM_STATUS_ICON_INTGAS,
	SYSTEM_STATUS_ICON_NORMAL,
	SYSTEM_STATUS_ICON_MAX
}SystemStatus_Icon;
/* End Enum */

/* Start Struct */
typedef union {
    U8	Byte;
    struct{
        U8		Level		: 7;
        bool	IsPresent	: 1;
    }Item;
}vBat_Status;

typedef struct {
	bool					DrawEvt;
	U8						ComIconTmr;
	bool					ComIconSet;
	bool					ComIconOut;
	SystemStatus_Icon		SysIconSet;
	SystemStatus_Icon		SysIconOut;
	U8						InfoTmr;
	U8						FlowTmr;
	vBat_Status				BatPrv;
	vBat_Status				BatCur;
}vStatusBar_Type;
/* End Struct */

/* Start Function */
VSTATUSBAR_EXT void vStatusBar( void );
/* End Function */

/* Start Variable */
VSTATUSBAR_EXT vStatusBar_Type	vStBar;
/* End Variable */


#endif // __VSTATUSBAR_H__
