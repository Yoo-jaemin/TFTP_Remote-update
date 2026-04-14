#define __SENSOR_UPLOAD_C__
    #include "Sensor_Upload.h"
#undef  __SENSOR_UPLOAD_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "SD_Card.h"
#include "usart.h"
#include "Task.h"
#include "CRC.h"
#include "Display.h"
#include "gpio.h"

#include "gdi.h"
#include "nand_flash.h"

#include "modbus_rtu_master.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool Address_Check(U32 dwCmd_add, U32 dwRead_add);
static void Receive_Check(U8 *pPacket_buff, U16 uPacket_size);
static void Upload_Fault_Check(void);
static void Timeout_Check(void);
static void Send_Packet(Send_HandleType* SendData);
static void SensorUpload_Verify(char* cFilebuf, U8* ucSensorData, U16 uSize);
static void SensorAppStart_Receive(SENSOR_TYPE nType);
static void SensorVersion_Receive(SENSOR_TYPE nType);
static void boot_Jump_check(void);
/* End Function */

/* Start Variable */
extern s_NandFlash Nand_flash;
extern u_Nand_Page nand;
extern MODBUS_MasterHandleType modbus;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Rx_Handler(void)
{
 	U8	ucHp, ucTp;
		
	ucHp = Packet_Handle.ucHp & USART3_RX_FIFO_MASK;
	ucTp = Packet_Handle.ucTp & USART3_RX_FIFO_MASK;
	
	Timeout_Check();
	
	if(Packet_Handle.Flag.Bit.bTimeout == TRUE){
		Packet_Handle.uCount = 0;
		Packet_Handle.Flag.Bit.bSTX = FALSE;
	}
	else {
		if( ucHp != ucTp ){
	  		Packet_Handle.ucTp++;
#ifdef _PACKET_CHECK
			printf( "\r\n Rx : " );
			for(U16 uCnt=0; uCnt<Packet_Handle.uReadSize[ucTp]; uCnt++ ){
		  		printf( " %02x", Packet_Handle.ucRxBuf[ucTp][uCnt] );
				if((uCnt & 0x0F) == 0x0F) printf( "\r\n    : " );
        	}
#endif
			Receive_Check(Packet_Handle.ucRxBuf[ucTp], Packet_Handle.uReadSize[ucTp]);	
		}
	}
}

