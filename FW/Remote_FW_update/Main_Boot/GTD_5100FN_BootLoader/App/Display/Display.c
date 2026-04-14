#define __DISPLAY_C__
    #include "Display.h"
#undef  __DISPLAY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gdi.h"
#include "IO_Ctrl.h"
#include "Flash.h"
#include "SD_Card.h"
#include "Task.h"
#include "Sensor_Upload.h"

#include "nand_flash.h"
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
extern s_NandFlash Nand_flash;
extern uint8_t g_Chk;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Title_Display(void)
{
  	char 	cStr[18];
	
  	GDI_SetBackColor( BLACK );
	GDI_StatusArea_Clear();
	GDI_SetFont(&Font11x16);
	GDI_StringColor (110, 14, GDI_STR_ALIGN_C, WHITE, "F/W UPLOAD");
	GDI_SetFont(&Font07x12);
	GDI_SetTextColor( GRAY );
	GDI_DrawHLine(25,30,170);
	
	sprintf(&cStr[0], "Main:%1.1f", FirmwareVersion);
	GDI_StringColor(26, 35, GDI_STR_ALIGN_L, LIGHTSKYBLUE, &cStr[0]);
	GDI_StringColor (27, 58, GDI_STR_ALIGN_L, YELLOW, "MAIN");
	GDI_StringColor (111, 58, GDI_STR_ALIGN_L, WHITE, "[  READY   ]");
	GDI_String (27, 100, GDI_STR_ALIGN_L, "EXIT");
	
	if(TaskStatus.Bit.bIR_Check == TRUE) {
		sprintf(&cStr[0], "Cvt:%1.1f", ((float)SensorUpload_Handle.uCavityFW_ver / 10));
		GDI_StringColor (89, 35, GDI_STR_ALIGN_L, LIGHTSKYBLUE, &cStr[0]);
		GDI_StringColor (27, 86, GDI_STR_ALIGN_L, WHITE, "CAVITY-IR");
		GDI_String (111, 86, GDI_STR_ALIGN_L, "[  READY   ]");
	}
	else {
	  	GDI_StringColor (89, 35, GDI_STR_ALIGN_L, GRAY, "Cvt:Dis");
		GDI_String (27, 86, GDI_STR_ALIGN_L, "CAVITY-IR");
		GDI_String (111, 86, GDI_STR_ALIGN_L, "[ DISABLE  ]");
	}
	
	if(TaskStatus.Bit.bCT_Check == TRUE) {
		sprintf(&cStr[0], "Ctg:%1.1f", ((float)SensorUpload_Handle.uCatridgeFW_ver / 10));
	  	GDI_StringColor (145, 35, GDI_STR_ALIGN_L, LIGHTSKYBLUE, &cStr[0]);
		GDI_StringColor (27, 72, GDI_STR_ALIGN_L, WHITE, "CARTRIDGE");
		GDI_String (111, 72, GDI_STR_ALIGN_L, "[  READY   ]");
	}
	else {
		GDI_StringColor (145, 35, GDI_STR_ALIGN_L, GRAY, "Ctg:Dis");
		GDI_String (27, 72, GDI_STR_ALIGN_L, "CARTRIDGE");
		GDI_String (111, 72, GDI_STR_ALIGN_L, "[ DISABLE  ]");
	}
}

void MenuSelect_Display(U8 ucMenu_pos)
{
  	static U8 ucOld_pos = 0;
	
	if(ucOld_pos == ucMenu_pos) return;
	
	GDI_SetTextColor( WHITE );
	switch(ucOld_pos)
	{
	  	case nMAIN_UPLOAD: GDI_String (27, 58, GDI_STR_ALIGN_L, "MAIN"); break;
		case nCT_SENSOR_UPLOAD: GDI_String (27, 72, GDI_STR_ALIGN_L, "CARTRIDGE"); break;
		case nIR_SENSOR_UPLOAD: GDI_String (27, 86, GDI_STR_ALIGN_L, "CAVITY-IR"); break;
		case nEXIT: GDI_String (27, 100, GDI_STR_ALIGN_L, "EXIT"); break;
		default:
		  	GDI_StatusArea_Clear();
		  	GDI_SetTextColor( RED );
			GDI_String (110, 136, GDI_STR_ALIGN_C, "Menu Error");
			break; 
	}
	
	GDI_SetTextColor( YELLOW );
	switch(ucMenu_pos)
	{
	  	case nMAIN_UPLOAD: GDI_String (27, 58, GDI_STR_ALIGN_L, "MAIN"); break;
		case nCT_SENSOR_UPLOAD: GDI_String (27, 72, GDI_STR_ALIGN_L, "CARTRIDGE"); break;
		case nIR_SENSOR_UPLOAD: GDI_String (27, 86, GDI_STR_ALIGN_L, "CAVITY-IR"); break;
		case nEXIT: GDI_String (27, 100, GDI_STR_ALIGN_L, "EXIT"); break;
		default:
		  	GDI_StatusArea_Clear();
		  	GDI_SetTextColor( RED );
			GDI_String (110, 136, GDI_STR_ALIGN_C, "Menu Error");
			break; 
	}
	
	ucOld_pos = ucMenu_pos;
}

