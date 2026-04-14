#define __MCP46X1_C__
    #include "mcp46x1.h"
#undef  __MCP46X1_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c2.h"
/* End Include */

/* Start Define */
#define MCP46X1_DEVICE_ADDRESS			( (uint16_t)0x52 )
#define MCP46X1_WIPER_MAX				( 255 )
/* End Define */

/* Start Enum */
enum {
	MCP46x1_CMD_WR_DATA,
	MCP46x1_CMD_INCREMENT,
	MCP46x1_CMD_DECREMENT,
	MCP46x1_CMD_RD_DATA,
	MCP46x1_CMD_MAX
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCP46X1_Init( void )
{
	bool	err = false;
	
	err |= MCP46X1_Rd_Wiper( MCP46x1_MEM_VOLATILE_WIPER_0, &MCP46x1_Wiper[0] );
	err |= MCP46X1_Rd_Wiper( MCP46x1_MEM_VOLATILE_WIPER_1, &MCP46x1_Wiper[1] );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MCP46X1_Wr_Wiper( U8 WiperAddress, U16 WiperValue )
{
	bool				err = true;
	U8					failcount = 255;
	MCP46x1_WiperType	WrWiper, RdWiper;
	U8					ReadIndex;
	
	WrWiper.Byte[0] = ((WiperAddress << 4 & 0xF0) | (MCP46x1_CMD_WR_DATA << 2 & 0x0C) | (WiperValue >> 8 & 0x01));
	WrWiper.Byte[1] = (uint8_t)(WiperValue & 0xFF);
		
	I2C2_Write( MCP46X1_DEVICE_ADDRESS, WrWiper.Byte, 2 );
	
	while( failcount-- ){
		
		if( (WiperAddress == MCP46x1_MEM_NON_VOLATILE_WIPER_0) || (WiperAddress == MCP46x1_MEM_NON_VOLATILE_WIPER_1) ){
			ReadIndex = WiperAddress - 2;
		}
		else {
			ReadIndex = WiperAddress;
		}
		
		MCP46X1_Rd_Wiper( WiperAddress, &RdWiper );
		
		if( RdWiper.Word == WiperValue ){
			MCP46x1_Wiper[ReadIndex].Word = RdWiper.Word;
			err = false;
			break;
		}
	}
	
	if( err == true ){ SystemError.Peripheral.Bit.Psen_Potentiometer = err; }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCP46X1_Rd_Wiper( U8 WiperAddress, MCP46x1_WiperType *pWiper )
{
	bool		err = true;
	uint8_t		Command;
	
	Command = ((WiperAddress << 4 & 0xF0) | (MCP46x1_CMD_RD_DATA << 2 & 0x0C));
	
	if( I2C2_Write( MCP46X1_DEVICE_ADDRESS, &Command, 1 ) == false ){
		if( I2C2_Read( MCP46X1_DEVICE_ADDRESS, pWiper->Byte, 2 ) == false ){
			pWiper->Word = SWAP16( pWiper->Word );
			err = false;
		}
	}
	
	return err;
}