void Sensor_Upload(SENSOR_TYPE Sensor_Type)
{
  	char cBuff[PAGE_SIZE];
	static U16 uCounting = 0;
    U32 dwStartAdd = 0;
	
    if(Sensor_Type == nCAVITY) dwStartAdd = IR_START_ADDR;
	else if(Sensor_Type == nCARTRIDGE) dwStartAdd = CT_START_ADDR;
    else {
        Packet_Handle.Flag.Bit.bError = TRUE;
        dprintf("\r\n Upload Start Sensor Type Error");
    }
    
	Upload_Fault_Check();
	
  	switch(Upload_State)
	{
		case nSTATE_IDLE:
		  	Reset_Status();
		  	break;
		case nSTATE_STOP:
		  	if(SensorUpload_Handle.Flag.Bit.bError == TRUE) {
		  		SensorUpload_Handle.Flag.Bit.bError = FALSE;
				SensorUpload_Handle.Send_Buff.ucRequstCID = nERASE_START;
				SensorUpload_Handle.Send_Buff.dwStartAdd = dwStartAdd;
				SensorUpload_Handle.Send_Buff.uDataSize = 0;
				Send_Packet(&SensorUpload_Handle.Send_Buff);
			}
		  	break;
		case nSTATE_FILE_CHECK:
            //dprintf("\r\n Sensor Upload Handle Status %x",  SensorUpload_Handle.Flag.All); 
		  	if(SensorUpload_Handle.Flag.Bit.bReady == TRUE) {
		  		Download_File_Find();
			  	Packet_Handle.Flag.Bit.bAck = FALSE;
				uCounting = 0;
				ucDisplay_Progress = 0;
				if((SD.Check.All & 0x1F) == FALSE) {
					SensorUpload_Handle.Send_Buff.ucRequstCID = nERASE_START;
					SensorUpload_Handle.Send_Buff.dwStartAdd = dwStartAdd;
					SensorUpload_Handle.Send_Buff.uDataSize = 0;
					Send_Packet(&SensorUpload_Handle.Send_Buff);
				}
			}
		  	break;
		case nSTATE_FLASH_ERASE:
		  	if(SensorUpload_Handle.Flag.Bit.bError == FALSE){
			  	uCounting++; //Progress Show
				if(ucDisplay_Progress < 99) ucDisplay_Progress = uCounting / 6;
				if(Packet_Handle.Flag.Bit.bAck == TRUE) {
					SensorUpload_Handle.Flag.Bit.bComplete = TRUE;
					uCounting = 0;
			  	}
		  	}
		  	break;
		case nSTATE_FLASH_UPLOAD:
		  	if(SensorUpload_Handle.Flag.Bit.bError == FALSE){
			  	if(Packet_Handle.Flag.Bit.bAck == TRUE){
		  			UploadCount.ulFileRead_offset = UploadCount.ulWrite * PAGE_SIZE;
					SensorUpload_Handle.Send_Buff.ucRequstCID = nWRITE_START;
					SensorUpload_Handle.Send_Buff.dwStartAdd = UploadCount.ulFileRead_offset + dwStartAdd;
					if (UploadCount.ulWrite == SD.uProgram_cnt - 1){
						FilePageLoad(SD.Info.fname, cBuff, SD.uProgram_tail, UploadCount.ulFileRead_offset);
						memcpy(SensorUpload_Handle.Send_Buff.ucData, cBuff, SD.uProgram_tail);
						SensorUpload_Handle.Send_Buff.uDataSize = SD.uProgram_tail;
						Send_Packet(&SensorUpload_Handle.Send_Buff);
						ucDisplay_Progress = ((UploadCount.ulWrite+1)*100)/SD.uProgram_cnt;
					}
					else {
						FilePageLoad(SD.Info.fname, cBuff, PAGE_SIZE, UploadCount.ulFileRead_offset);
						memcpy(SensorUpload_Handle.Send_Buff.ucData, cBuff, PAGE_SIZE);
						SensorUpload_Handle.Send_Buff.uDataSize = PAGE_SIZE;
						Send_Packet(&SensorUpload_Handle.Send_Buff);
						ucDisplay_Progress = (UploadCount.ulWrite*100)/SD.uProgram_cnt;
					}
			  		UploadCount.ulWrite++;
					Packet_Handle.Flag.Bit.bAck = FALSE;
				}
			}
		  	break;
		case nSTATE_FLASH_VERIFY:
		  	if(SensorUpload_Handle.Flag.Bit.bError == FALSE){
				if(Packet_Handle.Flag.Bit.bAck == TRUE) {
				  	if(SensorUpload_Handle.ucResponse_CID == nREAD_FLASH){
						if(UploadCount.ulVerify == SD.uProgram_cnt - 1){
							FilePageLoad(SD.Info.fname, cBuff, SD.uProgram_tail, UploadCount.ulFileRead_offset);
							SensorUpload_Verify(cBuff, SensorUpload_Handle.ucSensor_data, SD.uProgram_tail);
							ucDisplay_Progress = ((UploadCount.ulVerify+1)*100)/SD.uProgram_cnt;
						}
						else {
							FilePageLoad(SD.Info.fname, cBuff, PAGE_SIZE, UploadCount.ulFileRead_offset);
							SensorUpload_Verify(cBuff, SensorUpload_Handle.ucSensor_data, PAGE_SIZE);
							ucDisplay_Progress = (UploadCount.ulVerify*100)/SD.uProgram_cnt;
						}
						UploadCount.ulVerify++;
					}
					if(UploadCount.ulVerify != SD.uProgram_cnt) {
						UploadCount.ulFileRead_offset = UploadCount.ulVerify * PAGE_SIZE;
				  		SensorUpload_Handle.Send_Buff.ucRequstCID = nREAD_FLASH;
						SensorUpload_Handle.Send_Buff.dwStartAdd = UploadCount.ulFileRead_offset + dwStartAdd;
						if(UploadCount.ulVerify == SD.uProgram_cnt - 1) SensorUpload_Handle.Send_Buff.uDataSize = SD.uProgram_tail;
						else SensorUpload_Handle.Send_Buff.uDataSize = PAGE_SIZE;
						Send_Packet(&SensorUpload_Handle.Send_Buff);
					}
					Packet_Handle.Flag.Bit.bAck = FALSE;
				}
			}
			break;
		case nSTATE_FINISH:
		  	Display.Bit.bProgress = FALSE;
		  	break;
		default:	break;
	}
}



