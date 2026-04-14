#define __GPT_C__
    #include "gpt.h"
#undef  __GPT_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "modbus.h"
#include "sensing_process.h"
#include "led.h"
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
                 
void GPT_Init( void )
{
	TaskSyncTimer.Cnt._01_MSEC		= 0; 
	TaskSyncTimer.Cnt._02_MSEC_1	= 0;
	TaskSyncTimer.Cnt._02_MSEC_2	= 5;
	TaskSyncTimer.Cnt._04_MSEC		= 0;
	TaskSyncTimer.Cnt._10_MSEC		= 0;
	TaskSyncTimer.Cnt._01_SEC		= 25;
	
	TaskSyncTimer.Evt.Word = 0;
	/* Configure Timer 0 */
	
	// Enable Timer0 for periodic mode, counting down, using PCLK as clock source div16
	// 26000000 MHz / 16 / Load(1625) = 1 KHz = 1 msec
	
	GptLd( pADI_TMR0, 1625 );
	GptCfg( pADI_TMR0, TCTL_CLK_PCLK, TCTL_PRE_DIV16, BITM_TMR_CTL_MODE | BITM_TMR_CTL_EN );
	
	NVIC_EnableIRQ( TMR0_EVT_IRQn );
	NVIC_SetPriority( TMR0_EVT_IRQn, 1 );
	
	// Enable Timer0 for periodic mode, counting down, using PCLK as clock source div16
	// 26000000 MHz / 4 / Load(1300) = 20,000 Hz = 50 usec
	GptLd( pADI_TMR1, 325 );
	GptCfg( pADI_TMR1, TCTL_CLK_PCLK, TCTL_PRE_DIV1, BITM_TMR_CTL_MODE | BITM_TMR_CTL_EN );
	
	NVIC_EnableIRQ( TMR1_EVT_IRQn );
	NVIC_SetPriority( TMR1_EVT_IRQn, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GP_Tmr0_Int_Handler( void )
{
	volatile int	uiT0STA = 0;
	U8				_02_MS_ = TaskSyncTimer.Cnt._01_MSEC	& 1;
	U8				_04_MS_ = TaskSyncTimer.Cnt._02_MSEC_1	& 1;
	U8				_08_MS_ = TaskSyncTimer.Cnt._04_MSEC	& 1;
	U8				_40_MS_ = TaskSyncTimer.Cnt._10_MSEC	& 3;
	
	uiT0STA = GptSta( pADI_TMR0 );
	
	if( (uiT0STA & 0x0001) == 0x0001 ){						// timeout event pending
		
		GptClrInt( pADI_TMR0, BITM_TMR_CLRINT_TIMEOUT );	// Clear Timer 0 timeout interrupt
		
		TaskSyncTimer.Cnt._01_MSEC++;
		TaskSyncTimer.Evt.Bit._01_MSEC = true;
		
		switch( _02_MS_ ){
			case 0:
				TaskSyncTimer.Cnt._02_MSEC_1++;
				switch( _04_MS_ ){
					case 0:
						TaskSyncTimer.Evt.Bit._04_MSEC = true;
						break;
						
					case 1:
						TaskSyncTimer.Cnt._04_MSEC++;
						switch( _08_MS_ ){
							case 0: TaskSyncTimer.Evt.Bit._08_MSEC_1 = true; break;
							case 1: TaskSyncTimer.Evt.Bit._08_MSEC_2 = true; break;
						}
						break;
				}
				break;
				
			case 1:
				TaskSyncTimer.Cnt._02_MSEC_2--;
				if( TaskSyncTimer.Cnt._02_MSEC_2 == 0 ){
					TaskSyncTimer.Cnt._02_MSEC_2 =  5;
					
					TaskSyncTimer.Cnt._10_MSEC++;
					
					switch( _40_MS_ ){
						case 0: TaskSyncTimer.Evt.Bit._40_MSEC_1 = true; break; // Flash
						case 1: TaskSyncTimer.Evt.Bit._40_MSEC_2 = true; break;
						case 2: TaskSyncTimer.Evt.Bit._40_MSEC_3 = true; break;
						case 3:
							TaskSyncTimer.Cnt._01_SEC--;
							if( TaskSyncTimer.Cnt._01_SEC == 0 ){
								TaskSyncTimer.Cnt._01_SEC = 25;
								
								TaskSyncTimer.Evt.Bit._01_SEC = true;
							}
							break;
					}
				}
				break;
		}
		MODBUS_Rx_Packet_Check();
		Sensing_Process();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GP_Tmr1_Int_Handler( void )
{
	volatile int	uiT1STA = 0;
	
	uiT1STA = GptSta( pADI_TMR1 );
	
	if( (uiT1STA & 0x0001) == 0x0001 ){						// timeout event pending
		
		GptClrInt( pADI_TMR1, BITM_TMR_CLRINT_TIMEOUT );	// Clear Timer 1 timeout interrupt
		ADC_Conversion_Start();
	}
}


