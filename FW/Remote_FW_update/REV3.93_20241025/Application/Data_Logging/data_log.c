#define __DATA_LOG_C__
    #include "data_log.h"
#undef  __DATA_LOG_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "eeprom.h"
#include "pump.h"
#include "vmeasuring.h"
#include "power_control.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */   
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Logger_Config( void )
{
	U8		i;
	
	LogHandle.Mode			= DATA_LOG_MODE_DEBUG_PRINT;
	LogHandle.Period		= EepHandle.Sram[SYSTEM_EEP_LOG_INTERVAL_100MSEC];
	LogHandle.Period_Cnt	= 3;
	LogHandle.pCalendar		= &SysCalendar;
	
	dprintf( "\n,Psen,Flow,Duty,VSUP," );
	if( SystemOption.Item.IsBatMode == true ){ dprintf( "BatTemp,BatPer,VBUS,VBAT,IBUS,IBAT," ); }
	if( MeasHandle.pSen->pCtg->IsPresent == true ){
		dprintf( "SenTemp,SenHumi,AVDD,DVDD,VCB," );
		for( i=0; i<MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS]; i++ ){
			dprintf( "VZERO%u,VBIAS%u,ICB%u,DvSumTIA%u,DvSumVZERO%u,DvSumBIAS%u,PeakDvTIA%u,", i,i,i,i,i,i,i );
			dprintf( "PeakDvZERO%u,PeakDvBIAS%u,nDet%u,nRat%u,nPer%u,iPer%u,GasPer%u,Conc%u,", i,i,i,i,i,i,i,i );
		}
	}
	if( MeasHandle.pSen->pCvt->IsPresent == true ){
		dprintf( "SenTemp,SrcPwr," );
		for( i=0; i<MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; i++ ){
			dprintf( "BzTmr%u,BzCnt%u,HT%u,LT%u,DvMax%u,DvMaxSum%u,AvDvSum%u,PtmOffs%u,OcAvrg%u,", i+2,i+2,i,i,i,i,i,i,i );
			dprintf( "HP%u,LP%u,nDet%u,nRat%u,nPer%u,iPer%u,GasPer%u,Conc%u,", i,i,i+2,i+2,i+2,i+2,i+2,i+2 );
		}
	}
	dprintf( "OfsTmr, Filter,Er-1,Er-2" );
}   

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Logger( Measuring_HandleType *pMeas )
{
//#ifdef _DEBUG_BUILD
#if 0  
	U8		iCvt;
	U8		iSen;
	
	LogHandle.Period_Cnt--;
	if( LogHandle.Period_Cnt == 0 ){
		LogHandle.Period_Cnt = LogHandle.Period;
		
		switch( LogHandle.Mode ){
			case DATA_LOG_MODE_OFF:
				break;
				
			case DATA_LOG_MODE_DEBUG_PRINT:
#if 0
				dprintf( "\n  %02u/%02u/%02u %02u:%02u:%02u.%02u,",
					LogHandle.pCalendar->Date.Year,
					LogHandle.pCalendar->Date.Month,
					LogHandle.pCalendar->Date.Date,
					LogHandle.pCalendar->Time24.Hours,
					LogHandle.pCalendar->Time24.Minutes,
					LogHandle.pCalendar->Time24.Seconds,
					LogHandle.pCalendar->Time12.SubSeconds
				);
#else
				dprintf( "\n," );
#endif
				// Flowrate & V-Supply
				dprintf(
					"%4.0f,%4.0f,%4.1f,%6.3f,",
					pMeas->pCur->Psen,
					pMeas->pCur->FlowRate,
					PumpDrive.OutDuty,
					pMeas->pCur->VSUP
				);
				if( SystemOption.Item.IsBatMode == true ){
					dprintf(
						"%6.2f,%3u,%6.3f,%6.3f,%6.3f,%6.3f,",
						pMeas->pCur->BatSenTemp,
						PwrCtrl.BatLevel,
						pMeas->pCur->VBUS,
						pMeas->pCur->VBAT,
						pMeas->pCur->IBUS,
						pMeas->pCur->IBAT
					);
				}
				if( pMeas->pSen->pCtg->IsPresent == true ){
					dprintf(
						"%6.2f,%6.2f,%5.3f,%5.3f,%5.3f,",
						pMeas->pCur->CtgSenTemp,
						pMeas->pCur->CtgSenHumi,
						pMeas->pCur->CtgAVDD,
						pMeas->pCur->CtgDVDD,
						pMeas->pCur->CtgVCB
					);
					for( iSen=0; iSen<pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS]; iSen++ ){
						dprintf(
							"%5.3f,%5.3f,%5.3f,%5u,%5u,%5u,%5u,%5u,%5u,%6.4f,%10.1f,%8.2f,%8.2f,%8.2f,%s,",
							pMeas->pCur->CtgVZERO[iSen],
							pMeas->pCur->CtgVBIAS[iSen],
							pMeas->pCur->CtgICB[iSen],
							pMeas->pCur->CtgRawDvSum.LPTIA_LPF[iSen],
							pMeas->pCur->CtgRawDvSum.VZERO[iSen],
							pMeas->pCur->CtgRawDvSum.VBIAS[iSen],
							pMeas->pCur->CtgPeakDv.LPTIA_LPF[iSen],
							pMeas->pCur->CtgPeakDv.VZERO[iSen],
							pMeas->pCur->CtgPeakDv.VBIAS[iSen],
							pMeas->pCur->nDet[iSen],
							pMeas->pCur->nRat[iSen],
							pMeas->pCur->nPer[iSen],
							pMeas->pCur->iPer[iSen],
							pMeas->Gas[iSen].Per,
							vStrGasConc( pMeas->Gas[iSen].Inf.Dp, pMeas->Gas[iSen].Res )
						);
					}
				}
				if( pMeas->pSen->pCvt->IsPresent == true ){
					dprintf( "%6.2f,%4.0f,", pMeas->pCur->CvtSenTemp, pMeas->pCur->CvtSrcPwr );
					for( iCvt=0; iCvt<pMeas->pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; iCvt++ ){
						iSen = iCvt + CARTRIDGE_SEN_CH_MAX;
						dprintf(
							"%4u,%5u,%2u,%2u,%4u,%6u,%6u,%3u,%9.6f,%6.4f,%6.4f,%6.4f,%7.2f,%8.2f,%8.2f,%8.2f,%s,",
							pMeas->Gas[iSen].Inf.bZeroTmr,
							pMeas->Gas[iSen].Inf.bZeroCnt,
							pMeas->Dev.CvtHiTime[iCvt],
							pMeas->Dev.CvtLoTime[iCvt],
							pMeas->Dev.CvtRawDvMax[iCvt],
							pMeas->Dev.CvtRawDvMaxSum[iCvt],
							pMeas->Dev.CvtOcAvrgSum[iCvt],
							pMeas->pSen->pCvt->PtmOffs[iCvt],
							pMeas->pCur->CvtOcAvrg[iCvt],
							pMeas->pCur->CvtHiPeak[iCvt],
							pMeas->pCur->CvtLoPeak[iCvt],
							pMeas->pCur->nDet[iSen],
							pMeas->pCur->nRat[iSen],
							pMeas->pCur->nPer[iSen],
							pMeas->pCur->iPer[iSen],
							pMeas->Gas[iSen].Per,
							vStrGasConc( pMeas->Gas[iSen].Inf.Dp, pMeas->Gas[iSen].Res )
						);
					}
				}
				dprintf( "%5u,%08X, ", pMeas->Filter.CvtOffsInstabilityTmr, pMeas->Filter.Res.Word );
				dprintf( "%02u,%02u,", SystemError.Code[0], SystemError.Code[1] );
				break;
		}
	}
#endif
}


