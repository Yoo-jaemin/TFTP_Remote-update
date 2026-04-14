#define __POWER_SUPPLY_C__
    #include "power_supply.h"
#undef  __POWER_SUPPLY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "sensing_process.h"
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

void PowerSupply_Calculation( PowerSupply_CalculationType *pPower )
{
	if( SystemOption.Item.IsUniMode == true ){
		pPower->pRaw->P48 = (float)*pPower->pVSUP_P_ADC * VSUP_MONITOR_SLOPE;
		pPower->pRaw->P24 = (float)*pPower->pVSUP_N_ADC * VSUP_MONITOR_SLOPE;
	}
	else {
		pPower->pRaw->VSUP = ( (float)*pPower->pVSUP_P_ADC - (float)*pPower->pVSUP_N_ADC ) * VSUP_MONITOR_SLOPE;
	}
	
	pPower->pRaw->VBACKUP	= *pPower->pVBAT_ADC * iADC_LSB * 0.002f;
	pPower->pRaw->IBUS		= pPower->pCharger->IBUS.w * 0.001f;
	pPower->pRaw->IBAT		= pPower->pCharger->IBAT.w * 0.001f;
	pPower->pRaw->VBUS		= pPower->pCharger->VBUS.w * 0.001f;
	pPower->pRaw->VBAT		= pPower->pCharger->VBAT.w * 0.001f;
	pPower->pRaw->VSYS		= pPower->pCharger->VSYS.w * 0.001f;
}


