#define __ALLBASE_MODE_C__
    #include "allbase_mode.h"
#undef  __ALLBASE_MODE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vallbase_mode.h"
#include "key.h"
#include "normal_mode.h"
/* End Include */

/* Start Define */
#define MODE_QUIT_TIME			( 45000 ) // 40 msec * 45000 = 1,800,000 msec
#define MODE_VIEW_TIME			(     5 ) // 40 msec *     5 =       200 msec
#define CAL_PROGRESS_TIME		(    50 ) // 40 msec *    50 =     2,000 msec
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void AllBase_Update( void );
static void KeyProcess( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Enter_AllBase_Mode( void )
{
	AllBaseHandle.pMeas					= &MeasHandle;
	
	AllBaseHandle.pMsgBox				= &vMsgBox;
	AllBaseHandle.pMsgBox->ClearTitle	= true;
	AllBaseHandle.pMsgBox->ClearMsg		= true;
	
	AllBaseHandle.State					= ALLBASE_STATE_READY;
	AllBaseHandle.QuitTmr				= MODE_QUIT_TIME;
	AllBaseHandle.ViewTmr				= MODE_VIEW_TIME;
	
	AllBaseHandle.IsApproved			= false;
	
	vAllBase_Mode( &AllBaseHandle );
	
	SystemState = SYSTEM_STATE_ALLBASE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllBase_Mode_Task( void )
{
	KeyProcess();
	
	if( AllBaseHandle.TaskEvt == true ){
		AllBaseHandle.TaskEvt = false;
		
		if( AllBaseHandle.QuitTmr ){
			AllBaseHandle.QuitTmr--;
			if( AllBaseHandle.QuitTmr == 0 ){
				vAllBase_Clr();
				Enter_Normal_Mode();
				return;
			}
		}
		
		if( AllBaseHandle.ProgressTmr ){
			AllBaseHandle.ProgressTmr--;
			if( AllBaseHandle.ProgressTmr == 0 ){
				if( AllBaseHandle.State == ALLBASE_STATE_PROGRESS ){
					AllBaseHandle.State  = ALLBASE_STATE_COMPLETE;
					AllBase_Update();
				}
			}
		}
		
		if( AllBaseHandle.ViewTmr ){
			AllBaseHandle.ViewTmr--;
			if( AllBaseHandle.ViewTmr == 0 ){
				AllBaseHandle.ViewTmr = MODE_VIEW_TIME;
				AllBaseHandle.ViewEvt = true;
			}
		}
	}
	
	if( AllBaseHandle.ViewEvt == true ){
		AllBaseHandle.ViewEvt = false;
		vAllBase_Mode( &AllBaseHandle );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void AllBase_Update( void )
{
	float			Temp;
	float			bSpanScale;
	U16				wBuf[8];
	U8				i;
	U8				iRef;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
	
			if( AllBaseHandle.pMeas->Gas[i].Inf.bZeroUse == true ){
		
				if( i < CARTRIDGE_SEN_CH_MAX ){
					Temp = AllBaseHandle.pMeas->pCur->CtgSenTemp;
				}
				else {
					Temp = AllBaseHandle.pMeas->pCur->CvtSenTemp;
				}
				
				if( AllBaseHandle.pMeas->bZero.WarmUpEnd[i] == false ){
					AllBaseHandle.pMeas->bZero.WarmUpEnd[i] = true;
					AllBaseHandle.pMeas->bZero.WarmUpTmr[i] = 0;
				}
				
				bSpanScale = AllBaseHandle.pMeas->Gas[i].Inf.mSpan.f / AllBaseHandle.pMeas->Gas[i].Inf.mZero.f;
				AllBaseHandle.pMeas->Gas[i].Inf.bZero.f = AllBaseHandle.pMeas->pCur->nDet[i];
				AllBaseHandle.pMeas->Gas[i].Inf.bSpan.f = AllBaseHandle.pMeas->pCur->nDet[i] * bSpanScale;
				AllBaseHandle.pMeas->Gas[i].Inf.bZeroTemp.f = Temp;
				AllBaseHandle.pMeas->Gas[i].Inf.bSpanTemp.f = Temp;
				wBuf[0] = AllBaseHandle.pMeas->Gas[i].Inf.bZero.w.l;
				wBuf[1] = AllBaseHandle.pMeas->Gas[i].Inf.bZero.w.h;
				wBuf[2] = AllBaseHandle.pMeas->Gas[i].Inf.bZeroTemp.w.l;
				wBuf[3] = AllBaseHandle.pMeas->Gas[i].Inf.bZeroTemp.w.h;
				wBuf[4] = AllBaseHandle.pMeas->Gas[i].Inf.bSpan.w.l;
				wBuf[5] = AllBaseHandle.pMeas->Gas[i].Inf.bSpan.w.h;
				wBuf[6] = AllBaseHandle.pMeas->Gas[i].Inf.bSpanTemp.w.l;
				wBuf[7] = AllBaseHandle.pMeas->Gas[i].Inf.bSpanTemp.w.h;
				SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 8 );
				
				if( AllBaseHandle.pMeas->Gas[i].Inf.bZeroCnt < 65535 ){
					AllBaseHandle.pMeas->Gas[i].Inf.bZeroCnt++;
					SenEEP_Write_CHn( &AllBaseHandle.pMeas->Gas[i].Inf.bZeroCnt, i, SENSOR_EEP_CHn_bZERO_COUNT, 1 );
				}
				
				if( AllBaseHandle.pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF ){
					iRef = AllBaseHandle.pMeas->Conf.iRefCvt;
					bSpanScale = AllBaseHandle.pMeas->Gas[iRef].Inf.mSpan.f / AllBaseHandle.pMeas->Gas[iRef].Inf.mZero.f;
					AllBaseHandle.pMeas->Gas[iRef].Inf.bZero.f = AllBaseHandle.pMeas->pCur->nDet[iRef];
					AllBaseHandle.pMeas->Gas[iRef].Inf.bSpan.f = AllBaseHandle.pMeas->pCur->nDet[iRef] * bSpanScale;
					AllBaseHandle.pMeas->Gas[iRef].Inf.bZeroTemp.f = Temp;
					AllBaseHandle.pMeas->Gas[iRef].Inf.bSpanTemp.f = Temp;
					wBuf[0] = AllBaseHandle.pMeas->Gas[iRef].Inf.bZero.w.l;
					wBuf[1] = AllBaseHandle.pMeas->Gas[iRef].Inf.bZero.w.h;
					wBuf[2] = AllBaseHandle.pMeas->Gas[iRef].Inf.bZeroTemp.w.l;
					wBuf[3] = AllBaseHandle.pMeas->Gas[iRef].Inf.bZeroTemp.w.h;
					wBuf[4] = AllBaseHandle.pMeas->Gas[iRef].Inf.bSpan.w.l;
					wBuf[5] = AllBaseHandle.pMeas->Gas[iRef].Inf.bSpan.w.h;
					wBuf[6] = AllBaseHandle.pMeas->Gas[iRef].Inf.bSpanTemp.w.l;
					wBuf[7] = AllBaseHandle.pMeas->Gas[iRef].Inf.bSpanTemp.w.h;
					SenEEP_Write_CHn( wBuf, iRef, SENSOR_EEP_CHn_bZERO_L, 8 );
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( void )
{
	KEY_DataType	Key = KeyScan.Data;
	U16				i;
	
	KeyScan.Data = NoKey;
	
	if( Key != NoKey ){
		
		AllBaseHandle.ViewEvt = true;
		AllBaseHandle.QuitTmr = MODE_QUIT_TIME;
		
		switch( Key ){
			case Function:
				switch( AllBaseHandle.State ){
					case ALLBASE_STATE_READY:
						if( AllBaseHandle.IsApproved == true ){
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( MeasHandle.Gas[i].IsPresent == true ){
									if( AllBaseHandle.pMeas->Gas[i].Inf.bZeroUse == true ){
										AllBaseHandle.ProgressTmr = CAL_PROGRESS_TIME;
										AllBaseHandle.State = ALLBASE_STATE_PROGRESS;
									}
								}
							}
						}
						else {
							AllBaseHandle.TaskEvt = false;
							AllBaseHandle.ViewEvt = false;
							vAllBase_Clr();
							Enter_Normal_Mode();
						}
						break;
						
					case ALLBASE_STATE_COMPLETE:
						AllBaseHandle.State = ALLBASE_STATE_READY;
						break;
				}
				break;
				
			case Up:
			case Down:
				switch( AllBaseHandle.State ){
					case ALLBASE_STATE_READY:
						AllBaseHandle.IsApproved ^= true;
						break;
				}
				break;
				
			case Reset:
				if( AllBaseHandle.State != ALLBASE_STATE_PROGRESS ){
					AllBaseHandle.TaskEvt = false;
					AllBaseHandle.ViewEvt = false;
					vAllBase_Clr();
					Enter_Normal_Mode();
				}
				break;
		}
	}
}


