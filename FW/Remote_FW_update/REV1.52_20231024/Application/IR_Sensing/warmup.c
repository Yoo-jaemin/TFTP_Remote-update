#define __WARMUP_C__
    #include "warmup.h"
#undef  __WARMUP_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "btimer.h"
#include "main.h"
#include "ir.h"
#include "adc.h"
/* End Include */

/* Start Define */
#define ALL_LED_OFF()			HAL_GPIO_WritePin( GPIOC, (GPIO_PIN_6|GPIO_PIN_7), GPIO_PIN_RESET )
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

void Warm_Up_Task( void )
{
    if( !(sTimerTick[TMR_WARM_UP_HANDLE] & 0X8000) ){
        if( SystemState != SYSTEM_STATE_NORMAL ){
            SystemState  = SYSTEM_STATE_NORMAL;
            
            ALL_LED_OFF();
        }
    }
}


