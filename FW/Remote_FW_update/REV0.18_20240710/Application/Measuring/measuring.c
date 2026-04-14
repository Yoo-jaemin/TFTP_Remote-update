#define __MEASURING_C__
	#include "measuring.h"
#undef  __MEASURING_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "flash.h"
#include "modbus.h"
#include "led.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Measuring_Data_Calculation( Measuring_HandleType *pMeas );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Init( void )
{
	MeasHandle.pSen = &SensingProcess;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Task( void )
{
	U8		Hp = MeasHandle.pSen->Hp & SENSING_RAW_FIFO_MASK;
	U8		Tp = MeasHandle.pSen->Tp & SENSING_RAW_FIFO_MASK;
	
	if( Hp != Tp ){
		
		LED_Tog();
		Measuring_Data_Calculation( &MeasHandle );
                
                dprintf(" %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f"EOL, 
                                      MeasHandle.Data.LPTIA0_LPF.f, MeasHandle.Data.LPTIA1_LPF.f,
                                          MeasHandle.Data.SenTemp.f, MeasHandle.Data.SenHumi.f, MeasHandle.Data.McuTemp.f,
                                               MeasHandle.Data.AVDD.f, MeasHandle.Data.DVDD.f); 
		MODBUS_Mapping();
		
		MeasHandle.pSen->Tp++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Data_Calculation( Measuring_HandleType *pMeas )
{
	Sensing_RawType		*pRaw = &pMeas->pSen->Raw[MeasHandle.pSen->Tp & SENSING_RAW_FIFO_MASK];
	U8					iBuf;
	U8					iMux;
	U32					RawDvSum[ADMUX_MAX];
	Measuring_DataType	Result;
	U16					HiPeak[ADMUX_MAX], LoPeak[ADMUX_MAX];
	
	for( iMux=0; iMux<ADMUX_MAX; iMux++ ){ RawDvSum[iMux] = 0; }

	Result.McuTemp.f = 0;
	Result.AVDD.f = 0;
	Result.DVDD.f = 0;
	Result.VCB.f = 0;
	
	Result.VZERO0.f = 0;
	Result.VBIAS0.f = 0;
	Result.ICB0.f = 0;
	
	Result.VZERO1.f = 0;
	Result.VBIAS1.f = 0;
	Result.ICB1.f = 0;
	
	Result.LPTIA0_LPF.f = 0;
	Result.LPTIA1_LPF.f = 0;
	
	for( iMux=0; iMux<ADMUX_MAX; iMux++ ){
		HiPeak[iMux] = 0;
		LoPeak[iMux] = 65535;
	}
	
	for( iBuf=0; iBuf<pMeas->pSen->ADC_BuffSize; iBuf++ ){
		if( iBuf ){
			for( iMux=0; iMux<ADMUX_MAX; iMux++ ){
				RawDvSum[iMux] += abs( pRaw->ADC[iBuf].Word[iMux] - pRaw->ADC[iBuf-1].Word[iMux] );
				if( HiPeak[iMux] < pRaw->ADC[iBuf].Word[iMux] ){ HiPeak[iMux] = pRaw->ADC[iBuf].Word[iMux]; }
				if( LoPeak[iMux] > pRaw->ADC[iBuf].Word[iMux] ){ LoPeak[iMux] = pRaw->ADC[iBuf].Word[iMux]; }
			}
		}
		
		Result.McuTemp.f	+= pRaw->ADC[iBuf].Item.TEMP;
		Result.AVDD.f		+= pRaw->ADC[iBuf].Item.AVDD;
		Result.DVDD.f		+= pRaw->ADC[iBuf].Item.DVDD;
		Result.VCB.f		+= pRaw->ADC[iBuf].Item.VCB;
		
		Result.LPTIA0_LPF.f	+= pRaw->ADC[iBuf].Item.LPTIA0_LPF;
		Result.VZERO0.f		+= pRaw->ADC[iBuf].Item.VZERO0;
		Result.VBIAS0.f		+= pRaw->ADC[iBuf].Item.VBIAS0;
		Result.ICB0.f		+= pRaw->ADC[iBuf].Item.ICB0;
		//Result.VCE0.f		+= pRaw->ADC[iBuf].Item.VCE0;
		//Result.VRE0.f		+= pRaw->ADC[iBuf].Item.VRE0;
		
		Result.LPTIA1_LPF.f	+= pRaw->ADC[iBuf].Item.LPTIA1_LPF;
		Result.VZERO1.f		+= pRaw->ADC[iBuf].Item.VZERO1;
		Result.VBIAS1.f		+= pRaw->ADC[iBuf].Item.VBIAS1;
		Result.ICB1.f		+= pRaw->ADC[iBuf].Item.ICB1;
		//Result.VCE1.f		+= pRaw->ADC[iBuf].Item.VCE1;
		//Result.VRE1.f		+= pRaw->ADC[iBuf].Item.VRE1;
	}
	
	for( iMux=0; iMux<ADMUX_MAX; iMux++ ){
		if( RawDvSum[iMux] > 65535 ){
			RawDvSum[iMux] = 65535;
		}
		pMeas->Data.RawDvSum.Word[iMux] = RawDvSum[iMux];
		pMeas->Data.PeakDv.Word[iMux] = HiPeak[iMux] - LoPeak[iMux];
	}
	
	pMeas->Data.SenHumi.f		= HS300x_Humi_Calculation( pRaw->HS300x_Humi );
	pMeas->Data.SenTemp.f		= HS300x_Temp_Calculation( pRaw->HS300x_Temp );
	
	pMeas->Data.McuTemp.f		= ADC_TO_TEMP_PGA_1_5( Result.McuTemp.f		/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.AVDD.f			= ADC_TO_VOLT_PGA_1_5( Result.AVDD.f		/ pMeas->pSen->ADC_BuffSize ) * 2;
	pMeas->Data.DVDD.f			= ADC_TO_VOLT_PGA_1_5( Result.DVDD.f		/ pMeas->pSen->ADC_BuffSize ) * 2;
	pMeas->Data.VCB.f			= ADC_TO_VOLT_PGA_1_5( Result.VCB.f			/ pMeas->pSen->ADC_BuffSize ) * 2;
	
	pMeas->Data.LPTIA0_LPF.f	= ADC_TO_VOLT_PGA_1_5( Result.LPTIA0_LPF.f	/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.VZERO0.f		= ADC_TO_VOLT_PGA_1_5( Result.VZERO0.f		/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.VBIAS0.f		= ADC_TO_VOLT_PGA_1_5( Result.VBIAS0.f		/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.ICB0.f			= ADC_TO_VOLT_PGA_1_5( Result.ICB0.f		/ pMeas->pSen->ADC_BuffSize ) / 25 / 0.2;
	
	pMeas->Data.LPTIA1_LPF.f	= ADC_TO_VOLT_PGA_1_5( Result.LPTIA1_LPF.f	/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.VZERO1.f		= ADC_TO_VOLT_PGA_1_5( Result.VZERO1.f		/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.VBIAS1.f		= ADC_TO_VOLT_PGA_1_5( Result.VBIAS1.f		/ pMeas->pSen->ADC_BuffSize );
	pMeas->Data.ICB1.f			= ADC_TO_VOLT_PGA_1_5( Result.ICB1.f		/ pMeas->pSen->ADC_BuffSize ) / 25 / 0.2;
}


