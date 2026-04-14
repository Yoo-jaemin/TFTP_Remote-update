#ifndef   __KEY_H__
#define   __KEY_H__


#ifdef __KEY_C__
	#define KEY_EXT
#else
	#define KEY_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define KEY_SCAN_TASK_MSEC		( 8 )
#define KEY_SCAN_MSEC( msec)	( msec / KEY_SCAN_TASK_MSEC )
/* End Define */

/* Start Enum */
enum {
	KEY_INDEX_FUNCTION,
	KEY_INDEX_UP,
	KEY_INDEX_DOWN,
	KEY_INDEX_RESET,
	KEY_INDEX_RESET_UP,
	KEY_INDEX_MAX
}; 

typedef enum {
	NoKey,
	Function,
	Up,
	Down,
	Reset,
	Reset_Up
}KEY_DataType;

typedef enum {
	Short,
	Repeat,
	Long
}KEY_DetectionType;
/* End Enum */

/* Start Struct */
typedef struct { 
	GPIO_TypeDef			*Port;
	uint16_t				Pin;
}KEY_GpioType;

typedef struct { 
	KEY_GpioType			Tact_Function;
	KEY_GpioType			Reed_Function;
	KEY_GpioType			Tact_Up;
	KEY_GpioType			Reed_Up;
	KEY_GpioType			Tact_Down;
	KEY_GpioType			Reed_Down;
	KEY_GpioType			Tact_Reset;
	KEY_GpioType			Reed_Reset;
}KEY_GpioItem;

typedef union {
    U8  Byte;
    struct{
        bool				Function	: 1;
        bool				Up			: 1;
        bool				Down		: 1;
        bool				Reset		: 1;
        U8					Reserved	: 4;
    }Bit;
}KEY_ScanPort;

typedef struct {
	U16						Cnt;
	U16						Debouce;
	U16						RepeatStart;
	U16						Repeat;
	bool					IsRepeatMode;
	U16						Long;
}KEY_TickType;

typedef struct {
	U32						Tact_Function;
	U32						Reed_Function;
	U32						Tact_Up;
	U32						Reed_Up;
	U32						Tact_Down;
	U32						Reed_Down;
	U32						Tact_Reset;
	U32						Reed_Reset;
}KEY_ShortScanTick;

typedef struct{
	bool					Tact_Function;
	bool					Reed_Function;
	bool					Tact_Up;
	bool					Reed_Up;
	bool					Tact_Down;
	bool					Reed_Down;
	bool					Tact_Reset;
	bool					Reed_Reset;
}KEY_ShortScanFlag;

typedef struct {
	KEY_ShortScanTick		Tick;
	KEY_ShortScanFlag		Flag;
}KEY_ShortScanType;

typedef struct {
	bool					IsRun;
	KEY_GpioItem			Gpio;
	KEY_ScanPort			Port;
	KEY_ScanPort			Comp;
	KEY_ShortScanType		Short;
	KEY_TickType			Tick[KEY_INDEX_MAX];
	KEY_DataType			Data;
	KEY_DetectionType		Type;
}KEY_ScanType;
/* End Struct */

/* Start Function */
KEY_EXT bool KEY_Init( void );
KEY_EXT void KEY_Scan( void );
/* End Function */

/* Start Variable */  
KEY_EXT KEY_ScanType		KeyScan;
/* End Variable */


#endif // __KEY_H__
