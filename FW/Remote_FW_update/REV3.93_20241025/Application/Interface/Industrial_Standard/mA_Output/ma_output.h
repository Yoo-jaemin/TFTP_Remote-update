#ifndef   __mA_OUTPUT_H__
#define   __mA_OUTPUT_H__


#ifdef __mA_OUTPUT_C__
	#define mA_OUTPUT_EXT
#else
	#define mA_OUTPUT_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "measuring.h"
#include "dac8571.h" 
/* End Include */

/* Start Define */
#define XTR115_R_IN				( 10000.f ) // 10Kohm
#define XTR115_GAIN				( 100.f )
#define uAmpere_To_Dac( uA )	( uA * 0.000001f / XTR115_GAIN * XTR115_R_IN / DAC8571_LSB )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	GPIO_TypeDef	*nEnPort;
	uint16_t		nEnPin;
	GPIO_PinState	nEnPinState;
}mA_GpioType;

typedef struct {
	U16				Zero;
	U16				Span;
	S16				Offs;
}mA_InfoType;

typedef struct {
	mA_GpioType		Gpio;
	mA_InfoType		Inf;
	float			Set;
	float			Out;
}mA_HandleType;
/* End Struct */

/* Start Function */
mA_OUTPUT_EXT bool mA_Output_Init( void );
mA_OUTPUT_EXT void mA_Output_ReInit_Ch1( void );
mA_OUTPUT_EXT void mA_Output_ReInit_Ch2( void );
mA_OUTPUT_EXT void mA_Output_Task( void );
mA_OUTPUT_EXT float uA_Calculation( mA_HandleType *pmA );
/* End Function */

/* Start Variable */
mA_OUTPUT_EXT mA_HandleType		mA_Handle[MEASURING_GAS_OUTPUT_MAX];
/* End Variable */


#endif // __mA_OUTPUT_H__
