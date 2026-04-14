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
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
/* End Define */

/* Start Enum */
typedef enum {
	SYSTEM_STATE_WARM_UP = 0,
	SYSTEM_STATE_NORMAL
}SystemStateType;

typedef union {
    U16  Word;
    struct{
        bool	DebugMode	: 1; // 1
        bool	IrSensor	: 1; // 2
        bool	IrSource	: 1; // 3
        bool	eADC		: 1; // 4
        bool	iADC1		: 1; // 5
        bool	Eeprom_CRC	: 1; // 6
        bool	AT24C32		: 1; // 7
        bool	Driver		: 1; // 8
        bool	TPL0102		: 1; // 9
        bool	Init		: 1; // 10
        bool	I2C1_Driver	: 1; // 11
        bool	I2C2_Driver	: 1; // 12
        U8		Reserved	: 4;
    }Bit;
}SystemErrorType;
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
MAIN_EXT void _Error_Handler(char *, int);
/* End Function */

/* Start Variable */
extern SystemStateType		SystemState;
extern SystemErrorType		SystemError, SystemErrorBuf;
/* End Variable */


#endif // __MAIN_H__