void Sensor_UploadStart(SENSOR_TYPE Sensor_Type)
{
    printf(" Sensor_UploadStart call ~ \r\n");
    SensorUpload_Handle.Send_Buff.ucRequstCID = nBOOT_MODE;
    
    if (Sensor_Type == nCAVITY) SensorUpload_Handle.Send_Buff.dwStartAdd = IR_START_ADDR;
    else if (Sensor_Type == nCARTRIDGE) SensorUpload_Handle.Send_Buff.dwStartAdd = CT_START_ADDR;
    else dprintf("\r\n Upload Start Sensor Type Error");
	
    SensorUpload_Handle.Send_Buff.uDataSize = 0;
    Send_Packet(&SensorUpload_Handle.Send_Buff);
}


void Sensor_Command(SENSOR_CID nCID)
{
      SENSOR_TYPE nType = nCAVITY;	
  
      while(nType < nTYPE_MAX) 
      {
          switch(nType)
          {
              case nCARTRIDGE: 
                    CT_UPLOAD_ENABLE(); 
                    SensorUpload_Handle.Send_Buff.dwStartAdd = CT_START_ADDR;
                    break;
              case nCAVITY: 
                    IR_UPLOAD_ENABLE(); 
                    SensorUpload_Handle.Send_Buff.dwStartAdd = IR_START_ADDR;
                    break;
              default: dprintf("\r\n Sensor Type Error"); break;
          }
      
          SensorUpload_Handle.Send_Buff.ucRequstCID = nCID;
          SensorUpload_Handle.Send_Buff.uDataSize = 0;
          Send_Packet(&SensorUpload_Handle.Send_Buff);
              
          Packet_Handle.Flag.Bit.bReqStart = FALSE;
          Packet_Handle.uTimeout = 0;
      
          if (TaskStatus.Bit.bDir_Start != TRUE) SensorUpload_Handle.Flag.Bit.bSensorCheck = TRUE;
          else SensorUpload_Handle.Flag.Bit.bSensorCheck = FALSE;
                      
          while((SensorUpload_Handle.uSensorCheck_Time < SENSOR_REQ_TIME) && (SensorUpload_Handle.Flag.Bit.bSensorCheck == TRUE)) 
          {
              Rx_Handler();
              if (nCID == nSENSORCHECK) SensorVersion_Receive(nType);
              else if (nCID == nMAIN_START) SensorAppStart_Receive(nType);
              else SensorUpload_Handle.Flag.Bit.bSensorCheck = FALSE;
    
              if(SensorUpload_Handle.uSensorCheck_Time >= SENSOR_REQ_TIME) 
              {
                    if(nType == nCARTRIDGE) dprintf("\r\n Cartridge Disable");
                    else dprintf("\r\n Cavity Disable");
                    SensorUpload_Handle.Flag.Bit.bSensorCheck = FALSE;
              }
          } //end while   
              
          nType++;
          SensorUpload_Handle.uSensorCheck_Time = 0;
      } // end while
      
      SensorUpload_Handle.Flag.Bit.bSensorCheck = FALSE;
      SensorUpload_Handle.uSensorCheck_Time = 0;
}

