#define __CARTRIDGE_C__
    #include "cartridge.h"
#undef  __CARTRIDGE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "sensor_list.h"
#include "usart.h"
#include "btimer.h"
#include "measuring.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static MODBUS_MasterStatusType Wait_For_Cartridge_Response( MODBUS_MasterHandleType *pModbus );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Config( MODBUS_MasterHandleType *pModbus )
{
	U8		i;
	U8		RetryCnt = 1;
	U16		BuffSize;
	U16		Period;
	bool	err;
	
	if( (CavityHandle.IsPresent == true) && (CavityHandle.Inf[CAVITY_INF_SRC_FREQ] == 8) ){
		BuffSize = 100;
		Period = 125;
	}
	else {
		BuffSize = 90;
		Period = 100;
	}
	
	CartridgeHandle.Inf[CARTRIDGE_INF_CHANNELS] = 0;
	
	pModbus->SlaveAddress = CARTRIDGE_MODBUS_ADDRESS;
	
	while( RetryCnt ){
		
		RetryCnt--;
		err = false;
		
		for( i=CARTRIDGE_EEP_BLK_SYS; i<CARTRIDGE_EEP_BLK_MAX; i++ ){
			
			Cartridge_Eep_Read_Packet_Send( pModbus, i, 0, SENSOR_EEP_BLK_SIZE );
			if( Wait_For_Cartridge_Response( pModbus ) == MODBUS_RTU_MASTER_OK ){
				
				if( ((i == CARTRIDGE_EEP_BLK_SYS) && (CartridgeHandle.Eep[i][SENSOR_EEP_SYS_ADC_BUF_SIZE] != BuffSize)) ||
					((i == CARTRIDGE_EEP_BLK_SYS) && (CartridgeHandle.Eep[i][SENSOR_EEP_SYS_SENSING_PERIOD] != Period)) ||
					((i != CARTRIDGE_EEP_BLK_SYS) && (CartridgeHandle.Eep[i][SENSOR_EEP_CHn_PREFIX] != SENSOR_EEP_PREFIX)) ){
					
					Cartridge_Eep_Init_Packet_Send( pModbus, i );
					if( Wait_For_Cartridge_Response( pModbus ) == MODBUS_RTU_MASTER_OK ){
						
						Cartridge_Eep_Read_Packet_Send( pModbus, i, 0, SENSOR_EEP_BLK_SIZE );
						if( Wait_For_Cartridge_Response( pModbus ) != MODBUS_RTU_MASTER_OK ){ // Reload Failure
							err = true;
							break;
						}
					}
					else { // Init Failure
						err = true;
						break;
					}
				}
			}
			else { // Load Failure
				err = true;
				break;
			}
		}
		
		HAL_Delay( 500 );
			
		if( err == false ){
			Cartridge_Info_Read_Packet_Send( pModbus );
			if( Wait_For_Cartridge_Response( pModbus ) == MODBUS_RTU_MASTER_OK ){
				if( CartridgeHandle.Inf[CARTRIDGE_INF_BUF_SIZE] == (BuffSize * 2) ){
					break;
				}
				else {
					err = true;
				}
			}
			else {
				err = true;
			}
		}
	}
	
	if( err == false ){
		CartridgeHandle.IsPresent =  true;
	}
	else {
		CartridgeHandle.IsPresent = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Info_Read_Packet_Send( MODBUS_MasterHandleType *pModbus )
{
	pModbus->Function			= READ_INPUT_REGISTERS;
	pModbus->StartAddress		= 0;
	pModbus->RequestCount		= 9;
	pModbus->pReceiveBuf		= CartridgeHandle.Inf;
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;    
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Data_Read_Packet_Send( MODBUS_MasterHandleType *pModbus )
{
	pModbus->Function			= READ_INPUT_REGISTERS;
	pModbus->StartAddress		= 9;
	pModbus->RequestCount		= 70;
	pModbus->pReceiveBuf		= CartridgeHandle.ReceiveBuf;
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Wave_Read_Packet_Send( MODBUS_MasterHandleType *pModbus )
{
	if( CartridgeHandle.iWaveOut != CartridgeHandle.iWaveSet ){
		CartridgeHandle.iWaveOut  = CartridgeHandle.iWaveSet;
	}
	
	pModbus->Function			= READ_INPUT_REGISTERS;
	pModbus->StartAddress		= CartridgeHandle.iWaveOut * (CartridgeHandle.Inf[CARTRIDGE_INF_BUF_SIZE] / 2 + 1) + 109;
	pModbus->RequestCount		= CartridgeHandle.Inf[CARTRIDGE_INF_BUF_SIZE] / 2 + 1;
	pModbus->pReceiveBuf		= CartridgeHandle.ReceiveBuf;
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Eep_Read_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block, U16 wAddr, U16 wSize )
{     
	pModbus->Function			= READ_HOLDING_REGISTERS;
	pModbus->StartAddress		= SENSOR_EEP_BLK_BASE( Block ) + wAddr;
	pModbus->RequestCount		= wSize;
	pModbus->pReceiveBuf		= &CartridgeHandle.Eep[Block][wAddr];
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;    
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Eep_Write_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block, U16 wAddr, U16 wSize )
{
	pModbus->Function			= WRITE_MULTIPLE_REGISTERS;
	pModbus->StartAddress		= SENSOR_EEP_BLK_BASE( Block ) + wAddr;
	pModbus->RequestCount		= wSize;
	pModbus->ByteCount			= wSize * 2;
	pModbus->pRequestBuf		= &CartridgeHandle.Eep[Block][wAddr];
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;    
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Write_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cartridge_Eep_Init_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block )
{
	U16		wBuf[SENSOR_EEP_BLK_SIZE];
	U8		i;
		
	if( Block == CARTRIDGE_EEP_BLK_SYS ){
		if( (CavityHandle.IsPresent == true) && (CavityHandle.Inf[CAVITY_INF_SRC_FREQ] == 8) ){
			wBuf[0] = 100;
			wBuf[1] = 125;
		}
		else {
			wBuf[0] =  90;
			wBuf[1] = 100;
		}
		
		pModbus->Function		= WRITE_MULTIPLE_REGISTERS;
		pModbus->StartAddress	= SENSOR_EEP_SYS_ADC_BUF_SIZE;
		pModbus->RequestCount	= 2;
		pModbus->ByteCount		= 4;
		pModbus->pRequestBuf	= wBuf;
		pModbus->Status			= MODBUS_RTU_MASTER_BUSY;
		pModbus->TimeoutTick	= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL + 100; // + 4 sec.
	}
	else {
		for( i=0; i<SENSOR_EEP_BLK_SIZE; i++ ){
			wBuf[i] = SENSOR_EEP_CT_CHn_DEFAULT[i];
		}
		
		wBuf[SENSOR_EEP_CHn_SEN_NO] = CartridgeHandle.Eep[CARTRIDGE_EEP_BLK_SYS][SENSOR_EEP_SYS_CTG_SEN_TYPE_0 + Block];
		
		pModbus->Function		= WRITE_MULTIPLE_REGISTERS;
		pModbus->StartAddress	= SENSOR_EEP_BLK_BASE( Block );
		pModbus->RequestCount	= SENSOR_EEP_BLK_SIZE;
		pModbus->ByteCount		= SENSOR_EEP_BLK_SIZE * 2;
		pModbus->pRequestBuf	= wBuf;
		pModbus->Status			= MODBUS_RTU_MASTER_BUSY;
		pModbus->TimeoutTick	= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL + 100; // + 4 sec.
	}
	
	MODBUS_RTU_Write_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Cartridge_Packet_Sequence_Check( MODBUS_MasterHandleType *pModbus )
{
	U32		CurrSeqNo = CartridgeHandle.SequenceNo;
	U16		ReadSeqNo = CartridgeHandle.ReceiveBuf[0];
	bool	ret = false;
	
	if( CurrSeqNo != ReadSeqNo ){
		CurrSeqNo += 3;
		if( CurrSeqNo > ReadSeqNo ){
			ret = true;
		}
		else {
			dprintf( "\n, Cartridge frame is missed! %u, %u", CartridgeHandle.SequenceNo, ReadSeqNo );
		}
		CartridgeHandle.SequenceNo  = ReadSeqNo;
	}
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static MODBUS_MasterStatusType Wait_For_Cartridge_Response( MODBUS_MasterHandleType *pModbus )
{
	U8		timeout = 40;
	
	while( pModbus->Status == MODBUS_RTU_MASTER_BUSY ){
		
		MODBUS_RTU_Rx_Packet_Check_Master( pModbus );
		MODBUS_RTU_Rx_Handler_Master( pModbus );
		
		if( --timeout == 0 ){ timeout = 40, MODBUS_RTU_Timeout_Check_Master( pModbus ); }
		HAL_Delay( 0 );
	}
	
	return pModbus->Status;
}
