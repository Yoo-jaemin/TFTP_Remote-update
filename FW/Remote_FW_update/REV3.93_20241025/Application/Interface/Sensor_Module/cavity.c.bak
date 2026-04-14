#define __CAVITY_C__
    #include "cavity.h"
#undef  __CAVITY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "sensor_list.h"
#include "usart.h"
#include "btimer.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static MODBUS_MasterStatusType Wait_For_Cavity_Response( MODBUS_MasterHandleType *pModbus );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Config( MODBUS_MasterHandleType *pModbus )
{
	U8							i;
	
	CavityHandle.Inf[CAVITY_INF_CHANNELS] = 0;
	
	pModbus->SlaveAddress = CAVITY_MODBUS_ADDRESS;
	
	Cavity_Info_Read_Packet_Send( pModbus );
	if( Wait_For_Cavity_Response( pModbus ) == MODBUS_RTU_MASTER_OK ){
		
		for( i=CAVITY_EEP_BLK_SYS; i<CAVITY_EEP_BLK_MAX; i++ ){
			
			Cavity_Eep_Read_Packet_Send( pModbus, i, 0, SENSOR_EEP_BLK_SIZE );
			if( Wait_For_Cavity_Response( pModbus ) == MODBUS_RTU_MASTER_OK ){
				
				if( (i != CAVITY_EEP_BLK_SYS) && (CavityHandle.Eep[i][SENSOR_EEP_CHn_PREFIX] != SENSOR_EEP_PREFIX) ){
					
					Cavity_Eep_Init_Packet_Send( pModbus, i );
					if( Wait_For_Cavity_Response( pModbus ) == MODBUS_RTU_MASTER_OK ){
						
						Cavity_Eep_Read_Packet_Send( pModbus, i, 0, SENSOR_EEP_BLK_SIZE );
						if( Wait_For_Cavity_Response( pModbus ) != MODBUS_RTU_MASTER_OK ){ // Reload Failure
							break;
						}
					}
					else { // Init Failure
						break;
					}
				}
			}
			else { // Load Failure
				break;
			}
		}
	}
	
	if( CavityHandle.Inf[CAVITY_INF_FW_VER] < 149 ){
		CavityHandle.Inf[CAVITY_INF_BUF_SIZE] = 180;
		CavityHandle.Inf[CAVITY_INF_SRC_FREQ] = 10;
	}
	
	if( i == CAVITY_EEP_BLK_MAX ){
		CavityHandle.IsPresent = true;
	}
	else {
		CavityHandle.IsPresent = false;
		CavityHandle.Inf[CAVITY_INF_SRC_FREQ] = 10;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Info_Read_Packet_Send( MODBUS_MasterHandleType *pModbus )
{
	pModbus->Function			= READ_INPUT_REGISTERS;
	pModbus->StartAddress		= 0;
	pModbus->RequestCount		= 9;
	pModbus->pReceiveBuf		= CavityHandle.Inf;
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;    
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Data_Read_Packet_Send( MODBUS_MasterHandleType *pModbus )
{
	pModbus->Function			= READ_INPUT_REGISTERS;
	pModbus->StartAddress		= 9;
	pModbus->RequestCount		= 100;
	pModbus->pReceiveBuf		= CavityHandle.ReceiveBuf;
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Wave_Read_Packet_Send( MODBUS_MasterHandleType *pModbus )
{
	if( CavityHandle.iWaveOut != CavityHandle.iWaveSet ){
		CavityHandle.iWaveOut  = CavityHandle.iWaveSet;
	}
	
	pModbus->Function			= READ_INPUT_REGISTERS;
	pModbus->StartAddress		= CavityHandle.iWaveOut * (CavityHandle.Inf[CAVITY_INF_BUF_SIZE] / 2 + 1) + 109;
	pModbus->RequestCount		= CavityHandle.Inf[CAVITY_INF_BUF_SIZE] / 2 + 1;
	pModbus->pReceiveBuf		= CavityHandle.ReceiveBuf;
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Eep_Read_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block, U8 wAddr, U8 wSize )
{     
	pModbus->Function			= READ_HOLDING_REGISTERS;
	pModbus->StartAddress		= SENSOR_EEP_BLK_BASE( Block ) + wAddr;
	pModbus->RequestCount		= wSize;
	pModbus->pReceiveBuf		= &CavityHandle.Eep[Block][wAddr];
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;    
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Read_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Eep_Write_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block, U8 wAddr, U8 wSize )
{
	pModbus->Function			= WRITE_MULTIPLE_REGISTERS;
	pModbus->StartAddress		= SENSOR_EEP_BLK_BASE( Block ) + wAddr;
	pModbus->RequestCount		= wSize;
	pModbus->ByteCount			= wSize * 2;
	pModbus->pRequestBuf		= &CavityHandle.Eep[Block][wAddr];
	pModbus->Status				= MODBUS_RTU_MASTER_BUSY;    
	pModbus->TimeoutTick		= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL;
	
	MODBUS_RTU_Write_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cavity_Eep_Init_Packet_Send( MODBUS_MasterHandleType *pModbus, U8 Block )
{
	U16		wBuf[SENSOR_EEP_BLK_SIZE];
	U8		i;
	
	for( i=0; i<SENSOR_EEP_BLK_SIZE; i++ ){
		wBuf[i] = SENSOR_EEP_IR_CHn_DEFAULT[i];
	}
	
	switch( CavityHandle.Inf[CAVITY_INF_SEN_TYPE] ){
		
		case CAVITY_SEN_TYPE_LMM_274_X025:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_330; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_440; break;
				case CAVITY_EEP_BLK_CH3: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_000; break;
				case CAVITY_EEP_BLK_CH4: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_04_850; break;
			}
			break;
		
		case CAVITY_SEN_TYPE_LMM_274_X027:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_330; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_440; break;
				case CAVITY_EEP_BLK_CH3: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_000; break;
				case CAVITY_EEP_BLK_CH4: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_925; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_CH:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_330; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_950; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_DH:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_04_260; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_950; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_IH:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_04_660; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_950; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_X018:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_440; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_900; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_X021:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_925; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_900; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_X054:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_000; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_900; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_X040:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_04_520; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_720; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_X044:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_04_850; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_900; break;
			}
			break;
		
		case CAVITY_SEN_TYPE_LMM_274_X033:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_076; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_440; break;
				case CAVITY_EEP_BLK_CH3: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_000; break;
				case CAVITY_EEP_BLK_CH4: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_10_100; break;
			}
			break;
		
		case CAVITY_SEN_TYPE_LMM_274_X040:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_370; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_420; break;
				case CAVITY_EEP_BLK_CH3: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_000; break;
				case CAVITY_EEP_BLK_CH4: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_230; break;
			}
			break;
		
		case CAVITY_SEN_TYPE_LMM_274_X042:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_370; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_150; break;
				case CAVITY_EEP_BLK_CH3: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_790; break;
				case CAVITY_EEP_BLK_CH4: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_10_990; break;
			}
			break;
		
		case CAVITY_SEN_TYPE_LMM_274_X044:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_09_076; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_370; break;
				case CAVITY_EEP_BLK_CH3: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_08_000; break;
				case CAVITY_EEP_BLK_CH4: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_10_100; break;
			}
			break;
			
		case CAVITY_SEN_TYPE_LIM_272_X127:
			switch( Block ){
				case CAVITY_EEP_BLK_SYS:
				case CAVITY_EEP_BLK_CH3:
				case CAVITY_EEP_BLK_CH4: return;
				case CAVITY_EEP_BLK_CH1: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_10_990; break;
				case CAVITY_EEP_BLK_CH2: wBuf[SENSOR_EEP_CHn_SEN_NO] = IR_SEN_03_950; break;
			}
			break;
	}
	
	pModbus->Function		= WRITE_MULTIPLE_REGISTERS;
	pModbus->StartAddress	= SENSOR_EEP_BLK_BASE( Block );
	pModbus->RequestCount	= SENSOR_EEP_BLK_SIZE;
	pModbus->ByteCount		= SENSOR_EEP_BLK_SIZE * 2;
	pModbus->pRequestBuf	= wBuf;
	pModbus->Status			= MODBUS_RTU_MASTER_BUSY;
	pModbus->TimeoutTick	= MODBUS_RTU_MASTER_TIMEOUT_INTERVAL + 100; // + 4 sec.
	
	MODBUS_RTU_Write_Register_Packet_Send_Master( pModbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Cavity_Packet_Sequence_Check( MODBUS_MasterHandleType *pModbus )
{               
	U32		CurrSeqNo = CavityHandle.SequenceNo;
	U16		ReadSeqNo = CavityHandle.ReceiveBuf[0];
	bool	ret = false;
	
	if( CurrSeqNo != ReadSeqNo ){
		CurrSeqNo += 3;
		if( CurrSeqNo > ReadSeqNo ){
			ret = true;
		}
		else {
			dprintf( "\n, Cavity frame is missed! %u, %u", CavityHandle.SequenceNo, ReadSeqNo );
		}
		CavityHandle.SequenceNo  = ReadSeqNo;
	}
		
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static MODBUS_MasterStatusType Wait_For_Cavity_Response( MODBUS_MasterHandleType *pModbus )
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
