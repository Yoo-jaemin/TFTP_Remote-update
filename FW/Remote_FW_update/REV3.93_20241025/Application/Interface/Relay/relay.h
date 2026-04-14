#ifndef   __RELAY_H__
#define   __RELAY_H__


#ifdef __RELAY_C__
	#define RELAY_EXT
#else
	#define RELAY_EXT extern
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
    U16	Word;
    struct{
		bool			IsOutputEnable:	 1;
		bool			IsEnergizeMode:	 1;
        bool			IsBlinkingMode:	 1;
        U16				Reserved:		13;
    }Item;
}Relay_Option;

typedef union {
    U32	Word;
    struct{
        U8				Act_Tick:	8;
        U8				Rel_Tick:	8;
        U8				Act_Time:	8;
        U8				Rel_Time:	6;
		bool			IsEnable:	1;
		bool			DriveOut:	1;
    }Item;
}Relay_BlinkType;

typedef struct { 
	GPIO_TypeDef		*Port;
	uint16_t			Pin;
}Relay_GpioType;

typedef struct {
	Relay_GpioType		Gpio;
	Relay_Option		Opt;
	bool				Set;
	Relay_BlinkType		Blk;
	bool				Out;
}Relay_DriveItem; 

typedef struct {
	Relay_DriveItem		TRB;
	Relay_DriveItem		AL1;
	Relay_DriveItem		AL2;
	bool				IsTestMode;
	bool				UseWebTest;
}Relay_DriveType;
/* End Struct */

/* Start Function */
RELAY_EXT bool Relay_Init( void );
RELAY_EXT void Relay_ReInit( void );
RELAY_EXT void Relay_Task( void );
RELAY_EXT void Relay_Blink_Timer( void );
/* End Function */

/* Start Variable */
RELAY_EXT Relay_DriveType		RelayDrive;
/* End Variable */


#endif // __RELAY_H__
