#ifndef   __MAIN_H__
#define   __MAIN_H__


#ifdef __MAIN_C__
	#define MAIN_EXT
#else
	#define MAIN_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define Error_Handler()				_Error_Handler(__FILE__, __LINE__)
/* End Define */

/* Start Enum */
typedef enum {
	SYSTEM_STATE_WARM_UP = 0,
	SYSTEM_STATE_NORMAL
}System_StateType;
/* End Enum */

/* Start Struct */
typedef union {
    U16  Word;
    struct{
        bool	DebugMode	: 1; // 1
        bool	HS300x		: 1; // 2
        bool	Reserved_1	: 1; // 3
        bool	Reserved_2	: 1; // 4
        bool	iADC		: 1; // 5
        bool	Flash_CRC	: 1; // 6
        bool	Flash		: 1; // 7
        U16		Reserved_3	: 9;
    }Bit;
}System_ErrorType;
/* End Struct */

/* Start Function */
MAIN_EXT void _Error_Handler( char *file, int line );
/* End Function */

/* Start Variable */
MAIN_EXT System_StateType			SystemState;
MAIN_EXT System_ErrorType			SystemError;
/* End Variable */


#endif // __MAIN_H__