void StatusBox_Display(void)
{
  GDI_SetTextColor(GRAY);
  GDI_DrawHLine(25,123,170);
  GDI_DrawHLine(25,161,170);
  GDI_DrawVLine(25,123,38);
  GDI_DrawVLine(195,123,38);
}

void Status_Display(void)
{
  	char 	cStr[18];
	
	switch(Upload_State)
	{
		case nSTATE_IDLE:
		  	break;
		case nSTATE_STOP: 
		  	dprintf("\r\n Upload Fail... Progress Stop");
			Upload_State = nSTATE_FINISH;
			break;
		case nSTATE_FILE_CHECK:
		  	if((SD.Check.All & 0x3F) != FALSE){
			  	SD.Check.Bit.bComplet = FALSE;
				GDI_StatusArea_Clear();
				GDI_SetTextColor(RED);
				switch(SD.Check.All & 0x3F)
				{
					case 0x01: GDI_String (110, 136, GDI_STR_ALIGN_C, "SD Mount Fail"); break;
					case 0x02: GDI_String (110, 136, GDI_STR_ALIGN_C, "SD Not Insertion"); break;
					case 0x04: GDI_String (110, 136, GDI_STR_ALIGN_C, "Not Find File"); break;
					case 0x08: GDI_String (110, 136, GDI_STR_ALIGN_C, "File Open Fail"); break;
					case 0x10: GDI_String (110, 136, GDI_STR_ALIGN_C, "File Read Fail"); break;
					case 0x20: GDI_String (110, 136, GDI_STR_ALIGN_C, "SD Init Fail"); break;
					default: 
					  	sprintf(&cStr[0], "SD Error Code-%02x", SD.Check.All);
					  	GDI_String(110, 136, GDI_STR_ALIGN_C, &cStr[0]);
						break;
				}
				Upload_State = nSTATE_FINISH;
			}
			else {
			  	if(SD.Check.Bit.bComplet == TRUE) {																   
					SD.Check.Bit.bComplet = FALSE;
					UploadCount.ucErase = FLASH_USER_START_SECTOR;
					Upload_State = nSTATE_FLASH_ERASE;
					GDI_SetTextColor(WHITE);
				}
				else {
				  	if(TaskStatus.Bit.bMain_Upload == TRUE) Upload_State = nSTATE_FINISH;
					if(SensorUpload_Handle.Flag.Bit.bError == TRUE) Upload_State = nSTATE_FINISH;
				}
			}
			break;
		case nSTATE_FLASH_ERASE:
			if((Flash.ulEraseErr != HAL_FLASH_ERROR_NONE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
			  	Flash.ucErase_complet = FALSE;
				GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "ERASE......  FAIL");
				Display.Bit.bComplete = FALSE;
				Display.Bit.bFail = TRUE;
				Upload_State = nSTATE_FINISH;
			}
			else {
				if((Flash.ucErase_complet == TRUE) || (SensorUpload_Handle.Flag.Bit.bComplete == TRUE)){
				  	SensorUpload_Handle.Flag.Bit.bComplete = FALSE;
				  	Flash.ucErase_complet = FALSE;
					Upload_State = nSTATE_FLASH_UPLOAD;
					GDI_StringColor (110, 136, GDI_STR_ALIGN_C, WHITE, "WRITE...... READY");																    
					ucDisplay_Progress = 0;
			  	}
				else{
				  	sprintf(&cStr[0], "ERASE......  %3d%%", ucDisplay_Progress);
					GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
				}
			}
			break;
		case nSTATE_FLASH_UPLOAD:
			if(UploadCount.ulWrite == SD.uProgram_cnt){
			  	if((Flash.ucWriteErr == TRUE)|| (SensorUpload_Handle.Flag.Bit.bError == TRUE)){
				  	GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "WRITE......  FAIL");
					Display.Bit.bComplete = FALSE;
					Display.Bit.bFail = TRUE;
					Upload_State = nSTATE_STOP;
				}
				else {
					UploadCount.ulWrite = 0;
					UploadCount.ulVerify = 0;
					UploadCount.ulFileRead_offset = 0;
					Upload_State = nSTATE_FLASH_VERIFY;
					GDI_StringColor (110, 136, GDI_STR_ALIGN_C, WHITE, "VERIFY..... READY");
					ucDisplay_Progress = 0;
				}
			  	
			}
			else if(UploadCount.ulWrite == SD.uProgram_cnt - 1){
			  	if((Flash.ucWriteErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)){
				  	GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "WRITE......  FAIL");
					Display.Bit.bComplete = FALSE;
					Display.Bit.bFail = TRUE;
					Upload_State = nSTATE_STOP;
				}
				else if(SD.Check.All != FALSE){
				  	GDI_StatusArea_Clear();
					GDI_SetTextColor(RED);
					switch(SD.Check.All & 0x18)
					{
					  	case 0x08: GDI_String (110, 136, GDI_STR_ALIGN_C, "File Open Fail"); break;
						case 0x10: GDI_String (110, 136, GDI_STR_ALIGN_C, "File Read Fail"); break;
						default:
						  	sprintf(&cStr[0], "SD Error Code-%02x", SD.Check.All);
						  	GDI_String(110, 136, GDI_STR_ALIGN_C, &cStr[0]); 
							break;
					}
					Upload_State = nSTATE_STOP;
				}
				else{
					sprintf(&cStr[0], "WRITE......  %3d%%", ucDisplay_Progress);
					GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
				}
			}
			else {
			  	if((Flash.ucWriteErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)){
				  	GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "WRITE......  FAIL");
					Display.Bit.bComplete = FALSE;
					Display.Bit.bFail = TRUE;
					Upload_State = nSTATE_STOP;
			  	}
				else if(SD.Check.All != FALSE){
				  	GDI_StatusArea_Clear();
					GDI_SetTextColor(RED);
					switch(SD.Check.All & 0x18)
					{
					  	case 0x08: GDI_String (110, 136, GDI_STR_ALIGN_C, "File Open Fail"); break;
						case 0x10: GDI_String (110, 136, GDI_STR_ALIGN_C, "File Read Fail"); break;
						default: 
						  	sprintf(&cStr[0], "SD Error Code-%02x", SD.Check.All);
						  	GDI_String(110, 136, GDI_STR_ALIGN_C, &cStr[0]); 
							break;
					}
					Upload_State = nSTATE_STOP;
				}
				else{
				sprintf(&cStr[0], "WRITE......  %3d%%", ucDisplay_Progress);
				GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
				}
			}
			break;
		case nSTATE_FLASH_VERIFY: 
			if(UploadCount.ulVerify == SD.uProgram_cnt){
			  	if((Flash.ucVerifyErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)){
				  	GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "VERIFY.....  FAIL");
					Display.Bit.bComplete = FALSE;
					Display.Bit.bFail = TRUE;
					Upload_State = nSTATE_STOP;
				}
				else{
				  	UploadCount.ulWrite = 0;
					UploadCount.ulVerify = 0;
					UploadCount.ulFileRead_offset = 0;
					GDI_StringColor (110, 136, GDI_STR_ALIGN_C, LIGHTSKYBLUE, "UPLOAD.....   OK!");
					Display.Bit.bComplete = TRUE;
					Display.Bit.bFail = FALSE;
					Upload_State = nSTATE_FINISH;
				}
			  	
			}
			else if(UploadCount.ulVerify == SD.uProgram_cnt - 1){
				if((Flash.ucVerifyErr == TRUE)  || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
				  	GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "VERIFY.....  FAIL");
					Display.Bit.bComplete = FALSE;
					Display.Bit.bFail = TRUE;
					Upload_State = nSTATE_STOP;
				}
				else {
					sprintf(&cStr[0], "VERIFY.....  %3d%%", ucDisplay_Progress);
					GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
				}
			}
			else {
				if((Flash.ucVerifyErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)){
				  	GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "VERIFY.....  FAIL");
					Display.Bit.bComplete = FALSE;
					Display.Bit.bFail = TRUE;
					Upload_State = nSTATE_STOP;
				}
				else {
					sprintf(&cStr[0], "VERIFY.....  %3d%%", ucDisplay_Progress);
					GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
				}
			}
			break;
		case nSTATE_FINISH:
		  	if(TaskStatus.Bit.bIdle_flag == TRUE) {
				Upload_State = nSTATE_IDLE;
				TaskStatus.Bit.bIdle_flag = FALSE;
				dprintf("\r\n FINISH");
			}		
		  	break;
		default:	break;
	}
}

