#define __PUMP_C__
	#include "pump.h"
#undef  __PUMP_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */   
#include "tim3.h"    
#include "eeprom.h"            
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Set_Freq_Calculation( float Ctrl );
static void Out_Freq_Set( float Freq, float Duty );
static void Out_Duty_Set( float Duty );
/* End Function */

/* Start Variable */  
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Pump_Init( void )
{
	bool	err = false;
	
	Pump_Off();
	
	err |= SystemError.Peripheral.Bit.Pump_Timer;
	
	PumpDrive.SetFreq	= PUMP_CTRL_FREQ_MAX;
	PumpDrive.OutFreq	= PUMP_CTRL_FREQ_MAX;
	PumpDrive.SetDuty	= PUMP_CTRL_DUTY_MIN;
	PumpDrive.OutDuty	= PUMP_CTRL_DUTY_MIN;
	
	Out_Freq_Set( PumpDrive.OutFreq, PumpDrive.OutDuty );
	
	PumpDrive.IsEnabled = err ^ true;
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pump_On( void )
{
	bool	err = false;
	
	if( HAL_TIM_PWM_Start_IT( &TIM3_Handle, TIM_CHANNEL_2 ) != HAL_OK ){ err = true; }
	if( HAL_TIM_PWM_Start   ( &TIM3_Handle, TIM_CHANNEL_3 ) != HAL_OK ){ err = true; }
	
	SystemError.Peripheral.Bit.Pump_Timer = err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pump_Off( void )
{
	bool	err = false;
	
	if( HAL_TIM_PWM_Stop_IT( &TIM3_Handle, TIM_CHANNEL_2 ) != HAL_OK ){ err = true; }
	if( HAL_TIM_PWM_Stop   ( &TIM3_Handle, TIM_CHANNEL_3 ) != HAL_OK ){ err = true; }
	
	SystemError.Peripheral.Bit.Pump_Timer = err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pump_Drive( float Ctrl )
{
	Set_Freq_Calculation( Ctrl );
	
	if( PumpDrive.OutFreq != PumpDrive.SetFreq ){
		PumpDrive.OutFreq  = PumpDrive.SetFreq;
		
		Out_Freq_Set( PumpDrive.OutFreq, PumpDrive.OutDuty );
	}
	else if( PumpDrive.OutDuty != PumpDrive.SetDuty ){
		PumpDrive.OutDuty  = PumpDrive.SetDuty;
		
		Out_Duty_Set( PumpDrive.OutDuty );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Set_Freq_Calculation( float Ctrl )
{
	float	SetFreq;
	float	SetDuty;
	
	if( PumpDrive.IsEnabled == true ){
		
		if( Ctrl < 0.f ){ // Flow > Target
			
			if( PumpDrive.SetDuty == PUMP_CTRL_DUTY_MIN ){
				SetFreq = PumpDrive.SetFreq - Ctrl;
				if		( SetFreq < PUMP_CTRL_FREQ_MIN ){ SetFreq = PUMP_CTRL_FREQ_MIN; }
				else if	( SetFreq > PUMP_CTRL_FREQ_MAX ){ SetFreq = PUMP_CTRL_FREQ_MAX; }
				PumpDrive.SetFreq = SetFreq;
			}
			else {
				SetDuty = PumpDrive.SetDuty + Ctrl;
				if		( SetDuty < PUMP_CTRL_DUTY_MIN ){ SetDuty = PUMP_CTRL_DUTY_MIN; }
				else if	( SetDuty > PUMP_CTRL_DUTY_MAX ){ SetDuty = PUMP_CTRL_DUTY_MAX; }
				PumpDrive.SetDuty = SetDuty;
			}
		}
		else { // Flow <= Target
			
			if( PumpDrive.SetFreq == PUMP_CTRL_FREQ_MIN ){
				SetDuty = PumpDrive.SetDuty + Ctrl;
				if		( SetDuty < PUMP_CTRL_DUTY_MIN ){ SetDuty = PUMP_CTRL_DUTY_MIN; }
				else if	( SetDuty > PUMP_CTRL_DUTY_MAX ){ SetDuty = PUMP_CTRL_DUTY_MAX; }
				PumpDrive.SetDuty = SetDuty;
			}
			else {
				SetFreq = PumpDrive.SetFreq - Ctrl;
				if		( SetFreq < PUMP_CTRL_FREQ_MIN ){ SetFreq = PUMP_CTRL_FREQ_MIN; }
				else if	( SetFreq > PUMP_CTRL_FREQ_MAX ){ SetFreq = PUMP_CTRL_FREQ_MAX; }
				PumpDrive.SetFreq = SetFreq;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Out_Freq_Set( float Freq, float Duty )
{
	TIM3_Freq_Set( Freq, Duty );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Out_Duty_Set( float Duty )
{
	TIM3_Duty_Set( Duty );
}