static void SensorVersion_Receive(SENSOR_TYPE nType)
{
    if (Packet_Handle.Flag.Bit.bAck == TRUE) 
    {
        switch(nType)
        {	
            case nCARTRIDGE : 
                  SensorUpload_Handle.uCatridgeFW_ver = 0;
                  SensorUpload_Handle.uCatridgeFW_ver = (SensorUpload_Handle.ucSensor_data[1] << 8);
                  SensorUpload_Handle.uCatridgeFW_ver |= SensorUpload_Handle.ucSensor_data[2];
                  TaskStatus.Bit.bCT_Check = TRUE;
                  printf("\r\n CARTRIDGE F/W VERSION: %1.1f", ((float)SensorUpload_Handle.uCatridgeFW_ver / 10));
                  break;
            case nCAVITY: 
                  SensorUpload_Handle.uCavityFW_ver = 0;
                  SensorUpload_Handle.uCavityFW_ver = (SensorUpload_Handle.ucSensor_data[1] << 8);
                  SensorUpload_Handle.uCavityFW_ver |= SensorUpload_Handle.ucSensor_data[2];
                  TaskStatus.Bit.bIR_Check = TRUE;
                  printf("\r\n CAVITY F/W VERSION: %1.1f", ((float)SensorUpload_Handle.uCavityFW_ver / 10));
                  break;
            default: dprintf("\r\n Sensor Type Error"); break;
          }
          SensorUpload_Handle.Flag.Bit.bSensorCheck = FALSE;
          Packet_Handle.Flag.Bit.bAck = FALSE;
     }
}

static void SensorAppStart_Receive(SENSOR_TYPE nType)
{
    if (Packet_Handle.Flag.Bit.bAck == TRUE) 
    {
        switch(nType)
        {	
            case nCARTRIDGE: 
                TaskStatus.Bit.bCT_Start = TRUE;
                dprintf("\r\n Cartridge Enable");
                break;
            case nCAVITY: 
                TaskStatus.Bit.bIR_Start = TRUE;
                dprintf("\r\n Cavity Enable");
                break;
            default: dprintf("\r\n Sensor Type Error"); break;
	}
        
        SensorUpload_Handle.Flag.Bit.bSensorCheck = FALSE;
        Packet_Handle.Flag.Bit.bAck = FALSE;
    }
}

static bool Address_Check(U32 dwCmd_add, U32 dwRead_add)
{
  	bool bErr_flag = FALSE;
	
	if(dwCmd_add != dwRead_add) bErr_flag = TRUE;
	else bErr_flag = FALSE;
	
	return bErr_flag;
}

static void SensorUpload_Verify(char* cFilebuf, U8* ucSensorData, U16 uSize)
{	
  	int iErrFlag = FALSE;
	
  	iErrFlag = memcmp(cFilebuf, (const void*)ucSensorData, uSize);
	
	if(iErrFlag != FALSE) {
	  	SensorUpload_Handle.Flag.Bit.bError = TRUE;
		dprintf("\r\n Verify Error - %d", UploadCount.ulVerify);
	}
	//else SensorUpload_Handle.Flag.Bit.bError = FALSE;
}