void UploadStatus_Display(U8 ucSelect_menu)
{
  	GDI_StatusArea_Clear();
	
  	switch (ucSelect_menu)
	{
            case nMAIN_UPLOAD:
                  //printf(" UploadStatus_Display main\r\n");
                  if(Display.Bit.bProgress == TRUE) {
                      GDI_StringColor (111, 58, GDI_STR_ALIGN_L, YELLOW, "[ PROGRESS ]");
                      //printf(" PROGRESS \r\n");
                  } else if(Display.Bit.bComplete == TRUE) {
                      GDI_StringColor (111, 58, GDI_STR_ALIGN_L, LIGHTSKYBLUE, "[ COMPLETE ]");
                      //printf(" COMPLETE \r\n");
                  } else if(Display.Bit.bFail == TRUE) {
                      GDI_StringColor (111, 58, GDI_STR_ALIGN_L, RED, "[   FAIL   ]");
                      //printf(" FAIL \r\n");
                  } else {
                      GDI_StringColor (111, 58, GDI_STR_ALIGN_L, WHITE, "[  READY   ]");
                      //printf(" READY \r\n");
                  } break;
            case nCT_SENSOR_UPLOAD:
                  if(Display.Bit.bProgress == TRUE) GDI_StringColor (111, 72, GDI_STR_ALIGN_L, YELLOW, "[ PROGRESS ]");
                  else if(Display.Bit.bComplete == TRUE) GDI_StringColor (111, 72, GDI_STR_ALIGN_L, LIGHTSKYBLUE, "[ COMPLETE ]");
                  else if(Display.Bit.bFail == TRUE) GDI_StringColor (111, 72, GDI_STR_ALIGN_L, RED, "[   FAIL   ]");
                  else GDI_StringColor (111, 72, GDI_STR_ALIGN_L, WHITE, "[  READY   ]");
                  break;
            case nIR_SENSOR_UPLOAD: 
                  if(Display.Bit.bProgress == TRUE) GDI_StringColor (111, 86, GDI_STR_ALIGN_L, YELLOW, "[ PROGRESS ]");
                  else if(Display.Bit.bComplete == TRUE) GDI_StringColor (111, 86, GDI_STR_ALIGN_L, LIGHTSKYBLUE, "[ COMPLETE ]");
                  else if(Display.Bit.bFail == TRUE) GDI_StringColor (111, 86, GDI_STR_ALIGN_L, RED, "[   FAIL   ]");
                  else GDI_StringColor (111, 86, GDI_STR_ALIGN_L, WHITE, "[  READY   ]");
                  break;
            default: dprintf("\r\n Show Display Position Error"); break;
	}
    //Display.All = 0;
}

