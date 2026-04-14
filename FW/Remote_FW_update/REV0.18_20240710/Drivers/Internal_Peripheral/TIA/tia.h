#ifndef   __TIA_H__
#define   __TIA_H__


#ifdef __TIA_C__
	#define TIA_EXT
#else
	#define TIA_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
   LPTIA_SWMODE_OFF				= 0x0000, // ULP TIA off
   LPTIA_SWMODE_NORM			= 0x302c, // normal mode for all types of gas sensors
   LPTIA_SWMODE_NOISECANCEL		= 0x306C, // enable noise cancellation, only useful for zero-biased gas sensor
   LPTIA_SWMODE_DIODE			= 0x302D, // diode protection
   LPTIA_SWMODE_SHORT			= 0x302F, // short switch for very large current to avoid saturation
   LPTIA_SWMODE_AC				= 0x3180, // for AC impedance measurement
   LPTIA_SWMODE_RAMP			= 0x0094, // RAMP test on DE/SE
   LPTIA_SWMODE_HPCAPA			= 0x0094, // CAPA test with High power TIA
   LPTIA_SWMODE_LPCAPA			= 0x0014, // CAPA test with Low power TIA
   LPTIA_SWMODE_DUALSNS0		= 0x702C, // tia0 switch mode for dual gas sensor
   LPTIA_SWMODE_DUALSNS1		= 0x0020, // tia1 switch mode for dual gas sensor
   LPTIA_SWMODE_EXTRES			= 0x322C, // work mode with external RTIA for all types of gas sensors
   LPTIA_SWMODE_RESISTOR		= 0x3820, // Rister measuring in test
   LPTIA_SWMODE_EDASIG			= 0x6424, // EDA signal measurement
   LPTIA_SWMODE_EDARCAL			= 0xE034, // EDA R_CAL measurement
   LPTIA_SWMODE_EDASIG_EXTRES	= 0x6624, // EDA signal measurement with external resitor
   LPTIA_SWMODE_EDARCAL_EXTRES	= 0xE234, // EDA R_CAL measurement wiht external resistor
   LPTIA_SWMODE_ECG				= 0x0000, // ECG signal measurement mode
   LPTIA_SWMODE_2LEAD			= 0x3024, // 3 lead sensor, two lead mode
   LPTIA_SWMODE_VOTAGE_FOLLOWER	= 0x0AF0  // CB Sensor
}LPTIA_SwitchModeType;
/* End Enum */

/* Start Struct */
typedef union {
    U16  Word[2];
    struct{
		LPTIA_RLOAD_Type		RLOAD	:  4;
		LPTIA_RF_Type			RFILTER	:  4;
		LPTIA_RGAIN_Type		RGAIN	:  8;
		LPTIA_SwitchModeType	SWMODE	: 16;
    }Item;
}LPTIA_ConfType;
/* End Struct */

/* Start Function */
TIA_EXT void LPTIA_Config( U8 Channel, LPTIA_ConfType *pTia );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __TIA_H__
