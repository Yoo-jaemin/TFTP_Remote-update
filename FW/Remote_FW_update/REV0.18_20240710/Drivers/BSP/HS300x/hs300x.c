#define __HS300x_C__
    #include "hs300x.h"
#undef  __HS300x_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c.h"
#include "gpt.h"
#include "main.h"
/* End Include */

/* Start Define */
#define DEVICE_ADDRESS		( 0x44 << 1 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void HS300x_Measurement_Requests( void );
static void HS300x_Data_Fetch_Load( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HS300x_Task( void )
{
	if( HS300x.CmdMR == true ){
		HS300x_Measurement_Requests();
	}
	if( HS300x.CmdDF == true ){
		HS300x_Data_Fetch_Load();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void HS300x_Measurement_Requests( void )
{
	SystemError.Bit.HS300x = I2C_TxPacket( DEVICE_ADDRESS, NULL, 0 );
	HS300x.CmdMR = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void HS300x_Data_Fetch_Load( void )
{
	SystemError.Bit.HS300x = I2C_RxPacket( DEVICE_ADDRESS, HS300x.RxBuf, 4 );
	if( SystemError.Bit.HS300x == false ){
		if( (HS300x.RxBuf[0] & 0xC0) == 0x00 ){
			HS300x.Humi.b.h = HS300x.RxBuf[0] & 0x3F;
			HS300x.Humi.b.l = HS300x.RxBuf[1];
			HS300x.Temp.b.h = HS300x.RxBuf[2];
			HS300x.Temp.b.l = HS300x.RxBuf[3] & 0xFC;
			HS300x.Temp.w = (HS300x.Temp.w >> 2) & 0x3FFF;
			HS300x.CmdDF = false;
		}
	}
}


