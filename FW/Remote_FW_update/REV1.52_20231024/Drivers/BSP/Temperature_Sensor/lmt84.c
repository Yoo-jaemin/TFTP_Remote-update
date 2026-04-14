#define __LMT84_C__
    #include "lmt84.h"
#undef  __LMT84_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
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

float LMT84_Temperature_Calculation( float fVtemp )
{
    return ( ( 5.506 - sqrt( pow( -5.506, 2 ) + 4 * 0.00176 * ( 870.6 - fVtemp ))) / ( 2 * -0.00176 ) + 30 );
}


