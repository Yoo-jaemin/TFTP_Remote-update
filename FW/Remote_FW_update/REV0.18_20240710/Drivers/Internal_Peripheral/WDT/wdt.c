#define __WDT_C__
    #include "wdt.h"
#undef  __WDT_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gpt.h"
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
                 
void WDT_Init( void )
{
	// 32,768Hz / 256 = 128Hz = 0.0078125sec
	// 1sec / 0.0078125sec = 128
	AfeWdtLd( 128 );
	
	delay_10us( 1000 );
   
#if WDT_INTERRUPT_EN
   AfeWdtCfg( WDT_MODE_PERIODIC, ENUM_WDT_CTL_DIV256, WDT_IRQ_EN  , WDT_CLKIN_DIV1 );
   NVIC_EnableIRQ( AFE_Watchdog_IRQn );
#else 
   AfeWdtCfg( WDT_MODE_PERIODIC, ENUM_WDT_CTL_DIV256, WDT_RESET_EN, WDT_CLKIN_DIV1 );
#endif
       
	AfeWdtWindowCfg( WDT_WINDOW_DIS, 0 );
	AfeWdtPowerDown( AFEWDT_ALWAYS_ON ); 
	AfeWdtGo( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WDT_Refresh( void )
{
	if( TaskSyncTimer.Evt.Bit._01_MSEC == true ){
		TaskSyncTimer.Evt.Bit._01_MSEC = false;
		
		AfeWdtKick();
	}
}
