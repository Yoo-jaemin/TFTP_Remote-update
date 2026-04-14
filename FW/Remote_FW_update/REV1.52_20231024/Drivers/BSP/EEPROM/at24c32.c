#define __AT24C32_C__
    #include "AT24C32.h"
#undef  __AT24C32_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c2.h"
/* End Include */

/* Start Define */
#define AT24C32_DEVICE_ADDRESS			0xA0  
#define AT24C32_WRITE_PROTECT_PIN		GPIO_PIN_2
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void AT24C32_WriteProtect( bool use );
/* End Function */

/* Start Variable */
static uint8_t	AT24C32_ReadByte[2];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AT24C32_Init( void )
{
    GPIO_Init();
    AT24C32_WriteProtect( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AT24C32_WriteWord( U16 wAddr, U16 wData )
{
	static uint8_t	Byte[2];
	uint16_t		MemoryAddress = wAddr * 2;
	U8				FailCount = 5;
	bool			err = true;
	
	AT24C32_WriteProtect( false );
	
	Byte[0] = HI_BYTE( wData );
	Byte[1] = LO_BYTE( wData );
	
	while( FailCount )
	{
		I2C2_Write( AT24C32_DEVICE_ADDRESS, MemoryAddress, Byte, 2 );
		
		HAL_Delay( 70 );
		
		if( AT24C32_ReadWord( wAddr ) == wData ){
			err = false;
			break;
		}
		
		FailCount--;
	}
	
	if( err == true ){ SystemError.Bit.AT24C32 = err; }
	
	AT24C32_WriteProtect( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16 AT24C32_ReadWord( U16 wAddr )
{
    U16		MemoryAddress = wAddr * 2;
	U16		ret;
    
    I2C2_Read( AT24C32_DEVICE_ADDRESS, MemoryAddress, AT24C32_ReadByte, 2 );
	
	ret = ( ( (U16)AT24C32_ReadByte[0] << 8 & 0xFF00 ) | ( (U16)AT24C32_ReadByte[1] & 0x00FF ) );
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin( GPIOB, AT24C32_WRITE_PROTECT_PIN, GPIO_PIN_SET );

    GPIO_InitStructure.Pin       = AT24C32_WRITE_PROTECT_PIN;
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void AT24C32_WriteProtect( bool use )
{
	if( use ){ HAL_GPIO_WritePin( GPIOB, AT24C32_WRITE_PROTECT_PIN, GPIO_PIN_SET	); }
	else	 { HAL_GPIO_WritePin( GPIOB, AT24C32_WRITE_PROTECT_PIN, GPIO_PIN_RESET	); }
}


