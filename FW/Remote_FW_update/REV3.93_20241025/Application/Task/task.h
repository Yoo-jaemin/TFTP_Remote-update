#ifndef   __TASK_H__
#define   __TASK_H__


#ifdef __TASK_C__
	#define TASK_EXT
#else
	#define TASK_EXT extern
#endif

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
TASK_EXT bool Task_Init( void );
TASK_EXT void Main_Task( void );
TASK_EXT void TaskSyncTimer_Handler( void );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __TASK_H__
