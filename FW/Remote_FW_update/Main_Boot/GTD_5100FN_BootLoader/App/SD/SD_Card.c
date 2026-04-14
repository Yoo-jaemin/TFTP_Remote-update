#define __SD_CARD_C__
    #include "SD_Card.h"
#undef  __SD_CARD_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "fatfs.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void SD_Check(void);
/* End Function */

/* Start Variable */

//unsigned short Download_Cnt;
unsigned char Sram_CheckSum;
char buff[PAGE_SIZE];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U8 SDIO_SD_Init(void)
{
  	MX_FATFS_Init();
	
  	hsd.Instance = SDIO;
  	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  	hsd.Init.ClockDiv = 0;
	
	if(BSP_SD_Init() == MSD_OK) return MSD_OK;
	else return MSD_ERROR;
}

void SDIO_SD_DeInit(void)
{
  	if(BSP_SD_DeInit() != MSD_OK) Error_Handler();
	FATFS_DeInit();
}

void SD_Mount(void)
{
  	if(f_mount(&SD.FatFS, (TCHAR const*)SD.cPath, 0) != FR_OK) {
		SD.Check.Bit.bMount_err = TRUE;
		//printf("\r\n SD mount error"); 
  	}
	else {
	  	SD.Check.Bit.bMount_err = FALSE;
	  	//printf("\r\n SD mount OK");
	}
}

void SD_Check(void)
{
	U8 ucState;
		
	ucState = BSP_SD_IsDetected();
	if(ucState == SD_PRESENT) {
		SD.Check.Bit.bInsert_err = FALSE;
		SD_Mount();
		// Re-insert ther SD card after reading it DISK Err. init 
		// Init Error Fault set
		if(BSP_SD_Init() == MSD_OK) SD.Check.Bit.bInit_err = FALSE;
		else SD.Check.Bit.bInit_err = TRUE; 
	}
	else {
	  	SD.Check.Bit.bInsert_err = TRUE;
	  	//printf("\r\n SD Card Empty");
	}
}

void Download_File_Find(void)
{
    FRESULT res; 
    DIR Dir;
	FILINFO	FInfo;
	
	U8 	i = 0, j = 0, State = 0, Ext[3];
	
	SD_Check();
	
	if(SD.Check.Bit.bInit_err == TRUE) return;
	if(SD.Check.Bit.bInsert_err == TRUE) return;
	
	res = f_opendir(&Dir, SD.cPath);
    if (res)
    {
        put_rc(res);
		SD.Check.Bit.bFileOpen_err = TRUE;
        return;
    }

    for(;;) 
    {
        res = f_readdir(&Dir, &FInfo);
		if ((res != FR_OK) || !FInfo.fname[0]) {
			//SD.Check.Bit.bFileRead_err = TRUE;
            break;
		}
   
		for(i = 0 ; i < 13 ; i++)
		{
			if(State == 0)
			{
				if(FInfo.fname[i] == '.')
				{
					State++;
				}
			}
			else if(State == 1)
			{
				Ext[j] = FInfo.fname[i];
				j++;
				if(j > 2)
				{
					j = 0;
					State = 0;
				}
			}
		}

		if(memcmp(SD.pExtension, Ext, sizeof(Ext)) == FALSE)
		{
			SD.Check.Bit.bFileFind_err = FALSE;
			SD.Info.fsize = FInfo.fsize;
			memcpy(SD.Info.fname, FInfo.fname, sizeof(FInfo.fname));
			SD.uProgram_cnt = SD.Info.fsize / PAGE_SIZE;
			SD.uProgram_tail = SD.Info.fsize % PAGE_SIZE;
			if(SD.uProgram_tail)
			{
				SD.uProgram_cnt++;
			}
			dprintf("\r\n File Name : %s", SD.Info.fname);
			dprintf("\r\n File Size : S-%d, C-%d, T-%d", SD.Info.fsize, SD.uProgram_cnt, SD.uProgram_tail);
			break;
		}
		else
		{
			SD.Check.Bit.bFileFind_err = TRUE;
			dprintf("\r\n %d, %s", FInfo.fsize, FInfo.fname);
		}
	}
	SD.Check.Bit.bComplet = TRUE;
	SD.uPage_cnt = 0;
}

void put_rc(FRESULT rc)
{  
    const char *str =
        "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
    int i;

    for (i = 0; i != rc && *str; i++) 
    {
        while (*str++) ;
    }

    dprintf("\r\n FOpen Dir rc=%u FR_%s", (UINT)rc, str);
}

void FilePageLoad(char* cFstr, char* cBuf, U32 ulSize, U32 ulOffset)
{
  	UINT tmp;
		
	memset(cBuf, NULL, PAGE_SIZE);
		
	if (f_open(&SD.File, cFstr, FA_READ) == FR_OK) {
	  	SD.Check.Bit.bFileOpen_err = FALSE;
	  	f_lseek(&SD.File, ulOffset);
		if(f_read(&SD.File, cBuf, ulSize, &tmp) != FR_OK){
		  	SD.Check.Bit.bFileRead_err = TRUE;
			dprintf("\r\n File Read Fail");
		}
		else SD.Check.Bit.bFileRead_err = FALSE;
	}
	else {
	  	SD.Check.Bit.bFileOpen_err = TRUE;
	  	dprintf("\r\n File Open Fail");
	}
	f_close(&SD.File);

#ifdef _FLASH_CHECK
	SD.uData_cnt = 0;
	printf("\r\n add = 0x%08x Page No. = %d", MAIN_ADDR + ulOffset, SD.uPage_cnt);
	while(SD.uData_cnt < ulSize) {
		if((SD.uData_cnt & 0x0F) == 0)printf("\r\n");
		printf(" %02x", cBuf[SD.uData_cnt]);
		SD.uData_cnt++;
	}
	SD.uPage_cnt++;
#endif
}
			 

