#define __SELF_TEST_C__
    #include "self_test.h"
#undef  __SELF_TEST_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "iwdg.h"
#include "vself_test.h"
#include "key.h"
#include "normal_mode.h"
#include "password_mode.h"
#include "function_mode.h"
#include "measuring.h"
/* End Include */

/* Start Define */
#define TIMEOUT					(   60 ) // seconds
#define SENS_CHK_TIMEOUT		(  900 ) // seconds
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void WarmUpZero_Update( void );
/* End Function */

/* Start Variable */
static U16		WarmUpSec;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Enter_Self_Test_Mode( void )
{
#ifndef _DEBUG_BUILD
	if( IWDG_Init() == false ){
#endif
		if( (SystemOption.Item.IsBatMode == true) &&
			(MeasHandle.Conf.SenComb == MEASURING_SEN_COMB_CVT_DUAL) &&
			(
				(
					(MeasHandle.pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE] == CAVITY_SEN_TYPE_LIM_272_X127) &&
					(MeasHandle.pGas[0]->Inf.GasNo == IR_GAS_10_990_NF3_200_PPM)
				) ||
				(
					(MeasHandle.pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE] == CAVITY_SEN_TYPE_LIM_272_X054) &&
					(MeasHandle.pGas[0]->Inf.GasNo == IR_GAS_08_000_C4F8_2000_PPM)
				)
			)
		)
		{
			WarmUpSec = 120;
			SelfHandle.IsWarmUpZeroEnabled =  true;
		}
		else {
			WarmUpSec = 900;
			SelfHandle.IsWarmUpZeroEnabled = false;
		}
		
		SelfHandle.State		= SELF_TEST_STATE_PREPARING;
		SelfHandle.Item.Word	= 0xFFFFFFFF;
		SelfHandle.TaskEvt		= true;
		SelfHandle.Timeout		= TIMEOUT;
		SelfHandle.WarmUpSec	= WarmUpSec;
		
		MeasHandle.LpfIndex = 0;
		
		SystemState = SYSTEM_STATE_SELF_TEST;
		
