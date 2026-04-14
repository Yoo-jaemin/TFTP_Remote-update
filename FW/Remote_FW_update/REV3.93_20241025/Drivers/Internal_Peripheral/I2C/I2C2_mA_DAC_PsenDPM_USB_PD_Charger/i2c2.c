#define __I2C2_C__
    #include "i2c2.h"
#undef  __I2C2_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c.h"
/* End Include */

/* Start Define */
#define TIMEOUT				( 1000 )
#define CLOCK_SPEED			( 100000 )
#define RETRY_COUNT			( 5 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool I2C2_ReInit( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
/* I2C2 init function */
HAL_StatusTypeDef I2C2_Init( void )
{
    I2C2_Handle.Instance				= I2C2;
    I2C2_Handle.Init.ClockSpeed			= CLOCK_SPEED;
	I2C2_Handle.Init.DutyCycle			= I2C_DUTYCYCLE_16_9;
    I2C2_Handle.Init.AddressingMode		= I2C_ADDRESSINGMODE_7BIT;
	I2C2_Handle.Init.DualAddressMode	= I2C_DUALADDRESS_DISABLE;
	I2C2_Handle.Init.GeneralCallMode	= I2C_GENERALCALL_DISABLE;
	I2C2_Handle.Init.NoStretchMode		= I2C_NOSTRETCH_DISABLE;
	I2C2_Handle.Init.OwnAddress1		= 0;
	I2C2_Handle.Init.OwnAddress2		= 0;
    
    return HAL_I2C_Init( &I2C2_Handle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C2_Write( uint16_t DevAddr, uint8_t *pData, uint16_t Size )
{
	U8		i;
	bool	err = false;
	
	for( i=0; i<RETRY_COUNT; i++ ){
	
		if( HAL_I2C_Master_Transmit( &I2C2_Handle, DevAddr, pData, Size, TIMEOUT ) == HAL_OK ){
			break;
		}
		else {
			if( HAL_I2C_GetError( &I2C2_Handle ) != HAL_I2C_ERROR_AF ){
				if( I2C2_ReInit() == true ){
					err = true;
					break;
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C2_Bulk_Write( uint16_t DevAddr, uint8_t *pData, uint16_t Size )
{
	bool	err = false;
	
	if( HAL_I2C_Master_Transmit( &I2C2_Handle, DevAddr, pData, Size, 10000 ) != HAL_OK ){
		if( HAL_I2C_GetError( &I2C2_Handle ) != HAL_I2C_ERROR_AF ){
			err = I2C2_ReInit();
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C2_Read( uint16_t DevAddr, uint8_t *pData, uint16_t Size )
{
	U8		i;
	bool	err = false;
	
	for( i=0; i<RETRY_COUNT; i++ ){
	
		if( HAL_I2C_Master_Receive( &I2C2_Handle, DevAddr, pData, Size, TIMEOUT ) == HAL_OK ){
			break;
		}
		else {
			if( HAL_I2C_GetError( &I2C2_Handle ) != HAL_I2C_ERROR_AF ){
				if( I2C2_ReInit() == true ){
					err = true;
					break;
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C2_Mem_8Bit_Write( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	U8		i;
	bool	err = false;
	
	for( i=0; i<RETRY_COUNT; i++ ){
		
		if( HAL_I2C_Mem_Write( &I2C2_Handle, DevAddr, MemAddr, I2C_MEMADD_SIZE_8BIT, pData, Size, TIMEOUT ) == HAL_OK ){
			break;
		}
		else {
			if( HAL_I2C_GetError( &I2C2_Handle ) != HAL_I2C_ERROR_AF ){
				if( I2C2_ReInit() == true ){
					err = true;
					break;
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C2_Mem_8Bit_Read( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	U8		i;
	bool	err = false;
	
	for( i=0; i<RETRY_COUNT; i++ ){
		
		if( HAL_I2C_Mem_Read( &I2C2_Handle, DevAddr, MemAddr, I2C_MEMADD_SIZE_8BIT, pData, Size, TIMEOUT ) == HAL_OK ){
			break;
		}
		else {
			if( HAL_I2C_GetError( &I2C2_Handle ) != HAL_I2C_ERROR_AF ){
				if( I2C2_ReInit() == true ){
					err = true;
					break;
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool I2C2_ReInit( void )
{
	bool	err = false;
	
	if( HAL_I2C_DeInit( &I2C2_Handle ) != HAL_OK ){ err = true; }
    if( HAL_I2C_Init  ( &I2C2_Handle ) != HAL_OK ){ err = true; }
	
	return err;
}