static void Receive_Check(U8 *pPacket_buff, U16 uPacket_size)
{
  	bool bAddErr_flag = FALSE;
  	U16 uData_cnt = 0;
		
	SensorUpload_Handle.Flag.Bit.bCRC_Err = FALSE;
	SensorUpload_Handle.dwCalCRC = CRC_Calculate(pPacket_buff, uPacket_size-5);
	
	SensorUpload_Handle.dwReadCRC = 0;
	SensorUpload_Handle.dwReadCRC = (U32)(*(pPacket_buff + (uPacket_size - 5))) << 24;
	SensorUpload_Handle.dwReadCRC |= (U32)(*(pPacket_buff + (uPacket_size - 4))) << 16;
	SensorUpload_Handle.dwReadCRC |= (U32)(*(pPacket_buff + (uPacket_size - 3))) << 8;
	SensorUpload_Handle.dwReadCRC |= (U32)*(pPacket_buff + (uPacket_size - 2));
	
	if(SensorUpload_Handle.dwCalCRC != SensorUpload_Handle.dwReadCRC) {
		dprintf("\r\n CRC Error : Cal - %08X, Read - %08X", SensorUpload_Handle.dwCalCRC, SensorUpload_Handle.dwReadCRC);
		SensorUpload_Handle.Flag.Bit.bCRC_Err = TRUE;
	}
		
	if(SensorUpload_Handle.Flag.Bit.bCRC_Err == FALSE) {
	  	Packet_Handle.uTimeout = 0;
		Packet_Handle.Flag.Bit.bReqStart = FALSE;
				
		SensorUpload_Handle.ucResponse_CID = *(pPacket_buff + 1);
        
                if (TaskStatus.Bit.bCT_Upload) SensorUpload_Handle.dwRead_add = CT_START_ADDR;
                else if (TaskStatus.Bit.bIR_Upload) SensorUpload_Handle.dwRead_add = IR_START_ADDR;
                else SensorUpload_Handle.dwRead_add = SensorUpload_Handle.Send_Buff.dwStartAdd;
        
                SensorUpload_Handle.dwRead_add += (U32)(*(pPacket_buff + 4) << 24);
		SensorUpload_Handle.dwRead_add += (U32)(*(pPacket_buff + 5) << 16);
	  	SensorUpload_Handle.dwRead_add += (U32)(*(pPacket_buff + 6) << 8);
		SensorUpload_Handle.dwRead_add += (U32)*(pPacket_buff + 7);
        		
		bAddErr_flag = Address_Check(SensorUpload_Handle.Send_Buff.dwStartAdd, SensorUpload_Handle.dwRead_add);
		
		if(bAddErr_flag == TRUE) {
		 	Packet_Handle.Flag.Bit.bError = TRUE;
			dprintf("\r\n Add Error : Send - %08X, Read - %08X", SensorUpload_Handle.Send_Buff.dwStartAdd, SensorUpload_Handle.dwRead_add);
		}
		else {
                        dprintf("\r\n Add OK : Send - %08X, Read - %08X", SensorUpload_Handle.Send_Buff.dwStartAdd, SensorUpload_Handle.dwRead_add);
            
			SensorUpload_Handle.uDataLen = (*(pPacket_buff + 8) << 8);
			SensorUpload_Handle.uDataLen |= *(pPacket_buff + 9);
		
			while(uData_cnt < SensorUpload_Handle.uDataLen) {
				SensorUpload_Handle.ucSensor_data[uData_cnt] = *(pPacket_buff + uData_cnt + 10);
				uData_cnt++;
			}
			
			if(SensorUpload_Handle.ucResponse_CID == nREAD_FLASH) 
                            Packet_Handle.Flag.Bit.bAck = TRUE;
			else {
		  		if(SensorUpload_Handle.ucResponse_CID == nBOOT_MODE) SensorUpload_Handle.Flag.Bit.bReady = TRUE;
		   		
                                if(SensorUpload_Handle.ucSensor_data[0] == nACK) {
                                      Packet_Handle.Flag.Bit.bAck = TRUE;
                                      Packet_Handle.Flag.Bit.bNak = FALSE;
                                } else if(SensorUpload_Handle.ucSensor_data[0] == nNAK) {
                                      Packet_Handle.Flag.Bit.bAck = FALSE;
                                      Packet_Handle.Flag.Bit.bNak = TRUE;
				}
				else {
                                      Packet_Handle.Flag.Bit.bError = TRUE;
                                      Packet_Handle.Flag.Bit.bAck = FALSE;
                                      Packet_Handle.Flag.Bit.bNak = FALSE;
				}
			}
		}		
	}
	else Packet_Handle.Flag.Bit.bError = TRUE;
}

