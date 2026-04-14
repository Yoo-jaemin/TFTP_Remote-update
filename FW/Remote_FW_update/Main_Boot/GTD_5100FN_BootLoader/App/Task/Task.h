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
#define _1SEC_	1000
#define IDLE_TIME 1 //sec
/* End Define */

/* Start Enum */
typedef enum
{
  	nSTATE_IDLE,
	nSTATE_STOP,
	nSTATE_FILE_CHECK,
	nSTATE_GET_BINARY,
	nSTATE_FLASH_ERASE,
	nSTATE_FLASH_UPLOAD,
	nSTATE_FLASH_VERIFY,
	nSTATE_FINISH
}Upload_StatusType;

/* Start Struct */
typedef struct {
  	U16	uIdle_start;
	U16	uTimer_1S;
}SysTick_Handle;

typedef struct{
  	U8	ucErase;
	U32	ulFileRead_offset;
	U32	ulWrite;
	U32 ulVerify;
}UploadCount_Handle;

typedef union{
  	struct {
	  	bool bExit			:1;
		bool bMain_Upload	:1;
		bool bIR_Upload		:1;
		bool bCT_Upload		:1;
        
		bool bIR_Check		:1;
		bool bCT_Check		:1;
		bool bBoot_mode		:1;
		bool bIdle_flag		:1;
        
        bool bIR_Start		:1;
        bool bCT_Start		:1;
        bool bDir_Start 	:1;
        bool bPortable		:1; //Portable Model Check
        
        bool bEmpty12		:1;
        bool bEmpty13		:1;
        bool bEmpty14		:1;
        bool bEmpty15		:1;
	}Bit;
	U16	All;
}MainStatus;

/* End Struct */

/* Start Function */
TASK_EXT void SysTick_Task(void);
TASK_EXT void Boot_Task(void);
TASK_EXT void SysConfig(void);
TASK_EXT void Reset_Status(void);
TASK_EXT void BootKey_Check(bool bErr);


TASK_EXT void Remote_SysConfig(void);
TASK_EXT void Remote_Upload_Set(U8 select_menu);
TASK_EXT void Remote_Upload_Task(uint8_t chk);
TASK_EXT void Remote_Main_Upload(void);
TASK_EXT void Remote_Reset_Status(void);
/* End Function */

/* Start Variable */
TASK_EXT MainStatus TaskStatus;
TASK_EXT UploadCount_Handle UploadCount;
TASK_EXT SysTick_Handle Tick;
TASK_EXT Upload_StatusType Upload_State;
/* End Variable */


#endif // __TASK_H__