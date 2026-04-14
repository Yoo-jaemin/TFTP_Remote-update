#ifndef   __MAIN_H__
#define   __MAIN_H__


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
	SYSTEM_STATE_INIT,
	SYSTEM_STATE_CONFIG,
	SYSTEM_STATE_SELF_TEST,
	SYSTEM_STATE_NORMAL,
	SYSTEM_STATE_ALLBASE,
	SYSTEM_STATE_PASSWORD,
	SYSTEM_STATE_FUNCTION,
	SYSTEM_STATE_MAX
}System_StateType;
/* End Enum */

/* Start Struct */
typedef union {
    U32	Word;
    struct{
        bool	Eeprom_System				: 1; // 1
        bool	mA_Output_DAC_CH1			: 1; // 2
        bool	mA_Output_DAC_CH2			: 1; // 3
        bool	Psen_Potentiometer			: 1; // 4
        bool	Buzzer_Timer				: 1; // 5
        bool	Pump_Timer					: 1; // 6
        bool	WatchDog					: 1; // 7
        bool	iADC						: 1; // 8
        bool	Flash_Cartridge				: 1; // 9
        bool	Eeprom_Cavity				: 1; // 10
        bool	HS300x_Cartridge			: 1; // 11
        bool	Ptm_Cavity					: 1; // 12
        bool	iADC_Cartridge				: 1; // 13
        bool	eADC_Cavity					: 1; // 14
        bool	BatCharger					: 1; // 15
        bool	USB_PD_Controller			: 1; // 16
        bool	Driver_Cavity				: 1; // 17
        bool	Init_Cavity					: 1; // 18
        U32		Reserved_2					: 14;
    }Bit;
}System_Error_Peripheral;

typedef union {
    U32	Word;
    struct{
        bool	Power						: 1; // 1
        bool	Measurement_Parameter		: 1; // 2
        bool	CtgTemp						: 1; // 3
        bool	CvtTemp						: 1; // 4
        bool	Eeprom_CRC					: 1; // 5
        bool	CtgCom						: 1; // 6
        bool	CvtCom						: 1; // 7
        bool	FlowRate_Udr				: 1; // 8
        bool	FlowRate_Ovr				: 1; // 9
        bool	IR_SourcePower				: 1; // 10
        bool	IR_OffsNotStable			: 1; // 11
        bool	IR_OffsDrift_CH1			: 1; // 12
        bool	IR_OffsDrift_CH2			: 1; // 13
        bool	IR_OffsDrift_REF			: 1; // 14
        bool	IR_ExcSignal_CH1			: 1; // 15
        bool	IR_ExcSignal_CH2			: 1; // 16
        bool	IR_ExcSignal_REF			: 1; // 17
        bool	IR_NotStable_CH1			: 1; // 18
        bool	IR_NotStable_CH2			: 1; // 19
        bool	IR_NotStable_REF			: 1; // 20
        bool	Measurement_Pending			: 1; // 21
        bool	IR_OptPathContaminated_CH1	: 1; // 22
        bool	IR_OptPathContaminated_CH2	: 1; // 23
        bool	IR_OptPathContaminated_REF	: 1; // 24
        bool	CtgDac						: 1; // 25
        bool	CT_ExcSignal_CH1			: 1; // 26
        bool	CT_ExcSignal_CH2			: 1; // 27
        bool	CT_NotStable_CH1			: 1; // 28
        bool	CT_NotStable_CH2			: 1; // 29
        bool	CT_ExcCurrent_CH1			: 1; // 30
        bool	CT_ExcCurrent_CH2			: 1; // 31
        bool	CT_Gas_Deviation			: 1; // 32
    }Bit;
}System_Error_Main;

typedef struct {
	System_Error_Peripheral		Peripheral;
	System_Error_Main			Main;
	U16							Code[2];
}System_ErrorType;
/* End Struct */

/* Start Function */
extern void _Error_Handler( char *file, int line );
/* End Function */

/* Start Variable */
extern System_StateType			SystemState;
extern System_ErrorType			SystemError;
/* End Variable */


#endif // __MAIN_H__