static void Send_Packet(Send_HandleType *SendData)
{
  	U16	uPacket_cnt = 0;
	U16 uRead_cnt = 0;
	
	Packet_Handle.ucTxBuf[uPacket_cnt++] = STX;
	Packet_Handle.ucTxBuf[uPacket_cnt++] = SendData->ucRequstCID;
	uPacket_cnt += 2;
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8) (SendData->dwStartAdd >> 24);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8) (SendData->dwStartAdd >> 16);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8) (SendData->dwStartAdd >> 8);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8) SendData->dwStartAdd;
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8) (SendData->uDataSize >> 8);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8) SendData->uDataSize;
	
	if(Packet_Handle.ucTxBuf[1] == nWRITE_START){
		while(uRead_cnt < SendData->uDataSize) {
			Packet_Handle.ucTxBuf[uPacket_cnt++] = SendData->ucData[uRead_cnt];
			uRead_cnt++;
		}
	}
			
	Packet_Handle.ucTxBuf[uPacket_cnt++] = ETX;
	
	Packet_Handle.ucTxBuf[2] = (U8) (uPacket_cnt >> 8);
	Packet_Handle.ucTxBuf[3] = (U8) uPacket_cnt;
		
	Packet_Handle.dwCalCRC = CRC_Calculate(Packet_Handle.ucTxBuf, uPacket_cnt);
	
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8)(Packet_Handle.dwCalCRC >> 24);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8)(Packet_Handle.dwCalCRC >> 16);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8)(Packet_Handle.dwCalCRC >> 8);
	Packet_Handle.ucTxBuf[uPacket_cnt++] = (U8)Packet_Handle.dwCalCRC;
	
	Packet_Handle.ucTxBuf[uPacket_cnt++] = EOT;
	
#ifdef _PACKET_CHECK
	printf("\r\n Tx : ");
	for(U16 uCnt=0; uCnt < uPacket_cnt; uCnt++)
	{
	  	printf(" %02x",Packet_Handle.ucTxBuf[uCnt]);
		if((uCnt & 0x0F) == 0x0F) printf("\r\n    : ");
	}
#endif
	
	Packet_Handle.Flag.Bit.bReqStart = TRUE;
	
	if(Packet_Handle.ucTxBuf[1] == nMAIN_START) USART3_Packet_Send(Packet_Handle.ucTxBuf, uPacket_cnt);
	else USART3_Packet_Send_IT(Packet_Handle.ucTxBuf, uPacket_cnt);
}


static void Upload_Fault_Check(void)
{
	if((Packet_Handle.Flag.Bit.bTimeout == TRUE) || (Packet_Handle.Flag.Bit.bError == TRUE) 
	    || (Packet_Handle.Flag.Bit.bNak == TRUE)) {
	  	if(Packet_Handle.Flag.Bit.bTimeout == TRUE) Show_Display(nRESPONSE_ERR);
	  	Packet_Handle.Flag.Bit.bError = FALSE;
	  	Packet_Handle.Flag.Bit.bTimeout = FALSE;
		Packet_Handle.uTimeout = 0;
		SensorUpload_Handle.Flag.Bit.bError = TRUE;
		SensorUpload_Handle.Flag.Bit.bReady = FALSE;
	}
}

