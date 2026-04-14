#define __BTIMER_C__
    #include "btimer.h"
#undef  __BTIMER_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "task_sync_timer.h"
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

void HAL_SYSTICK_Callback( void )
{          
    TaskSyncTimer_Handler(); 
}
