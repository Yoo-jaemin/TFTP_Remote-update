#define __IR_C__
    #include "ir.h"
#undef  __IR_C__

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
#if 0
float		SrcPwr = 450.f;
ufloat		Hp[CAVITY_SEN_CH_MAX] = { 4.4f, 4.4f, 4.4f, 4.4f };
ufloat		Lp[CAVITY_SEN_CH_MAX] = { 0.6f, 0.6f, 0.6f, 0.6f };
ufloat		Op[CAVITY_SEN_CH_MAX] = { 3.8f, 3.8f, 3.8f, 3.8f };
ufloat		Oa[CAVITY_SEN_CH_MAX] = { 2.5f, 2.5f, 2.5f, 2.5f };
#endif
/* End Variable */

/* Start Function */
/* End Function */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Raw_Calculation( IR_Raw_CalculationType *pIr )
{
	U16				*pData	= pIr->pReceiveBuf+14;
	U8				i;
	
	pIr->pRaw->SrcPwr = *pData++;
	pIr->pRaw->McuTemp.w.l = *pData++;
	pIr->pRaw->McuTemp.w.h = *pData++;
	pIr->pRaw->SenTemp.w.l = *pData++;
	pIr->pRaw->SenTemp.w.h = *pData++;
	
	for( i=0; i<SensingProcess.pCvt->Inf[CAVITY_INF_CHANNELS]; i++ ){
		
		pData = ( pIr->pReceiveBuf + 20 + ( i * 20 ) );
		
		pIr->pRaw->HiPeak[i].w.l = *pData++;
		pIr->pRaw->HiPeak[i].w.h = *pData++;
		pIr->pRaw->LoPeak[i].w.l = *pData++;
		pIr->pRaw->LoPeak[i].w.h = *pData++;
		pIr->pRaw->OcPtoP[i].w.l = *pData++;
		pIr->pRaw->OcPtoP[i].w.h = *pData++;
		pIr->pRaw->HiAvrg[i].w.l = *pData++;
		pIr->pRaw->HiAvrg[i].w.h = *pData++;
		pIr->pRaw->LoAvrg[i].w.l = *pData++;
		pIr->pRaw->LoAvrg[i].w.h = *pData++;
		pIr->pRaw->AvPtoP[i].w.l = *pData++;
		pIr->pRaw->AvPtoP[i].w.h = *pData++;
		pIr->pRaw->OcAvrg[i].w.l = *pData++;
		pIr->pRaw->OcAvrg[i].w.h = *pData++;
		pIr->pRaw->HiTime[i] = *pData++;
		pIr->pRaw->LoTime[i] = *pData++;
		pIr->pRaw->RawDvMax[i] = *pData++;
	}
}


