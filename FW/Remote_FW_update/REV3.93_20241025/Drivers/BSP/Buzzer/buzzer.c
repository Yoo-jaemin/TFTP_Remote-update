#define __BUZZER_C__
	#include "buzzer.h"
#undef  __BUZZER_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */   
#include "tim12.h"
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
static void Buzzer_Pwm_Start_IT( void );
static void Buzzer_Pwm_Start( void );
static void Buzzer_Pwm_Stop_IT( void );
static void Buzzer_Pwm_Stop( void );
static void Buzzer_Set_Tone( void );
static void Buzzer_Alarm1( void );
static void Buzzer_Alarm2( void );
static void Buzzer_MaxPower( void );
/* End Function */

/* Start Variable */
static const float VolumeToDuty[] = { 0.f, 3.f, 5.f, 8.f, 10.f, 14.f, 20.f, 28.f, 50.f };

static const Buzzer_NoteType	Alarm1[] = { {3200, 100}, {0, 900} };
static const Buzzer_NoteType	Alarm2[] = { {3200, 500}, {0, 500} };
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Buzzer_Init( void )
{
	bool	err = false;
	
	if( SystemOption.Item.BuzPresent == true ){
		
		Buzzer_Pwm_Stop_IT();
		BuzDrive.Volume = EepHandle.Sram[SYSTEM_EEP_BUZZER_VOLUME];
		TIM12_Freq_Set( BUZZER_RATED_FREQ, VolumeToDuty[BuzDrive.Volume] );
		
		err = SystemError.Peripheral.Bit.Buzzer_Timer;
		
		BuzDrive.Melody.AL1 = Buzzer_Alarm1;
		BuzDrive.Melody.AL2 = Buzzer_Alarm2;
		BuzDrive.Melody.Max = Buzzer_MaxPower;
		
		BuzDrive.Status = BUZ_STATE_STOP;
	}
	else {
		BuzDrive.Status = BUZ_STATE_NOT_PRESENT;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Buzzer_Drive( void )
{
	if( BuzDrive.ToneDuration ){
		BuzDrive.ToneDuration--;
	}
	else {
		if( BuzDrive.ToneCnt < BuzDrive.NoteLength ){
			BuzDrive.ToneCnt++;
			
			BuzDrive.pTone++;
			
			Buzzer_Set_Tone();
		}
		else {
			if( BuzDrive.IsRepeatMode == true ){
				
				BuzDrive.pTone = BuzDrive.pNote;
				BuzDrive.ToneCnt = 1;
				
				Buzzer_Set_Tone();
			}
			else {
				Buzzer_Stop();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Buzzer_Volume_Set( U8 Volume )
{
	TIM12_Duty_Set( VolumeToDuty[Volume] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Buzzer_PlayTone( float Freq, U8 Volume, U16 mSec )
{
	TIM12_Freq_Set( Freq, VolumeToDuty[Volume] );
	
	BuzDrive.NoteLength = 0;
	BuzDrive.IsRepeatMode = false;
	
	BuzDrive.ToneDuration = (U32)(Freq / 1000 * mSec);
	
	BuzDrive.Status = BUZ_STATE_TONE;
	
	Buzzer_Pwm_Start_IT();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Buzzer_PlayNote( const Buzzer_NoteType *pNote, U16 NoteLength, bool IsRepeat )
{
	BuzDrive.pNote = (Buzzer_NoteType *)pNote;
	BuzDrive.NoteLength = ( NoteLength / sizeof(Buzzer_NoteType) );
	BuzDrive.IsRepeatMode = IsRepeat;
	
	BuzDrive.pTone = BuzDrive.pNote;
	BuzDrive.ToneCnt = 1;
	
	Buzzer_Set_Tone();
	
	BuzDrive.Status = BUZ_STATE_NOTE;
	
	Buzzer_Pwm_Start_IT();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Buzzer_Stop( void )
{
	switch( BuzDrive.Status ){
		case BUZ_STATE_TONE:
		case BUZ_STATE_NOTE:
			BuzDrive.NoteLength = 0;
			BuzDrive.IsRepeatMode = false;
			BuzDrive.ToneDuration = 0;
			BuzDrive.Status = BUZ_STATE_STOP;
			Buzzer_Pwm_Stop_IT();
			break;
			
		case BUZ_STATE_MAX_POWER:
			BuzDrive.NoteLength = 0;
			BuzDrive.IsRepeatMode = false;
			BuzDrive.ToneDuration = 0;
			BuzDrive.Status = BUZ_STATE_STOP;
			Buzzer_Pwm_Stop();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Pwm_Start_IT( void )
{
	if( HAL_TIM_PWM_Start_IT( &TIM12_Handle, TIM_CHANNEL_2 ) != HAL_OK ){
		SystemError.Peripheral.Bit.Buzzer_Timer = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Pwm_Start( void )
{
	if( HAL_TIM_PWM_Start( &TIM12_Handle, TIM_CHANNEL_2 ) != HAL_OK ){
		SystemError.Peripheral.Bit.Buzzer_Timer = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Pwm_Stop_IT( void )
{
	if( HAL_TIM_PWM_Stop_IT( &TIM12_Handle, TIM_CHANNEL_2 ) != HAL_OK ){
		SystemError.Peripheral.Bit.Buzzer_Timer = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Pwm_Stop( void )
{
	if( HAL_TIM_PWM_Stop( &TIM12_Handle, TIM_CHANNEL_2 ) != HAL_OK ){
		SystemError.Peripheral.Bit.Buzzer_Timer = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Set_Tone( void )
{
	if( BuzDrive.pTone->Freq ){
		TIM12_Freq_Set( BuzDrive.pTone->Freq, VolumeToDuty[BuzDrive.Volume] );
		BuzDrive.ToneDuration = (U32)((float)BuzDrive.pTone->Freq / 1000 * BuzDrive.pTone->mSec); 
	}
	else {
		TIM12_Freq_Set( 1000.f, 0.f );
		BuzDrive.ToneDuration = BuzDrive.pTone->mSec;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Alarm1( void )
{
	Buzzer_PlayNote( Alarm1, sizeof(Alarm1), true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_Alarm2( void )
{
	Buzzer_PlayNote( Alarm2, sizeof(Alarm2), true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Buzzer_MaxPower( void )
{
	BuzDrive.Status = BUZ_STATE_MAX_POWER;
	
	TIM12_Freq_Set( BUZZER_RATED_FREQ, 50.f );
	Buzzer_Pwm_Start();
}


