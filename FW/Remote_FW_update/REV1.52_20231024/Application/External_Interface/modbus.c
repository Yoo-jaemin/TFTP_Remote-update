#define __MODBUS_C__
    #include "modbus.h"
#undef  __MODBUS_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */   
#include "option.h" 
#include "eeprom.h"        
#include "usart3.h"     
#include "version.h"    
#include "crc16.h"      
#include "ir.h"         
#include "ads868x.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */  
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void MODBUS_Read_Input_Register_Slave( U8 *pRxBuf );
static void MODBUS_Read_Holding_Register_Slave( U8 *pRxBuf );
static void MODBUS_Write_Single_Register_Slave( U8 *pRxBuf );
static void MODBUS_Write_Multiple_Register_Slave( U8 *pRxBuf );
static void MODBUS_InputRegister_System_Status_Mapping( void );
/* End Function */

/* Start Variable */  
static U16  Modbus_Reg_3xxxx[MODBUS_3xxxx_MAX];
static U8	Modbus_TxBuf[255];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Init( void )
{
	Modbus_Addr = EepHandle.Sram[EEP_SYS_MODBUS_ADDRESS];
	Modbus_TimeoutTick = 0;
	Modbus_TimeoutFlag = false;
	memset( Modbus_Reg_3xxxx, 0, sizeof( Modbus_Reg_3xxxx ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_InputRegister_Mapping( IR_DataType *ir )
{
	U16		i, BufSize, iWave[4];
	uWord	Wave[IR_WL_MAX];
	
	// Information
	Modbus_Reg_3xxxx[MODBUS_30001] = SystemOption.DetType;
	Modbus_Reg_3xxxx[MODBUS_30002] = (U16)(SystemOption.HardwereVersion * 10);
	Modbus_Reg_3xxxx[MODBUS_30003] = (U16)(FirmwareVersion * 100);
	Modbus_Reg_3xxxx[MODBUS_30004] = 0;
	Modbus_Reg_3xxxx[MODBUS_30005] = 0;
	Modbus_Reg_3xxxx[MODBUS_30006] = IrProcess.BufSize;
	Modbus_Reg_3xxxx[MODBUS_30007] = IrProcess.SrcFreq;
	Modbus_Reg_3xxxx[MODBUS_30008] = SystemOption.SenType;
	Modbus_Reg_3xxxx[MODBUS_30009] = SystemOption.Channels; 
	
	// Status & IR Calculated Data
	Modbus_Reg_3xxxx[MODBUS_30010]++;
	//Modbus_Reg_3xxxx[MODBUS_30011] = SystemState;
	//Modbus_Reg_3xxxx[MODBUS_30012] = SystemError.Word;
	Modbus_Reg_3xxxx[MODBUS_30013] = IrProcess.BufSize / 2;
	Modbus_Reg_3xxxx[MODBUS_30014] = IrAmpCal.RunFlag;
	Modbus_Reg_3xxxx[MODBUS_30015] = IrAmpCal.ErrFlag;
	Modbus_Reg_3xxxx[MODBUS_30016] = IrAmpCal.Status[IR_WL_1];                                      
	Modbus_Reg_3xxxx[MODBUS_30017] = IrAmpCal.Status[IR_WL_2];
	Modbus_Reg_3xxxx[MODBUS_30018] = IrAmpCal.Status[IR_WL_3];
	Modbus_Reg_3xxxx[MODBUS_30019] = IrAmpCal.Status[IR_WL_4];
	Modbus_Reg_3xxxx[MODBUS_30020] = (((U16)IrAmpCal.Offs[IR_WL_1] << 8 & 0xFF00) | ((U16)IrAmpCal.Gain[IR_WL_1] & 0x00FF));
	Modbus_Reg_3xxxx[MODBUS_30021] = (((U16)IrAmpCal.Offs[IR_WL_2] << 8 & 0xFF00) | ((U16)IrAmpCal.Gain[IR_WL_2] & 0x00FF));
	Modbus_Reg_3xxxx[MODBUS_30022] = (((U16)IrAmpCal.Offs[IR_WL_3] << 8 & 0xFF00) | ((U16)IrAmpCal.Gain[IR_WL_3] & 0x00FF));
	Modbus_Reg_3xxxx[MODBUS_30023] = (((U16)IrAmpCal.Offs[IR_WL_4] << 8 & 0xFF00) | ((U16)IrAmpCal.Gain[IR_WL_4] & 0x00FF));
	Modbus_Reg_3xxxx[MODBUS_30024] = (U16)( ir->SrcPwr );
	Modbus_Reg_3xxxx[MODBUS_30025] = ir->McuTemp.w.l;
	Modbus_Reg_3xxxx[MODBUS_30026] = ir->McuTemp.w.h;
	Modbus_Reg_3xxxx[MODBUS_30027] = ir->SenTemp.w.l;
	Modbus_Reg_3xxxx[MODBUS_30028] = ir->SenTemp.w.h;
	Modbus_Reg_3xxxx[MODBUS_30029] = 0;
	
	// Channel-1
	Modbus_Reg_3xxxx[MODBUS_30030] = ir->HiPeak[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30031] = ir->HiPeak[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30032] = ir->LoPeak[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30033] = ir->LoPeak[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30034] = ir->OcPtoP[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30035] = ir->OcPtoP[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30036] = ir->HiAvrg[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30037] = ir->HiAvrg[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30038] = ir->LoAvrg[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30039] = ir->LoAvrg[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30040] = ir->AvPtoP[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30041] = ir->AvPtoP[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30042] = ir->OcAvrg[IR_WL_1].w.l;
	Modbus_Reg_3xxxx[MODBUS_30043] = ir->OcAvrg[IR_WL_1].w.h;
	Modbus_Reg_3xxxx[MODBUS_30044] = ir->HiTime[IR_WL_1];
	Modbus_Reg_3xxxx[MODBUS_30045] = ir->LoTime[IR_WL_1];
	Modbus_Reg_3xxxx[MODBUS_30046] = ir->RawDvMax[IR_WL_1];
	Modbus_Reg_3xxxx[MODBUS_30047] = 0;
	Modbus_Reg_3xxxx[MODBUS_30048] = 0;
	Modbus_Reg_3xxxx[MODBUS_30049] = 0;
	
	// Channel-2
	Modbus_Reg_3xxxx[MODBUS_30050] = ir->HiPeak[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30051] = ir->HiPeak[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30052] = ir->LoPeak[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30053] = ir->LoPeak[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30054] = ir->OcPtoP[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30055] = ir->OcPtoP[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30056] = ir->HiAvrg[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30057] = ir->HiAvrg[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30058] = ir->LoAvrg[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30059] = ir->LoAvrg[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30060] = ir->AvPtoP[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30061] = ir->AvPtoP[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30062] = ir->OcAvrg[IR_WL_2].w.l;
	Modbus_Reg_3xxxx[MODBUS_30063] = ir->OcAvrg[IR_WL_2].w.h;
	Modbus_Reg_3xxxx[MODBUS_30064] = ir->HiTime[IR_WL_2];
	Modbus_Reg_3xxxx[MODBUS_30065] = ir->LoTime[IR_WL_2];
	Modbus_Reg_3xxxx[MODBUS_30066] = ir->RawDvMax[IR_WL_2];
	Modbus_Reg_3xxxx[MODBUS_30067] = 0;
	Modbus_Reg_3xxxx[MODBUS_30068] = 0;
	Modbus_Reg_3xxxx[MODBUS_30069] = 0; 
	
	// Channel-3
	Modbus_Reg_3xxxx[MODBUS_30070] = ir->HiPeak[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30071] = ir->HiPeak[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30072] = ir->LoPeak[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30073] = ir->LoPeak[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30074] = ir->OcPtoP[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30075] = ir->OcPtoP[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30076] = ir->HiAvrg[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30077] = ir->HiAvrg[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30078] = ir->LoAvrg[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30079] = ir->LoAvrg[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30080] = ir->AvPtoP[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30081] = ir->AvPtoP[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30082] = ir->OcAvrg[IR_WL_3].w.l;
	Modbus_Reg_3xxxx[MODBUS_30083] = ir->OcAvrg[IR_WL_3].w.h;
	Modbus_Reg_3xxxx[MODBUS_30084] = ir->HiTime[IR_WL_3];
	Modbus_Reg_3xxxx[MODBUS_30085] = ir->LoTime[IR_WL_3];
	Modbus_Reg_3xxxx[MODBUS_30086] = ir->RawDvMax[IR_WL_3];
	Modbus_Reg_3xxxx[MODBUS_30087] = 0;
	Modbus_Reg_3xxxx[MODBUS_30088] = 0;
	Modbus_Reg_3xxxx[MODBUS_30089] = 0; 
	
	// Channel-4
	Modbus_Reg_3xxxx[MODBUS_30090] = ir->HiPeak[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30091] = ir->HiPeak[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30092] = ir->LoPeak[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30093] = ir->LoPeak[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30094] = ir->OcPtoP[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30095] = ir->OcPtoP[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30096] = ir->HiAvrg[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30097] = ir->HiAvrg[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30098] = ir->LoAvrg[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30099] = ir->LoAvrg[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30100] = ir->AvPtoP[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30101] = ir->AvPtoP[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30102] = ir->OcAvrg[IR_WL_4].w.l;
	Modbus_Reg_3xxxx[MODBUS_30103] = ir->OcAvrg[IR_WL_4].w.h;
	Modbus_Reg_3xxxx[MODBUS_30104] = ir->HiTime[IR_WL_4];
	Modbus_Reg_3xxxx[MODBUS_30105] = ir->LoTime[IR_WL_4];
	Modbus_Reg_3xxxx[MODBUS_30106] = ir->RawDvMax[IR_WL_4];
	Modbus_Reg_3xxxx[MODBUS_30107] = 0;
	Modbus_Reg_3xxxx[MODBUS_30108] = 0;
	Modbus_Reg_3xxxx[MODBUS_30109] = 0;
	
	// Below are wave data
	BufSize = IrProcess.BufSize / 2;
	
	iWave[0] = MODBUS_30110;
	iWave[1] = iWave[0] + BufSize + 1;
	iWave[2] = iWave[1] + BufSize + 1;
	iWave[3] = iWave[2] + BufSize + 1;
	
	Modbus_Reg_3xxxx[iWave[0]] = Modbus_Reg_3xxxx[MODBUS_30010];
	Modbus_Reg_3xxxx[iWave[1]] = Modbus_Reg_3xxxx[MODBUS_30010];
	Modbus_Reg_3xxxx[iWave[2]] = Modbus_Reg_3xxxx[MODBUS_30010];
	Modbus_Reg_3xxxx[iWave[3]] = Modbus_Reg_3xxxx[MODBUS_30010];
	
	iWave[0]++;
	iWave[1]++;
	iWave[2]++;
	iWave[3]++;
	
	for( i=0; i<BufSize; i++ ){
		Wave[0].b.l = ir->Wave[0][i * 2 + 0];
		Wave[0].b.h = ir->Wave[0][i * 2 + 1];
		Wave[1].b.l = ir->Wave[1][i * 2 + 0];
		Wave[1].b.h = ir->Wave[1][i * 2 + 1];
		Wave[2].b.l = ir->Wave[2][i * 2 + 0];
		Wave[2].b.h = ir->Wave[2][i * 2 + 1];
		Wave[3].b.l = ir->Wave[3][i * 2 + 0];
		Wave[3].b.h = ir->Wave[3][i * 2 + 1];
		Modbus_Reg_3xxxx[iWave[0]+i] = Wave[0].w;
		Modbus_Reg_3xxxx[iWave[1]+i] = Wave[1].w;
		Modbus_Reg_3xxxx[iWave[2]+i] = Wave[2].w;
		Modbus_Reg_3xxxx[iWave[3]+i] = Wave[3].w;
	}
	iWave[3]++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Packet_Decoder_Slave( U8* pBuf )
{
	if( pBuf[0] == Modbus_Addr ){
		switch( pBuf[1] ){
			case READ_HOLDING_REGISTERS:
				MODBUS_Read_Holding_Register_Slave( pBuf );
				break;
				
			case READ_INPUT_REGISTERS:
				MODBUS_InputRegister_System_Status_Mapping();
				MODBUS_Read_Input_Register_Slave( pBuf );
				break;
				
			case WRITE_SINGLE_REGISTER:
				MODBUS_Write_Single_Register_Slave( pBuf );
				break;
				
			case WRITE_MULTIPLE_REGISTERS	:
				MODBUS_Write_Multiple_Register_Slave( pBuf );
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Timeout_Check_Slave( void )
{
	if( Modbus_TimeoutTick & 0x8000 ){
		if( Modbus_TimeoutTick == 0x8000 ){
			Modbus_TimeoutTick = 0;
			Modbus_TimeoutFlag = true;
		}
		else {
			Modbus_TimeoutTick--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_Read_Input_Register_Slave( U8 *pRxBuf )
{
        //printf(" MODBUS_Read_Input_Register_Slave\r\n");
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U16		bc;
	U16		Len;
	
	wCRC = CRC16( pRxBuf, 6 );
	if( (LO_BYTE(wCRC) != pRxBuf[6]) || (HI_BYTE(wCRC) != pRxBuf[7]) ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf("  Read Input Register Request Packet CRC Failure!\n");
		}
		return;
	}
	
	ad = ((U16)pRxBuf[2] << 8) | ((U16)pRxBuf[3]);
	wc = ((U16)pRxBuf[4] << 8) | ((U16)pRxBuf[5]);
	bc = wc*2;
	
	if( ad+wc > MODBUS_3xxxx_MAX ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf( "  Maximum index of input register write commands has been exceeded.\n\n" );
		}
		return;
	}
	
	Modbus_TxBuf[0] = pRxBuf[0];
	Modbus_TxBuf[1] = pRxBuf[1];
	Modbus_TxBuf[2] = bc;
	
	for( i=0; i<wc; i++ ){
		Modbus_TxBuf[3+i*2] = HI_BYTE( Modbus_Reg_3xxxx[ad+i] );
		Modbus_TxBuf[4+i*2] = LO_BYTE( Modbus_Reg_3xxxx[ad+i] );
	}
	
	wCRC = CRC16( &Modbus_TxBuf[0], bc+3 );
	Modbus_TxBuf[bc+3] = LO_BYTE( wCRC );
	Modbus_TxBuf[bc+4] = HI_BYTE( wCRC );
	
	Len = bc+5;
	
	if( Modbus_TimeoutFlag == true ){
		dprintf( "  Read Input Register Timeout!\n" );
		return;
	}
	else {
		Modbus_TimeoutTick = 0;
		Modbus_TimeoutFlag = false;
	}
		
	USART3_Packet_Send( Modbus_TxBuf, Len );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_Read_Holding_Register_Slave( U8 *pRxBuf )
{
        //printf(" MODBUS_Read_Holding_Register_Slave\r\n");
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U16		bc;
	U16		Len;
	
	wCRC = CRC16( pRxBuf, 6 );
	if( (LO_BYTE(wCRC) != pRxBuf[6]) || (HI_BYTE(wCRC) != pRxBuf[7]) ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf("  Read Holding Register Request Packet CRC Failure!\n");
		}
		return;
	}
	
	ad = ((U16)pRxBuf[2] << 8) | ((U16)pRxBuf[3]);
	wc = ((U16)pRxBuf[4] << 8) | ((U16)pRxBuf[5]);
	bc = wc*2;
	
	if( ad+wc > EEP_MAX ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf( "  Maximum index of holding register write commands has been exceeded.\n\n" );
		}
		return;
	}
	
	Modbus_TxBuf[0] = pRxBuf[0];
	Modbus_TxBuf[1] = pRxBuf[1];
	Modbus_TxBuf[2] = bc;
	
	for( i=0; i<wc; i++ ){
		Modbus_TxBuf[3+i*2] = HI_BYTE(EepHandle.Sram[ad+i]);
		Modbus_TxBuf[4+i*2] = LO_BYTE(EepHandle.Sram[ad+i]);
	}
	
	wCRC = CRC16( &Modbus_TxBuf[0], bc+3 );
	Modbus_TxBuf[bc+3] = LO_BYTE( wCRC );
	Modbus_TxBuf[bc+4] = HI_BYTE( wCRC );
	
	Len = bc+5;
	
	if( Modbus_TimeoutFlag == true ){
		dprintf( "  Read Holding Register Timeout!\n" );
		return;
	}
	else {
		Modbus_TimeoutTick = 0;
		Modbus_TimeoutFlag = false;
	}                         
	
	USART3_Packet_Send( Modbus_TxBuf, Len );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_Write_Single_Register_Slave( U8 *pRxBuf )
{
        printf(" MODBUS_Write_Single_Register_Slave\r\n");
	U16		addr;
	U16		data;
	U16		wCRC;
	
	wCRC = CRC16( pRxBuf, 6 );
	if( (LO_BYTE(wCRC) != pRxBuf[6]) || (HI_BYTE(wCRC) != pRxBuf[7]) ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf("  Write Single Register Request Packet CRC Failure!\n");
		}
		return;
	}
	
	addr = ((U16)pRxBuf[2] << 8) | ((U16)pRxBuf[3]);
	data = ((U16)pRxBuf[4] << 8) | ((U16)pRxBuf[5]);
	
	if( addr >= EEP_MAX ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf( "  Maximum index of holding register write commands has been exceeded.\n\n" );
		}
		return;
	}
	
	if( addr > EEP_PREFIX ){
		
		EEPROM_Update( &data, addr, 1 );
		USART3_Packet_Send( pRxBuf, 8 );
	} else {
                // boot jump -> IR FW Update
                //HAL_Delay(50);
                printf(" IR boot Jump! \r\n");
                HAL_NVIC_SystemReset();
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_Write_Multiple_Register_Slave( U8 *pRxBuf )
{
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U8		bc;
	U16		eep[EEPROM_UPDATE_BUFF_SIZE];
	
	bc = pRxBuf[6];
	wCRC = CRC16( pRxBuf, 7+bc );
	if( (LO_BYTE(wCRC) != pRxBuf[7+bc]) || (HI_BYTE(wCRC) != pRxBuf[8+bc]) ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf("  Write Multiple Register Request Packet CRC Failure!\n");
		}
		return;
	}
	
	ad = ((U16)pRxBuf[2] << 8) | ((U16)pRxBuf[3]);
	wc = ((U16)pRxBuf[4] << 8) | ((U16)pRxBuf[5]);
	
	if( ad+wc > EEP_MAX ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf( "  Maximum index of holding register write commands has been exceeded.\n\n" );
		}
		return;
	}
	
	if( wc > EEPROM_UPDATE_BUFF_SIZE ){
		if( IrProcess.LogMode == IR_LOG_OFF ){
			dprintf( "  Maximum packet size of holding register write commands has been exceeded.\n" );
		}
		return;
	}
	
	for( i=0; i<wc; i++ ){
		
		if( ad+i > EEP_PREFIX ){
			eep[i] = ((U16)pRxBuf[7+i*2] << 8) | ((U16)pRxBuf[8+i*2]);
		}
	}
	
	EEPROM_Update( eep, ad, wc );
	
	for( i=0; i<6; i++ ){
		Modbus_TxBuf[i] = pRxBuf[i];
	}
	wCRC = CRC16( Modbus_TxBuf, 6 );
	Modbus_TxBuf[6] = LO_BYTE( wCRC );
	Modbus_TxBuf[7] = HI_BYTE( wCRC );
	USART3_Packet_Send( Modbus_TxBuf, 8 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MODBUS_InputRegister_System_Status_Mapping( void )
{
	Modbus_Reg_3xxxx[MODBUS_30011] = SystemState;
	Modbus_Reg_3xxxx[MODBUS_30012] = SystemError.Word;
}


