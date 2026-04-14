#define __PRESSURE_C__
    #include "pressure.h"
#undef  __PRESSURE_C__

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

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */ 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pressure_Calculation( Pressure_CalculationType *pPressure )
{
	pPressure->pRaw->AmpOut	= *pPressure->pAmpOut_ADC * iADC_LSB;
	pPressure->pRaw->AmpRef	= *pPressure->pAmpRef_ADC * iADC_LSB;
}


