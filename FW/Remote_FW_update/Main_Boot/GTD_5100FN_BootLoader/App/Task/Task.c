#define __TASK_C__
    #include "Task.h"
#undef  __TASK_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "IO_Ctrl.h"
#include "gpio.h"
#include "Flash.h"
#include "SD_Card.h"
#include "Display.h"
#include "Sensor_Upload.h"
#include "usart.h"
#include "CRC.h"

#include "nand_flash.h"
#include "gdi.h"
#include "modbus_rtu_master.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Menu_Operation(void);
static void FuncKey_Set(bool ucSet, U8 ucMenu_pos);
static void RSTKey_Set(bool ucSet, U8 ucMenu_pos);
static void Upload_Task(void);
static void Main_Upload(void);
//static void SysDeInit(void);
static void MainApp_Start(void);
static void bootJump_check(void);

/* End Function */

/* Start Variable */
typedef void (*pFunction)(void);
pFunction Jump_To_Application;
unsigned long JumpAddress = 0;


MODBUS_MasterHandleType modbus;

s_NandFlash Nand_flash;
extern u_Nand_Page nand;
extern uint8_t g_Chk;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SysTick_Task(void)

{
  	if(Upload_State == nSTATE_FINISH){
	  	Tick.uTimer_1S++;
  		if(Tick.uTimer_1S > _1SEC_) {
	  		Tick.uTimer_1S = 0;
	  		if(Tick.uIdle_start > 0) Tick.uIdle_start--;
			else {
			  	TaskStatus.Bit.bIdle_flag = TRUE;
			}
		}
	}
	
	if(Packet_Handle.Flag.Bit.bReqStart == TRUE) Packet_Handle.uTimeout++;
	if(SensorUpload_Handle.Flag.Bit.bSensorCheck == TRUE) SensorUpload_Handle.uSensorCheck_Time++;
	
  	Key_Check();
}

void BootKey_Check(bool bErr)
{
	if(TECT_FUNC_READ() == SWITCH_ON) return;
	if(bErr != FALSE) dprintf("\r\n Init Error");    
    
        if (((*(volatile unsigned long*)MAIN_ADDR) & 0x2FFC0000 ) == 0x20000000) {
            TaskStatus.Bit.bDir_Start = TRUE;
            Sensor_Command(nMAIN_START);
            TaskStatus.Bit.bDir_Start = FALSE;
            SysDeInit();
            /* Jump to user application */
            JumpAddress = *(volatile unsigned long*) (MAIN_ADDR + 4);
            Jump_To_Application = (pFunction) JumpAddress;
            /* Initialize user application's Stack Pointer */
            __set_MSP(*(volatile unsigned long*) MAIN_ADDR);
            Jump_To_Application(); 
	}
	else {
            dprintf("\r\n Nothing Main F/W ...");
            Packet_Handle.Flag.Bit.bAck = FALSE;
            Packet_Handle.Flag.Bit.bNak = FALSE;
            TaskStatus.Bit.bDir_Start = FALSE;
	}
}

void SysConfig(void)
{
      TaskStatus.All = FALSE;
      Packet_Handle.Flag.All = FALSE;
      SensorUpload_Handle.Flag.All = FALSE;
      Key.Setting.Bit.bFucn = FALSE;
    
      if(PORTABLE_MODEL_CHECK() == 0 ) TaskStatus.Bit.bPortable = TRUE;
      else TaskStatus.Bit.bPortable = FALSE;
    
      if(TaskStatus.Bit.bPortable == TRUE)
      {
          SHDN_VSEN_ON();
          HAL_Delay(150); // 15ms < IR Sensor Response
                          // 100ms < Cartridge Response
      }
    
      Sensor_Command(nSENSORCHECK);
      TaskStatus.Bit.bBoot_mode = TRUE;
      Title_Display();				
      StatusBox_Display();
      Tick.uIdle_start = IDLE_TIME;
}

void Boot_Task(void)
{
	Menu_Operation();
	Rx_Handler();
	
	if(TaskStatus.Bit.bBoot_mode != TRUE) Upload_Task();
}

static void Upload_Task(void)
{
    switch(TaskStatus.All & 0x000F)
    {
        case EXIT :        MainApp_Start(); 
        case MAIN_UPLOAD : Main_Upload(); break;
        case IR_UPLOAD :   Sensor_Upload(nCAVITY); break;
        case CT_UPLOAD :   Sensor_Upload(nCARTRIDGE); break;      
        default: TaskStatus.Bit.bBoot_mode = TRUE; break;
    }
    Status_Display();
}

