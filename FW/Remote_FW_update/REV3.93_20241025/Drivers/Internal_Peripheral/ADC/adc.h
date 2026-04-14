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
#define iADC_RES						( 4095.f )
#define iADC_VREF						( 3000.f ) // mV
#define iADC_LSB 						( iADC_VREF / iADC_RES )
#define iADC_vScale						( iADC_LSB * 0.001f )
/* End Define */

/* Start Enum */
enum {   
	iADC_VSUP_N,
	iADC_VSUP_P,
	iADC_LMT_TEMP,  
	iADC_PSEN_AMPOUT,
	iADC_PSEN_AMPREF,
	iADC_MCU_TEMP,
	iADC_VBAT,
	iADC_MAX
};
/* End Enum */

/* Start Struct */ 
typedef struct {
	bool					IsVersionScanMode;
	bool					ConversionComplete;
	uint32_t				NoOfConversion;
	U16						Raw[iADC_MAX];
	ADC_HandleTypeDef		Handle;
}iADC_HandleType;
/* End Struct */

/* Start Function */ 
ADC_EXT bool iADC_Init( void );
ADC_EXT void iADC_BatteryScan( bool use );
ADC_EXT void iADC_Conversion_Start( void );
/* End Function */

/* Start Variable */
ADC_EXT iADC_HandleType		iADC; 
/* End Variable */


#endif // __ADC_H__
