#define __SENSING_PROCESS_C__
	#include "sensing_process.h"
#undef  __SENSING_PROCESS_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
//#include "storage.h"
#include "usart3.h"
#include "flow_control.h"
#include "option.h"
#include "power_control.h"
/* End Include */

/* Start Define */
#define SENSOR_COM_ERR_LIMIT		( 10 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static bool Sensing_Data_Calculation( void );
static void Sensing_Data_Calculation_CT( Cartridge_HandleType *pCtg );
static void Sensing_Data_Calculation_IR( Cavity_HandleType *pCvt );
static void Sensing_Data_Calculation_Internal_ADC_Part( iADC_HandleType *pADC );
static void Sensor_Status_Load_CT( Cartridge_HandleType *pCtg );
static void Sensor_Status_Load_IR( Cavity_HandleType *pCvt );

/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sensing_Process_Init( void )
{
	GPIO_Init();
	                                                      
	SensingProcess.State			= SENSING_PROCESS_CTG_SENS_DATA_REQUEST;
	SensingProcess.Sync.Byte		= 0;
	
	SensingProcess.pCtg				= &CartridgeHandle;
	SensingProcess.CtgEep.Hp		= 0;
	SensingProcess.CtgEep.Tp		= 0;
	SensingProcess.CtgComErrCnt		= 0;
	SensingProcess.CtgWaveModeSet	= false;
	SensingProcess.CtgWaveModeOut	= false;
	
	SensingProcess.pCvt				= &CavityHandle;
	SensingProcess.CvtEep.Hp		= 0;
	SensingProcess.CvtEep.Tp		= 0;
	SensingProcess.CvtComErrCnt		= 0;
	SensingProcess.CvtWaveModeSet	= false;
	SensingProcess.CvtWaveModeOut	= false;
	
	SensingProcess.piADC			= &iADC;
	
	return MODBUS_RTU_Init_Master( &SensingProcess.Modbus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sensing_Process( void )
{
	U8		Block;
	U16		wAddr, wSize;
	
	MODBUS_RTU_Rx_Handler_Master( &SensingProcess.Modbus );
	
	switch( SensingProcess.State ){
		case SENSING_PROCESS_CTG_SENS_DATA_REQUEST:
			if( SensingProcess.pCtg->IsPresent == true ){
				SENSOR_USART_PORT_SELECTION( CARTRIDGE_USART_PORT );
				SensingProcess.Modbus.SlaveAddress = CARTRIDGE_MODBUS_ADDRESS;
				if( SensingProcess.CtgWaveModeOut != SensingProcess.CtgWaveModeSet ){
					SensingProcess.CtgWaveModeOut  = SensingProcess.CtgWaveModeSet;
				}
				if( SensingProcess.CtgWaveModeOut == true )	{ Cartridge_Wave_Read_Packet_Send( &SensingProcess.Modbus ); }
				else										{ Cartridge_Data_Read_Packet_Send( &SensingProcess.Modbus ); }
				SensingProcess.State = SENSING_PROCESS_CTG_SENS_DATA_RECEIVE;
			}
			else {
				SensingProcess.Sync.Item.CT = true;
				SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_REQUEST;
			}
			break;
			
		case SENSING_PROCESS_CTG_SENS_DATA_RECEIVE:
			if( SensingProcess.Modbus.Status != MODBUS_RTU_MASTER_BUSY ){
				switch( SensingProcess.Modbus.Status ){
					case MODBUS_RTU_MASTER_OK:
						SensingProcess.CtgComErrCnt = 0;
						SystemError.Main.Bit.CtgCom = false;
						SensingProcess.Sync.Item.CT = Cartridge_Packet_Sequence_Check( &SensingProcess.Modbus );
						Sensor_Status_Load_CT( SensingProcess.pCtg );
						break;
						
					case MODBUS_RTU_MASTER_ADDRESS_MISMATCH:
					case MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE:
					case MODBUS_RTU_MASTER_CRC_ERROR:
					case MODBUS_RTU_MASTER_TIMEOUT:
						if( SystemError.Main.Bit.CtgCom == true ){
							SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_REQUEST;
						}
						else {
							SensingProcess.CtgComErrCnt++;
						}
						break;
				}
				if( SensingProcess.CtgEep.Hp != SensingProcess.CtgEep.Tp ){
					SensingProcess.State = SENSING_PROCESS_CTG_EEP_WRITE_REQUEST;
				}
				else {
					SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_REQUEST;
				}
			}
			break;
			
		case SENSING_PROCESS_CTG_EEP_WRITE_REQUEST:
			Block = SensingProcess.CtgEep.Block[SensingProcess.CtgEep.Tp];
			wAddr = SensingProcess.CtgEep.wAddr[SensingProcess.CtgEep.Tp];
			wSize = SensingProcess.CtgEep.wSize[SensingProcess.CtgEep.Tp];
			Cartridge_Eep_Write_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
			SensingProcess.State = SENSING_PROCESS_CTG_EEP_WRITE_RECEIVE;
			break;
			
		case SENSING_PROCESS_CTG_EEP_WRITE_RECEIVE:
			if( SensingProcess.Modbus.Status != MODBUS_RTU_MASTER_BUSY ){
				switch( SensingProcess.Modbus.Status ){
					case MODBUS_RTU_MASTER_OK:
						SystemError.Main.Bit.CtgCom = false;
						Block = SensingProcess.CtgEep.Block[SensingProcess.CtgEep.Tp];
						wAddr = SensingProcess.CtgEep.wAddr[SensingProcess.CtgEep.Tp];
						wSize = SensingProcess.CtgEep.wSize[SensingProcess.CtgEep.Tp];
						SensingProcess.CtgComErrCnt = 0;
						Cartridge_Eep_Read_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
						SensingProcess.State = SENSING_PROCESS_CTG_EEP_RELOAD;
						break;
						
					case MODBUS_RTU_MASTER_ADDRESS_MISMATCH:
					case MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE:
					case MODBUS_RTU_MASTER_CRC_ERROR:
					case MODBUS_RTU_MASTER_TIMEOUT:
						if( SystemError.Main.Bit.CtgCom == true ){
							SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_REQUEST;
						}
						else {
							Block = SensingProcess.CtgEep.Block[SensingProcess.CtgEep.Tp];
							wAddr = SensingProcess.CtgEep.wAddr[SensingProcess.CtgEep.Tp];
							wSize = SensingProcess.CtgEep.wSize[SensingProcess.CtgEep.Tp];
							Cartridge_Eep_Write_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
							SensingProcess.CtgComErrCnt++;
						}
						break;
				}
			}
			break;
			
		case SENSING_PROCESS_CTG_EEP_RELOAD:
			 switch( SensingProcess.Modbus.Status ){
				case MODBUS_RTU_MASTER_OK:
					SensingProcess.CtgComErrCnt = 0;
					SystemError.Main.Bit.CtgCom = false;
					SensingProcess.CtgEep.Tp = SensingProcess.CtgEep.Tp + 1 & SEN_EEP_HANDLE_FIFO_MASK;
					SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_REQUEST;
					break;
					
				case MODBUS_RTU_MASTER_ADDRESS_MISMATCH:
				case MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE:
				case MODBUS_RTU_MASTER_CRC_ERROR:
				case MODBUS_RTU_MASTER_TIMEOUT:
					if( SystemError.Main.Bit.CtgCom == true ){
						SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_REQUEST;
					}
					else {
						Block = SensingProcess.CtgEep.Block[SensingProcess.CtgEep.Tp];
						wAddr = SensingProcess.CtgEep.wAddr[SensingProcess.CtgEep.Tp];
						wSize = SensingProcess.CtgEep.wSize[SensingProcess.CtgEep.Tp];
						Cartridge_Eep_Read_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
						SensingProcess.CtgComErrCnt++;
					}
					break;
			}
			break;
			
		case SENSING_PROCESS_CVT_SENS_DATA_REQUEST:
			if( SensingProcess.pCvt->IsPresent == true ){
				SENSOR_USART_PORT_SELECTION( CAVITY_PORT );
				SensingProcess.Modbus.SlaveAddress = CAVITY_MODBUS_ADDRESS;
				if( SensingProcess.CvtWaveModeOut != SensingProcess.CvtWaveModeSet ){
					SensingProcess.CvtWaveModeOut  = SensingProcess.CvtWaveModeSet;
				}
				if( SensingProcess.CvtWaveModeOut == true )	{ Cavity_Wave_Read_Packet_Send( &SensingProcess.Modbus ); }
				else										{ Cavity_Data_Read_Packet_Send( &SensingProcess.Modbus ); }
				SensingProcess.State = SENSING_PROCESS_CVT_SENS_DATA_RECEIVE;
			}
			else {
				SensingProcess.Sync.Item.IR = true;
				SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_START;
			}
			break;
			
		case SENSING_PROCESS_CVT_SENS_DATA_RECEIVE:
			if( SensingProcess.Modbus.Status != MODBUS_RTU_MASTER_BUSY ){
				switch( SensingProcess.Modbus.Status ){
					case MODBUS_RTU_MASTER_OK:
						SensingProcess.CvtComErrCnt = 0;
						SystemError.Main.Bit.CvtCom = false;
						SensingProcess.Sync.Item.IR = Cavity_Packet_Sequence_Check( &SensingProcess.Modbus );
						Sensor_Status_Load_IR( SensingProcess.pCvt );
						break;
						
					case MODBUS_RTU_MASTER_ADDRESS_MISMATCH:
					case MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE:
					case MODBUS_RTU_MASTER_CRC_ERROR:
					case MODBUS_RTU_MASTER_TIMEOUT:
						if( SystemError.Main.Bit.CvtCom == true ){
							SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_START;
						}
						else {
							SensingProcess.CvtComErrCnt++;
						}
						break;
				}
				if( SensingProcess.CvtEep.Hp != SensingProcess.CvtEep.Tp ){
					SensingProcess.State = SENSING_PROCESS_CVT_EEP_WRITE_REQUEST;
				}
				else {
					SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_START;
				}
			}
			break;
			
		case SENSING_PROCESS_CVT_EEP_WRITE_REQUEST:
			Block = SensingProcess.CvtEep.Block[SensingProcess.CvtEep.Tp];
			wAddr = SensingProcess.CvtEep.wAddr[SensingProcess.CvtEep.Tp];
			wSize = SensingProcess.CvtEep.wSize[SensingProcess.CvtEep.Tp];
			Cavity_Eep_Write_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
			SensingProcess.State = SENSING_PROCESS_CVT_EEP_WRITE_RECEIVE;
			break;
			
		case SENSING_PROCESS_CVT_EEP_WRITE_RECEIVE:
			if( SensingProcess.Modbus.Status != MODBUS_RTU_MASTER_BUSY ){
				switch( SensingProcess.Modbus.Status ){
					case MODBUS_RTU_MASTER_OK:
						SensingProcess.CvtComErrCnt = 0;
						SystemError.Main.Bit.CvtCom = false;
						break;
						
					case MODBUS_RTU_MASTER_ADDRESS_MISMATCH:
					case MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE:
					case MODBUS_RTU_MASTER_CRC_ERROR:
					case MODBUS_RTU_MASTER_TIMEOUT:
						if( SystemError.Main.Bit.CvtCom == true ){
							SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_START;
						}
						else {
							SensingProcess.CvtComErrCnt++;
						}
						break;
				}
				Block = SensingProcess.CvtEep.Block[SensingProcess.CvtEep.Tp];
				wAddr = SensingProcess.CvtEep.wAddr[SensingProcess.CvtEep.Tp];
				wSize = SensingProcess.CvtEep.wSize[SensingProcess.CvtEep.Tp];
				Cavity_Eep_Read_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
				SensingProcess.State = SENSING_PROCESS_CVT_EEP_RELOAD;
			}
			break;
			
		case SENSING_PROCESS_CVT_EEP_RELOAD:
			switch( SensingProcess.Modbus.Status ){
				case MODBUS_RTU_MASTER_OK:
					SensingProcess.CvtComErrCnt = 0;
					SystemError.Main.Bit.CvtCom = false;
					SensingProcess.CvtEep.Tp = SensingProcess.CvtEep.Tp + 1 & SEN_EEP_HANDLE_FIFO_MASK;
					SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_START;
					break;
					
				case MODBUS_RTU_MASTER_ADDRESS_MISMATCH:
				case MODBUS_RTU_MASTER_UNUSED_FUNCTION_CODE:
				case MODBUS_RTU_MASTER_CRC_ERROR:
				case MODBUS_RTU_MASTER_TIMEOUT:
					if( SystemError.Main.Bit.CvtCom == true ){
						SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_START;
					}
					else {
						SensingProcess.CvtComErrCnt++;
						Block = SensingProcess.CvtEep.Block[SensingProcess.CvtEep.Tp];
						wAddr = SensingProcess.CvtEep.wAddr[SensingProcess.CvtEep.Tp];
						wSize = SensingProcess.CvtEep.wSize[SensingProcess.CvtEep.Tp];
						Cavity_Eep_Read_Packet_Send( &SensingProcess.Modbus, Block, wAddr, wSize );
					}
					break;
			}
			break;
			
		case SENSING_PROCESS_iADC_CONVERSION_START:
			iADC_Conversion_Start();
			SensingProcess.State = SENSING_PROCESS_iADC_CONVERSION_CHECK;
			break;
			
		case SENSING_PROCESS_iADC_CONVERSION_CHECK:
			if( SensingProcess.piADC->ConversionComplete == true ){
				SensingProcess.piADC->ConversionComplete = false;
				SensingProcess.Sync.Item.iADC = true;
				if( SystemOption.Item.IsBatMode == true ){
					SensingProcess.State = SENSING_PROCESS_BAT_CHARGER_ADCs_READ;
				}
				else {
					SensingProcess.State = SENSING_PROCESS_CTG_SENS_DATA_REQUEST;
					SensingProcess.Sync.Item.BatCharger = true;
				}
			}
			break;
			
		case SENSING_PROCESS_BAT_CHARGER_ADCs_READ:
			if( BQ2579x_Read_ADC( &PwrCtrl.BQ ) == false ){
				SensingProcess.Sync.Item.BatCharger = true;
				SensingProcess.State = SENSING_PROCESS_CTG_SENS_DATA_REQUEST;
			}
			else {
				SystemError.Peripheral.Bit.BatCharger = true;
			}
			break;
	}
	
	if( SensingProcess.CtgComErrCnt > SENSOR_COM_ERR_LIMIT ){ SystemError.Main.Bit.CtgCom = true; }
	if( SensingProcess.CvtComErrCnt > SENSOR_COM_ERR_LIMIT ){ SystemError.Main.Bit.CvtCom = true; }
	
	return Sensing_Data_Calculation();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SenEEP_Write_CHn( U16 *pData, U8 iCh, U16 wAddr, U16 wSize )
{
	if( iCh < CARTRIDGE_SEN_CH_MAX ){
		SenEEP_Write_Ctg( pData, CARTRIDGE_EEP_BLK_CH1 + iCh, wAddr, wSize );
	}
	else {
		SenEEP_Write_Cvt( pData, CAVITY_EEP_BLK_CH1 + iCh - CARTRIDGE_SEN_CH_MAX, wAddr, wSize );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void SenEEP_Write_Ctg( U16 *pData, U8 Block, U16 wAddr, U16 wSize )
{
	U8		Hp = SensingProcess.CtgEep.Hp & SEN_EEP_HANDLE_FIFO_MASK;
	U8		i;
	
	for( i=0; i<wSize; i++ ){ SensingProcess.pCtg->Eep[Block][wAddr+i] = *(pData+i); }
	SensingProcess.CtgEep.Block[Hp] = Block;
	SensingProcess.CtgEep.wAddr[Hp] = wAddr;
	SensingProcess.CtgEep.wSize[Hp] = wSize;
	SensingProcess.CtgEep.Hp = SensingProcess.CtgEep.Hp + 1 & SEN_EEP_HANDLE_FIFO_MASK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void SenEEP_Write_Cvt( U16 *pData, U8 Block, U16 wAddr, U16 wSize )
{
	U8		Hp = SensingProcess.CvtEep.Hp & SEN_EEP_HANDLE_FIFO_MASK;
	U8		i;
	
	for( i=0; i<wSize; i++ ){ SensingProcess.pCvt->Eep[Block][wAddr+i] = *(pData+i); }
	SensingProcess.CvtEep.Block[Hp] = Block;
	SensingProcess.CvtEep.wAddr[Hp] = wAddr;
	SensingProcess.CvtEep.wSize[Hp] = wSize;
	SensingProcess.CvtEep.Hp = SensingProcess.CvtEep.Hp + 1 & SEN_EEP_HANDLE_FIFO_MASK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	
    __HAL_RCC_GPIOB_CLK_ENABLE();
	
	SENSOR_USART_PORT_SELECTION( CAVITY_PORT );
	
    GPIO_InitStructure.Pin		= GPIO_PIN_14;
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_PULLUP;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_HIGH;
	
    HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
static bool Sensing_Data_Calculation( void )
{
	if( SensingProcess.Sync.Byte == 0x0F ){
		SensingProcess.Sync.Byte = 0;
		
		Sensing_Data_Calculation_CT( SensingProcess.pCtg );
		Sensing_Data_Calculation_IR( SensingProcess.pCvt );
		Sensing_Data_Calculation_Internal_ADC_Part( SensingProcess.piADC );
		
		return true;
	}
	
	return false;
} 
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensing_Data_Calculation_CT( Cartridge_HandleType *pCtg )
{
	U8		i;
	
	if( pCtg->IsPresent == true ){
		if( SensingProcess.CtgWaveModeOut == true ){
			for( i=0; i<pCtg->Inf[CARTRIDGE_INF_BUF_SIZE]; i++ ){
				if( i & 1 ) { pCtg->Wave[pCtg->iWaveOut][i] = HI_BYTE( pCtg->ReceiveBuf[1 + i / 2] ); }
				else		{ pCtg->Wave[pCtg->iWaveOut][i] = LO_BYTE( pCtg->ReceiveBuf[1 + i / 2] ); }
			}
		}
		else {
			SensingProcess.Calculation.CT.pReceiveBuf = pCtg->ReceiveBuf;
			SensingProcess.Calculation.CT.pRaw = &SensingProcess.Raw.CT;
			CT_Calculation( &SensingProcess.Calculation.CT );
		}
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensing_Data_Calculation_IR( Cavity_HandleType *pCvt )
{
	U8		i;
	                 
	if( pCvt->IsPresent == true ){
		if( SensingProcess.CvtWaveModeOut == true ){
			for( i=0; i<pCvt->Inf[CAVITY_INF_BUF_SIZE]; i++ ){
				if( i & 1 ) { pCvt->Wave[pCvt->iWaveOut][i] = HI_BYTE( pCvt->ReceiveBuf[1 + i / 2] ); }
				else		{ pCvt->Wave[pCvt->iWaveOut][i] = LO_BYTE( pCvt->ReceiveBuf[1 + i / 2] ); }
			}
		}
		else {
			//pCvt->State = pCvt->ReceiveBuf[1];
			//pCvt->Error.Word = pCvt->ReceiveBuf[2];
			//pCvt->WaveSize = pCvt->ReceiveBuf[3];
			pCvt->AmpCalRunFlag = pCvt->ReceiveBuf[4];
			pCvt->AmpCalErrFlag = pCvt->ReceiveBuf[5];
			for( i=0; i<pCvt->Inf[CAVITY_INF_CHANNELS]; i++ ){
				pCvt->AmpCalStatus[CAVITY_SEN_CH_1+i] = pCvt->ReceiveBuf[6+i];
				pCvt->PtmOffs[CAVITY_SEN_CH_1+i] = HI_BYTE( pCvt->ReceiveBuf[10+i] );
				pCvt->PtmGain[CAVITY_SEN_CH_1+i] = LO_BYTE( pCvt->ReceiveBuf[10+i] );
			}
			
			SensingProcess.Calculation.IR.pReceiveBuf = pCvt->ReceiveBuf;
			SensingProcess.Calculation.IR.pRaw = &SensingProcess.Raw.IR;
			IR_Raw_Calculation( &SensingProcess.Calculation.IR );
		}
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensing_Data_Calculation_Internal_ADC_Part( iADC_HandleType *pADC )
{
	SensingProcess.Calculation.Power.pVSUP_N_ADC = &pADC->Raw[iADC_VSUP_N];
	SensingProcess.Calculation.Power.pVSUP_P_ADC = &pADC->Raw[iADC_VSUP_P];
	SensingProcess.Calculation.Power.pVBAT_ADC = &pADC->Raw[iADC_VBAT];
	SensingProcess.Calculation.Power.pCharger = &PwrCtrl.BQ.ADCs;
	SensingProcess.Calculation.Power.pRaw = &SensingProcess.Raw.Power;
	PowerSupply_Calculation( &SensingProcess.Calculation.Power );
	
	SensingProcess.Calculation.Pressure.pAmpOut_ADC = &pADC->Raw[iADC_PSEN_AMPOUT];
	SensingProcess.Calculation.Pressure.pAmpRef_ADC = &pADC->Raw[iADC_PSEN_AMPREF];
	SensingProcess.Calculation.Pressure.pRaw = &SensingProcess.Raw.Pressure;
	Pressure_Calculation( &SensingProcess.Calculation.Pressure );
	                                                                    
	SensingProcess.Calculation.Temp.pLmtTemp_ADC = &pADC->Raw[iADC_LMT_TEMP];
	SensingProcess.Calculation.Temp.pMcuTemp_ADC = &pADC->Raw[iADC_MCU_TEMP];
	SensingProcess.Calculation.Temp.pCharger = &PwrCtrl.BQ.ADCs;
	SensingProcess.Calculation.Temp.pRaw = &SensingProcess.Raw.Temp;
	Temperature_Calculation( &SensingProcess.Calculation.Temp );
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensor_Status_Load_CT( Cartridge_HandleType *pCtg )
{
	if( SensingProcess.CtgWaveModeOut == false ){
		pCtg->State = pCtg->ReceiveBuf[1];
		pCtg->Error.Word = pCtg->ReceiveBuf[2];
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensor_Status_Load_IR( Cavity_HandleType *pCvt )
{
	if( SensingProcess.CvtWaveModeOut == false ){
		pCvt->State = pCvt->ReceiveBuf[1];
		pCvt->Error.Word = pCvt->ReceiveBuf[2];
	}
}


