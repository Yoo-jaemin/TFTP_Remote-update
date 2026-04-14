#define __I2C1_C__
    #include "i2c1.h"
#undef  __I2C1_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "i2c.h"
/* End Include */

/* Start Define */
#define TIMEOUT				( 1000 )
#define CLOCK_SPEED			( 100000 )
#define RETRY_COUNT			( 2 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool I2C1_ReInit( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
/* I2C1 init function */
HAL_StatusTypeDef I2C1_Init( void )
{
    I2C1_Handle.Instance				= I2C1;
    I2C1_Handle.Init.ClockSpeed			= CLOCK_SPEED;
    I2C1_Handle.Init.DutyCycle			= I2C_DUTYCYCLE_16_9;
    I2C1_Handle.Init.OwnAddress1		= 0;
    I2C1_Handle.Init.AddressingMode		= I2C_ADDRESSINGMODE_7BIT;
    I2C1_Handle.Init.DualAddressMode	= I2C_DUALADDRESS_DISABLE;
    I2C1_Handle.Init.OwnAddress2		= 0;
    I2C1_Handle.Init.GeneralCallMode	= I2C_GENERALCALL_DISABLE;
    I2C1_Handle.Init.NoStretchMode		= I2C_NOSTRETCH_DISABLE; 
    
    return HAL_I2C_Init( &I2C1_Handle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C1_Mem_16Bit_Write( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	U8		i;
	bool	err = false;
	
	for( i=0; i<RETRY_COUNT; i++ ){
		
		if( HAL_I2C_Mem_Write( &I2C1_Handle, DevAddr, MemAddr, I2C_MEMADD_SIZE_16BIT, pData, Size, TIMEOUT ) == HAL_OK ){
			break;
		}
		else {
			if( HAL_I2C_GetError( &I2C1_Handle ) != HAL_I2C_ERROR_AF ){
				if( I2C1_ReInit() == true ){
					err = true;
					break;
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool I2C1_Mem_16Bit_Read( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	U8		i;
	bool	err = false;
	
	for( i=0; i<RETRY_COUNT; i++ ){
		
		if( HAL_I2C_Mem_Read( &I2C1_Handle, DevAddr, MemAddr, I2C_MEMADD_SIZE_16BIT, pData, Size, TIMEOUT ) == HAL_OK ){
			break;
		}
		else {
			if( HAL_I2C_GetError( &I2C1_Handle ) != HAL_I2C_ERROR_AF ){
				if( I2C1_ReInit() == true ){
					err = true;
					break;
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool I2C1_ReInit( void )
{
	bool	err = false;
	
	if( HAL_I2C_DeInit( &I2C1_Handle ) != HAL_OK ){ err = true; }
    if( HAL_I2C_Init  ( &I2C1_Handle ) != HAL_OK ){ err = true; }
	
	return err;
}