void Show_Display(DisplayIndicator nDisplay)
{
    GDI_StatusArea_Clear();
    
    switch(nDisplay)
    {
        case nNONE_SENSOR_FW: GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "Nothing Sensor F/W"); break;
        case nNONE_MAIN_FW: GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "Nothing Main F/W"); break;
        case nRESPONSE_ERR: GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "Response Fail"); break;
        default: dprintf("\r\n Show Display Error"); break;
    }
}

void Display_Off(void)
{
    DisplayOff();
}




////////////////////////////////////////////////////////////////////////////////////////////////////

// jmyoo
void Remote_Title_Display(void)
{
  	char 	cStr[18];
	
  	GDI_SetBackColor( BLACK );
	GDI_StatusArea_Clear();
	GDI_SetFont(&Font11x16);
	GDI_StringColor (110, 14, GDI_STR_ALIGN_C, WHITE, "Remote F/W UPLOAD");
	GDI_SetFont(&Font07x12);
	GDI_SetTextColor( GRAY );
	GDI_DrawHLine(25,30,170);
	
	sprintf(&cStr[0], "Main:%1.1f", FirmwareVersion);
	GDI_StringColor(26, 35, GDI_STR_ALIGN_L, LIGHTSKYBLUE, &cStr[0]);
	GDI_StringColor (27, 58, GDI_STR_ALIGN_L, WHITE, "MAIN");
	GDI_StringColor (111, 58, GDI_STR_ALIGN_L, WHITE, "[  READY   ]");
	GDI_String (27, 100, GDI_STR_ALIGN_L, "EXIT");
        
	if(TaskStatus.Bit.bIR_Check == TRUE) {
		sprintf(&cStr[0], "Cvt:%1.1f", ((float)SensorUpload_Handle.uCavityFW_ver / 10));
		GDI_StringColor (89, 35, GDI_STR_ALIGN_L, LIGHTSKYBLUE, &cStr[0]);
		GDI_StringColor (27, 86, GDI_STR_ALIGN_L, WHITE, "CAVITY-IR");
		GDI_String (111, 86, GDI_STR_ALIGN_L, "[  READY   ]");
	}
	else {
	  	GDI_StringColor (89, 35, GDI_STR_ALIGN_L, GRAY, "Cvt:Dis");
		GDI_String (27, 86, GDI_STR_ALIGN_L, "CAVITY-IR");
		GDI_String (111, 86, GDI_STR_ALIGN_L, "[ DISABLE  ]");
	}
	
	if(TaskStatus.Bit.bCT_Check == TRUE) {
		sprintf(&cStr[0], "Ctg:%1.1f", ((float)SensorUpload_Handle.uCatridgeFW_ver / 10));
	  	GDI_StringColor (145, 35, GDI_STR_ALIGN_L, LIGHTSKYBLUE, &cStr[0]);
		GDI_StringColor (27, 72, GDI_STR_ALIGN_L, WHITE, "CARTRIDGE");
		GDI_String (111, 72, GDI_STR_ALIGN_L, "[  READY   ]");
	}
	else {
		GDI_StringColor (145, 35, GDI_STR_ALIGN_L, GRAY, "Ctg:Dis");
		GDI_String (27, 72, GDI_STR_ALIGN_L, "CARTRIDGE");
		GDI_String (111, 72, GDI_STR_ALIGN_L, "[ DISABLE  ]");
	}
}

