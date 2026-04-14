#ifndef   __LED_H__
#define   __LED_H__


#ifdef __LED_C__
	#define LED_EXT
#else
	#define LED_EXT extern
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
typedef struct { 
	GPIO_TypeDef			*Port;
	uint16_t				Pin;
}LED_GpioType;

typedef struct { 
	LED_GpioType			Function;
	LED_GpioType			Up;
	LED_GpioType			Down;
	LED_GpioType			Reset;
	LED_GpioType			TRB;
	LED_GpioType			AL1;
	LED_GpioType			AL2;
}LED_GpioItem;

typedef union {
	U8  Byte;
	struct{
		bool				Function	: 1;
		bool				Up			: 1;
		bool				Down		: 1;
		bool				Reset		: 1;
		bool				TRB			: 1;
		bool				AL1			: 1;
		bool				AL2			: 1; 
		U8					Reserved	: 1;
	}Bit;
}LED_Indicator;

typedef struct {
	LED_GpioItem			Gpio;
	LED_Indicator			Set;
	LED_Indicator			Out;
}LED_DriveType;
/* End Struct */

/* Start Function */
LED_EXT bool LED_Init( void );
LED_EXT void LED_Task( void );
/* End Function */

/* Start Variable */
LED_EXT LED_DriveType		LedDrive; 
/* End Variable */


#endif // __LED_H__
