#ifndef   __ADS868x_H__
#define   __ADS868x_H__


#ifdef __ADS868x_C__
	#define ADS868x_EXT
#else
	#define ADS868x_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define ADS868x_RES                         (65536.f)
#define ADS868x_VREF                        (5.12f) // V
#define ADS868x_LSB                         (ADS868x_VREF/ADS868x_RES)
#define ADS868x_x10000_LSB					(ADS868x_LSB*10000)
#define ADS868x_VoltageToAdcValue( v )      ((float)v/ADS868x_LSB)

#define ADS868x_AUX_RES                     (65536.f)
#define ADS868x_AUX_VREF                    (4096.f) // mV
#define ADS868x_AUX_LSB                     (ADS868x_AUX_VREF/ADS868x_AUX_RES)
/* End Define */

/* Start Enum */   
enum {
    ADS868x_PROG_REG_AUTO_SEQ_EN = 0x01,
    ADS868x_PROG_REG_CH_PWR_DOWN,
    ADS868x_PROG_REG_FEATURE_SELECT,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_0 = 0x05,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_1,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_2,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_3,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_4,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_5,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_6,
    ADS868x_PROG_REG_INPUT_RANGE_SELECT_7,
    ADS868x_PROG_REG_COMMAND_READ_BACK = 0x3F,
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
ADS868x_EXT void ADS868x_Init( void );
ADS868x_EXT U16 ADS868x_Manual_Conversion( U8 ReadCh );
ADS868x_EXT void ADS868x_Channel_Set( U8 SetCh );
ADS868x_EXT U16 ADS868x_Temp_Read( void );
ADS868x_EXT bool ADS868x_Program_Register_Write( U8 addr, U8 data );
ADS868x_EXT bool ADS868x_Gain_Check( U8 ReadCh );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __ADS868x_H__


