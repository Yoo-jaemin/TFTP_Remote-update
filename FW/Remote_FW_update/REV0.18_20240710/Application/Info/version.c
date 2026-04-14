#define __VERSION_C__
    #include "version.h"
#undef  __VERSION_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "adc.h"
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

void System_Ver_Load( void )
{
	float	fData;
	U16		i;
	
    fData = ADC_TO_VOLT_PGA_1_5( ADC_CHn_Polling( MUXSELP_AIN5, MUXSELN_VSET1P1 ) );
	
    for( i=0; i<11; i++ ){ // Revision 0.0 ~ 1.1
        if( fData < ((0.15f * i) + 0.35f) ){ break; }
    }
	
	HardwareVersion = i * 0.1f;
	
	dprintf( "  HARDWARE REVISION: %5.3f[V}, %3.1f\n", fData, HardwareVersion ); 
}


