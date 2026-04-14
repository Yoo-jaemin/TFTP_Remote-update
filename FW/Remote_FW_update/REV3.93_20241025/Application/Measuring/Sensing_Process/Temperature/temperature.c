#define __TEMPERATURE_C__
    #include "temperature.h"
#undef  __TEMPERATURE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "sensing_process.h"
/* End Include */

/* Start Define */
#define LMT_Temp_Calculation( adc )				LMT84_Temperature_Calculation( adc * iADC_LSB )
#define MCU_Temp_Calculation( adc )				( ( adc * iADC_LSB - 760.f ) / 2.5f + 25.f )
#define BAT_Temp_Calculation( adc )				( adc * 0.5f )
#define BAT_Jper_Calculation( adc )				( adc * 0.0976563f )
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

void Temperature_Calculation( Temperature_CalculationType *pTemperature )
{      
	pTemperature->pRaw->LmtTemp = LMT_Temp_Calculation( *pTemperature->pLmtTemp_ADC );                                
	pTemperature->pRaw->McuTemp = MCU_Temp_Calculation( *pTemperature->pMcuTemp_ADC );
	pTemperature->pRaw->BatTemp = BAT_Temp_Calculation( pTemperature->pCharger->TDIE.w );
	pTemperature->pRaw->BatJper = BAT_Jper_Calculation( pTemperature->pCharger->TS.w );
}



