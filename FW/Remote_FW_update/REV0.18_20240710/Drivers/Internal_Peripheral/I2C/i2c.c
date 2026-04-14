#define __I2C_C__
    #include "i2c.h"
#undef  __I2C_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "DmaLib.h"
#include "crc16.h"
#include "main.h"
/* End Include */

/* Start Define */
#define I2C0_ERR_CNT			( 1000 )
#define ErrCheck( reg, cnt )	while( (pADI_I2C0->MSTAT & reg) != reg ){ if( --cnt == 0 ){ return true; } }
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool I2C0_ErrCheck( U16 Reg );
/* End Function */
                                     
/* Start Variable */
U16		I2C_ErrCntMin;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void I2C_Init( void )
{
	DioCfgPin( pADI_GPIO0, PIN4, 1 );
	DioCfgPin( pADI_GPIO0, PIN5, 1 );
	DioDsPin( pADI_GPIO0, PIN4, 1 );
	DioDsPin( pADI_GPIO0, PIN5, 1 );
	
	I2cBaud( pADI_I2C0, 0x80, 0x81 );
	I2cMCfg( pADI_I2C0, 0, 0, BITM_I2C_MCTL_MASEN );
	
	I2C_ErrCntMin = I2C0_ERR_CNT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This function Starts an I2C Write sequence
bool I2C_TxPacket( U16 DevAddr, U8 *pData, U16 Size )
{
	U16		i;
	bool	err = false;
	
	I2cFifoFlush( pADI_I2C0, MASTER, 1 );
	I2cFifoFlush( pADI_I2C0, MASTER, 0 );
	
	if( Size ){
		I2cTx( pADI_I2C0, MASTER, *pData++ );
	}
	
	I2cMWrCfg( pADI_I2C0, DevAddr );
	
	for( i=1; i<Size; i++ ){
		if( I2C0_ErrCheck( 0x0004 ) == false ){
			I2cTx( pADI_I2C0, MASTER, *pData++ );
		}
		else {
			err = true;
			break;
		}
	}
	
	if( err == false ){
		err = I2C0_ErrCheck( 0x0100 );
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function used to commence I2C Read sequence
bool I2C_RxPacket( U16 DevAddr, U8 *pData, U16 Size )
{
	U16		i;
	bool	err = false;
	
	I2cFifoFlush( pADI_I2C0, MASTER, 1 );		// Flush Master Tx FIFO
	I2cFifoFlush( pADI_I2C0, MASTER, 0 );
	
	I2cMRdCfg( pADI_I2C0, DevAddr, Size, 0 );	// read "length" number of bytes from slave
	
	for( i=0; i<Size; i++ ){
		if( I2C0_ErrCheck( 0x0008 ) == false ){
			*pData++ = I2cRx( pADI_I2C0, MASTER );
		}
		else {
			err = true;
			break;
		}
	}
	
	if( err == false ){
		err = I2C0_ErrCheck( 0x0100 );
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool I2C0_ErrCheck( U16 Reg )
{
	U16		err_cnt = I2C0_ERR_CNT;
	bool	err = false;
	
	while( (pADI_I2C0->MSTAT & Reg) != Reg ){
		if( --err_cnt == 0 ){ err = true; }
	};
	
	if( I2C_ErrCntMin > err_cnt ){
		I2C_ErrCntMin = err_cnt;
	}
	
	return err;
}


