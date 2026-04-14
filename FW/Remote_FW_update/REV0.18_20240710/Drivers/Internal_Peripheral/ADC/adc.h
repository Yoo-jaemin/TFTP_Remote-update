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
#define ADC_VREF						( 1.82 )
#define ADC_VBIAS_CAP					( 1.11 )
#define ADC_TEMP_K						( 8.13 )
#define ADC_TO_VOLT_PGA_1_5( adc )		( (ADC_VREF / 1.5 * ((((float)adc - 32768) / 32768) * 1.835 / 1.82)) + ADC_VBIAS_CAP )
#define ADC_TO_TEMP_PGA_1_5( adc )		( (float)adc / ( 1.5 * ADC_TEMP_K ) - 273.15 )
/* End Define */

/* Start Enum */
enum {
    ADMUX_TEMP,
    ADMUX_AVDD,
    ADMUX_DVDD,
    ADMUX_VCB, // CB Sensor Power
    ADMUX_LPTIA0_LPF,
    ADMUX_VZERO0,
    ADMUX_VBIAS0,
    ADMUX_ICB0, // CB Sensor Current CH-0
    //ADMUX_VCE0,
    //ADMUX_VRE0,
    ADMUX_LPTIA1_LPF,
    ADMUX_VZERO1,
    ADMUX_VBIAS1,
    ADMUX_ICB1, // CB Sensor Current CH-1
    //ADMUX_VCE1,
    //ADMUX_VRE1,
    ADMUX_MAX
};
/* End Enum */

/* Start Struct */
typedef union {
    U16  Word[ADMUX_MAX];
    struct{
		U16		TEMP;
        U16		AVDD;
		U16		DVDD;
		U16		VCB;
		U16		LPTIA0_LPF;
		U16		VZERO0;
		U16		VBIAS0;
		U16		ICB0;
		//U16		VCE0;
		//U16		VRE0;
		U16		LPTIA1_LPF;
		U16		VZERO1;
		U16		VBIAS1;
		U16		ICB1;
		//U16		VCE1;
		//U16		VRE1;
    }Item;
}ADC_RawType;

typedef struct {
	ADC_RawType		Raw;
	U8				iMux;
	//U16				Cnt[ADMUX_MAX];
	U32				ConvItem[ADMUX_MAX];
	__IO uint32_t	CheckRdy[ADMUX_MAX];
	__IO uint32_t*	pReadReg[ADMUX_MAX];
	bool			ConvComplete;
	bool			ConvReady;
}ADC_HandleType;
/* End Struct */

/* Start Function */
ADC_EXT void ADC_Init( void );
ADC_EXT U16 ADC_CHn_Polling( U32 MuxP, U32 MuxN );
ADC_EXT void ADC_Conversion_Ready( void );
ADC_EXT void ADC_Conversion_Start( void );
/* End Function */

/* Start Variable */
ADC_EXT ADC_HandleType	ADC_Handle;
/* End Variable */


#endif // __ADC_H__
