#define __TPL0102_C__
    #include "tpl0102.h"
#undef  __TPL0102_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c1.h"
#include "ir.h"
#include "option.h"
/* End Include */

/* Start Define */
#define TPL0102_DEVICE_MAX					( 4 )
#define TPL0102_FIFO_SIZE					( 8 )
#define TPL0102_FIFO_MASK					( TPL0102_FIFO_SIZE - 1 )        
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	U8					Hp;
	U8					Tp;
	U8					Data[TPL0102_FIFO_SIZE];
	U16					RegAddr[TPL0102_FIFO_SIZE];
}TPL0102_HandleType;
/* End Struct */

/* Start Function */
static bool TPL0102_Read( U16 DevAddr, U16 RegAddr, U8 *pData, U16 Size );
static bool TPL0102_Write( U16 DevAddr, U16 RegAddr, U8 *pData, U16 Size );
/* End Function */

/* Start Variable */
TPL0102_HandleType		TPL0102_Handle[TPL0102_DEVICE_MAX];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPL0102_Read( U16 DevAddr, U16 RegAddr, U8 *pData, U16 Size )
{
	return I2C1_Read( TPL0102_DeviceAddress( DevAddr ), RegAddr, pData, Size );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPL0102_Write( U16 DevAddr, U16 RegAddr, U8 *pData, U16 Size )
{                        
	bool		err				= true;
	U8			failcount		= 255;
	U16			DeviceAddress	= TPL0102_DeviceAddress( DevAddr );
	static U8	CheckBuf;
	
	I2C1_Write( DeviceAddress, RegAddr, pData, Size );
	
	while( failcount ){
		
		I2C1_Read( DeviceAddress, RegAddr, &CheckBuf, Size );
		
		if( CheckBuf == *pData ){
			err = false;
			break;
		}
		
		failcount--;
	}
	
	if( err == true ){ SystemError.Bit.TPL0102 = err; }
		
	return SystemError.Bit.TPL0102;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TPL0102_Init( void )
{
	bool	err = false;
    U8      reg[8][2], w;
    
#if 0
    reg[0][0] = 50;
    reg[0][1] = 50;
    for( w=0; w<8; w++ ){
        TPL0102_Write( w, 0, &reg[0][0], 2 ); 
    }
    return;
#endif
    
    for( w=0; w<SystemOption.Channels; w++ ){
        
		if( TPL0102_Read( w, 0, &reg[w][0], 2 ) == false ){
        	IrAmpCal.Offs[w] = reg[w][0];
        	IrAmpCal.Gain[w] = reg[w][1];
		}
		else {
			return true;
		}
    }
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void TPL0102_WiperUpdate( U16 DevAddr, U16 RegAddr, U8 Data )
{
	TPL0102_HandleType		*p = &TPL0102_Handle[DevAddr];
	U8						Hp = p->Hp & TPL0102_FIFO_MASK;
	
	p->Data[Hp]= Data;
	p->RegAddr[Hp] = RegAddr;
	p->Hp++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TPL0102_WiperHandler( U16 DevAddr )
{
	TPL0102_HandleType		*p = &TPL0102_Handle[DevAddr];
	U8						Hp = p->Hp & TPL0102_FIFO_MASK;
	U8						Tp = p->Tp & TPL0102_FIFO_MASK;
	
	if( Hp != Tp ){
		
		p->Tp++;
		
		TPL0102_Write( DevAddr, p->RegAddr[Tp], &p->Data[Tp], 1 );
	}
}


