#define __TIA_C__
    #include "tia.h"
#undef  __TIA_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
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
                 
void LPTIA_Config( U8 Channel, LPTIA_ConfType *pTia )
{
    // Potentiostat Power up
    AfeLpTiaPwrDown( Channel, 0 );
    // Normal Mode
    AfeLpTiaAdvanced( Channel, BANDWIDTH_NORMAL, CURRENT_NOR ); 
    // short switch for very large current to avoid saturation
    AfeLpTiaSwitchCfg( Channel, (uint16_t)LPTIA_SWMODE_SHORT );              
    
    AfeLpTiaCon( Channel, pTia->Item.RLOAD, pTia->Item.RGAIN, pTia->Item.RFILTER ); 
    AfeLpTiaSwitchCfg( Channel, (uint16_t)pTia->Item.SWMODE );
}
