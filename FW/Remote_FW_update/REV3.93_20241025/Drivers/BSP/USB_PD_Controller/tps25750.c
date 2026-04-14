#define __TPS25750_C__
    #include "tps25750.h"
#undef  __TPS25750_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c2.h"
#include "tps25750_config.h"
/* End Include */

/* Start Define */
#define TPS25750_DEVICE_ADDRESS			( 0x21 )
#define TPS25750_BUNDLE_ADDRESS			( 0x50 )
#define ERROR_CNT						( 10 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool TPS25750_Patch_Bundle_Update( void );
static bool TPS25750_PBM_GetError( uint16_t MemAddr );
static bool TPS25750_PBM_Precess( void );
static bool TPS25750_Clear_DeadBatteryFlag( TPS25750_HandleType *ppd );
static bool TPS25750_Reg_Write( uint16_t MemAddr, uint8_t *pData, uint16_t Size );
static bool TPS25750_Reg_Read( uint16_t MemAddr, uint8_t *pData, uint16_t Size );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TPS25750_Init( TPS25750_HandleType *ppd )
{
	bool	err = false;
	
	err |= TPS25750_Patch_Bundle_Update();
	err |= TPS25750_Read_Status( ppd );
	err |= TPS25750_Read_BootStatus( ppd );
	err |= TPS25750_Clear_DeadBatteryFlag( ppd );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TPS25750_Read_Status( TPS25750_HandleType *ppd )
{
	U8		b[6];
	bool	err = TPS25750_Reg_Read( 0x1A, b, 6 );
	
	if( err == false ){
		ppd->Status.Byte[0] = b[1];
		ppd->Status.Byte[1] = b[2];
		ppd->Status.Byte[2] = b[3];
		ppd->Status.Byte[3] = b[4];
		ppd->Status.Byte[4] = b[5];
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TPS25750_Read_BootStatus( TPS25750_HandleType *ppd )
{
	U8		b[6];
	bool	err = TPS25750_Reg_Read( 0x2D, b, 6 );
	
	if( err == false ){
		ppd->BootStatus.Byte[0] = b[1];
		ppd->BootStatus.Byte[1] = b[2];
		ppd->BootStatus.Byte[2] = b[3];
		ppd->BootStatus.Byte[3] = b[4];
		ppd->BootStatus.Byte[4] = b[5];
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TPS25750_Patch_Bundle_Reload( void )
{
	U8		b[5];
	
	if( TPS25750_Reg_Read( 0x03, b, 5 ) == false ){
		if( (b[1] == 'P') && (b[2] == 'T') && (b[3] == 'C') &&(b[4] == 'H') ){
			return TPS25750_PBM_Precess();
		}
	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPS25750_Patch_Bundle_Update( void )
{
	U8		b[5];
	U16		eCnt;
	
	dprintf( "\n" );
	
	for( eCnt=0; eCnt<ERROR_CNT; eCnt++ ){
		
		if( TPS25750_Reg_Read( 0x03, b, 5 ) == false ){
			if		( (b[1] == 'P') && (b[2] == 'T') && (b[3] == 'C') &&(b[4] == 'H') ){
				dprintf( "  PTCH rCnt: %d\n", eCnt );
				return TPS25750_PBM_Precess();
			}
			else if	( (b[1] == 'A') && (b[2] == 'P') && (b[3] == 'P') &&(b[4] == ' ') ){
				dprintf( "  APP  rCnt: %d\n", eCnt );
				return false;
			}
		}
	}
	dprintf( "  MODE eCnt: %d\n", eCnt );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPS25750_PBM_Precess( void )
{
	U8		b[7];
	uulong	BundleSize;
	bool	err = false;
	
	BundleSize.l = sizeof( TPS25750_Config_Low_Region );
		
	b[0] = 6; // Byte Count
	b[1] = BundleSize.b._0; // Bundle Size  7: 0
	b[2] = BundleSize.b._1; // Bundle Size 15: 8
	b[3] = BundleSize.b._2; // Bundle Size 23:16
	b[4] = BundleSize.b._3; // Bundle Size 31:24
	b[5] = TPS25750_BUNDLE_ADDRESS; // Slave Address
	b[6] = 50; // Timeout Value (x100msec)
	err |= TPS25750_Reg_Write( 0x09, b, 7 );
	HAL_Delay( 10 );
	b[0] = 4; // Byte Count
	b[1] = 'P';
	b[2] = 'B';
	b[3] = 'M';
	b[4] = 's';
	err |= TPS25750_Reg_Write( 0x08, b, 5 );
	HAL_Delay( 10 );
	err |= TPS25750_PBM_GetError( 0x08 );
	err |= TPS25750_PBM_GetError( 0x09 );
	err |= I2C2_Bulk_Write( (TPS25750_BUNDLE_ADDRESS << 1), (uint8_t *)TPS25750_Config_Low_Region, BundleSize.l );
	HAL_Delay( 10 );
	
	b[0] = 4; // Byte Count
	b[1] = 'P';
	b[2] = 'B';
	b[3] = 'M';
	b[4] = 'c';
	err |= TPS25750_Reg_Write( 0x08, b, 5 );
	HAL_Delay( 100 );
	err |= TPS25750_PBM_GetError( 0x08 );
	err |= TPS25750_PBM_GetError( 0x09 );
	HAL_Delay( 10 );
	err |= TPS25750_PBM_GetError( 0x03 );
	
	if( err == true ){
		dprintf( "  TPS25750 PATCH LOAD FAILURE\n" );
	}
	else {
		dprintf( "  TPS25750 PATCH LOAD SUCCESS\n" );
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPS25750_PBM_GetError( uint16_t MemAddr )
{
	U8		b[5];
	U8		c[5];
	U16		eCnt;
	
	switch( MemAddr ){
		case 0x03:
			c[1] = 'A';
			c[2] = 'P';
			c[3] = 'P';
			c[4] = ' ';
			break;
			
		case 0x08:
		case 0x09:
			c[1] = 0;
			c[2] = 0;
			c[3] = 0;
			c[4] = 0;
			break;
	}
	
	for( eCnt=0; eCnt<ERROR_CNT; eCnt++ ){
	
		if( I2C2_Mem_8Bit_Read( (TPS25750_DEVICE_ADDRESS << 1), MemAddr, b, 5 ) == false ){
			if( (b[1] == c[1]) && (b[2] == c[2]) && (b[3] == c[3]) &&(b[4] == c[4]) ){
				dprintf( "  PBMx rCnt: %d\n", eCnt );
				return false;
			}
		}
	}
	
    dprintf( "  PBMx eCnt: %d\n", eCnt );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPS25750_Clear_DeadBatteryFlag( TPS25750_HandleType *ppd )
{
	U8			b[5];
	bool		err = false;
	
	if( ppd->BootStatus.Item.DeadBatteryFlag == true ){
		
		dprintf( "\n  DEAD BATTERY DETECTION!\n" );
		
		b[0] = 4; // Byte Count
		b[1] = 'D';
		b[2] = 'B';
		b[3] = 'f';
		b[4] = 'g';
		
		err |= TPS25750_Reg_Write( 0x08, b, 5 );
		HAL_Delay( 10 );
		err |= TPS25750_PBM_GetError( 0x08 );
		err |= TPS25750_PBM_GetError( 0x09 );
		err |= TPS25750_Read_BootStatus( ppd );
		
		err |= ppd->BootStatus.Item.DeadBatteryFlag;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPS25750_Reg_Write( uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	return I2C2_Mem_8Bit_Write( (TPS25750_DEVICE_ADDRESS << 1), MemAddr, pData, Size );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool TPS25750_Reg_Read( uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	return I2C2_Mem_8Bit_Read( (TPS25750_DEVICE_ADDRESS << 1), MemAddr, pData, Size );
}


