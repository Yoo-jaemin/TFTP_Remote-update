#define __CT_C__
    #include "ct.h"
#undef  __CT_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "sensing_process.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Variable */
/* End Variable */

/* Start Function */
/* End Function */
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CT_Calculation( CT_Raw_CalculationType *pCt )
{
	U16		*pData	= pCt->pReceiveBuf+8;
	U8		i;
	
	pCt->pRaw->SenHumi.w.l = *pData++;
	pCt->pRaw->SenHumi.w.h = *pData++;
	pCt->pRaw->SenTemp.w.l = *pData++;
	pCt->pRaw->SenTemp.w.h = *pData++;
	pCt->pRaw->McuTemp.w.l = *pData++;
	pCt->pRaw->McuTemp.w.h = *pData++;
	
	pCt->pRaw->AVDD.w.l = *pData++;
	pCt->pRaw->AVDD.w.h = *pData++;
	pCt->pRaw->DVDD.w.l = *pData++;
	pCt->pRaw->DVDD.w.h = *pData++;
	pCt->pRaw->VCB.w.l = *pData++;
	pCt->pRaw->VCB.w.h = *pData++;
	
	for( i=0; i<SensingProcess.pCtg->Inf[CARTRIDGE_INF_CHANNELS]; i++ ){
		
		pData = ( pCt->pReceiveBuf + 20 + ( i * 25 ) );
		
		pCt->pRaw->RawDvSum.LPTIA_LPF[i] = *pData++;
		pCt->pRaw->RawDvSum.VZERO[i] = *pData++;
		pCt->pRaw->RawDvSum.VBIAS[i] = *pData++;
		pCt->pRaw->PeakDv.LPTIA_LPF[i] = *pData++;
		pCt->pRaw->PeakDv.VZERO[i] = *pData++;
		pCt->pRaw->PeakDv.VBIAS[i] = *pData++;
		pCt->pRaw->LPTIA_LPF[i].w.l = *pData++;
		pCt->pRaw->LPTIA_LPF[i].w.h = *pData++;
		pCt->pRaw->VZERO[i].w.l = *pData++;
		pCt->pRaw->VZERO[i].w.h = *pData++;
		pCt->pRaw->VBIAS[i].w.l = *pData++;
		pCt->pRaw->VBIAS[i].w.h = *pData++;
		pCt->pRaw->ICB[i].w.l = *pData++;
		pCt->pRaw->ICB[i].w.h = *pData++;
	}
}