static void Menu_Operation(void)
{
    U8	ucMenu_pos, ucFunc_set, ucRST_set;
    
    Key_Operation(&TaskStatus);
    
    ucMenu_pos = Key.Setting.ucPosition & 0x0F;
    ucFunc_set = Key.Setting.ucPosition & 0x20;
    ucRST_set = Key.Setting.ucPosition & 0x40;
    
    MenuSelect_Display(ucMenu_pos);
    FuncKey_Set(ucFunc_set, ucMenu_pos);
    RSTKey_Set(ucRST_set, ucMenu_pos);
}

static void FuncKey_Set(bool ucSet, U8 ucMenu_pos)
{
      if(ucSet == FALSE) return;
      
      TaskStatus.All &= 0x0830;
      switch(ucMenu_pos)
      {		  		
              case nEXIT:
                    TaskStatus.Bit.bExit = TRUE;  
                    break;
              case nMAIN_UPLOAD:
                    TaskStatus.Bit.bMain_Upload = TRUE; 
                    SD.pExtension = MAIN_EXTENSION; 
                    break;
              case nCT_SENSOR_UPLOAD:
                      TaskStatus.Bit.bCT_Upload = TRUE;
                      CT_UPLOAD_ENABLE();
                      SD.pExtension = CT_EXTENSION; 
                      Sensor_UploadStart(nCARTRIDGE);
                      break;
              case nIR_SENSOR_UPLOAD:
                      TaskStatus.Bit.bIR_Upload = TRUE;
                      IR_UPLOAD_ENABLE();
                      SD.pExtension = IR_EXTENSION;
                      Sensor_UploadStart(nCAVITY);
                      break;
              default:
                      TaskStatus.Bit.bBoot_mode = TRUE;
                      dprintf("\r\n Func Key Set Menu Err"); 
                      break;
      }
      
      //dprintf("\r\n Func Key Set %x", TaskStatus.All); 
      
      Display.Bit.bProgress = TRUE;
      if(ucMenu_pos != nEXIT) UploadStatus_Display(ucMenu_pos);
         
      Key.Setting.Bit.bFucn = FALSE;
      Upload_State = nSTATE_FILE_CHECK;
}

static void RSTKey_Set(bool ucSet, U8 ucMenu_pos)// Main Start 
{
    if(ucSet == FALSE)	return;
    
    TaskStatus.All &= 0x0830;
    
    TaskStatus.Bit.bExit = TRUE;
    
    Key.Setting.Bit.bRST = FALSE;
    Upload_State = nSTATE_FILE_CHECK;
}

void Reset_Status(void)
{
    Key.Setting.Bit.bRST = FALSE;
    
    UploadCount.ulFileRead_offset = 0; // Upload Variable Init
    UploadCount.ulWrite = 0;
    UploadCount.ulVerify = 0;
            
    SD.Check.All = FALSE; //Error Flag Init
    Flash.ulEraseErr = FALSE;
    Flash.ucWriteErr = FALSE;
    Flash.ucVerifyErr = FALSE;
    Flash.ucErase_complet = FALSE;
    SensorUpload_Handle.Flag.All = FALSE;
    
    TaskStatus.All &= 0x0830;
    TaskStatus.Bit.bBoot_mode = TRUE; // Boot Mode change
    Tick.uIdle_start = IDLE_TIME;
    UploadStatus_Display(Key.Setting.ucPosition & 0x0F);
    
    //dprintf("\r\n Sensor Upload Flag %x", SensorUpload_Handle.Flag.All); 
}

