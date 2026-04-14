#ifndef   __ADC_H__
#define   __ADC_H__


#ifdef __ADC_C__
	#define ADC_EXT
#else
	#define ADC_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define iADC_RES				(4095.f)
#define iADC_VREF				(3300.f) // mV
#define iADC_LSB				(iADC_VREF/iADC_RES)

                  
#define VDD_APPLI				((uint32_t) 3300.f) /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS			((uint32_t) 4095.f) /* Max value with a full range of 12 bits */

/* Internal temperature sensor: constants data used for indicative values in  */
/* this example. Refer to device datasheet for min/typ/max values.            */
/* For more accurate values, device should be calibrated on offset and slope  */
/* for application temperature range.                                         */
#define iTemp_V25				(1430.f) /* Internal temperature sensor, parameter V25 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define iTemp_Average_Slope		(4300.f) /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */                                                               /* This calibration parameter is intended to calculate the actual VDDA from Vrefint ADC measurement. */

/* Private macro -------------------------------------------------------------*/

/**
  * @brief  Computation of temperature (unit: degree Celsius) from the internal
  *         temperature sensor measurement by ADC.
  *         Computation is using temperature sensor standard parameters (refer
  *         to device datasheet).
  *         Computation formula:
  *         Temperature = (VTS - V25)/Avg_Slope + 25
  *         with VTS = temperature sensor voltage
  *              Avg_Slope = temperature sensor slope (unit: uV/DegCelsius)
  *              V25 = temperature sensor @25degC and Vdda 3.3V (unit: mV)
  *         Calculation validity conditioned to settings: 
  *          - ADC resolution 12 bits (need to scale value if using a different 
  *            resolution).
  *          - Power supply of analog voltage Vdda 3.3V (need to scale value 
  *            if using a different analog voltage supply value).
  * @param TS_ADC_DATA: Temperature sensor digital value measured by ADC
  * @retval None
  */
#define MCU_Temp_Calculation( adc )		( ( iTemp_V25 - adc * iADC_LSB ) * 1000.f / iTemp_Average_Slope + 25.f )

/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */ 
ADC_EXT void ADC1_Init( void );
ADC_EXT void ADC1_Conversion_Start( void );
/* End Function */

/* Start Variable */ 
ADC_EXT ADC_HandleTypeDef	iADC1_Handle;
ADC_EXT U16					iADC1_Raw[2];
/* End Variable */


#endif // __ADC_H__