static void Timeout_Check(void)
{
  	if(Packet_Handle.Flag.Bit.bReqStart == TRUE) {
  		if(Packet_Handle.uTimeout > PACKET_TIMEOUT) {
	  		Packet_Handle.Flag.Bit.bReqStart = FALSE;
			Packet_Handle.uTimeout = 0;
		  	Packet_Handle.Flag.Bit.bTimeout = TRUE;
			dprintf("\r\n Response Timeout");
		}
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////////

// jmyoo 
void Remote_Sensor_Upload(SENSOR_TYPE Sensor_Type)
{
    //printf("\r\n Remote_Sensor_Upload() ! \r\n");
    static U16 uCounting = 0;
    U32 dwStartAdd = 0;
    
    if (Sensor_Type == nCAVITY) 
        dwStartAdd = IR_START_ADDR;
    else if (Sensor_Type == nCARTRIDGE) 
        dwStartAdd = CT_START_ADDR;
    else {
        Packet_Handle.Flag.Bit.bError = TRUE;
        dprintf("\r\n Upload Start Sensor Type Error");
    }
    
    Upload_Fault_Check();
    
    switch (Upload_State)
    {
      case nSTATE_IDLE:
           Remote_Reset_Status();
           boot_Jump_check();
      case nSTATE_STOP :
           //printf(" Upload_State : nSTATE_STOP\r\n");
           break;
      case nSTATE_FILE_CHECK :
           //printf(" Upload_State : nSTATE_FILE_CHECK \r\n");
           if(SensorUpload_Handle.Flag.Bit.bReady == TRUE) {
              GDI_SetTextColor(WHITE);
              GDI_StringColor (110, 136, GDI_STR_ALIGN_C, WHITE, "FILE CHECK......  ");
              
              Remote_Download_File_Find();
              Packet_Handle.Flag.Bit.bAck = FALSE;
              uCounting = 0;
              ucDisplay_Progress = 0;
              if (Nand_flash.bFile_chk == true) {
                  SensorUpload_Handle.Send_Buff.ucRequstCID = nERASE_START;
                  SensorUpload_Handle.Send_Buff.dwStartAdd = dwStartAdd;
                  SensorUpload_Handle.Send_Buff.uDataSize = 0;
                  Send_Packet(&SensorUpload_Handle.Send_Buff);
              }
           } break;
      case nSTATE_FLASH_ERASE :
           //printf(" Upload_State : nSTATE_FLASH_ERASE \r\n");
           if(SensorUpload_Handle.Flag.Bit.bError == FALSE) {
              uCounting++; 
              if (ucDisplay_Progress < 99) ucDisplay_Progress = uCounting / 6;
              if (Packet_Handle.Flag.Bit.bAck == TRUE) {
                  SensorUpload_Handle.Flag.Bit.bComplete = TRUE;
                  uCounting = 0;
              }
            } break;
      case nSTATE_FLASH_UPLOAD :   
           if ( (SensorUpload_Handle.Flag.Bit.bError == FALSE) && (Packet_Handle.Flag.Bit.bAck == TRUE) ) {
              
              UploadCount.ulFileRead_offset = UploadCount.ulWrite * PAGE_SIZE;
              SensorUpload_Handle.Send_Buff.ucRequstCID = nWRITE_START;
              SensorUpload_Handle.Send_Buff.dwStartAdd = UploadCount.ulFileRead_offset + dwStartAdd;
              
              printf("\r\n\r\n ======================================================= "); 
              printf("\r\n UploadCount.ulWrite : %d | Nand_flash.program_cnt : %d \r\n", UploadCount.ulWrite, Nand_flash.program_cnt); 
              
              if (UploadCount.ulWrite == Nand_flash.program_cnt -1) {
                  Remote_FilePageLoad();
                  memcpy(SensorUpload_Handle.Send_Buff.ucData, nand.byte, Nand_flash.program_tail);
                  SensorUpload_Handle.Send_Buff.uDataSize = Nand_flash.program_tail;
                  Send_Packet(&SensorUpload_Handle.Send_Buff);
                  ucDisplay_Progress = (UploadCount.ulWrite*100) / Nand_flash.program_cnt;
              } else {
                  Remote_FilePageLoad();
                  memcpy(SensorUpload_Handle.Send_Buff.ucData, nand.byte, PAGE_SIZE);
                  SensorUpload_Handle.Send_Buff.uDataSize = PAGE_SIZE;
                  Send_Packet(&SensorUpload_Handle.Send_Buff);
                  ucDisplay_Progress = (UploadCount.ulWrite*100) / Nand_flash.program_cnt;
              }
              if (HAL_NAND_Address_Inc(&NandBank3_Handle, &NandAddr)!= NAND_VALID_ADDRESS) 
                  printf("\r\n Addr Inc Err\r\n");
              
              UploadCount.ulWrite++;
              Packet_Handle.Flag.Bit.bAck = FALSE;
           } break;
      case nSTATE_FLASH_VERIFY :
            //printf(" Upload_State : nSTATE_FLASH_VERIFY \r\n");
            if ( (SensorUpload_Handle.Flag.Bit.bError == FALSE) && (Packet_Handle.Flag.Bit.bAck == TRUE) ) {
                if (SensorUpload_Handle.ucResponse_CID == nREAD_FLASH) {                
                    if (UploadCount.ulVerify == Nand_flash.program_cnt - 1) {
                        Remote_FilePageLoad();
                        SensorUpload_Verify((char*) nand.byte, SensorUpload_Handle.ucSensor_data, Nand_flash.program_tail);
                        ucDisplay_Progress = ((UploadCount.ulVerify+1)*100)/Nand_flash.program_cnt;
                    } else {
                        Remote_FilePageLoad();
                        SensorUpload_Verify((char*) nand.byte, SensorUpload_Handle.ucSensor_data, PAGE_SIZE);
                        ucDisplay_Progress = ((UploadCount.ulVerify+1)*100)/Nand_flash.program_cnt;
                    }
                    if (HAL_NAND_Address_Inc(&NandBank3_Handle, &NandAddr)!= NAND_VALID_ADDRESS) 
                      printf("\r\n Addr Inc Err\r\n");
                    
                    UploadCount.ulVerify++;
                }
              
                if ( UploadCount.ulVerify != Nand_flash.program_cnt ) {
                    printf("\r\n\r\n ======================================================= "); 
                    printf("\r\n UploadCount.ulVerify : %d | Nand_flash.program_cnt : %d \r\n", UploadCount.ulVerify, Nand_flash.program_cnt); 
                    
                    UploadCount.ulFileRead_offset = UploadCount.ulVerify * PAGE_SIZE;
                    SensorUpload_Handle.Send_Buff.ucRequstCID = nREAD_FLASH;
                    SensorUpload_Handle.Send_Buff.dwStartAdd = UploadCount.ulFileRead_offset + dwStartAdd;
                    
                    if (UploadCount.ulVerify == Nand_flash.program_cnt - 1) 
                         SensorUpload_Handle.Send_Buff.uDataSize = Nand_flash.program_tail;
                    else 
                         SensorUpload_Handle.Send_Buff.uDataSize = PAGE_SIZE;
                    Send_Packet(&SensorUpload_Handle.Send_Buff);
                }
              
                Packet_Handle.Flag.Bit.bAck = FALSE;
            } break;
      case nSTATE_FINISH :
         Display.Bit.bProgress = FALSE;
         break;  
    } // end switch
}// end func

static void boot_Jump_check(void)
{
    //printf("\r\n boot_Jump_check Func! \r\n");    
    NandAddr.Page  = 0;         
    NandAddr.Block = 14;
    NandAddr.Plane = 0;
    HAL_NAND_Read_Page_8b(&NandBank3_Handle, &NandAddr, nand.byte, 1);
    
    if (nand.byte[2]) {
        printf("\r\n IR bootJump_ ! \r\n");
        IR_UPLOAD_ENABLE();
        Cavity_BootJump_Write_Packet_Send( &modbus );
        HAL_Delay(80);
        HAL_NVIC_SystemReset();
    }
}