static void Main_Upload(void)
{
  	char	cBuff[PAGE_SIZE];
	
	switch(Upload_State)
	{
		case nSTATE_IDLE:
		  	Reset_Status();
		  	break;
		case nSTATE_STOP:
		  	FlashSector_Erase(FLASH_USER_START_SECTOR); 
			break;
		case nSTATE_FILE_CHECK:
		  	if(TaskStatus.Bit.bExit != TRUE) Download_File_Find();
			break;
		case nSTATE_FLASH_ERASE:
		  	if(UploadCount.ucErase < nSECTOR_MAX) FlashSector_Erase(UploadCount.ucErase);
			else Flash.ucErase_complet = TRUE;
			ucDisplay_Progress = ((UploadCount.ucErase - FLASH_USER_START_SECTOR)*100)/(nSECTOR_MAX - FLASH_USER_START_SECTOR);
			UploadCount.ucErase++;
			break;
		case nSTATE_FLASH_UPLOAD:
		  	UploadCount.ulFileRead_offset = UploadCount.ulWrite * PAGE_SIZE;
			if(UploadCount.ulWrite == SD.uProgram_cnt - 1){
				FilePageLoad(SD.Info.fname, cBuff, SD.uProgram_tail, UploadCount.ulFileRead_offset);
				Upload_Flash(cBuff, SD.uProgram_tail, UploadCount.ulFileRead_offset);
				ucDisplay_Progress = ((UploadCount.ulWrite+1)*100)/SD.uProgram_cnt;
			}
			else {
				FilePageLoad(SD.Info.fname, cBuff, PAGE_SIZE, UploadCount.ulFileRead_offset);
				Upload_Flash(cBuff, PAGE_SIZE, UploadCount.ulFileRead_offset);
				ucDisplay_Progress = (UploadCount.ulWrite*100)/SD.uProgram_cnt;
			}
			UploadCount.ulWrite++;
			break;
		case nSTATE_FLASH_VERIFY:
			UploadCount.ulFileRead_offset = UploadCount.ulVerify * PAGE_SIZE;
			if(UploadCount.ulVerify == SD.uProgram_cnt - 1){
				FilePageLoad(SD.Info.fname, cBuff, SD.uProgram_tail, UploadCount.ulFileRead_offset);
				Verify_Flash(cBuff, SD.uProgram_tail, UploadCount.ulFileRead_offset);
				ucDisplay_Progress = ((UploadCount.ulVerify+1)*100)/SD.uProgram_cnt;
			}
			else {
			  	FilePageLoad(SD.Info.fname, cBuff, PAGE_SIZE, UploadCount.ulFileRead_offset);
				Verify_Flash(cBuff, PAGE_SIZE, UploadCount.ulFileRead_offset);
				ucDisplay_Progress = (UploadCount.ulVerify*100)/SD.uProgram_cnt;
			}
			UploadCount.ulVerify++;
			break;
		case nSTATE_FINISH:
		 	Display.Bit.bProgress = FALSE;
		  	break;
		default:	break;
	}
}

static void MainApp_Start(void)
{
      if (((*(volatile unsigned long*)MAIN_ADDR) & 0x2FFC0000 ) == 0x20000000) 
      {
          if (Upload_State == nSTATE_FILE_CHECK) 
          {
              TaskStatus.Bit.bDir_Start = TRUE;
              Sensor_Command(nMAIN_START);
              if (TaskStatus.Bit.bPortable == TRUE) {
                  Display_Off();
                  SHDN_VSEN_OFF();
              }
              
              TaskStatus.Bit.bDir_Start = FALSE;
              SysDeInit();
              // Jump to user application
              JumpAddress = *(volatile unsigned long*) (MAIN_ADDR + 4);
              Jump_To_Application = (pFunction) JumpAddress;
              // Initialize user application's Stack Pointer
              __set_MSP(*(volatile unsigned long*) MAIN_ADDR);
              Jump_To_Application();
           }
      }
      else 
      {
          if(Upload_State != nSTATE_FINISH) Show_Display(nNONE_MAIN_FW);
          if(Upload_State != nSTATE_IDLE) Upload_State = nSTATE_FINISH;
      }
}

