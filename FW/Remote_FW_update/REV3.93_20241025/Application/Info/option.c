#define __OPTION_C__
    #include "option.h"
#undef  __OPTION_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */                                    
#define RotarySwitch_Pin_8					( GPIO_PIN_11 ) 
#define RotarySwitch_Pin_4					( GPIO_PIN_12 )
#define RotarySwitch_Pin_2					( GPIO_PIN_13 )
#define RotarySwitch_Pin_1					( GPIO_PIN_14 )
#define RotarySwitch_GPIO_Port				( GPIOF )

#define PoeDetection_Pin					( GPIO_PIN_2 )
#define BatDetection_Pin					( GPIO_PIN_6 )
#define BuzDetection_Pin					( GPIO_PIN_5 )
#define OptDetection_GPIO_Port				( GPIOG )

#define UniDetection_Pin					( GPIO_PIN_6 )
#define UniDetection_GPIO_Port				( GPIOA )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void ); 
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool System_Opt_Load( void )
{
	bool	err = true;
	
	U16		i, cnt;
	U8		dat, buf, cop;
	
	GPIO_Init();
	
	for( i=0, cnt=0, buf=0, cop=0; i<0xFFFF; i++ ){
		
		dat = 0;
		
        if( HAL_GPIO_ReadPin( RotarySwitch_GPIO_Port, RotarySwitch_Pin_1	) == GPIO_PIN_RESET	){ dat |= 0x01; }
        if( HAL_GPIO_ReadPin( RotarySwitch_GPIO_Port, RotarySwitch_Pin_2	) == GPIO_PIN_RESET	){ dat |= 0x02; }
        if( HAL_GPIO_ReadPin( RotarySwitch_GPIO_Port, RotarySwitch_Pin_4	) == GPIO_PIN_RESET	){ dat |= 0x04; }
        if( HAL_GPIO_ReadPin( RotarySwitch_GPIO_Port, RotarySwitch_Pin_8	) == GPIO_PIN_RESET	){ dat |= 0x08; }
        if( HAL_GPIO_ReadPin( UniDetection_GPIO_Port, UniDetection_Pin		) == GPIO_PIN_RESET	){ dat |= 0x10; }
        if( HAL_GPIO_ReadPin( OptDetection_GPIO_Port, PoeDetection_Pin		) == GPIO_PIN_SET	){ dat |= 0x20; }
        if( HAL_GPIO_ReadPin( OptDetection_GPIO_Port, BatDetection_Pin		) == GPIO_PIN_RESET	){ dat |= 0x40; }
        if( HAL_GPIO_ReadPin( OptDetection_GPIO_Port, BuzDetection_Pin		) == GPIO_PIN_RESET	){ dat |= 0x80; }
		
		cop = dat ^ buf;
		
		if( cop ){
			cnt = 0;
		}
		else {
			cnt++;
			if( cnt == 1000 ){
				SystemOption.Byte = dat;
				if( SystemOption.Item.IsBatMode == true ){
					SystemOption.Item.IsPoeMode = false;
				}
				err = false;
				break;
			}
		}
		buf = dat;
		
		HAL_Delay( 0 );
	}
	
	dprintf( "\n  SYSTEM OPTION: 0x%02X\n", SystemOption.Byte );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
    
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStructure.Pin		= ( RotarySwitch_Pin_8 | RotarySwitch_Pin_4 | RotarySwitch_Pin_2 | RotarySwitch_Pin_1 );
    GPIO_InitStructure.Mode		= GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull		= GPIO_PULLUP;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_WritePin( RotarySwitch_GPIO_Port, GPIO_InitStructure.Pin, GPIO_PIN_SET );
    HAL_GPIO_Init( RotarySwitch_GPIO_Port, &GPIO_InitStructure );
	
	
	GPIO_InitStructure.Pin		= ( PoeDetection_Pin | BuzDetection_Pin | BatDetection_Pin );
	
	HAL_GPIO_WritePin( OptDetection_GPIO_Port, GPIO_InitStructure.Pin, GPIO_PIN_SET );
    HAL_GPIO_Init( OptDetection_GPIO_Port, &GPIO_InitStructure );
	
	
	GPIO_InitStructure.Pin		= ( UniDetection_Pin );
	
	HAL_GPIO_WritePin( UniDetection_GPIO_Port, GPIO_InitStructure.Pin, GPIO_PIN_SET );
    HAL_GPIO_Init( UniDetection_GPIO_Port, &GPIO_InitStructure );
}