void Remote_Status_Display(void)
{
    char 	cStr[18];
    
    //printf("\r\n Remote_Status_Display() ! \r\n");
    switch(Upload_State)
    {
        case nSTATE_IDLE:
             break;
        case nSTATE_STOP : 
            dprintf("\r\n Upload Fail... Progress Stop");
            Upload_State = nSTATE_FINISH;
            break;
        case nSTATE_FILE_CHECK :
          if (Nand_flash.bFile_chk == true) {
              Nand_flash.bFile_chk = false;
              UploadCount.ucErase = FLASH_USER_START_SECTOR; //main flash
              Upload_State = nSTATE_FLASH_ERASE;
              GDI_StatusArea_Clear();
              GDI_SetTextColor(WHITE);
          } else {
              //Upload_State = nSTATE_FINISH;
              Upload_State = nSTATE_FILE_CHECK;
          } break;
        case nSTATE_FLASH_ERASE :
          if ((Flash.ulEraseErr != HAL_FLASH_ERROR_NONE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
              GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "ERASE......  FAIL");
              Display.Bit.bComplete = FALSE;
              Display.Bit.bFail = TRUE;
              Upload_State = nSTATE_FINISH;
          } else {
              if ((Flash.ucErase_complet == TRUE) || (SensorUpload_Handle.Flag.Bit.bComplete == TRUE)) {
                  SensorUpload_Handle.Flag.Bit.bComplete = FALSE;
                  Upload_State = nSTATE_FLASH_UPLOAD;
                  GDI_StringColor (110, 136, GDI_STR_ALIGN_C, WHITE, "WRITE...... READY");																    
                  ucDisplay_Progress = 0;
                   
                  switch (g_Chk)
                  {
                      case nMAIN_UPLOAD : 
                          NandAddr.Page  = 0;
                          NandAddr.Block = 0;
                          NandAddr.Plane = 0;
                          break;
                      case nCT_SENSOR_UPLOAD :
                          NandAddr.Page  = 0;
                          NandAddr.Block = 10;
                          NandAddr.Plane = 0; 
                          break;
                      case nIR_SENSOR_UPLOAD : 
                          NandAddr.Page  = 0;
                          NandAddr.Block = 12;
                          NandAddr.Plane = 0;
                          break;
                   }
              } else {
                  sprintf(&cStr[0], "ERASE......  %3d%%", ucDisplay_Progress);
                  GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);  
              }
          } break;
        case nSTATE_FLASH_UPLOAD :
            if (UploadCount.ulWrite == Nand_flash.program_cnt) {
                if ((Flash.ucWriteErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
                    GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "WRITE......  FAIL");
                    Display.Bit.bComplete = FALSE;
                    Display.Bit.bFail = TRUE;
                    Upload_State = nSTATE_STOP;
                } else {
                    UploadCount.ulWrite = 0;
                    UploadCount.ulVerify = 0;
                    UploadCount.ulFileRead_offset = 0;
                    Upload_State = nSTATE_FLASH_VERIFY;
                    GDI_StringColor (110, 136, GDI_STR_ALIGN_C, WHITE, "VERIFY..... READY");
                    ucDisplay_Progress = 0;
                    
                    switch (g_Chk)
                    {
                        case nMAIN_UPLOAD : 
                            NandAddr.Page  = 0;
                            NandAddr.Block = 0;
                            NandAddr.Plane = 0;
                            break;
                        case nCT_SENSOR_UPLOAD :
                            NandAddr.Page  = 0;
                            NandAddr.Block = 10;
                            NandAddr.Plane = 0; 
                            break;
                        case nIR_SENSOR_UPLOAD : 
                            NandAddr.Page  = 0;
                            NandAddr.Block = 12;
                            NandAddr.Plane = 0;
                            break;
                     }
                }
            } else {
                if ((Flash.ucWriteErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
                    GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "WRITE......  FAIL");
                    Display.Bit.bComplete = FALSE;
                    Display.Bit.bFail = TRUE;
                    Upload_State = nSTATE_STOP;
                } else {
                    sprintf(&cStr[0], "WRITE......  %3d%%", ucDisplay_Progress);
                    GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
                }
            } break;
        case nSTATE_FLASH_VERIFY :     
              if (UploadCount.ulVerify == Nand_flash.program_cnt) {
                  if ((Flash.ucVerifyErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
                      GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "VERIFY.....  FAIL");
                      Display.Bit.bComplete = FALSE;
                      Display.Bit.bFail = TRUE;
                      Upload_State = nSTATE_STOP;
                  } else {
                      UploadCount.ulWrite = 0;
                      UploadCount.ulVerify = 0;
                      UploadCount.ulFileRead_offset = 0;
                      GDI_StringColor (110, 136, GDI_STR_ALIGN_C, LIGHTSKYBLUE, "UPLOAD.....   OK!");
                      Display.Bit.bComplete = TRUE;
                      Display.Bit.bFail = FALSE;
                      Upload_State = nSTATE_FINISH;
                  }
              } else {
                  if ((Flash.ucVerifyErr == TRUE) || (SensorUpload_Handle.Flag.Bit.bError == TRUE)) {
                      GDI_StringColor (110, 136, GDI_STR_ALIGN_C, RED, "VERIFY.....  FAIL");
                      Display.Bit.bComplete = FALSE;
                      Display.Bit.bFail = TRUE;
                      Upload_State = nSTATE_STOP;
                  } else {
                      sprintf(&cStr[0], "VERIFY.....  %3d%%", ucDisplay_Progress);
                      GDI_String (110, 136, GDI_STR_ALIGN_C, &cStr[0]);
                  }
              } break;
        case nSTATE_FINISH : 
              if (TaskStatus.Bit.bIdle_flag == TRUE) {
                  Upload_State = nSTATE_IDLE;
                  TaskStatus.Bit.bIdle_flag = FALSE;
                  dprintf("\r\n FINISH");
              } break;
    } // end switch
}