/*
static void MainApp_Start(void)
{
  	if(((*(volatile unsigned long*)MAIN_ADDR) & 0x2FFC0000 ) == 0x20000000) {
	  	if(Upload_State == nSTATE_FILE_CHECK) {
		  	if((TaskStatus.Bit.bCT_Check != TRUE) && (TaskStatus.Bit.bIR_Check != TRUE)) Packet_Handle.Flag.Bit.bTimeout = TRUE;
			else {
				TaskStatus.Bit.bCT_Start = FALSE;
                TaskStatus.Bit.bIR_Start = FALSE;
                Sensor_Command(nMAIN_START);
                if(TaskStatus.Bit.bPortable == TRUE) Display_Off();
			}
		}
		if( (TaskStatus.Bit.bCT_Start == true) || (TaskStatus.Bit.bIR_Start == true) ) {
            TaskStatus.Bit.bCT_Start  = FALSE;
            TaskStatus.Bit.bIR_Start = FALSE;
            //dprintf("\r\n App Start...");
            if(TaskStatus.Bit.bPortable == TRUE) SHDN_VSEN_OFF();
			SysDeInit();
			// Jump to user application
			JumpAddress = *(volatile unsigned long*) (MAIN_ADDR + 4);
			Jump_To_Application = (pFunction) JumpAddress;
			// Initialize user application's Stack Pointer
			__set_MSP(*(volatile unsigned long*) MAIN_ADDR);
			Jump_To_Application();
		}
		else {
	  		if(Packet_Handle.Flag.Bit.bNak == TRUE) {
			  	if(Upload_State != nSTATE_FINISH) {
					Show_Display(nNONE_SENSOR_FW);
					Packet_Handle.Flag.Bit.bNak = FALSE;
			  	}
				if(Upload_State != nSTATE_IDLE) Upload_State = nSTATE_FINISH;
			}
		}
		
		if(Packet_Handle.Flag.Bit.bTimeout == TRUE) {
		  	if(Upload_State != nSTATE_FINISH) {
				Show_Display(nRESPONSE_ERR);
				Packet_Handle.Flag.Bit.bTimeout = FALSE;
			}
	  		if(Upload_State != nSTATE_IDLE) Upload_State = nSTATE_FINISH;
		}
	}
	else {
		if(Upload_State != nSTATE_FINISH) Show_Display(nNONE_MAIN_FW);
		if(Upload_State != nSTATE_IDLE) Upload_State = nSTATE_FINISH;
	}
}*/


// Remote update functions : jmyoo
// *************************************************************************************************
void Remote_SysConfig(void)
{
      TaskStatus.All = FALSE;
      Packet_Handle.Flag.All = FALSE;
      SensorUpload_Handle.Flag.All = FALSE;
      
      Sensor_Command(nSENSORCHECK);
      //Sensor_VersionCheck();
      Remote_Title_Display();
      StatusBox_Display();
      Tick.uIdle_start = IDLE_TIME;
}

void Remote_Upload_Set( U8 select_menu )
{
      switch (select_menu)
      {
        case nMAIN_UPLOAD :
              printf(" Remote_Upload_Set : nMAIN_UPLOAD\r\n");
              TaskStatus.Bit.bMain_Upload = TRUE; 
              break;
        case nCT_SENSOR_UPLOAD :
              printf(" Remote_Upload_Set : nCT_SENSOR_UPLOAD\r\n");
              TaskStatus.Bit.bCT_Upload = TRUE;
              CT_UPLOAD_ENABLE();
              Sensor_UploadStart(nCARTRIDGE);
              break;
        case nIR_SENSOR_UPLOAD :
              printf(" Remote_Upload_Set : nIR_SENSOR_UPLOAD\r\n");
              TaskStatus.Bit.bIR_Upload = TRUE;
              IR_UPLOAD_ENABLE();
              Sensor_UploadStart(nCAVITY);
              break;
      }               
     
     Display.Bit.bProgress = TRUE;
     UploadStatus_Display(select_menu);

     //HAL_Delay(100);
     Upload_State = nSTATE_FILE_CHECK;
}

void Remote_Upload_Task(uint8_t chk)
{
      Rx_Handler();
      
      switch (chk)
      {
          case nMAIN_UPLOAD :
               Remote_Main_Upload();
               break;
          case nCT_SENSOR_UPLOAD :
               Remote_Sensor_Upload(nCARTRIDGE);
               break;
          case nIR_SENSOR_UPLOAD :   
               Remote_Sensor_Upload(nCAVITY);
               break;
      } // end switch
      
      Remote_Status_Display();
}

