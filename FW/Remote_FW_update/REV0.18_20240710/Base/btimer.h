#ifndef   __BTIMER_H__
#define   __BTIMER_H__


#ifdef __BTIMER_C__
	#define BTIMER_EXT
#else
	#define BTIMER_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
enum {
	TMR_10MS = 0,
	TMR_40MS,
	TMR_INTERNAL_ADC_HANDLE,
	TMR_WARM_UP_HANDLE,          
	TMR_LED_BLINK,          
	TMR_RTC_CHECK,
	TMR_BUFF_MAX
};
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
BTIMER_EXT void Base_Timer_Init( void );
BTIMER_EXT void Base_Timer_Handler( void );
/* End Function */

/* Start Variable */
BTIMER_EXT U16      sTimerTick[ TMR_BUFF_MAX ];
BTIMER_EXT U16      sTimerFlag[ TMR_BUFF_MAX ];
/* End Variable */


#endif // __BTIMER_H__
