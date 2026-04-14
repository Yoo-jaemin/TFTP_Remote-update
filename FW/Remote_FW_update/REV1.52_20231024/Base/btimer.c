#define __BTIMER_C__
    #include "btimer.h"
#undef  __BTIMER_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "ir.h"        
#include "usart3.h"     
#include "led.h"     
#include "adc.h"     
#include "option.h"  
#include "modbus.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Variable */
/* End Variable */

/* Start Function */
static void Base_Timer_Handler( void );
/* End Function */ 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Base_Timer_Init( void )
{
	sTimerTick[TMR_10MS] = 100;
	sTimerTick[TMR_40MS] = 0;
	sTimerTick[TMR_WARM_UP_HANDLE] = 0x8000+1499;
	sTimerFlag[TMR_WARM_UP_HANDLE] = 0;
	sTimerTick[TMR_LED_BLINK] = 0x8004;
	sTimerFlag[TMR_LED_BLINK] = 0;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_SYSTICK_Callback( void )
{
	IR_Signal_Processing();
    Base_Timer_Handler();
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Base_Timer_Handler( void )
{
	U16		bTn;
	
	USART3_Rx_Packet_Check();
	MODBUS_Timeout_Check_Slave();
	
	IR_Process_Timer();
	
	sTimerTick[TMR_10MS]--;
	if( sTimerTick[TMR_10MS] == 0 ){
		sTimerTick[TMR_10MS] = 100;
		
		bTn = sTimerTick[TMR_40MS] & 3;
		
        switch( bTn ){
			case 0:
				if( sTimerTick[TMR_WARM_UP_HANDLE] & 0x8000 ){
					if( sTimerTick[TMR_WARM_UP_HANDLE] == 0x8000 ){
						sTimerTick[TMR_WARM_UP_HANDLE] = 0;
						
						sTimerFlag[TMR_WARM_UP_HANDLE] = 1;
					}
					else {
						sTimerTick[TMR_WARM_UP_HANDLE]--;
					}
				}
				break;
				
			case 1:   
				if( sTimerTick[TMR_LED_BLINK] & 0x8000 ){
					if( sTimerTick[TMR_LED_BLINK] == 0x8000 ){
						sTimerTick[TMR_LED_BLINK] = 0x8004;
						
						sTimerFlag[TMR_LED_BLINK] = 1;
					}
					else {
						sTimerTick[TMR_LED_BLINK]--;
					}
				}
				break;
				
			case 2:
				break;
				
			case 3:
				break;
		}
		
		sTimerTick[TMR_40MS]++;
	}
}


