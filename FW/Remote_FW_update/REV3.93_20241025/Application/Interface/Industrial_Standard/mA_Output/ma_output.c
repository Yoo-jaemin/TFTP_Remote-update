#define __mA_OUTPUT_C__
    #include "ma_output.h"
#undef  __mA_OUTPUT_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "adc.h"
#include "eeprom.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void mA_Output_Enable_Port_Write( mA_GpioType *Gpio );
static void GPIO_Init( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool mA_Output_Init( void )
{
	U8				i;
	float			uA;
	GPIO_PinState	pin;
	
	if( ((SystemOption.Item.IsBatMode == false) && (SystemOption.Item.IsPoeMode == false)) ||
		 (SystemOption.Item.IsUniMode ==  true) )
	{
		
		mA_Handle[0].Inf.Zero = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH1];
		mA_Handle[0].Inf.Span = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_CAL_SPAN_uA_CH1];
		mA_Handle[0].Inf.Offs = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH1];
		mA_Handle[1].Inf.Zero = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH2];
		mA_Handle[1].Inf.Span = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_CAL_SPAN_uA_CH2];
		mA_Handle[1].Inf.Offs = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH2];
		
		GPIO_Init();
		
		mA_Handle[0].Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1];
		mA_Handle[0].Out = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1];
		mA_Handle[1].Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2];
		mA_Handle[1].Out = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2];
		
		for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
			
			pin = GPIO_PIN_RESET;
				
			uA = uA_Calculation( &mA_Handle[i] );
			
			if( uA < 500.f ){
				pin = GPIO_PIN_SET;
			}
			else {
				DAC8571_Write( i, uAmpere_To_Dac( uA ) );
			}
			
			if( mA_Handle[i].Gpio.nEnPinState != pin ){
				mA_Handle[i].Gpio.nEnPinState  = pin;
				mA_Output_Enable_Port_Write( &mA_Handle[i].Gpio );
			}
		}
	}
	
	return SystemError.Peripheral.Bit.mA_Output_DAC_CH1 | SystemError.Peripheral.Bit.mA_Output_DAC_CH2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mA_Output_ReInit_Ch1( void )
{
	mA_Handle[0].Inf.Zero = EepHandle.pDefault[SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH1];
	mA_Handle[0].Inf.Span = EepHandle.pDefault[SYSTEM_EEP_mA_OUTPUT_CAL_SPAN_uA_CH1];
	mA_Handle[0].Inf.Offs = EepHandle.pDefault[SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH1];
	
	mA_Handle[0].Out = mA_Handle[0].Set - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mA_Output_ReInit_Ch2( void )
{
	mA_Handle[1].Inf.Zero = EepHandle.pDefault[SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH2];
	mA_Handle[1].Inf.Span = EepHandle.pDefault[SYSTEM_EEP_mA_OUTPUT_CAL_SPAN_uA_CH2];
	mA_Handle[1].Inf.Offs = EepHandle.pDefault[SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH2];
	
	mA_Handle[1].Out = mA_Handle[1].Set - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mA_Output_Task( void )
{
	U8				i;
	float			uA;
	GPIO_PinState	pin;
	
	for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
		
		pin = GPIO_PIN_RESET;
		
		if( mA_Handle[i].Out != mA_Handle[i].Set ){
			mA_Handle[i].Out  = mA_Handle[i].Set;
			
			uA = uA_Calculation( &mA_Handle[i] );
			
			if( mA_Handle[i].Out < 500.f ){
				pin = GPIO_PIN_SET;
			}
			else {
				DAC8571_Write( i, uAmpere_To_Dac( uA ) );
			}
			
			if( mA_Handle[i].Gpio.nEnPinState != pin ){
				mA_Handle[i].Gpio.nEnPinState  = pin;
				mA_Output_Enable_Port_Write( &mA_Handle[i].Gpio );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float uA_Calculation( mA_HandleType *pmA )
{
	float	out;
	float	uA;
	float	slope;
	float	z;
	float	s;
	
	out = pmA->Out;
			
	if		( out <     0.f ){ out =     0.f; }
	else if	( out > 24000.f ){ out = 24000.f; }
			
	z =  8000.f - pmA->Inf.Zero;
	s = 40000.f - pmA->Inf.Span;
	
	slope = (s - z) / 16000.f;
	uA = (out - 4000.f) * slope + z + pmA->Inf.Offs;
	
	return uA;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mA_Output_Enable_Port_Write( mA_GpioType *Gpio )
{
	HAL_GPIO_WritePin( Gpio->nEnPort, Gpio->nEnPin, Gpio->nEnPinState );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
	
	mA_Handle[0].Gpio.nEnPort		= GPIOF;
	mA_Handle[0].Gpio.nEnPin		= GPIO_PIN_4;
	mA_Handle[0].Gpio.nEnPinState	= GPIO_PIN_RESET;
	
	mA_Handle[1].Gpio.nEnPort		= GPIOG;
	mA_Handle[1].Gpio.nEnPin		= GPIO_PIN_10;
	mA_Handle[1].Gpio.nEnPinState	= GPIO_PIN_RESET;
    
    GPIO_InitStructure.Mode			= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull			= GPIO_NOPULL;
    GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_LOW;
	
    GPIO_InitStructure.Pin = mA_Handle[0].Gpio.nEnPin;
    HAL_GPIO_Init( mA_Handle[0].Gpio.nEnPort, &GPIO_InitStructure );
    
    GPIO_InitStructure.Pin = mA_Handle[1].Gpio.nEnPin;
    HAL_GPIO_Init( mA_Handle[1].Gpio.nEnPort, &GPIO_InitStructure );
	
	HAL_GPIO_WritePin( mA_Handle[0].Gpio.nEnPort, mA_Handle[0].Gpio.nEnPin, mA_Handle[0].Gpio.nEnPinState );
	HAL_GPIO_WritePin( mA_Handle[1].Gpio.nEnPort, mA_Handle[1].Gpio.nEnPin, mA_Handle[1].Gpio.nEnPinState );
}


