#ifndef   __POWER_CONTROL_H__
#define   __POWER_CONTROL_H__


#ifdef __POWER_CONTROL_C__
	#define POWER_CONTROL_EXT
#else
	#define POWER_CONTROL_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "bq2579x.h"
#include "tps25750.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef union {
    U16	word;
    struct{
        U8					Tmr			: 8; // 0~7
        bool				Cur			: 1; // 8
        bool				Prv			: 1; // 9
        bool				Cop			: 1; // 10
        bool				IsPressed	: 1; // 11
		U8					CountDown	: 3; // 12~14
		bool				Reserved	: 1; // 15
    }Item;
}Power_KeyScanType;
/* End Enum */

/* Start Struct */
typedef struct {
	BQ2579x_HandleType		BQ;
	TPS25750_HandleType		PD;
	Power_KeyScanType		KeyScan;
	U8						BatLevel;
	U8						BatScanTmr;
	U8						PatchReloadTmr;
}Power_CtrlType;
/* End Struct */

/* Start Function */
POWER_CONTROL_EXT bool Power_Control_Init( void );
POWER_CONTROL_EXT void Power_Control_Task( void );
/* End Function */

/* Start Variable */
POWER_CONTROL_EXT Power_CtrlType		PwrCtrl;
/* End Variable */


#endif // __POWER_CONTROL_H__
