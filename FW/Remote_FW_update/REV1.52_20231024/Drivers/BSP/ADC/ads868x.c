#define __ADS868x_C__
    #include "ads868x.h"
#undef  __ADS868x_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "spi.h"
#include "lmt84.h"
#include "led.h"
#include "option.h"
/* End Include */

/* Start Define */                                                                                   
#define ADS868x_nCS_Pin         GPIO_PIN_4
#define ADS868x_nCS_GPIO_Port   GPIOA
#define ADS868x_nCS_H()         HAL_GPIO_WritePin( ADS868x_nCS_GPIO_Port, ADS868x_nCS_Pin, GPIO_PIN_SET )
#define ADS868x_nCS_L()         HAL_GPIO_WritePin( ADS868x_nCS_GPIO_Port, ADS868x_nCS_Pin, GPIO_PIN_RESET )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static U16 ADS868x_32BIT_TransmitReceive( U16 tx );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADS868x_Init( void )
{
	U8	err = 0;
    
	GPIO_Init();
   
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_AUTO_SEQ_EN, 0x00 );
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_CH_PWR_DOWN, 0x00 );
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_FEATURE_SELECT, 0x00 );
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_0, 0x06 );
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_1, 0x06 );
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_2, 0x06 );
    err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_3, 0x06 );
    
    if( SystemOption.Channels > 4 ){
        err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_4, 0x06 );
        err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_5, 0x06 );
        err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_6, 0x06 );
        err += ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_7, 0x06 );
    }
    
    if( err ){
        SystemError.Bit.eADC = 1;
    }
    else {
        SystemError.Bit.eADC = 0;
        
        ADS868x_32BIT_TransmitReceive( 0xC000 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	GPIO_InitTypeDef   GPIO_InitStruct = { 0 };
	
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin( ADS868x_nCS_GPIO_Port, ADS868x_nCS_Pin, GPIO_PIN_SET );
    
	GPIO_InitStruct.Pin		= ADS868x_nCS_Pin;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init( ADS868x_nCS_GPIO_Port, &GPIO_InitStruct ); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static U16 ADS868x_32BIT_TransmitReceive( U16 tx )
{  
    U16     TxD[2];
    U16     RxD[2];
    
    TxD[0] = tx;
    TxD[1] = 0;
    
    ADS868x_nCS_L();
    HAL_SPI_TransmitReceive( &SPI1_Handle, (uint8_t *)TxD, (uint8_t *)RxD, 2, 10000 );
    ADS868x_nCS_H();
    
    return RxD[1];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16 ADS868x_Manual_Conversion( U8 ReadCh )
{
    U16 tx;
    
    switch( ReadCh ){
        case 0: tx = 0xC400; break; // Channel-1 Set, Channel-0 Read
        case 1: tx = 0xC800; break;
        case 2: tx = 0xCC00; break;
        case 3: tx = 0xD000; break;
        case 4: tx = 0xD400; break;
        case 5: tx = 0xD800; break;
        case 6: tx = 0xDC00; break;
        case 7: tx = 0xC000; break;
    }
	
	if( ReadCh == (SystemOption.Channels-1) ){
		tx = 0xC000;
	}
    
    return ADS868x_32BIT_TransmitReceive( tx );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADS868x_Channel_Set( U8 SetCh )
{   
    U16     tx;
    
    switch( SetCh ){
        case 0: tx = 0xC000; break;
        case 1: tx = 0xC400; break;
        case 2: tx = 0xC800; break;
        case 3: tx = 0xCC00; break;
        case 4: tx = 0xD000; break;
        case 5: tx = 0xD400; break;
        case 6: tx = 0xD800; break;
        case 7: tx = 0xDC00; break;
        case 8: tx = 0xE000; break;
    }
                                        
    ADS868x_32BIT_TransmitReceive( tx );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16 ADS868x_Temp_Read( void )
{
    return ADS868x_32BIT_TransmitReceive( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ADS868x_Program_Register_Write( U8 addr, U8 data )
{
    U16     tx;
    U16     rx;
    U8      read;
    bool    err = false;
    
    tx = (U16)addr << 9 | 0x0100 | (U16)data;
    
    rx = ADS868x_32BIT_TransmitReceive( tx );
    
    read = rx >> 8;
    
    if( data != read ){
        err = true;
    }
    
    return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ADS868x_Gain_Check( U8 ReadCh )
{
    bool    err = false;
	U16		ch;
    
    switch( ReadCh ){
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_0: ch = 0x0A00; break; // 0000 1010 0000 0000 = 0x0005 << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_1: ch = 0x0C00; break; // 0000 1100 0000 0000 = 0x0006 << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_2: ch = 0x0E00; break; // 0000 1110 0000 0000 = 0x0007 << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_3: ch = 0x1000; break; // 0001 0000 0000 0000 = 0x0008 << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_4: ch = 0x1200; break; // 0001 0010 0000 0000 = 0x0009 << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_5: ch = 0x1400; break; // 0001 0100 0000 0000 = 0x000A << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_6: ch = 0x1600; break; // 0001 0110 0000 0000 = 0x000B << 9  
        case ADS868x_PROG_REG_INPUT_RANGE_SELECT_7: ch = 0x1800; break; // 0001 1000 0000 0000 = 0x000C << 9  
    }
    
    if( ADS868x_32BIT_TransmitReceive( ch ) != 0x0600 ){
        err = true;
    }
    
    return err;
}