#ifndef _DEBUG_BUILD
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Self_Test_Task( void )
{
	KEY_DataType	Key = KeyScan.Data;
	U16				RawDvMaxSumLmt;
#if 0
	U16				i;
#endif
	
	KeyScan.Data = NoKey;
	
	switch( Key ){
		case Reset:
			if( SelfHandle.WarmUpSec < (WarmUpSec - 10) ){
				SelfHandle.WarmUpSec = 0;
				SelfHandle.TaskEvt = true;
			}
			break;
			
		case Function:
			if( KeyScan.Type == Long ){
				if( SelfHandle.IsWarmUpZeroEnabled == false ){
					Pump_On();
				}
				*MeasHandle.pCur = MeasHandle.Raw;
				Enter_Password_Mode();
				return;
			}
			break;
	}
	
	if( SelfHandle.TaskEvt == true ){
		SelfHandle.TaskEvt = false;
		
		if( SelfHandle.Timeout ){
			SelfHandle.Timeout--;
		}
		else {
			switch( SelfHandle.State ){
				case SELF_TEST_STATE_PREPARING:
					SelfHandle.State = SELF_TEST_STATE_PREPARING_FAILURE;
					break;
					
				case SELF_TEST_STATE_SENSOR_CHECK:
					SelfHandle.State = SELF_TEST_STATE_SENSOR_FAILURE;
					break;
					
				case SELF_TEST_STATE_IR_SRC_CHECK:
					SelfHandle.State = SELF_TEST_STATE_IR_SRC_FAILURE;
					break;
					
				case SELF_TEST_STATE_FLOW_CHECK:
					SelfHandle.State = SELF_TEST_STATE_FLOW_FAILURE;
					break;
			}
		}
		
		switch( SelfHandle.State ){
			case SELF_TEST_STATE_PREPARING:
				if( MeasHandle.LpfIndex ){
					if( SelfHandle.IsWarmUpZeroEnabled == true ){
						SelfHandle.Timeout = 0;
						SelfHandle.State = SELF_TEST_STATE_WARM_UP;
					}
					else {
						SelfHandle.Timeout = SENS_CHK_TIMEOUT;
						SelfHandle.State = SELF_TEST_STATE_SENSOR_CHECK;
					}
#if 0
					for( i=0; i<MEASURING_LPF_BUF_SIZE; i++ ){
						MeasHandle.Lpf[i] = MeasHandle.Raw;
					}
#endif
					GDI_Clear( RGB.BLACK );
				}
				else {
					return;
				}
				break;
				
			case SELF_TEST_STATE_SENSOR_CHECK:
				switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_DET_TYPE] ){
					case 970: RawDvMaxSumLmt =  777.f; break;
					case 972: RawDvMaxSumLmt = 1100.f; break;
					default : RawDvMaxSumLmt =  777.f; break;
				}
				if( MeasHandle.Dev.nDet[0] < 0.0001f ){ SelfHandle.Item.Bit.nDet_0 = false; }
				if( MeasHandle.Dev.nDet[1] < 0.0001f ){ SelfHandle.Item.Bit.nDet_1 = false; }
				if( MeasHandle.Dev.nDet[2] < 0.0001f ){ SelfHandle.Item.Bit.nDet_2 = false; }
				if( MeasHandle.Dev.nDet[3] < 0.0001f ){ SelfHandle.Item.Bit.nDet_3 = false; }
				if( MeasHandle.Dev.nDet[4] < 0.0001f ){ SelfHandle.Item.Bit.nDet_4 = false; }
				if( MeasHandle.Dev.nDet[5] < 0.0001f ){ SelfHandle.Item.Bit.nDet_5 = false; }
				if( MeasHandle.Dev.CvtHiTime[0] < 5 ){ SelfHandle.Item.Bit.CvtHiTime_0 = false; }
				if( MeasHandle.Dev.CvtHiTime[1] < 5 ){ SelfHandle.Item.Bit.CvtHiTime_1 = false; }
				if( MeasHandle.Dev.CvtHiTime[2] < 5 ){ SelfHandle.Item.Bit.CvtHiTime_2 = false; }
				if( MeasHandle.Dev.CvtHiTime[3] < 5 ){ SelfHandle.Item.Bit.CvtHiTime_3 = false; }
				if( MeasHandle.Dev.CvtLoTime[0] < 5 ){ SelfHandle.Item.Bit.CvtLoTime_0 = false; }
				if( MeasHandle.Dev.CvtLoTime[1] < 5 ){ SelfHandle.Item.Bit.CvtLoTime_1 = false; }
				if( MeasHandle.Dev.CvtLoTime[2] < 5 ){ SelfHandle.Item.Bit.CvtLoTime_2 = false; }
				if( MeasHandle.Dev.CvtLoTime[3] < 5 ){ SelfHandle.Item.Bit.CvtLoTime_3 = false; }
				if( MeasHandle.Dev.CvtOcAvrgSum[0] < 500 ){ SelfHandle.Item.Bit.CvtOcAvrg_0 = false; }
				if( MeasHandle.Dev.CvtOcAvrgSum[1] < 500 ){ SelfHandle.Item.Bit.CvtOcAvrg_1 = false; }
				if( MeasHandle.Dev.CvtOcAvrgSum[2] < 500 ){ SelfHandle.Item.Bit.CvtOcAvrg_2 = false; }
				if( MeasHandle.Dev.CvtOcAvrgSum[3] < 500 ){ SelfHandle.Item.Bit.CvtOcAvrg_3 = false; }
				if( MeasHandle.pCur->CtgRawDvSum.LPTIA_LPF[0] < 2000 ){ SelfHandle.Item.Bit.CtgRawDvSum_LPTIA_LPF_0 = false; }
				if( MeasHandle.pCur->CtgRawDvSum.VZERO[0] < 300 ){ SelfHandle.Item.Bit.CtgRawDvSum_VZERO_0 = false; }
				if( MeasHandle.pCur->CtgRawDvSum.VBIAS[0] < 300 ){ SelfHandle.Item.Bit.CtgRawDvSum_VBIAS_0 = false; }
				if( MeasHandle.pCur->CtgRawDvSum.LPTIA_LPF[1] < 2000 ){ SelfHandle.Item.Bit.CtgRawDvSum_LPTIA_LPF_1 = false; }
				if( MeasHandle.pCur->CtgRawDvSum.VZERO[1] < 300 ){ SelfHandle.Item.Bit.CtgRawDvSum_VZERO_1 = false; }
				if( MeasHandle.pCur->CtgRawDvSum.VBIAS[1] < 300 ){ SelfHandle.Item.Bit.CtgRawDvSum_VBIAS_1 = false; }
				if( MeasHandle.Dev.CvtRawDvMaxSum[0] < RawDvMaxSumLmt ){ SelfHandle.Item.Bit.CvtRawDvMaxSum_0 = false; }
				if( MeasHandle.Dev.CvtRawDvMaxSum[1] < RawDvMaxSumLmt ){ SelfHandle.Item.Bit.CvtRawDvMaxSum_1 = false; }
				if( MeasHandle.Dev.CvtRawDvMaxSum[2] < RawDvMaxSumLmt ){ SelfHandle.Item.Bit.CvtRawDvMaxSum_2 = false; }
				if( MeasHandle.Dev.CvtRawDvMaxSum[3] < RawDvMaxSumLmt ){ SelfHandle.Item.Bit.CvtRawDvMaxSum_3 = false; }
				SelfHandle.Item.Bit.Reserved = 0;
				if( SelfHandle.Item.Word == 0 ){
					SelfHandle.Timeout = TIMEOUT;
					if( MeasHandle.pSen->pCvt->IsPresent == true ){
						SelfHandle.State = SELF_TEST_STATE_IR_SRC_CHECK;
					}
					else {
						if( SelfHandle.IsWarmUpZeroEnabled == true ){
							FlowCtrl.pPump->SetDuty = FlowCtrl.Cal.SpanDuty;
						}
						Pump_On();
						SelfHandle.State = SELF_TEST_STATE_FLOW_CHECK;
					}
				}
				break;
				
			case SELF_TEST_STATE_IR_SRC_CHECK:
				if( (MeasHandle.pCur->CvtSrcPwr > 400.f) && (MeasHandle.pCur->CvtSrcPwr < 500.f) ){
					SelfHandle.Timeout = TIMEOUT;
					if( SelfHandle.IsWarmUpZeroEnabled == true ){
						FlowCtrl.pPump->SetDuty = FlowCtrl.Cal.SpanDuty;
					}
					Pump_On();
					SelfHandle.State = SELF_TEST_STATE_FLOW_CHECK;
				}
				break;
				
			case SELF_TEST_STATE_FLOW_CHECK:
				if( SelfHandle.IsWarmUpZeroEnabled == true ){
					if( MeasHandle.pCur->FlowRate > ((float)FlowCtrl.Cal.Targ_mL * 0.9f) ){
						SelfHandle.State = SELF_TEST_STATE_SUCCESS;
					}
				}
				else {
					if( MeasHandle.pCur->FlowRate > ((float)FlowCtrl.Cal.Targ_mL * 0.5f) ){
						SelfHandle.Timeout = 0;
						SelfHandle.State = SELF_TEST_STATE_WARM_UP;
					}
				}
				break;
				
			case SELF_TEST_STATE_WARM_UP:
				switch( SelfHandle.WarmUpSec ){
					case 0:
						if( SelfHandle.IsWarmUpZeroEnabled == true ){
							SelfHandle.Timeout = SENS_CHK_TIMEOUT;
							SelfHandle.State = SELF_TEST_STATE_SENSOR_CHECK;
						}
						else {
							SelfHandle.State = SELF_TEST_STATE_SUCCESS;
						}
						break;
						
					case 1:
						if( SelfHandle.IsWarmUpZeroEnabled == true ){
							WarmUpZero_Update();
						}
						SelfHandle.WarmUpSec--;
						break;
						
					default:
						SelfHandle.WarmUpSec--;
						break;
				}
				break;
				
			case SELF_TEST_STATE_SUCCESS:
				Enter_Normal_Mode();
				return;
		}
		
		vSelf_Test( &SelfHandle );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void WarmUpZero_Update( void )
{
	float			Temp;
	float			bSpanScale;
#if 0
	U16				wBuf[8];
#endif
	U8				i;
	U8				iRef;
	
	MeasHandle.bZero.StableTmr = 0;
	MeasHandle.bZero.NotStable = false;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
	
			if( MeasHandle.Gas[i].Inf.bZeroUse == true ){
		
				if( i < CARTRIDGE_SEN_CH_MAX ){
					Temp = MeasHandle.pCur->CtgSenTemp;
				}
				else {
					Temp = MeasHandle.pCur->CvtSenTemp;
				}
				
				if( MeasHandle.bZero.WarmUpEnd[i] == false ){
					MeasHandle.bZero.WarmUpEnd[i] = true;
					MeasHandle.bZero.WarmUpTmr[i] = 0;
				}
				
				bSpanScale = MeasHandle.Gas[i].Inf.mSpan.f / MeasHandle.Gas[i].Inf.mZero.f;
				MeasHandle.Gas[i].Inf.bZero.f = MeasHandle.pCur->nDet[i];
				MeasHandle.Gas[i].Inf.bSpan.f = MeasHandle.pCur->nDet[i] * bSpanScale;
				MeasHandle.Gas[i].Inf.bZeroTemp.f = Temp;
				MeasHandle.Gas[i].Inf.bSpanTemp.f = Temp;
#if 0
				wBuf[0] = MeasHandle.Gas[i].Inf.bZero.w.l;
				wBuf[1] = MeasHandle.Gas[i].Inf.bZero.w.h;
				wBuf[2] = MeasHandle.Gas[i].Inf.bZeroTemp.w.l;
				wBuf[3] = MeasHandle.Gas[i].Inf.bZeroTemp.w.h;
				wBuf[4] = MeasHandle.Gas[i].Inf.bSpan.w.l;
				wBuf[5] = MeasHandle.Gas[i].Inf.bSpan.w.h;
				wBuf[6] = MeasHandle.Gas[i].Inf.bSpanTemp.w.l;
				wBuf[7] = MeasHandle.Gas[i].Inf.bSpanTemp.w.h;
				SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 8 );
				
				if( MeasHandle.Gas[i].Inf.bZeroCnt < 65535 ){
					MeasHandle.Gas[i].Inf.bZeroCnt++;
					SenEEP_Write_CHn( &MeasHandle.Gas[i].Inf.bZeroCnt, i, SENSOR_EEP_CHn_bZERO_COUNT, 1 );
				}
#endif
				
				if( MeasHandle.Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF ){
					iRef = MeasHandle.Conf.iRefCvt;
					bSpanScale = MeasHandle.Gas[iRef].Inf.mSpan.f / MeasHandle.Gas[iRef].Inf.mZero.f;
					MeasHandle.Gas[iRef].Inf.bZero.f = MeasHandle.pCur->nDet[iRef];
					MeasHandle.Gas[iRef].Inf.bSpan.f = MeasHandle.pCur->nDet[iRef] * bSpanScale;
					MeasHandle.Gas[iRef].Inf.bZeroTemp.f = Temp;
					MeasHandle.Gas[iRef].Inf.bSpanTemp.f = Temp;
#if 0
					wBuf[0] = MeasHandle.Gas[iRef].Inf.bZero.w.l;
					wBuf[1] = MeasHandle.Gas[iRef].Inf.bZero.w.h;
					wBuf[2] = MeasHandle.Gas[iRef].Inf.bZeroTemp.w.l;
					wBuf[3] = MeasHandle.Gas[iRef].Inf.bZeroTemp.w.h;
					wBuf[4] = MeasHandle.Gas[iRef].Inf.bSpan.w.l;
					wBuf[5] = MeasHandle.Gas[iRef].Inf.bSpan.w.h;
					wBuf[6] = MeasHandle.Gas[iRef].Inf.bSpanTemp.w.l;
					wBuf[7] = MeasHandle.Gas[iRef].Inf.bSpanTemp.w.h;
					SenEEP_Write_CHn( wBuf, iRef, SENSOR_EEP_CHn_bZERO_L, 8 );
#endif
				}
			}
		}
	}
}


