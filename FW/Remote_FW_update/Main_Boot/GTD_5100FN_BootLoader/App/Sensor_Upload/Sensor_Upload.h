#ifndef   __SENSOR_UPLOAD_H__
#define   __SENSOR_UPLOAD_H__

#ifdef __SENSOR_UPLOAD_C__
	#define SENSOR_UPLOAD_EXT
#else
	#define SENSOR_UPLOAD_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
#define SENSOR_STARTADD 0x08010000U

#define USART3_RX_FIFO_SIZE			2
#define USART3_RX_FIFO_MASK			(USART3_RX_FIFO_SIZE-1)
#define PACKET_BUFF_SIZE			0x500

#define STX 0x02
#define ETX 0x03
#define EOT 0x04

#define BOOT                    0x00
#define MAIN_START              0x01
#define SENSOR_CHECK_REQUEST    0X02
#define SENSOR_START_REQUEST    0x03

#define CRC_SIZE 4
#define PACKET_TIMEOUT 5000 //ms
#define SENSOR_REQ_TIME 500
/* End Define */

/* Start Enum */
typedef enum
{
    nSENSORCHECK,
    nBOOT_MODE,
    nERASE_START,
    nWRITE_START,
    nREAD_FLASH,
    nMAIN_START,
    nACK = 6,
    nNAK = 15,
    nMAX
} SENSOR_CID;

typedef enum
{
    nCAVITY,
    nCARTRIDGE,
    nTYPE_MAX
}SENSOR_TYPE;
/* End Enum */

/* Start Struct */
typedef union{
  	struct {
	  	bool	bComplete		:1;
		bool	bError			:1;
		bool	bCRC_Err		:1;
		bool	bEmpty3			:1;
		bool	bSensorCheck	:1;
		bool	bBoot			:1;
		bool	bReady			:1;
		bool	bEmpty7			:1;
	}Bit;
	U8	All;
}ProgressFlag;

typedef union{
  	struct {
	  	bool	bSTX		:1;
		bool	bETX		:1;
		bool	bAck		:1;
		bool	bNak		:1;
		bool	bEOT		:1;
		bool	bError		:1;
		bool	bReqStart	:1;
		bool	bTimeout	:1;
	}Bit;
	U8	All;
}PacketFlag;


typedef struct{
  	PacketFlag	        Flag;
	U8			ucCRC_cnt;
	U8			ucHp;
	U8			ucTp;
	U8			ucBuff;
	U8			ucTxBuf[PACKET_BUFF_SIZE];
	U8			ucRxBuf[USART3_RX_FIFO_SIZE][PACKET_BUFF_SIZE];
  	U16			uReadSize[USART3_RX_FIFO_SIZE];
	U16			uLen;
	U16			uDataSize[USART3_RX_FIFO_SIZE];
	U16 		        uCount;
	U16			uTimeout;
	U32			dwCalCRC;
}Packet_HandleType;

typedef struct{
  	SENSOR_CID		ucRequstCID;
	U8				ucData[PAGE_SIZE];
	U16				uDataSize;
	U32				dwStartAdd;
}Send_HandleType;

typedef struct{
  	ProgressFlag	Flag;
	Send_HandleType Send_Buff;
	U8				ucResponse_CID;
	U8				ucSensor_data[PAGE_SIZE];
	U16				uSensorCheck_Time;
	U16				uCavityFW_ver;
	U16				uCatridgeFW_ver;
	U16				uDataLen;
	U32				dwRead_add;
	U32 			dwReadCRC;
	U32 			dwCalCRC;
}Upload_HandleType;




/* End Struct */

/* Start Function */
SENSOR_UPLOAD_EXT void Sensor_UploadStart(SENSOR_TYPE Sensor_Type);
//SENSOR_UPLOAD_EXT void Sensor_VersionCheck(void);
SENSOR_UPLOAD_EXT void Sensor_Command(SENSOR_CID nCID);
SENSOR_UPLOAD_EXT void Rx_Handler(void);
SENSOR_UPLOAD_EXT void Sensor_Upload(SENSOR_TYPE Sensor_Type);

SENSOR_UPLOAD_EXT void Remote_Sensor_Upload(SENSOR_TYPE Sensor_Type);

/* End Function */

/* Start Variable */
SENSOR_UPLOAD_EXT Packet_HandleType Packet_Handle;
SENSOR_UPLOAD_EXT Upload_HandleType SensorUpload_Handle;
/* End Variable */


#endif // __SENSOR_UPLOAD_H__