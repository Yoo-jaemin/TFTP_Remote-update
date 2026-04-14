#define __DAC_C__
    #include "dac.h"
#undef  __DAC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */
#define VZERO_TO_CZERO( vzero )		( (uint16_t)((vzero - 200) *   63.0 / 2166 + 0.5) )
#define VBIAS_TO_CBIAS( vbias )		( (uint16_t)((vbias - 200) * 4095.0 / 2200 + 0.5) )
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
                 
// 12bit 0.2V ~ 2.4  V (1LSB  0.537uV) / 12bit DAC = 0.2V + (LPDACDAT Bits[11: 0] *  0.54mV) + 0.54mV
//  6bit 0.2V ~ 2.366V (1LSB 34.38 mV) /  6bit DAC = 0.2V + (LPDACDAT Bits[17:12] * 34.38mV)

// 1000 = mV
// Czero = (uint16_t)((Vzero * 1000 - 200) *   63.0 / 2166 + 0.5);  6bit
// Cbias = (uint16_t)((Vbias * 1000 - 200) * 4095.0 / 2200 + 0.5); 12bit

// 200 is 200mV offset for code 0, 63 is (2^6 -1), 2166 is max voltage - 200mV offset. 0.5 is to round LSs up.

void LPDAC_Config( U8 Channel, LPDAC_ConfType *pDac )
{   
    uint16_t CBias, CZero;
    
    LPDacPwrCtrl( Channel, PWR_UP );
    
    CZero = VZERO_TO_CZERO( pDac->VZERO );  
 
    if( pDac->VZERO == pDac->VBIAS ){
        LPDacCfg( Channel, LPDACSWNOR, VBIAS12BIT_VZERO12BIT_SHORT, LPDACREF2P5 );
        CBias = CZero * 64;
    }
    else {
        LPDacCfg( Channel, LPDACSWNOR, VBIAS12BIT_VZERO6BIT, LPDACREF2P5 );
        CBias = VBIAS_TO_CBIAS( pDac->VBIAS );
    }
        
    LPDacWr( Channel, CZero, CBias );
}
