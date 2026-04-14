#define __BTIMER_C__
    #include "btimer.h"
#undef  __BTIMER_C__

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

/* Start Variable */
/* End Variable */

/* Start Function */
void Base_Timer_Init( void )
{
    sTimerTick[TMR_10MS] = 10;
    sTimerTick[TMR_40MS] = 0;
    sTimerTick[TMR_INTERNAL_ADC_HANDLE] = 0;
    sTimerFlag[TMR_INTERNAL_ADC_HANDLE] = 0;
    sTimerTick[TMR_WARM_UP_HANDLE] = 0x8000+499;
    sTimerFlag[TMR_WARM_UP_HANDLE] = 0;
    sTimerTick[TMR_LED_BLINK] = 0x8004;
    sTimerFlag[TMR_LED_BLINK] = 0;
    sTimerTick[TMR_RTC_CHECK] = 0x8000+24;
    sTimerFlag[TMR_RTC_CHECK] = 0;
}

void Base_Timer_Handler( void )
{
	U16		bTn;
    
    //USART3_Rx_Packet_Check();
    
    if( sTimerTick[TMR_INTERNAL_ADC_HANDLE] & 0x8000 ){
        if( sTimerTick[TMR_INTERNAL_ADC_HANDLE] == 0x8000 ){
            sTimerTick[TMR_INTERNAL_ADC_HANDLE] = 0;
                            
            //ADC1_Conversion_Start();
        }
        else {
            sTimerTick[TMR_INTERNAL_ADC_HANDLE]--;
        }
    }
    
    sTimerTick[TMR_10MS]--;
	if( sTimerTick[TMR_10MS] == 0 ){
		sTimerTick[TMR_10MS] = 10;
		
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
			    if( sTimerTick[TMR_RTC_CHECK] & 0x8000 ){
			        if( sTimerTick[TMR_RTC_CHECK] == 0x8000 ){
			            sTimerTick[TMR_RTC_CHECK] = 0x8000+24;
			            
			            sTimerFlag[TMR_RTC_CHECK] = 1;
			        }
			        else {
			            sTimerTick[TMR_RTC_CHECK]--;
			        }
			    }
			    break;
				
			case 3:
			    break;
		}
		
		sTimerTick[TMR_40MS]++;
	}
}

void HAL_SYSTICK_Callback( void )
{          
    Base_Timer_Handler(); 
}
/* End Function */
