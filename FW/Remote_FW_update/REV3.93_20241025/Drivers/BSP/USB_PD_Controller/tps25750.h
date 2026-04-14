#ifndef   __TPS25750_H__
#define   __TPS25750_H__


#ifdef __TPS25750_C__
	#define TPS25750_EXT
#else
	#define TPS25750_EXT extern
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
typedef union {
    U8	Byte[5];
    struct{
        bool				PlugPresent					: 1; // 0-0
        U8					ConnState					: 3; // 0-1~3
        bool				PlugOrientation				: 1; // 0-4
        bool				PortRole					: 1; // 0-5
        bool				DataRole					: 1; // 0-6
        bool				Reserved_1_1				: 1; // 0-7
		
        U8					Reserved_1_2				: 8; // 1-8~15
		
        U8					Reserved_1_3				: 4; // 2-16~19
        U8					VbusStatus					: 2; // 2-20~21
        U8					UsbHostPresent				: 2; // 2-22~23
		
        U8					ActingAsLegacy				: 2; // 3-24~25
        bool				Reserved_2					: 1; // 3-26
		bool				Bist						: 1; // 3-27
		U8					Reserved_3					: 4; // 2-28~31
		
        U8					Reserved_4					: 8; // 4-0~7
    }Item;
}TPS25750_Status;

typedef union {
    U8	Byte[5];
    struct{
		bool				PatchHeaderErr				: 1; // 0-0
		bool				Reserved_1					: 1; // 0-1
		bool				DeadBatteryFlag				: 1; // 0-2
		bool				I2cEepromPresent			: 1; // 0-3
		U8					Reserved_2_1				: 4; // 0-4~7
		
		U8					Reserved_2_2				: 2; // 1-8~9
		bool				patchdownloaderr			: 1; // 1-10
		U8					Reserved_3_1				: 5; // 1-11~15
		
		U8					Reserved_3_2				: 3; // 2-16~18
		bool				MasterTSD					: 1; // 2-19
		U8					Reserved_4_1				: 4; // 2-20~23
		
		U8					Reserved_4_2				: 5; // 3-24~28
        U8					PatchConfigSource			: 3; // 3-29~31
		
        U8					REV_ID						: 8; // 4-0~7
    }Item;
}TPS25750_BootStatus;

typedef struct {
	TPS25750_Status			Status;
	TPS25750_BootStatus		BootStatus;
}TPS25750_HandleType;
/* End Struct */

/* Start Function */
TPS25750_EXT bool TPS25750_Init( TPS25750_HandleType *ppd );
TPS25750_EXT bool TPS25750_Read_Status( TPS25750_HandleType *ppd );
TPS25750_EXT bool TPS25750_Read_BootStatus( TPS25750_HandleType *ppd );
TPS25750_EXT bool TPS25750_Patch_Bundle_Reload( void );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __TPS25750_H__
