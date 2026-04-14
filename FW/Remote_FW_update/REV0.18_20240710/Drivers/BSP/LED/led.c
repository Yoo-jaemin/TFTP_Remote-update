#define __LED_C__
    #include "led.h"
#undef  __LED_C__

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

void LED_Init( void )
{
    DioCfgPin( pADI_GPIO1, PIN1, 1 );
	DioOenPin( pADI_GPIO1, PIN1, 1 );	// P1.1 as an output pin
	DioDsPin ( pADI_GPIO1, PIN1, 1 );	// Set drive strngth to full power
	DioSetPin( pADI_GPIO1, PIN1 );
}


