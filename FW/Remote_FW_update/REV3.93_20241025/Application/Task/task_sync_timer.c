#define __TASK_SYNC_TIMER_C__
    #include "task_sync_timer.h"
#undef  __TASK_SYNC_TIMER_C__

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

void TaskSyncTimer_Init( void )
{                                   
	TaskSyncTimer.Cnt._01_MSEC		= 0; 
	TaskSyncTimer.Cnt._02_MSEC_1	= 0;
	TaskSyncTimer.Cnt._02_MSEC_2	= 2500;
	TaskSyncTimer.Cnt._04_MSEC		= 0;
	TaskSyncTimer.Cnt._10_MSEC		= 0;
	TaskSyncTimer.Cnt._01_SEC		= 25;
	
	TaskSyncTimer.Evt.Word = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskSyncTimer_Handler( void )
{                                                   
	U8	_02_MS_ = TaskSyncTimer.Cnt._01_MSEC	& 1;
	U8	_04_MS_ = TaskSyncTimer.Cnt._02_MSEC_1	& 1;
	U8	_08_MS_ = TaskSyncTimer.Cnt._04_MSEC	& 1;
	U8	_40_MS_ = TaskSyncTimer.Cnt._10_MSEC	& 3;
	
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
					case 0: TaskSyncTimer.Evt.Bit._40_MSEC_1 = true; break;
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
}


