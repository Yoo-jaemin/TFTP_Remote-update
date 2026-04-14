#define __DAC8571_C__
    #include "dac8571.h"
#undef  __DAC8571_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c2.h" 
/* End Include */

/* Start Define */
#define DAC8571_Device_Address( Addr )			( (uint16_t)0x98| (Addr << 2 & 0x04) )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DAC8571_Write( U8 DeviceAddr, float fDAC )
{
	static uint8_t	Byte[3];
	U16				wDAC;
	
	if( fDAC >= DAC8571_RES ){ fDAC = DAC8571_RES - 1; }
	wDAC = (U16)fDAC;
	
	Byte[0] = 0x10; // Write temporary register and load DAC with data
	Byte[1] = HI_BYTE( wDAC );
	Byte[2] = LO_BYTE( wDAC );
	
	if( I2C2_Write( DAC8571_Device_Address( DeviceAddr ), Byte, 3 ) == true ){ 
		if( DeviceAddr == 0 ){
			SystemError.Peripheral.Bit.mA_Output_DAC_CH1 = true;
		}
		else {
			SystemError.Peripheral.Bit.mA_Output_DAC_CH2 = true;
		}
	}
}