void Remote_Main_Upload(void)
{
      //printf(" Remote_Main_Upload\r\n");
      switch (Upload_State)
      {
            case nSTATE_IDLE:
                  Remote_Reset_Status();
                  bootJump_check();
                  break;
            case nSTATE_STOP :
                  printf(" nSTATE_STOP !\r\n");
                  //FlashSector_Erase(FLASH_USER_START_SECTOR); 
                  break;
            case nSTATE_FILE_CHECK :
                 GDI_SetTextColor(WHITE);
                 GDI_StringColor (110, 136, GDI_STR_ALIGN_C, WHITE, "FILE CHECK......  ");
                 Remote_Download_File_Find(); 
                 break;
            case nSTATE_FLASH_ERASE :                   
                 if ( UploadCount.ucErase < nSECTOR_MAX ) 
                    FlashSector_Erase(UploadCount.ucErase);
                 else 
                    Flash.ucErase_complet = TRUE;
                 ucDisplay_Progress = ((UploadCount.ucErase - FLASH_USER_START_SECTOR)*100)/(nSECTOR_MAX - FLASH_USER_START_SECTOR);
                 UploadCount.ucErase++;
                 break;
            case nSTATE_FLASH_UPLOAD :
                 UploadCount.ulFileRead_offset = UploadCount.ulWrite * PAGE_SIZE; 
                 if (UploadCount.ulWrite == Nand_flash.program_cnt - 1) {
                        Remote_FilePageLoad();
                        Upload_Flash((char*) nand.byte, Nand_flash.program_tail, UploadCount.ulFileRead_offset);
                        ucDisplay_Progress = ((UploadCount.ulWrite+1)*100) / Nand_flash.program_cnt;
                 } else {
                        Remote_FilePageLoad();
                        Upload_Flash((char*) nand.byte, PAGE_SIZE, UploadCount.ulFileRead_offset);
                        ucDisplay_Progress = (UploadCount.ulWrite*100) / Nand_flash.program_cnt;
                 }
                 if (HAL_NAND_Address_Inc(&NandBank3_Handle, &NandAddr)!= NAND_VALID_ADDRESS) 
                      printf("\r\n Addr Inc Err\r\n");
                 UploadCount.ulWrite++;
                 break;
            case nSTATE_FLASH_VERIFY : 
                  UploadCount.ulFileRead_offset = UploadCount.ulVerify * PAGE_SIZE;
                  if (UploadCount.ulVerify == Nand_flash.program_cnt  - 1) {
                        Remote_FilePageLoad();
                        Verify_Flash((char*) nand.byte,  Nand_flash.program_tail, UploadCount.ulFileRead_offset);
                        ucDisplay_Progress = ((UploadCount.ulVerify+1)*100) / Nand_flash.program_cnt;
                  } else {
                        Remote_FilePageLoad();
                        Verify_Flash((char*) nand.byte, PAGE_SIZE, UploadCount.ulFileRead_offset);
                        ucDisplay_Progress = (UploadCount.ulVerify*100) / Nand_flash.program_cnt;
                  }
                  if (HAL_NAND_Address_Inc(&NandBank3_Handle, &NandAddr)!= NAND_VALID_ADDRESS) 
                        printf("\r\n Addr Inc Err\r\n");
                  UploadCount.ulVerify++;
                  break;
            case nSTATE_FINISH:
                  Display.Bit.bProgress = FALSE;
                  break;        
      } // end switch
} // end func

void Remote_Reset_Status(void)
{
    //printf(" Remote_Reset_Status() call~\r\n");
    
    UploadCount.ulFileRead_offset = 0; // Upload 변수 초기화
    UploadCount.ulWrite = 0;
    UploadCount.ulVerify = 0;
            
    Flash.ulEraseErr = FALSE;
    Flash.ucWriteErr = FALSE;
    Flash.ucVerifyErr = FALSE;
    Flash.ucErase_complet = FALSE;
    SensorUpload_Handle.Flag.All = FALSE;
    
    TaskStatus.All &= 0x0830;
    TaskStatus.Bit.bBoot_mode = TRUE; // Boot Mode로 변경
    Tick.uIdle_start = IDLE_TIME;
    UploadStatus_Display(g_Chk);
}

static void bootJump_check(void)
{
    //printf("\r\n bootJump_check Func! \r\n");
    NandAddr.Page  = 0;         
    NandAddr.Block = 14;
    NandAddr.Plane = 0;
    HAL_NAND_Read_Page_8b(&NandBank3_Handle, &NandAddr, nand.byte, 1);
    
    if (nand.byte[1]) {
         printf("\r\n CT bootJump_ ! \r\n");
         CT_UPLOAD_ENABLE();
         Cartridge_BootJump_Write_Packet_Send( &modbus );
         HAL_Delay(80);
         HAL_NVIC_SystemReset();
    } else if (nand.byte[2]) {
         printf("\r\n IR bootJump_ ! \r\n");
         IR_UPLOAD_ENABLE();
         Cavity_BootJump_Write_Packet_Send( &modbus );
         HAL_Delay(80);
         HAL_NVIC_SystemReset();
    }
}
