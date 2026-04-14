#define __RELAY_C__
    #include "relay.h"
#undef  __RELAY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "eeprom.h"
#include "option.h"
/* End Include */

/* Start Define */
#define ENERGIZED			( GPIO_PIN_RESET )
#define DE_ENERGIZED		( GPIO_PIN_SET   )
/* End Define */

/* Start Enum */
enum {
	AL1,
	AL2,
	TRB
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void Relay_Tmr( Relay_DriveItem *pRelay );
static void Relay_Set( Relay_DriveItem *pRelay );
static void Relay_Test_Set( Relay_DriveItem *pRelay );
static void Relay_Gpio_Write( Relay_DriveItem *pRelay );
/* End Function */

/* Start Variable */ 
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Relay_Init( void )
{
	bool	err = false;
	
	if( SystemOption.Item.IsBatMode == false ){
	
		GPIO_Init();
		
		RelayDrive.TRB.Opt.Word = EepHandle.Sram[SYSTEM_EEP_RELAY_OPTION_TRB];
		RelayDrive.AL1.Opt.Word = EepHandle.Sram[SYSTEM_EEP_RELAY_OPTION_AL1];
		RelayDrive.AL2.Opt.Word = EepHandle.Sram[SYSTEM_EEP_RELAY_OPTION_AL2];
		
		RelayDrive.TRB.Set = false;
		RelayDrive.AL1.Set = false;
		RelayDrive.AL2.Set = false;
		
		RelayDrive.TRB.Blk.Item.IsEnable = false;
		RelayDrive.AL1.Blk.Item.IsEnable = false;
		RelayDrive.AL2.Blk.Item.IsEnable = false;
		
		RelayDrive.TRB.Blk.Item.DriveOut = false;
		RelayDrive.AL1.Blk.Item.DriveOut = false;
		RelayDrive.AL2.Blk.Item.DriveOut = false;
		
		RelayDrive.TRB.Blk.Item.Act_Time = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_RELAY_BLINK_TIME_TRB] );
		RelayDrive.TRB.Blk.Item.Rel_Time = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_RELAY_BLINK_TIME_TRB] );
		RelayDrive.AL1.Blk.Item.Act_Time = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_RELAY_BLINK_TIME_AL1] );
		RelayDrive.AL1.Blk.Item.Rel_Time = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_RELAY_BLINK_TIME_AL1] );
		RelayDrive.AL2.Blk.Item.Act_Time = HI_BYTE( EepHandle.Sram[SYSTEM_EEP_RELAY_BLINK_TIME_AL2] );
		RelayDrive.AL2.Blk.Item.Rel_Time = LO_BYTE( EepHandle.Sram[SYSTEM_EEP_RELAY_BLINK_TIME_AL2] );
		
		RelayDrive.TRB.Out = true;
		RelayDrive.AL1.Out = true;
		RelayDrive.AL2.Out = true;
		
		RelayDrive.IsTestMode = false;
		RelayDrive.UseWebTest = false;
		
		Relay_Task();
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Relay_ReInit( void )
{
	RelayDrive.TRB.Opt.Word = EepHandle.pDefault[SYSTEM_EEP_RELAY_OPTION_TRB];
	RelayDrive.AL1.Opt.Word = EepHandle.pDefault[SYSTEM_EEP_RELAY_OPTION_AL1];
	RelayDrive.AL2.Opt.Word = EepHandle.pDefault[SYSTEM_EEP_RELAY_OPTION_AL2];
	
	RelayDrive.TRB.Set = false;
	RelayDrive.AL1.Set = false;
	RelayDrive.AL2.Set = false;
	
	RelayDrive.TRB.Blk.Item.IsEnable = false;
	RelayDrive.AL1.Blk.Item.IsEnable = false;
	RelayDrive.AL2.Blk.Item.IsEnable = false;
	
	RelayDrive.TRB.Blk.Item.DriveOut = false;
	RelayDrive.AL1.Blk.Item.DriveOut = false;
	RelayDrive.AL2.Blk.Item.DriveOut = false;
	
	RelayDrive.TRB.Blk.Item.Act_Time = HI_BYTE( EepHandle.pDefault[SYSTEM_EEP_RELAY_BLINK_TIME_TRB] );
	RelayDrive.TRB.Blk.Item.Rel_Time = LO_BYTE( EepHandle.pDefault[SYSTEM_EEP_RELAY_BLINK_TIME_TRB] );
	RelayDrive.AL1.Blk.Item.Act_Time = HI_BYTE( EepHandle.pDefault[SYSTEM_EEP_RELAY_BLINK_TIME_AL1] );
	RelayDrive.AL1.Blk.Item.Rel_Time = LO_BYTE( EepHandle.pDefault[SYSTEM_EEP_RELAY_BLINK_TIME_AL1] );
	RelayDrive.AL2.Blk.Item.Act_Time = HI_BYTE( EepHandle.pDefault[SYSTEM_EEP_RELAY_BLINK_TIME_AL2] );
	RelayDrive.AL2.Blk.Item.Rel_Time = LO_BYTE( EepHandle.pDefault[SYSTEM_EEP_RELAY_BLINK_TIME_AL2] );
	
	RelayDrive.TRB.Out = true;
	RelayDrive.AL1.Out = true;
	RelayDrive.AL2.Out = true;
	
	RelayDrive.IsTestMode = false;
	RelayDrive.UseWebTest = false;
	
	Relay_Task();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Relay_Task( void )
{
	if( RelayDrive.IsTestMode == true ){
		Relay_Test_Set( &RelayDrive.TRB );
		Relay_Test_Set( &RelayDrive.AL1 );
		Relay_Test_Set( &RelayDrive.AL2 );
	}
	else {
		Relay_Set( &RelayDrive.TRB );
		Relay_Set( &RelayDrive.AL1 );
		Relay_Set( &RelayDrive.AL2 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Relay_Blink_Timer( void )
{
	Relay_Tmr( &RelayDrive.TRB );
	Relay_Tmr( &RelayDrive.AL1 );
	Relay_Tmr( &RelayDrive.AL2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	
    /**BSP LED GPIO Configuration   
    PF6     ------> ALARM-1 
    PF7     ------> ALARM-2
    PF8     ------> TROUBLE
    */
	
    __HAL_RCC_GPIOF_CLK_ENABLE();
	
	RelayDrive.TRB.Gpio.Port	= GPIOF;
	RelayDrive.AL1.Gpio.Port	= GPIOF;
	RelayDrive.AL2.Gpio.Port	= GPIOF;
	
	RelayDrive.TRB.Gpio.Pin		= GPIO_PIN_8;
	RelayDrive.AL1.Gpio.Pin		= GPIO_PIN_6;
	RelayDrive.AL2.Gpio.Pin		= GPIO_PIN_7;
	
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_NOPULL;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pin		= ( RelayDrive.TRB.Gpio.Pin | RelayDrive.AL1.Gpio.Pin | RelayDrive.AL2.Gpio.Pin );
	
	HAL_GPIO_WritePin( RelayDrive.TRB.Gpio.Port, RelayDrive.TRB.Gpio.Pin, GPIO_PIN_RESET );
	HAL_GPIO_WritePin( RelayDrive.AL1.Gpio.Port, RelayDrive.AL1.Gpio.Pin, GPIO_PIN_RESET );
	HAL_GPIO_WritePin( RelayDrive.AL2.Gpio.Port, RelayDrive.AL2.Gpio.Pin, GPIO_PIN_RESET );
	
    HAL_GPIO_Init( GPIOF, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Relay_Tmr( Relay_DriveItem *pRelay )
{
	if( pRelay->Blk.Item.IsEnable == true ){
		if( pRelay->Blk.Item.DriveOut == true ){
			
			pRelay->Blk.Item.Act_Tick--;
			if( pRelay->Blk.Item.Act_Tick == 0 ){
				pRelay->Blk.Item.DriveOut = false;
				pRelay->Blk.Item.Rel_Tick = pRelay->Blk.Item.Rel_Time;
			}
		}
		else {
			pRelay->Blk.Item.Rel_Tick--;
			if( pRelay->Blk.Item.Rel_Tick == 0 ){
				pRelay->Blk.Item.DriveOut = true;
				pRelay->Blk.Item.Act_Tick = pRelay->Blk.Item.Act_Time;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Relay_Set( Relay_DriveItem *pRelay )
{
	if( pRelay->Opt.Item.IsBlinkingMode == true ){
		if( pRelay->Set == true ){
			if( pRelay->Blk.Item.IsEnable != true ){
				pRelay->Blk.Item.IsEnable  = true;
				pRelay->Blk.Item.DriveOut  = true;
				pRelay->Blk.Item.Act_Tick  = pRelay->Blk.Item.Act_Time;
				pRelay->Blk.Item.Rel_Tick  = pRelay->Blk.Item.Rel_Time;
			}
		}
		else {
			pRelay->Blk.Item.IsEnable  = false;
			pRelay->Blk.Item.DriveOut  = false;
		}
		
		if( pRelay->Out != pRelay->Blk.Item.DriveOut ){
			pRelay->Out  = pRelay->Blk.Item.DriveOut;
			
			Relay_Gpio_Write( pRelay );
		}
	}
	else {
		if( pRelay->Out != pRelay->Set ){
			pRelay->Out  = pRelay->Set;
			
			Relay_Gpio_Write( pRelay );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Relay_Test_Set( Relay_DriveItem *pRelay )
{
	if( pRelay->Out != pRelay->Set ){
		pRelay->Out  = pRelay->Set;
	}
	
	HAL_GPIO_WritePin( pRelay->Gpio.Port, pRelay->Gpio.Pin, (GPIO_PinState)pRelay->Out );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Relay_Gpio_Write( Relay_DriveItem *pRelay )
{
	bool	Out;
	
	if( pRelay->Opt.Item.IsEnergizeMode == true ){
		Out = pRelay->Out ^ true;
	}
	else {
		Out = pRelay->Out;
	}
	
	HAL_GPIO_WritePin( pRelay->Gpio.Port, pRelay->Gpio.Pin, (GPIO_PinState)Out );
} 

