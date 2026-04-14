#define __KEY_C__
    #include "key.h"
#undef  __KEY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "led.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */

#define KEY_BIT_FUNCTION			( BIT0 )
#define KEY_BIT_UP					( BIT1 )
#define KEY_BIT_DOWN				( BIT2 )
#define KEY_BIT_RESET				( BIT3 )
#define KEY_BIT_RESET_UP			( KEY_BIT_RESET | KEY_BIT_UP )

/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void KEY_Detection( void );
static void KEY_Short_Check( KEY_ScanType *pKey );
/* End Function */

/* Start Variable */
static LED_DriveType	*pLed = &LedDrive;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool KEY_Init( void )
{
	bool	err = false;
	U8		i;
	
    GPIO_Init();
	
	KeyScan.Data = NoKey;
	
	KeyScan.Port.Byte = 0xFF;
	KeyScan.Comp.Byte = 0x00;
	
	KeyScan.Short.Flag.Tact_Function	= false;
	KeyScan.Short.Flag.Reed_Function	= false;
	KeyScan.Short.Flag.Tact_Up			= false;
	KeyScan.Short.Flag.Reed_Up			= false;
	KeyScan.Short.Flag.Tact_Down		= false;
	KeyScan.Short.Flag.Reed_Down		= false;
	KeyScan.Short.Flag.Tact_Reset		= false;
	KeyScan.Short.Flag.Reed_Reset		= false;
	
	for( i=0; i<KEY_INDEX_MAX; i++ ){
		switch( i ){ 
			case KEY_INDEX_FUNCTION:
			case KEY_INDEX_RESET:
			case KEY_INDEX_RESET_UP:
				KeyScan.Tick[i].IsRepeatMode = false;
				break;
				
			case KEY_INDEX_UP:
			case KEY_INDEX_DOWN:
				KeyScan.Tick[i].IsRepeatMode = true;
				break;
		}
		KeyScan.Tick[i].Cnt			= 0;
		KeyScan.Tick[i].Debouce		= KEY_SCAN_MSEC(   40 );
		KeyScan.Tick[i].RepeatStart	= KEY_SCAN_MSEC(  400 );
		KeyScan.Tick[i].Repeat		= KEY_SCAN_MSEC(   50 );
		KeyScan.Tick[i].Long		= KEY_SCAN_MSEC( 2000 );
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KEY_Scan( void )
{
	static KEY_ScanPort		bData;
	
	bData.Byte = 0xFF;
	
	if( SystemState == SYSTEM_STATE_INIT ){ return; }
	
	KEY_Short_Check( &KeyScan );
	
	if( KeyScan.Short.Flag.Tact_Function == false ){
		bData.Bit.Function &= HAL_GPIO_ReadPin( KeyScan.Gpio.Tact_Function.Port, KeyScan.Gpio.Tact_Function.Pin );
	}
	if( KeyScan.Short.Flag.Reed_Function == false ){
		bData.Bit.Function &= HAL_GPIO_ReadPin( KeyScan.Gpio.Reed_Function.Port, KeyScan.Gpio.Reed_Function.Pin );
	}
	if( KeyScan.Short.Flag.Tact_Up == false ){
		bData.Bit.Up &= HAL_GPIO_ReadPin( KeyScan.Gpio.Tact_Up.Port, KeyScan.Gpio.Tact_Up.Pin );
	}
	if( KeyScan.Short.Flag.Reed_Up == false ){
		bData.Bit.Up &= HAL_GPIO_ReadPin( KeyScan.Gpio.Reed_Up.Port, KeyScan.Gpio.Reed_Up.Pin );
	}
	if( KeyScan.Short.Flag.Tact_Down == false ){
		bData.Bit.Down &= HAL_GPIO_ReadPin( KeyScan.Gpio.Tact_Down.Port, KeyScan.Gpio.Tact_Down.Pin );
	}
	if( KeyScan.Short.Flag.Reed_Down == false ){
		bData.Bit.Down &= HAL_GPIO_ReadPin( KeyScan.Gpio.Reed_Down.Port, KeyScan.Gpio.Reed_Down.Pin );
	}
	if( KeyScan.Short.Flag.Tact_Reset == false ){
		bData.Bit.Reset &= HAL_GPIO_ReadPin( KeyScan.Gpio.Tact_Reset.Port, KeyScan.Gpio.Tact_Reset.Pin );
	}
	if( KeyScan.Short.Flag.Reed_Reset == false ){
		bData.Bit.Reset &= HAL_GPIO_ReadPin( KeyScan.Gpio.Reed_Reset.Port, KeyScan.Gpio.Reed_Reset.Pin );
	}
	
	KeyScan.Comp.Byte = KeyScan.Port.Byte ^ bData.Byte;
	KeyScan.Port.Byte = bData.Byte;
	
	if( KeyScan.Comp.Byte ){
		KeyScan.Tick[KEY_INDEX_FUNCTION].Cnt		= 0;
		KeyScan.Tick[KEY_INDEX_UP].Cnt				= 0;
		KeyScan.Tick[KEY_INDEX_DOWN].Cnt			= 0;
		KeyScan.Tick[KEY_INDEX_RESET].Cnt			= 0;
		KeyScan.Tick[KEY_INDEX_RESET_UP].Cnt		= 0;
		
		if( (KeyScan.Comp.Bit.Function	& KeyScan.Port.Bit.Function	) == true ){ pLed->Set.Bit.Function	= false; }
		if( (KeyScan.Comp.Bit.Up		& KeyScan.Port.Bit.Up		) == true ){ pLed->Set.Bit.Up		= false; }
		if( (KeyScan.Comp.Bit.Down	 	& KeyScan.Port.Bit.Down		) == true ){ pLed->Set.Bit.Down		= false; }
		if( (KeyScan.Comp.Bit.Reset	 	& KeyScan.Port.Bit.Reset	) == true ){ pLed->Set.Bit.Reset	= false; }
	}
	else {
		KEY_Detection();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	uint32_t			GpioC_All_Pin;
	uint32_t			GpioE_All_Pin;
	uint32_t			GpioF_All_Pin;
	uint32_t			GpioG_All_Pin;
	
    /**BSP KEY GPIO Configuration
    PC3     ------> REED S/W UP
	
    PE0     ------> TACK S/W UP
    PE1     ------> TACK S/W DOWN
    PE2     ------> TACK S/W RESET
    PE3     ------> TACK S/W FUNCTION
	
    PF9     ------> REED S/W FUNCTION
	
    PG0     ------> REED S/W RESET
    PG1     ------> REED S/W DOWN
    */
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	KeyScan.Gpio.Reed_Up.Port		= GPIOC;
	
	KeyScan.Gpio.Tact_Up.Port		= GPIOE;
	KeyScan.Gpio.Tact_Down.Port		= GPIOE;
	KeyScan.Gpio.Tact_Reset.Port	= GPIOE;
	KeyScan.Gpio.Tact_Function.Port	= GPIOE;
	
	KeyScan.Gpio.Reed_Function.Port	= GPIOF;
	
	KeyScan.Gpio.Reed_Reset.Port	= GPIOG;
	KeyScan.Gpio.Reed_Down.Port		= GPIOG;
	
	KeyScan.Gpio.Reed_Up.Pin		= GPIO_PIN_3;
	
	KeyScan.Gpio.Tact_Up.Pin		= GPIO_PIN_0;
	KeyScan.Gpio.Tact_Down.Pin		= GPIO_PIN_1;
	KeyScan.Gpio.Tact_Reset.Pin		= GPIO_PIN_2;
	KeyScan.Gpio.Tact_Function.Pin	= GPIO_PIN_3;
	
	KeyScan.Gpio.Reed_Function.Pin	= GPIO_PIN_9;
	
	KeyScan.Gpio.Reed_Reset.Pin		= GPIO_PIN_0;
	KeyScan.Gpio.Reed_Down.Pin		= GPIO_PIN_1;
	
    GpioC_All_Pin					= ( GPIO_PIN_3 );
    GpioE_All_Pin					= ( GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );
    GpioF_All_Pin					= ( GPIO_PIN_9 );
    GpioG_All_Pin					= ( GPIO_PIN_0 | GPIO_PIN_1 );
	
    GPIO_InitStructure.Mode      	= GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull      	= GPIO_PULLUP;
    GPIO_InitStructure.Speed     	= GPIO_SPEED_FREQ_HIGH;
	
    GPIO_InitStructure.Pin = GpioC_All_Pin;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = GpioE_All_Pin;
	HAL_GPIO_Init( GPIOE, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = GpioF_All_Pin;
    HAL_GPIO_Init( GPIOF, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = GpioG_All_Pin;
    HAL_GPIO_Init( GPIOG, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KEY_Detection( void )
{
	U8				bCode;
	U8				i;
	KEY_DataType	KeyBuf;
	
	bCode = ~KeyScan.Port.Byte;
	
	switch( bCode ){
		case KEY_BIT_FUNCTION		: i = KEY_INDEX_FUNCTION,		KeyBuf = Function;		break;
		case KEY_BIT_UP				: i = KEY_INDEX_UP,				KeyBuf = Up;			break;
		case KEY_BIT_DOWN			: i = KEY_INDEX_DOWN,			KeyBuf = Down;			break;
		case KEY_BIT_RESET			: i = KEY_INDEX_RESET,			KeyBuf = Reset;			break;
		case KEY_BIT_RESET_UP		: i = KEY_INDEX_RESET_UP,		KeyBuf = Reset_Up;		break;
		default: return;
	}
	
	if( KeyScan.Tick[i].Cnt != 0xFFFF ){
		KeyScan.Tick[i].Cnt++;
	}
	
	if( KeyScan.Tick[i].Cnt == KeyScan.Tick[i].Debouce ){
		KeyScan.Data = KeyBuf;
		KeyScan.Type = Short;
		switch( i ){
			case KEY_INDEX_FUNCTION:	pLed->Set.Bit.Function	= true; break;
			case KEY_INDEX_UP:			pLed->Set.Bit.Up		= true; break;
			case KEY_INDEX_DOWN:		pLed->Set.Bit.Down		= true; break;
			case KEY_INDEX_RESET:		pLed->Set.Bit.Reset		= true; break;
			case KEY_INDEX_RESET_UP:	pLed->Set.Bit.Reset		= true; pLed->Set.Bit.Up = true; break;
		}
	}
	else if( KeyScan.Tick[i].Cnt == KeyScan.Tick[i].RepeatStart ){
		if( KeyScan.Tick[i].IsRepeatMode == true ){
			KeyScan.Tick[i].Cnt = KeyScan.Tick[i].RepeatStart - KeyScan.Tick[i].Repeat;
			KeyScan.Data = KeyBuf;
			KeyScan.Type = Repeat;
		}
	}
	else if( KeyScan.Tick[i].Cnt == KeyScan.Tick[i].Long ){
		KeyScan.Data = KeyBuf;
		KeyScan.Type = Long;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KEY_Short_Check( KEY_ScanType *pKey )
{
	U8				i;
	GPIO_TypeDef	*Port;
	uint16_t		Pin;
	U32				*pTick;
	bool			*pFlag;
	
	
	for( i=0; i<8; i++ ){
		switch( i ){
			case 0:
				Port = KeyScan.Gpio.Tact_Function.Port;
				Pin = KeyScan.Gpio.Tact_Function.Pin;
				pTick = &pKey->Short.Tick.Tact_Function;
				pFlag = &pKey->Short.Flag.Tact_Function;
				break;
				
			case 1:
				Port = KeyScan.Gpio.Reed_Function.Port;
				Pin = KeyScan.Gpio.Reed_Function.Pin;
				pTick = &pKey->Short.Tick.Reed_Function;
				pFlag = &pKey->Short.Flag.Reed_Function;
				break;
				
			case 2:
				Port = KeyScan.Gpio.Tact_Up.Port;
				Pin = KeyScan.Gpio.Tact_Up.Pin;
				pTick = &pKey->Short.Tick.Tact_Up;
				pFlag = &pKey->Short.Flag.Tact_Up;
				break;
				
			case 3:
				Port = KeyScan.Gpio.Reed_Up.Port;
				Pin = KeyScan.Gpio.Reed_Up.Pin;
				pTick = &pKey->Short.Tick.Reed_Up;
				pFlag = &pKey->Short.Flag.Reed_Up;
				break;
				
			case 4:
				Port = KeyScan.Gpio.Tact_Down.Port;
				Pin = KeyScan.Gpio.Tact_Down.Pin;
				pTick = &pKey->Short.Tick.Tact_Down;
				pFlag = &pKey->Short.Flag.Tact_Down;
				break;
				
			case 5:
				Port = KeyScan.Gpio.Reed_Down.Port;
				Pin = KeyScan.Gpio.Reed_Down.Pin;
				pTick = &pKey->Short.Tick.Reed_Down;
				pFlag = &pKey->Short.Flag.Reed_Down;
				break;
				
			case 6:
				Port = KeyScan.Gpio.Tact_Reset.Port;
				Pin = KeyScan.Gpio.Tact_Reset.Pin;
				pTick = &pKey->Short.Tick.Tact_Reset;
				pFlag = &pKey->Short.Flag.Tact_Reset;
				break;
				
			case 7:
				Port = KeyScan.Gpio.Reed_Reset.Port;
				Pin = KeyScan.Gpio.Reed_Reset.Pin;
				pTick = &pKey->Short.Tick.Reed_Reset;
				pFlag = &pKey->Short.Flag.Reed_Reset;
				break;
		}
	
		if( HAL_GPIO_ReadPin( Port, Pin ) == GPIO_PIN_RESET ){
			if( *pTick < KEY_SCAN_MSEC( 600000 ) ){
				*pTick += 1;
			}
			else {
				*pFlag = true;
			}
		}
		else {
			*pTick = 0;
			*pFlag = false;
		}
	}
}

