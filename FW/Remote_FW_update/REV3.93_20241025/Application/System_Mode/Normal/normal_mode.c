#define __NORMAL_MODE_C__
    #include "normal_mode.h"
#undef  __NORMAL_MODE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "allbase_mode.h"
#include "password_mode.h"
#include "function_mode.h"
#include "vmeasuring.h"
#include "vstatusbar.h"
#include "iwdg.h"
#include "micro_sd.h"
#include "rtc.h"
#include "lcd.h"
#include "power_control.h"
#include "buzzer.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Sys_Error_Scan( void );
static void Sys_Error_Decoding( void );
static void Gas_Alarm_Scan( void );
static void IO_Set( void );
static void mA_Set( void );
static void KeyProcess( void );
/* End Function */

/* Start Variable */
static bool	IsFirstEnt = false;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Enter_Normal_Mode( void )
{
	U8		i;
	
	NormHandle.pMeas	= &MeasHandle;
	NormHandle.pPump	= &PumpDrive;
	NormHandle.pOption	= &SystemOption;
	NormHandle.pRly		= &RelayDrive;
	NormHandle.pLed		= &LedDrive;
	
	NormHandle.pPump->IsEnabled = true;
	
	NormHandle.pRly->AL1.Set = false;
	NormHandle.pRly->AL2.Set = false;
	NormHandle.pRly->TRB.Set = false;
	NormHandle.pRly->AL1.Out = true;
	NormHandle.pRly->AL2.Out = true;
	NormHandle.pRly->TRB.Out = true;
	NormHandle.pRly->IsTestMode = false;
	
	NormHandle.pLed->Set.Bit.AL1 = false;
	NormHandle.pLed->Set.Bit.AL2 = false;
	NormHandle.pLed->Set.Bit.TRB = false;
	
	NormHandle.pmA[0] = &mA_Handle[0];
	NormHandle.pmA[1] = &mA_Handle[1];
	if( NormHandle.pMeas->Mode.Item.Maintenance == true ){
		NormHandle.pmA[0]->Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1];
		NormHandle.pmA[1]->Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2];
	}
	else {
		NormHandle.pmA[0]->Set = 4000;
		NormHandle.pmA[1]->Set = 4000;
	}
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		NormHandle.pMeas->Alr[i].Latched = false;
	}
	
	vMeasuring_DrawBase();
	
	NormHandle.pMeas->Test.Item.GasSimulation = false;
	NormHandle.pMeas->pSen->CtgWaveModeSet = false;
	NormHandle.pMeas->pSen->CvtWaveModeSet = false;
	
	NormHandle.TaskEvt = true;
	
	if( (SystemState != SYSTEM_STATE_PASSWORD) && (SystemState != SYSTEM_STATE_ALLBASE) ){
		GDI_Clear( RGB.BLACK );
		LcdDrive.BL_Init();
		LcdDrive.Brightness_Set( LcdDrive.BrightnessLevel );
	}
	
	for( i=0; i<KEY_INDEX_MAX; i++ ){ KeyScan.Tick[i].Long = KEY_SCAN_MSEC( 2000 ); }
	KeyScan.Tick[KEY_INDEX_UP  ].IsRepeatMode = false;
	KeyScan.Tick[KEY_INDEX_DOWN].IsRepeatMode = false;
	
	IWDG_Reload_Counter_Set( 1 );
	
	uSD_Handle.Scan.IsChanged = 0;
	
	vStBar.ComIconSet = 0;
	vStBar.ComIconOut = 1;
	vStBar.SysIconOut = SYSTEM_STATUS_ICON_MAX;
	vStBar.FlowTmr = 1;
	vStBar.InfoTmr = 1;
	vStBar.BatPrv.Item.Level = 101;
	
	iADC_BatteryScan( false );
	SysCalendar.SecondEvt = true;
	SysCalendar.MinuteEvt = true;
	
	if( IsFirstEnt == false ){
		NormHandle.KeyLock = false;
		for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
			if( MeasHandle.Gas[i].IsPresent == true ){
				NormHandle.pMeas->Gas[i].nPerMin = MeasHandle.pCur->nPer[i];
				NormHandle.pMeas->Gas[i].nPerMax = MeasHandle.pCur->nPer[i];
				NormHandle.pMeas->Gas[i].iPerMin = MeasHandle.pCur->iPer[i];
				NormHandle.pMeas->Gas[i].iPerMax = MeasHandle.pCur->iPer[i];
				NormHandle.pMeas->Filter.NotStableTmrMax[i] = 0;
			}
		}
		IsFirstEnt = true;
	}
	
	MeasHandle.Filter.pFlow->TroubleTmr = (MeasHandle.Filter.pFlow->TroubleSec * CavityHandle.Inf[CAVITY_INF_SRC_FREQ]);
	SystemError.Main.Word = 0;
	
	Buzzer_Stop();
	
	SystemState = SYSTEM_STATE_NORMAL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Normal_Mode_Task( void )
{
	if( NormHandle.TaskEvt == true ){
		NormHandle.TaskEvt = false;
		
		Sys_Error_Scan();
		Gas_Alarm_Scan();
		IO_Set();
		vMeasuring();
	}
	
	KeyProcess();
	vStatusBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sys_Error_Scan( void )
{
	SystemError.Peripheral.Bit.HS300x_Cartridge	= NormHandle.pMeas->pSen->pCtg->Error.Bit.HS300x;
	SystemError.Peripheral.Bit.iADC_Cartridge	= NormHandle.pMeas->pSen->pCtg->Error.Bit.iADC;
	SystemError.Peripheral.Bit.Flash_Cartridge	= NormHandle.pMeas->pSen->pCtg->Error.Bit.Flash;
	
	SystemError.Peripheral.Bit.eADC_Cavity		= NormHandle.pMeas->pSen->pCvt->Error.Bit.eADC;
	SystemError.Peripheral.Bit.Eeprom_Cavity	= NormHandle.pMeas->pSen->pCvt->Error.Bit.AT24C32;
	SystemError.Peripheral.Bit.Driver_Cavity	= NormHandle.pMeas->pSen->pCvt->Error.Bit.Driver;
	SystemError.Peripheral.Bit.Ptm_Cavity		= NormHandle.pMeas->pSen->pCvt->Error.Bit.TPL0102;
	SystemError.Peripheral.Bit.Init_Cavity		= NormHandle.pMeas->pSen->pCvt->Error.Bit.Init;
	
	if( (EepHandle.CRC_Error == true) ||
		(NormHandle.pMeas->pSen->pCtg->Error.Bit.Flash_CRC == true) ||
		(NormHandle.pMeas->pSen->pCvt->Error.Bit.Eeprom_CRC == true) )
	{
		SystemError.Main.Bit.Eeprom_CRC = true;
	}
	else {
		SystemError.Main.Bit.Eeprom_CRC = false;
	}
	
	Sys_Error_Decoding();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sys_Error_Decoding( void )
{
	U16		Code[2] = { 0, 0 };
	
	// Common
	if( SystemError.Peripheral.Word )													{ Code[0] = Code[1] =  1; }
	else if( SystemError.Main.Bit.Power == true )										{ Code[0] = Code[1] = 31; }
	else if( SystemError.Main.Bit.Measurement_Parameter == true )						{ Code[0] = Code[1] = 44; }
	else if( SystemError.Main.Bit.Eeprom_CRC == true )									{ Code[0] = Code[1] = 13; }
	else if( SystemError.Main.Bit.FlowRate_Udr == true )								{ Code[0] = Code[1] = 21; }
	else if( SystemError.Main.Bit.FlowRate_Ovr == true )								{ Code[0] = Code[1] = 22; }
	else if( SystemError.Main.Bit.Measurement_Pending == true )							{ Code[0] = Code[1] = 50; }
	else {
		if( NormHandle.pMeas->Conf.OutMax < MEASURING_GAS_OUTPUT_MAX ){ // Single
			if( NormHandle.pMeas->pGas[0]->Inf.Method == MEASURING_METHOD_CT ){
				if( SystemError.Main.Bit.CtgTemp == true )								{ Code[0] = 43; }
				else if( SystemError.Main.Bit.CtgCom == true )							{ Code[0] = 11; }
				else if( SystemError.Main.Bit.CtgDac == true )							{ Code[0] =  8; }
				else if( SystemError.Main.Bit.CT_ExcSignal_CH1 == true )				{ Code[0] = 41; }
				else if( SystemError.Main.Bit.CT_NotStable_CH1 == true )				{ Code[0] = 47; }
				else if( SystemError.Main.Bit.CT_ExcCurrent_CH1 == true )				{ Code[0] = 48; }
				else if( SystemError.Main.Bit.CT_Gas_Deviation == true ){
					if( MeasHandle.Filter.CtgGasDevTmr.Item.IsCh1Higher == true )		{ Code[0] = 51; }
					else																{ Code[0] = 52; }
				}
			}
			else {
				if( SystemError.Main.Bit.CvtTemp == true )								{ Code[0] = 43; }
				else if( SystemError.Main.Bit.CvtCom == true )							{ Code[0] = 11; }
				else if( SystemError.Main.Bit.IR_SourcePower == true )					{ Code[0] = 40; }
				else if( SystemError.Main.Bit.IR_OffsNotStable == true )				{ Code[0] = 45; }
				else if( (SystemError.Main.Bit.IR_ExcSignal_CH1 == true) ||
						 (SystemError.Main.Bit.IR_ExcSignal_REF == true) )				{ Code[0] = 41; }
				else if( (SystemError.Main.Bit.IR_OffsDrift_CH1 == true) ||
						 (SystemError.Main.Bit.IR_OffsDrift_REF == true) )				{ Code[0] = 42; }
				else if( (SystemError.Main.Bit.IR_NotStable_CH1 == true) ||
						 (SystemError.Main.Bit.IR_NotStable_REF == true) )				{ Code[0] = 47; }
				else if( (SystemError.Main.Bit.IR_OptPathContaminated_CH1 == true) ||
						 (SystemError.Main.Bit.IR_OptPathContaminated_REF == true) )	{ Code[0] =  7; }
			}
		}
		else { // Dual
			if( NormHandle.pMeas->pGas[0]->Inf.Method == MEASURING_METHOD_CT ){
				if( SystemError.Main.Bit.CtgTemp == true )								{ Code[0] = 43; }
				else if( SystemError.Main.Bit.CtgCom == true )							{ Code[0] = 11; }
				else if( SystemError.Main.Bit.CtgDac == true )							{ Code[0] =  8; }
				else if( SystemError.Main.Bit.CT_ExcSignal_CH1 == true )				{ Code[0] = 41; }
				else if( SystemError.Main.Bit.CT_NotStable_CH1 == true )				{ Code[0] = 47; }
				else if( SystemError.Main.Bit.CT_ExcCurrent_CH1 == true )				{ Code[0] = 48; }
				else if( SystemError.Main.Bit.CT_Gas_Deviation == true ){
					if( MeasHandle.Filter.CtgGasDevTmr.Item.IsCh1Higher == true )		{ Code[0] = 51; }
					else																{ Code[0] = 52; }
				}
			}
			else {
				if( SystemError.Main.Bit.CvtTemp == true )								{ Code[0] = 43; }
				else if( SystemError.Main.Bit.CvtCom == true )							{ Code[0] = 11; }
				else if( SystemError.Main.Bit.IR_SourcePower == true )					{ Code[0] = 40; }
				else if( SystemError.Main.Bit.IR_OffsNotStable == true )				{ Code[0] = 45; }
				else if( (SystemError.Main.Bit.IR_ExcSignal_CH1 == true) ||
						 (SystemError.Main.Bit.IR_ExcSignal_REF == true) )				{ Code[0] = 41; }
				else if( (SystemError.Main.Bit.IR_OffsDrift_CH1 == true) ||
						 (SystemError.Main.Bit.IR_OffsDrift_REF == true) )				{ Code[0] = 42; }
				else if( (SystemError.Main.Bit.IR_NotStable_CH1 == true) ||
						 (SystemError.Main.Bit.IR_NotStable_REF == true) )				{ Code[0] = 47; }
				else if( (SystemError.Main.Bit.IR_OptPathContaminated_CH1 == true) ||
						 (SystemError.Main.Bit.IR_OptPathContaminated_REF == true) )	{ Code[0] =  7; }
			}
			if( NormHandle.pMeas->pGas[1]->Inf.Method == MEASURING_METHOD_CT ){
				if( SystemError.Main.Bit.CtgTemp == true )								{ Code[1] = 43; }
				else if( SystemError.Main.Bit.CtgCom == true )							{ Code[1] = 11; }
				else if( SystemError.Main.Bit.CtgDac == true )							{ Code[1] =  8; }
				else if( SystemError.Main.Bit.CT_ExcSignal_CH2 == true )				{ Code[1] = 41; }
				else if( SystemError.Main.Bit.CT_NotStable_CH2 == true )				{ Code[1] = 47; }
				else if( SystemError.Main.Bit.CT_ExcCurrent_CH2 == true )				{ Code[1] = 48; }
			}
			else { // IR Single
				if( SystemError.Main.Bit.CvtTemp == true )								{ Code[1] = 43; }
				else if( SystemError.Main.Bit.CvtCom == true )							{ Code[1] = 11; }
				else if( SystemError.Main.Bit.IR_SourcePower == true )					{ Code[1] = 40; }
				else if( SystemError.Main.Bit.IR_OffsNotStable == true )				{ Code[1] = 45; }
				else if( (SystemError.Main.Bit.IR_ExcSignal_CH2 == true) ||
						 (SystemError.Main.Bit.IR_ExcSignal_REF == true) )				{ Code[1] = 41; }
				else if( (SystemError.Main.Bit.IR_OffsDrift_CH2 == true) ||
						 (SystemError.Main.Bit.IR_OffsDrift_REF == true) )				{ Code[1] = 42; }
				else if( (SystemError.Main.Bit.IR_NotStable_CH2 == true) ||
						 (SystemError.Main.Bit.IR_NotStable_REF == true) )				{ Code[1] = 47; }
				else if( (SystemError.Main.Bit.IR_OptPathContaminated_CH2 == true) ||
						 (SystemError.Main.Bit.IR_OptPathContaminated_REF == true) )	{ Code[1] =  7; }
			}
		}
	}
	
	// No error except for under gas
	if( (Code[0] == 0) && (SystemError.Code[0] == 30) ){ Code[0] = 30; }
	if( (Code[1] == 0) && (SystemError.Code[1] == 30) ){ Code[1] = 30; }
	
	SystemError.Code[0] = Code[0];
	SystemError.Code[1] = Code[1];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Gas_Alarm_Scan( void )
{
	U16						hiscale;
	float					gas;
	float					ovr;
	float					udr;
	U8						i;
	bool					_1st;
	bool					_2nd;
	S16						_1st_dead;
	S16						_2nd_dead;
	
	for( i=0; i<NormHandle.pMeas->Conf.OutMax; i++ ){
		
		if( (SystemError.Code[i] == 0) || (SystemError.Code[i] == 30) ){
			// Decision Parameter Load
			hiscale = NormHandle.pMeas->pGas[i]->Inf.HighScale;
			gas	= (float)NormHandle.pMeas->pGas[i]->Res;
			ovr = hiscale *  1.1f;
			udr = hiscale * -0.1f;
			_1st_dead = (S16)(hiscale / 100 * NormHandle.pMeas->pAlr[i]->Inf._1stDead);
			_2nd_dead = (S16)(hiscale / 100 * NormHandle.pMeas->pAlr[i]->Inf._2ndDead);
			
			// 1'st Alarm Check
			if( NormHandle.pMeas->pAlr[i]->Inf._1stTypeIncrease == true ){
				if( NormHandle.pMeas->pAlr[i]->Item.Bit.a1st != true ){
					if( gas >= NormHandle.pMeas->pAlr[i]->Inf._1stLevel ){ _1st = true; }
					else { _1st = false; }
				}
				else {
					if( (gas < (NormHandle.pMeas->pAlr[i]->Inf._1stLevel - _1st_dead)) || (gas == 0) ){ _1st = false; }
					else { _1st = NormHandle.pMeas->pAlr[i]->Item.Bit.a1st; }
				}
			}
			else {
				if( NormHandle.pMeas->pAlr[i]->Item.Bit.a1st != true ){
					if( gas <= NormHandle.pMeas->pAlr[i]->Inf._1stLevel ){ _1st = true; }
					else { _1st = false; }
				}
				else {
					if( (gas > (NormHandle.pMeas->pAlr[i]->Inf._1stLevel + _1st_dead)) || (gas == hiscale) ){ _1st = false; }
					else { _1st = NormHandle.pMeas->pAlr[i]->Item.Bit.a1st; }
				}
			}
			
			// 2'nd Alarm Check
			if( NormHandle.pMeas->pAlr[i]->Inf._2ndTypeIncrease == true ){
				if( NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd != true ){
					if( gas >= NormHandle.pMeas->pAlr[i]->Inf._2ndLevel ){ _2nd = true; }
					else { _2nd = false; }
				}
				else {
					if( (gas < (NormHandle.pMeas->pAlr[i]->Inf._2ndLevel - _2nd_dead)) || (gas == 0) ){ _2nd = false; }
					else { _2nd = NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd; }
				}
			}
			else {
				if( NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd != true ){
					if( gas <= NormHandle.pMeas->pAlr[i]->Inf._2ndLevel ){ _2nd = true; }
					else { _2nd = false; }
				}
				else {
					if( (gas > (NormHandle.pMeas->pAlr[i]->Inf._2ndLevel + _2nd_dead)) || (gas == hiscale) ){ _2nd = false; }
					else { _2nd = NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd; }
				}
			}
			
			// Over & Under Alarm Check
			if( gas >= ovr ){
				NormHandle.pMeas->pAlr[i]->Item.Bit.aOvr = true;
				NormHandle.pMeas->pAlr[i]->Item.Bit.tUdr = false;
			}
			else if( gas <= udr ){
				NormHandle.pMeas->pAlr[i]->Item.Bit.tUdr = NormHandle.pMeas->pAlr[i]->Inf.IsUnderCheckEnabled;
				NormHandle.pMeas->pAlr[i]->Item.Bit.aOvr = false;
			}
			else {
				NormHandle.pMeas->pAlr[i]->Item.Bit.aOvr = false;
				NormHandle.pMeas->pAlr[i]->Item.Bit.tUdr = false;
			}
			
			// If the sensor is problem, gas alarm don't update.
			if( NormHandle.pMeas->pAlr[i]->Item.Bit.tUdr ){
				SystemError.Code[i] = 30;
				if( NormHandle.pMeas->pAlr[i]->Latched == false ){
					NormHandle.pMeas->pAlr[i]->Item.Bit.a1st = false;
					NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd = false;
				}
			}
			else {
				SystemError.Code[i] = 0;
				// 1'st Alarm Update
				if( _1st == true ){
					if( NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Pre != true ){
						NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Pre  = true;
						NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Dly  = NormHandle.pMeas->pAlr[i]->Inf._1stDelaySec;
					}
					else {
						if( NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Dly ){
							NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Dly--;
						}
					}
					if( (NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Pre == true) &&
						(NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Dly == 0) )
					{
						if( NormHandle.pMeas->pAlr[i]->Item.Bit.a1st != true ){
							NormHandle.pMeas->pAlr[i]->Item.Bit.a1st  = true;
							if( NormHandle.pMeas->pAlr[i]->Inf._1stLatchEnabled == true ){
								NormHandle.pMeas->pAlr[i]->Latched = true;
							}
						}
					}
				}
				else {
					NormHandle.pMeas->pAlr[i]->Inf.Det._1st_Pre = false;
					if( NormHandle.pMeas->pAlr[i]->Latched == false ){
						NormHandle.pMeas->pAlr[i]->Item.Bit.a1st = false;
					}
				}
				// 2'nd Alarm Update
				if( _2nd == true ){
					if( NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Pre != true ){
						NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Pre  = true;
						NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Dly  = NormHandle.pMeas->pAlr[i]->Inf._2ndDelaySec;
					}
					else {
						if( NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Dly ){
							NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Dly--;
						}
					}
					if( (NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Pre == true) &&
						(NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Dly == 0) )
					{
						if( NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd != true ){
							NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd  = true;
							if( NormHandle.pMeas->pAlr[i]->Inf._2ndLatchEnabled == true ){
								NormHandle.pMeas->pAlr[i]->Latched = true;
							}
						}
					}
				}
				else {
					NormHandle.pMeas->pAlr[i]->Inf.Det._2nd_Pre = false;
					if( NormHandle.pMeas->pAlr[i]->Latched == false ){
						NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd = false;
					}
				}
			}
		}
		else {
			if( NormHandle.pMeas->pAlr[i]->Latched == false ){
				NormHandle.pMeas->pAlr[i]->Item.Bit.a1st = false;
				NormHandle.pMeas->pAlr[i]->Item.Bit.a2nd = false;
				NormHandle.pMeas->pAlr[i]->Item.Bit.aOvr = false;
			}
			NormHandle.pMeas->pAlr[i]->Item.Bit.tUdr = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IO_Set( void )
{
	bool	TRB = false;
	bool	AL1 = false;
	bool	AL2 = false;
	
	switch( NormHandle.pMeas->Conf.OutMax ){
		case 1: // One Channel Mode
			if( SystemError.Code[0] ){
				TRB = true;
				if( NormHandle.pMeas->pAlr[0]->Latched ){
					AL1 = NormHandle.pMeas->pAlr[0]->Item.Bit.a1st;
					AL2 = NormHandle.pMeas->pAlr[0]->Item.Bit.a2nd;
				}
			}
			else {
				AL1 = NormHandle.pMeas->pAlr[0]->Item.Bit.a1st;
				AL2 = NormHandle.pMeas->pAlr[0]->Item.Bit.a2nd;
			}
			break;
			
		case 2: // Two Channel Mode
			if( SystemError.Code[0] ){
				TRB = true;
				AL1 = NormHandle.pMeas->pAlr[0]->Latched;
			}
			else {
				AL1 = NormHandle.pMeas->pAlr[0]->Item.Bit.a1st | NormHandle.pMeas->pAlr[0]->Item.Bit.a2nd;
			}
			if( SystemError.Code[1] ){
				TRB = true;
				AL2 = NormHandle.pMeas->pAlr[1]->Latched;
			}
			else {
				AL2 = NormHandle.pMeas->pAlr[1]->Item.Bit.a1st | NormHandle.pMeas->pAlr[1]->Item.Bit.a2nd;
			}
			break;
	}
	
	if( SystemOption.Item.BuzPresent == true ){
		if( (AL1 != NormHandle.pLed->Set.Bit.AL1) || (AL2 != NormHandle.pLed->Set.Bit.AL2) ){
			if		( AL2 == true )	{ BuzDrive.Melody.AL2(); }
			else if	( AL1 == true )	{ BuzDrive.Melody.AL1(); }
			else					{ Buzzer_Stop(); }
		}
	}
	
	NormHandle.pLed->Set.Bit.TRB = TRB;
	NormHandle.pLed->Set.Bit.AL1 = AL1;
	NormHandle.pLed->Set.Bit.AL2 = AL2;
	
	if( NormHandle.pMeas->Mode.Item.Maintenance == true ){
		NormHandle.pRly->TRB.Set = false;
		NormHandle.pRly->AL1.Set = false;
		NormHandle.pRly->AL2.Set = false;
	}
	else if( NormHandle.pMeas->Test.Item.WebTest_CH1 | NormHandle.pMeas->Test.Item.WebTest_CH2 ){
		NormHandle.pRly->TRB.Set = TRB & NormHandle.pRly->UseWebTest;
		NormHandle.pRly->AL1.Set = AL1 & NormHandle.pRly->UseWebTest;
		NormHandle.pRly->AL2.Set = AL2 & NormHandle.pRly->UseWebTest;
	}
	else {
		NormHandle.pRly->TRB.Set = TRB & NormHandle.pRly->TRB.Opt.Item.IsOutputEnable;
		NormHandle.pRly->AL1.Set = AL1 & NormHandle.pRly->AL1.Opt.Item.IsOutputEnable;
		NormHandle.pRly->AL2.Set = AL2 & NormHandle.pRly->AL2.Opt.Item.IsOutputEnable;
	}
	
	if( (SystemOption.Item.IsPoeMode == false) || (SystemOption.Item.IsUniMode == true) ){ mA_Set(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mA_Set( void )
{
	float				Gas;
	float				HighScale;
	U8					i;
	
	for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
		
		if( (i == 1) && (NormHandle.pMeas->Conf.OutMax < 2 ) ){
			NormHandle.pmA[i]->Set = 0.f;
		}
		else {
			if( NormHandle.pMeas->Mode.Item.Maintenance == true ){
				NormHandle.pmA[0]->Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1];
				NormHandle.pmA[1]->Set = EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2];
			}
			else {
				Gas = NormHandle.pMeas->pGas[i]->Out;
				HighScale = NormHandle.pMeas->pGas[i]->Inf.HighScale;
				
				switch( SystemError.Code[i] ){
					case  0:
						if( NormHandle.pMeas->pAlr[i]->Item.Bit.aOvr ){
							NormHandle.pmA[i]->Set = 22000.f;
						}
						else {
							NormHandle.pmA[i]->Set = 16000.f * (Gas / HighScale) + 4000.f;
						}
						break;
						
					case  1:
					case 11:
					case 13:
					case 31:
					case 44: NormHandle.pmA[i]->Set = 2500.f; break;
					case  7:
					case  8:
					case 30:
					case 40:
					case 41:
					case 42:
					case 43:
					case 45:
					case 47:
					case 50: NormHandle.pmA[i]->Set = 2000.f; break;
					case 21:
					case 22: NormHandle.pmA[i]->Set = 1500.f; break;
					default: NormHandle.pmA[i]->Set =    0.f; break;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( void )
{
	KEY_DataType	Key = KeyScan.Data;
	
	KeyScan.Data  = NoKey;
	
	if( (Key == Reset) && (KeyScan.Type == Long) ){
		NormHandle.KeyLock ^= true;
	}
	else {
		if( NormHandle.KeyLock == false ){
			switch( Key ){
				case Function:
					if( KeyScan.Type == Long ){
						Enter_Password_Mode();
					}
					break;
					
				case Up:
					if( KeyScan.Type == Long ){
						Enter_AllBase_Mode();
					}
					break;
					
				case Down:
					if( KeyScan.Type == Long ){
						if( SystemOption.Item.IsBatMode == false ){
							NormHandle.pMeas->Mode.Item.Maintenance ^= true;
							Measuring_Maintenance_Quit_Time_Refresh();
							EEPROM_Update( &NormHandle.pMeas->Mode.Word, SYSTEM_EEP_MEASURING_MODE, 1 );
						}
					}
					break;
					
				case Reset:
					if( NormHandle.pMeas->pAlr[0]->Latched | NormHandle.pMeas->pAlr[1]->Latched ){
						NormHandle.pMeas->pAlr[0]->Latched = false;
						NormHandle.pMeas->pAlr[1]->Latched = false;
					}
					Buzzer_Stop();
					break;
					
				case Reset_Up:
					if( KeyScan.Type == Long ){
						NormHandle.pMeas->Mode.Item.Engineering ^= true;
						EEPROM_Update( &NormHandle.pMeas->Mode.Word, SYSTEM_EEP_MEASURING_MODE, 1 );
					}
					break;
			}
		}
	}
}


