#define __FUNC_FLOWSET_C__
    #include "func_flowset.h"
#undef  __FUNC_FLOWSET_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "vfunc_flowset.h"
#include "eeprom.h"
/* End Include */

/* Start Define */
#define PTM_RA_B					( 10000.f ) // ohm
#define PTM_RLSB					( PTM_RA_B / FLOW_SET_PTM_RESOLUTION ) // 10000 / 256 = 39.0625 ohm

#define PTM_INIT_WIPER_OFFS			( 128.f )
#define PTM_INIT_WIPER_GAIN			(   0.f )
#define PTM_RGAIN( wiper )			( PTM_RA_B - PTM_RLSB * wiper ) // 2500 ohm
#define AMP_RFEED					( 100000.f ) // ohm

#define TARGET_GAIN_MIN				( AMP_RFEED / PTM_RA_B + 1 ) // x1
#define TARGET_GAIN_MAX				(  201.f ) // x1

#define ZERO_MARGIN					( 200.f ) // mV
#define SPAN_MARGIN					( 300.f ) // mV

#define TASK_TICK_DFT				( 0x8000 +  50 - 1 ) // 8 *  50 =  400 msec
#define TASK_TICK_DLY				( 0x8000 + 500 - 1 ) // 8 * 500 = 4000 msec
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static float Target_mV_Search( U16 mL );
static void PTM_Offs_Set( U16 Data );
static void PTM_Gain_Set( U16 Data );
static void PTM_Offs_Increase( U16 *pData );
static void PTM_Offs_Decrease( U16 *pData );
static void FlowSet_ModeSet( FlowSet_StatusType Status );
static void KeyProcess( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
static float	TargetZero;
static float	TargetZeroMin;
static float	TargetZeroMax;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_FlowSet( Func_HandleType *pFunc )
{
	KeyProcess( pFunc );
	if( pFunc->ViewEvt == true ){ vFunc_FlowSet( pFunc, &FlowSet ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowSet_Handler( void )
{
	float	Target_Full;
	float	Result_Full;
	
	if( FlowSet.Tick & 0x8000 ){
	                                                                                              
		FlowSet.AmpOut = FlowSet.AmpOut * 0.99f + FlowSet.pFlow->pSen->Raw.Pressure.AmpOut * 0.01f;
		FlowSet.AmpRef = FlowSet.AmpRef * 0.99f + FlowSet.pFlow->pSen->Raw.Pressure.AmpRef * 0.01f;
	
		if( SystemError.Peripheral.Bit.Psen_Potentiometer	== true ){ FlowSet.Result = FLOW_SET_RESULT_PTM_HW_ERROR;		}
		if( SystemError.Peripheral.Bit.Pump_Timer			== true ){ FlowSet.Result = FLOW_SET_RESULT_PUMP_ERROR;			}
		
		if( FlowSet.Result != FLOW_SET_RESULT_SUCCESS ){
			FlowSet_ModeSet( FLOW_SET_STATUS_CAL_END );
		}
		
		if( FlowSet.Tick == 0x8000 ){
			FlowSet.Tick = TASK_TICK_DFT;
		}
		else {
			if( FlowSet.Tick > (TASK_TICK_DLY - 125) ){
				FlowSet.AmpOut = FlowSet.pFlow->pSen->Raw.Pressure.AmpOut;
				FlowSet.AmpRef = FlowSet.pFlow->pSen->Raw.Pressure.AmpRef;
			}
			FlowSet.Tick--;
			return;
		}
	}
	
	switch( FlowSet.Status ){
		case FLOW_SET_STATUS_CAL_GAIN_SET_OFFS_SCAN:
			if( FlowSet.AmpOut > ( PTM_INIT_WIPER_OFFS * FLOW_SET_PTM_OFFS_VLSB + TargetZero) ){
				FlowSet.Cal.Ptm.SysOffs.Word = 0;
			}
			else {
				if( FlowSet.AmpOut > TargetZero ){
					FlowSet.Cal.Ptm.SysOffs.Word -= ( (FlowSet.AmpOut - TargetZero) / FLOW_SET_PTM_OFFS_VLSB );
				}
			}
			PTM_Offs_Set( FlowSet.Cal.Ptm.SysOffs.Word );
			FlowSet_ModeSet( FLOW_SET_STATUS_CAL_GAIN_SET_ZERO_SCAN );
			break;
			
		case FLOW_SET_STATUS_CAL_GAIN_SET_ZERO_SCAN:
			FlowSet_ModeSet( FLOW_SET_STATUS_CAL_GAIN_SET_SPAN_SCAN );
			break;
			
		case FLOW_SET_STATUS_CAL_GAIN_SET_SPAN_SCAN:
			FlowSet_ModeSet( FLOW_SET_STATUS_CAL_OFFS_SCAN );
			break;
			
		case FLOW_SET_STATUS_CAL_OFFS_SCAN:
			if		( FlowSet.AmpOut < TargetZeroMin )	{ FlowSet_ModeSet( FLOW_SET_STATUS_CAL_OFFS_INCREASE ); }
			else if	( FlowSet.AmpOut > TargetZeroMax )	{ FlowSet_ModeSet( FLOW_SET_STATUS_CAL_OFFS_DECREASE ); }
			else										{ FlowSet_ModeSet( FLOW_SET_STATUS_CAL_ZERO_DECISION ); }
			break;
			
		case FLOW_SET_STATUS_CAL_OFFS_INCREASE:
			if( FlowSet.AmpOut < TargetZeroMin ){ PTM_Offs_Increase( &FlowSet.Cal.Ptm.SysOffs.Word ); }
			else { FlowSet_ModeSet( FLOW_SET_STATUS_CAL_ZERO_DECISION ); }
			break;
			
		case FLOW_SET_STATUS_CAL_OFFS_DECREASE:
			if( FlowSet.AmpOut > TargetZeroMax ){ PTM_Offs_Decrease( &FlowSet.Cal.Ptm.SysOffs.Word ); }
			else { FlowSet_ModeSet( FLOW_SET_STATUS_CAL_ZERO_DECISION ); }
			break;
			
		case FLOW_SET_STATUS_CAL_ZERO_DECISION:
			if( fabs( FlowSet.AmpOut - TargetZero ) > ZERO_MARGIN ){
				FlowSet.Result = FLOW_SET_RESULT_EXCEED_ZERO_RANGE;
				FlowSet_ModeSet( FLOW_SET_STATUS_CAL_END );
			}
			else {
				FlowSet_ModeSet( FLOW_SET_STATUS_CAL_SPAN_DECISION );
			}
			break;
			
		case FLOW_SET_STATUS_CAL_SPAN_DECISION:
			Target_Full = Target_mV_Search( FlowSet.Cal.Span_mL );
			Result_Full = FlowSet.AmpOut - FlowSet.Cal.Zero.f;
			if( fabs( Target_Full - Result_Full ) > SPAN_MARGIN ){
				FlowSet.Result = FLOW_SET_RESULT_EXCEED_SPAN_RANGE;
			}
			FlowSet_ModeSet( FLOW_SET_STATUS_CAL_END );
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float FlowSet_Amp_Gain( U16 Wiper )
{
	return 1.f + AMP_RFEED / PTM_RGAIN( Wiper );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




















static float Target_mV_Search( U16 mL )
{
	float	ret = 0;
	
	if( (FlowSet.pFlow->pSen->pCvt->IsPresent == true) &&
		(FlowSet.pFlow->pSen->pCvt->Inf[CAVITY_INF_DET_TYPE] == 972) )
	{
		switch( mL ){
			case  100: ret =   23.85f; break;
			case  150: ret =   36.81f; break;
			case  200: ret =   54.39f; break;
			case  250: ret =   71.34f; break;
			case  300: ret =   94.65f; break;
			case  350: ret =  118.41f; break;
			case  400: ret =  145.74f; break;
			case  450: ret =  170.10f; break;
			case  500: ret =  197.76f; break;
			case  550: ret =  226.29f; break;
			case  600: ret =  257.94f; break;
			case  650: ret =  284.55f; break;
			case  700: ret =  313.47f; break;
			case  750: ret =  345.24f; break;
			case  800: ret =  382.05f; break;
			case  850: ret =  424.77f; break;
			case  900: ret =  469.14f; break;
			case  950: ret =  510.96f; break;
			case 1000: ret =  554.40f; break;
		}
	}
	else {
		switch( mL ){
			case  100: ret =   55.65f; break;
			case  150: ret =   85.89f; break;
			case  200: ret =  126.91f; break;
			case  250: ret =  166.46f; break;
			case  300: ret =  220.85f; break;
			case  350: ret =  276.29f; break;
			case  400: ret =  340.06f; break;
			case  450: ret =  396.90f; break;
			case  500: ret =  461.44f; break;
			case  550: ret =  528.01f; break;
			case  600: ret =  601.86f; break;
			case  650: ret =  663.95f; break;
			case  700: ret =  731.43f; break;
			case  750: ret =  805.56f; break;
			case  800: ret =  891.45f; break;
			case  850: ret =  991.13f; break;
			case  900: ret = 1094.66f; break;
			case  950: ret = 1192.24f; break;
			case 1000: ret = 1293.60f; break;
		}
	}
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void PTM_Offs_Set( U16 Data )
{
	MCP46X1_Wr_Wiper( FLOW_PTM_OFFS_SET_ADDRESS, Data );
}
                                         
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void PTM_Gain_Set( U16 Data )
{
	MCP46X1_Wr_Wiper( FLOW_PTM_GAIN_SET_ADDRESS, Data );
}
                                         
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void PTM_Offs_Increase( U16 *pData )
{
	if( *pData < 256 ){
		*pData += 1;
		
		PTM_Offs_Set( *pData );
	}
	else {
		FlowSet_ModeSet( FLOW_SET_STATUS_CAL_ZERO_DECISION );
	}
}
                                         
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void PTM_Offs_Decrease( U16 *pData )
{
	if( *pData ){
		*pData -= 1;
		
		PTM_Offs_Set( *pData );
	}
	else {
		FlowSet_ModeSet( FLOW_SET_STATUS_CAL_ZERO_DECISION );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FlowSet_ModeSet( FlowSet_StatusType Status )
{
	Func_HandleType		*pFunc = &FuncHandle;
	float				Target_Full;
	float				Target_Gain;
	float				Target_Gain_Resistor;
	static U16			wBuf[10];
	
	switch( Status ){
		case FLOW_SET_STATUS_READY:
			MoveFuncTop();
			break;
			
		case FLOW_SET_STATUS_ITEM_SET:
			FlowSet.pFlow = &FlowCtrl;
			FlowSet.Cal = FlowSet.pFlow->Cal;
			FlowSet.pFlow->pPump->IsEnabled = true;
			if( (FlowSet.pFlow->pSen->pCvt->IsPresent == true) &&
				(FlowSet.pFlow->pSen->pCvt->Inf[CAVITY_INF_DET_TYPE] == 972) )
			{
				TargetZero = 1250.f;
				TargetZeroMin = 1230.f;
				TargetZeroMax = 1270.f;
			}
			else {
				TargetZero = 600.f;
				TargetZeroMin = 580.f;
				TargetZeroMax = 620.f;
			}
			pFunc->Item.Lv3 = 0;
			pFunc->Item.Lv4 = 0;
			pFunc->Level = FUNC_LEVEL_2;
			break;
			
		case FLOW_SET_STATUS_SPAN_SET:
			FlowSet.pFlow->pPump->IsEnabled = false;
			Pump_On();
			FlowSet.Cal = FlowSet.pFlow->Cal;
			FlowSet.pFlow->pPump->SetFreq = FlowSet.Cal.SpanFreq;
			FlowSet.pFlow->pPump->SetDuty = FlowSet.Cal.SpanDuty;
			if( FlowSet.Tick & 0x8000 ){
				FlowSet.Tick = 0;
				PTM_Offs_Set( FlowSet.pFlow->Cal.Ptm.SysOffs.Word );
				PTM_Gain_Set( FlowSet.pFlow->Cal.Ptm.SysGain.Word );
			}
			pFunc->Item.Lv3 = 0;
			pFunc->Item.Lv4 = 0;
			pFunc->IsApproved = false;
			pFunc->Level = FUNC_LEVEL_3;
			break;
			
		case FLOW_SET_STATUS_TARG_SET:
			FlowSet.Cal = FlowSet.pFlow->Cal;
			pFunc->Item.Lv3 = 0;
			pFunc->IsApproved = false;
			pFunc->Level = FUNC_LEVEL_3;
			break;
			
		case FLOW_SET_STATUS_TRB_CHECK_TIME_SET:
			FlowSet.TroubleSec = FlowSet.pFlow->TroubleSec;
			pFunc->Item.Lv3 = 0;
			pFunc->IsApproved = false;
			pFunc->Level = FUNC_LEVEL_3;
			break;
			
		case FLOW_SET_STATUS_CAL_GAIN_SET_OFFS_SCAN:
			Pump_Off();
			FlowSet.Cal.SpanFreq = FlowSet.pFlow->pPump->SetFreq;
			FlowSet.Cal.SpanDuty = FlowSet.pFlow->pPump->SetDuty;
			FlowSet.Cal.Ptm.SysOffs.Word = PTM_INIT_WIPER_OFFS;
			FlowSet.Cal.Ptm.SysGain.Word = PTM_INIT_WIPER_GAIN;
			PTM_Offs_Set( FlowSet.Cal.Ptm.SysOffs.Word );
			PTM_Gain_Set( FlowSet.Cal.Ptm.SysGain.Word );
			FlowSet.Tick = TASK_TICK_DLY;
			FlowSet.Result = FLOW_SET_RESULT_SUCCESS;
			pFunc->Level = FUNC_LEVEL_4;
			break;
			
		case FLOW_SET_STATUS_CAL_GAIN_SET_ZERO_SCAN:
			FlowSet.Tick = TASK_TICK_DLY;
			break;
			
		case FLOW_SET_STATUS_CAL_GAIN_SET_SPAN_SCAN:
			FlowSet.Cal.Zero.f = FlowSet.AmpOut;
			Pump_On();
			FlowSet.pFlow->pPump->SetFreq = FlowSet.Cal.SpanFreq;
			FlowSet.Tick = TASK_TICK_DLY;
			break;
			
		case FLOW_SET_STATUS_CAL_OFFS_SCAN:
			FlowSet.Cal.Span.f = FlowSet.AmpOut;
			Pump_Off();
			Target_Full = Target_mV_Search( FlowSet.Cal.Span_mL );
			Target_Gain = (Target_Full / ((FlowSet.Cal.Span.f - FlowSet.Cal.Zero.f) / FlowSet_Amp_Gain( PTM_INIT_WIPER_GAIN )));
			if		( Target_Gain < TARGET_GAIN_MIN ){ Target_Gain = TARGET_GAIN_MIN; }
			else if	( Target_Gain > TARGET_GAIN_MAX ){ Target_Gain = TARGET_GAIN_MAX; }
			Target_Gain_Resistor = PTM_RA_B - AMP_RFEED / (Target_Gain - 1.f);
			FlowSet.Cal.Ptm.SysGain.Word = (U16)(Target_Gain_Resistor / PTM_RLSB);
			PTM_Gain_Set( FlowSet.Cal.Ptm.SysGain.Word );
			FlowSet.Tick = TASK_TICK_DLY;
			break;
			
		case FLOW_SET_STATUS_CAL_ZERO_DECISION:
			FlowSet.Tick = TASK_TICK_DLY;
			break;
			
		case FLOW_SET_STATUS_CAL_SPAN_DECISION:
			FlowSet.Cal.Zero.f = FlowSet.AmpOut;
			Pump_On();
			FlowSet.pFlow->pPump->SetFreq = FlowSet.Cal.SpanFreq;
			FlowSet.Tick = TASK_TICK_DLY;
			break;
			
		case FLOW_SET_STATUS_CAL_END:
			if( FlowSet.Result == FLOW_SET_RESULT_SUCCESS ){
				FlowSet.Cal.Span.f = FlowSet.AmpOut;
				FlowSet.Cal.Targ_mL = FlowSet.Cal.Span_mL;
				FlowSet.pFlow->Cal = FlowSet.Cal;
				wBuf[0] = FlowSet.pFlow->Cal.Ptm.SysOffs.Word;
				wBuf[1] = FlowSet.pFlow->Cal.Ptm.SysGain.Word;
				wBuf[2] = FlowSet.pFlow->Cal.Span_mL;
				wBuf[3] = (U16)(FlowSet.pFlow->Cal.SpanFreq * 100.f);
				wBuf[4] = (U16)(FlowSet.pFlow->Cal.SpanDuty * 100.f);
				wBuf[5] = FlowSet.pFlow->Cal.Span.w.l;
				wBuf[6] = FlowSet.pFlow->Cal.Span.w.h;
				wBuf[7] = FlowSet.pFlow->Cal.Zero.w.l;
				wBuf[8] = FlowSet.pFlow->Cal.Zero.w.h;
				wBuf[9] = FlowSet.pFlow->Cal.Targ_mL;
				EEPROM_Update( wBuf, SYSTEM_EEP_FLOW_CTRL_PTM_OFFS, 10 );
				PTM_Offs_Update();
				PTM_Gain_Update();
			}
			Pump_On();
			FlowSet.pFlow->pPump->IsEnabled = true;
			break;
	}
	
	FlowSet.Status = Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( Func_HandleType *pFunc )
{
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
			FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );
			pFunc->State.Bit.Flow = true;
			break;
			
		case FUNC_LEVEL_2:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 0: FlowSet_ModeSet( FLOW_SET_STATUS_SPAN_SET ); break;
						case 1: FlowSet_ModeSet( FLOW_SET_STATUS_TARG_SET ); break;
						case 2: FlowSet_ModeSet( FLOW_SET_STATUS_TRB_CHECK_TIME_SET ); break;
					}
					break;
					
				case Up:
					if( pFunc->Item.Lv2 ){
						pFunc->Item.Lv2--;
					}
					break;
					
				case Down:
					if( pFunc->Item.Lv2 < 2 ){
						pFunc->Item.Lv2++;
					}
					break;
					
				case Reset:
					FlowSet_ModeSet( FLOW_SET_STATUS_READY );
					pFunc->State.Bit.Flow = false;
					break;
			}
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv2 ){
				case 0: // Auto Calibration
					switch( pFunc->KeyData ){
						case Function:
							if( pFunc->Item.Lv3 < 3 ){
								pFunc->Item.Lv3++;
							}
							else {
								if( pFunc->IsApproved == true )	{ FlowSet_ModeSet( FLOW_SET_STATUS_CAL_GAIN_SET_OFFS_SCAN );	}
								else							{ FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );					}
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( FlowSet.Cal.Span_mL < FLOW_CTRL_MAX ){
										FlowSet.Cal.Span_mL += 50;
									}
									break;
									
								case 1:
									if( FlowSet.pFlow->pPump->SetFreq != PUMP_CTRL_FREQ_MIN ){
										Func_fSum( PUMP_CTRL_FREQ_MIN, -1.0f, &FlowSet.pFlow->pPump->SetFreq );
									}
									else {
										Func_fSum( PUMP_CTRL_DUTY_MAX,  1.0f, &FlowSet.pFlow->pPump->SetDuty );
									}
									break;
									
								case 2:
									if( FlowSet.pFlow->pPump->SetFreq != PUMP_CTRL_FREQ_MIN ){
										Func_fSum( PUMP_CTRL_FREQ_MIN, -0.1f, &FlowSet.pFlow->pPump->SetFreq );
									}
									else {
										Func_fSum( PUMP_CTRL_DUTY_MAX,  0.1f, &FlowSet.pFlow->pPump->SetDuty );
									}
									break;
									
								case 3:
									pFunc->IsApproved ^= true;
									break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0:
									if( FlowSet.Cal.Span_mL > 100 ){
										FlowSet.Cal.Span_mL -= 50;
									}
									break;
									
								case 1:
									if( FlowSet.pFlow->pPump->SetDuty != PUMP_CTRL_DUTY_MIN ){
										Func_fSum( PUMP_CTRL_DUTY_MIN, -1.0f, &FlowSet.pFlow->pPump->SetDuty );
									}
									else {
										Func_fSum( PUMP_CTRL_FREQ_MAX,  1.0f, &FlowSet.pFlow->pPump->SetFreq );
									}
									break;
									
								case 2:
									if( FlowSet.pFlow->pPump->SetDuty != PUMP_CTRL_DUTY_MIN ){
										Func_fSum( PUMP_CTRL_DUTY_MIN, -0.1f, &FlowSet.pFlow->pPump->SetDuty );
									}
									else {
										Func_fSum( PUMP_CTRL_FREQ_MAX,  0.1f, &FlowSet.pFlow->pPump->SetFreq );
									}
									break;
									
								case 3:
									pFunc->IsApproved ^= true;
									break;
							}
							break;
							
						case Reset:
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							else {
								FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );
							}
							break;
					}
					break;
					
				case 1: // Target Flow Set
					switch( pFunc->KeyData ){
						case Function:
							if( pFunc->Item.Lv3 == 1 ){
								if( pFunc->IsApproved == true ){
									if( FlowSet.pFlow->Cal.Targ_mL != FlowSet.Cal.Targ_mL ){
										FlowSet.pFlow->Cal.Targ_mL  = FlowSet.Cal.Targ_mL;
										EEPROM_Update( &FlowSet.pFlow->Cal.Targ_mL, SYSTEM_EEP_FLOW_CTRL_TARG_mL, 1 );
									}
								}
								FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );
							}
							else {
								pFunc->Item.Lv3++;
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0: if( FlowSet.Cal.Targ_mL < FLOW_CTRL_MAX ){ FlowSet.Cal.Targ_mL += 50; } break;									
								case 1: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0: if( FlowSet.Cal.Targ_mL > FLOW_CTRL_MIN ){ FlowSet.Cal.Targ_mL -= 50; } break;
								case 1: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Reset:
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							else {
								FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );
							}
							break;
					}
					break;
					
				case 2: // Trouble Check Time Set
					switch( pFunc->KeyData ){
						case Function:
							if( pFunc->Item.Lv3 == 1 ){
								if( pFunc->IsApproved == true ){
									if( FlowSet.pFlow->TroubleSec != FlowSet.TroubleSec ){
										FlowSet.pFlow->TroubleSec  = FlowSet.TroubleSec;
										EEPROM_Update( &FlowSet.pFlow->TroubleSec, SYSTEM_EEP_FLOW_TRB_CHECK_SEC, 1 );
									}
								}
								FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );
							}
							else {
								pFunc->Item.Lv3++;
							}
							break;
							
						case Up:
							switch( pFunc->Item.Lv3 ){
								case 0: if( FlowSet.TroubleSec < 60 ){ FlowSet.TroubleSec++; } break;
								case 1: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Down:
							switch( pFunc->Item.Lv3 ){
								case 0: if( FlowSet.TroubleSec >  5 ){ FlowSet.TroubleSec--; } break;
								case 1: pFunc->IsApproved ^= true; break;
							}
							break;
							
						case Reset:
							if( pFunc->Item.Lv3 ){
								pFunc->Item.Lv3--;
							}
							else {
								FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET );
							}
							break;
					}
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->KeyData ){
				case Function:
					if( FlowSet.Status == FLOW_SET_STATUS_CAL_END ){
						if( FlowSet.Result == FLOW_SET_RESULT_SUCCESS )	{ FlowSet_ModeSet( FLOW_SET_STATUS_ITEM_SET ); }
						else											{ FlowSet_ModeSet( FLOW_SET_STATUS_SPAN_SET ); }
					}
					break;
					
				case Reset:
					FlowSet_ModeSet( FLOW_SET_STATUS_SPAN_SET );
					break;
			}
			break;
	}
}


