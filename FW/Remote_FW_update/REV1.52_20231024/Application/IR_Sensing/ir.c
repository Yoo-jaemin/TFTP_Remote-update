#define __IR_C__
    #include "ir.h"
#undef  __IR_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "ir.h" 
#include "spi.h"
#include "ads868x.h"
#include "tim1.h"    
#include "tpl0102.h" 
#include "led.h"    
#include "lmt84.h"  
#include "modbus.h"   
#include "adc.h"   
#include "option.h"
#include "eeprom.h"
/* End Include */

/* Start Define */
#define AMP_CAL_OFFS_MARGIN						( 0.05f )
#define AMP_CAL_GAIN_MARGIN						( 0.02f )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union{
	U8		Byte;
    struct{
		bool	B0: 1;
        bool	B1: 1;
        bool	B2: 1;
        bool	B3: 1;
        bool	B4: 1;
        bool	B5: 1;
        bool	B6: 1;
        bool	B7: 1;
    }Bit;
}GainErrType;

typedef union{
	U16		Word;
    struct{
		bool	Auto_Seq_En:			1;
        bool	Ch_Pwr_Down:			1;
        bool	Feature_Selection:		1;
		bool	Input_Range_Select_0:	1;
        bool	Input_Range_Select_1:	1;
        bool	Input_Range_Select_2:	1;
        bool	Input_Range_Select_3:	1;
        bool	Input_Range_Select_4:	1;
        bool	Input_Range_Select_5:	1;
        bool	Input_Range_Select_6:	1;
        bool	Input_Range_Select_7:	1;
        bool	Reserved:				5;
    }Bit;
}InitErrType;
/* End Struct */

/* Start Variable */
static GainErrType		GainErr = { 0 };
static InitErrType		InitErr = { 0 };
/* End Variable */

/* Start Function */
static void IR_Sampling_Start( void );
static void IR_Calculation( IR_DataType *ir );
static void IR_Log_Handler( U8 bTp );
static void IR_Amp_Offs_Write( U16 i, U8 Data );
static void IR_Amp_Gain_Write( U16 i, U8 Data );
static bool IR_Amp_Offs_Decrease( U16 i );
static bool IR_Amp_Offs_Increase( U16 i );
static bool IR_Amp_Gain_Decrease( U16 i );
static bool IR_Amp_Gain_Increase( U16 i );
static void IR_Amp_Cal_Handler( IR_DataType *ir, U16 i );
static void IR_Amp_Cal_Start( U16 i );
static void IR_Manual_Amp_Set_Check( U16 i );
static void IR_Amp_Cal_All_Manual_Value_Eep_Update( void );
static void IR_Amp_Cal_Manual_Offs_Eep_Update( U16 i );
static void IR_Amp_Cal_Manual_Gain_Eep_Update( U16 i );
/* End Function */
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Init( void )
{                              
	U16		DefaultOffs = 3000;
	U16		DefaultGain = 2500;
	U16		w;
	
	for( w=0; w<IR_WL_MAX; w++ ){
    	IrAmpCal.Status[w] = AMP_CAL_IDLE;
		IrAmpCal.ManualOffs[w] = EepHandle.Sram[EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_PTM_MANUAL_OFFS];
		IrAmpCal.ManualGain[w] = EepHandle.Sram[EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_PTM_MANUAL_GAIN];
		
		if( EepHandle.Sram[EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_TARGET_mV_OFFS] == 13107 ){
			EEPROM_Update( &DefaultOffs, EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_TARGET_mV_OFFS, 1 );
			IrAmpCal.TargetOffs[w] = DefaultOffs * 0.001f;
		}
		else {
			IrAmpCal.TargetOffs[w] = EepHandle.Sram[EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_TARGET_mV_OFFS] * 0.001f;
		}
		if( EepHandle.Sram[EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_TARGET_mV_GAIN] == 16499 ){
			EEPROM_Update( &DefaultGain, EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_TARGET_mV_GAIN, 1 );
			IrAmpCal.TargetGain[w] = DefaultGain * 0.001f;
		}
		else {
			IrAmpCal.TargetGain[w] = EepHandle.Sram[EEPROM_Block_Address( w ) + EEP_IR1_AMP_CAL_TARGET_mV_GAIN] * 0.001f;
		}
	}
	
	IrAmpCal.RunFlag = EepHandle.Sram[EEP_SYS_AMP_CAL_RUN_FLAG];
    IrAmpCal.ErrFlag = 0;
	
    IrProcess.State = IR_PROCESS_IDLE;
	IrProcess.FifoHp = 0;
	IrProcess.FifoTp = 0;
	IrProcess.iWave = 0;
	IrProcess.iBuff = 0;
	IrProcess.LogMode = EepHandle.Sram[EEP_SYS_LOG_MODE];
	IrProcess.LogHandleCnt = 0;
	IrProcess.ModbusWaveOutputChannel = 0;
	
	switch( SystemOption.DetType ){
		case 970: IrProcess.SrcFreq = 10; IrProcess.BufSize = 180; break;
		case 972: IrProcess.SrcFreq =  8; IrProcess.BufSize = 200; break;
		case 974: IrProcess.SrcFreq = 10; IrProcess.BufSize = 180; break;
		case 976: IrProcess.SrcFreq = 10; IrProcess.BufSize = 180; break;
	}
	
    IrProcess.SrcPowerCheckTmr = 0;
	IrProcess.SrcPowerCheckSet = 0x8000 + (U16)(1.f / IrProcess.SrcFreq / 4.f / 0.0001f);
	                                   
	if(( IrProcess.LogMode >= IR_LOG_TERATERM_FULL ) && ( IrProcess.LogMode <= IR_LOG_TERATERM_1SEC )){
    	dprintf( "\n," );
    	dprintf( "       , CH-01 ,       ,    " );
    	dprintf( "       , CH-02 ,       ,    " );
    	dprintf( "       , CH-03 ,       ,    " );
    	dprintf( "       , CH-04 ,       ,    " );
    	dprintf( " SEN- , MCU- , SRC" );
    	dprintf( "\n," );
		dprintf( "1C-PTOP,HI-MARG,LO-MARG,    " );
    	dprintf( "1C-PTOP,HI-MARG,LO-MARG,    " );
    	dprintf( "1C-PTOP,HI-MARG,LO-MARG,    " );
    	dprintf( "1C-PTOP,HI-MARG,LO-MARG,    " );
    	dprintf( " TEMP , TEMP , PWR" );
	}
    
	dprintf( "\n" );
	
    IR_Source_PWM_Start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Process_Start( void )
{
    IrProcess.SrcPowerCheckTmr = IrProcess.SrcPowerCheckSet;
	IrData[IrProcess.FifoHp & IR_FIFO_MASK].iADC_ConversionComplete = false;
	
	IR_Sampling_Start();
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Process_Timer( void )
{
	if( IrProcess.SrcPowerCheckTmr & 0x8000 ){
		if( IrProcess.SrcPowerCheckTmr == 0x8000 ){
			IrProcess.SrcPowerCheckTmr = 0;
			
			ADC1_Conversion_Start();
			
			TP_TMR_T();
		}
		else {
			IrProcess.SrcPowerCheckTmr--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Signal_Processing( void )
{
	U8		bHp = IrProcess.FifoHp & IR_FIFO_MASK;
	
    switch( IrProcess.State ){
        case IR_PROCESS_ADS868x_START_CH_SET:
            ADS868x_Channel_Set( 0 );
            IrProcess.State = IR_PROCESS_WAVE_SENSING;
            break;
            
        case IR_PROCESS_WAVE_SENSING:
			TP_IR_SENSING_H();
            if( IrProcess.iWave < SystemOption.Channels ){
                IrData[bHp].IrRaw[IrProcess.iWave][IrProcess.iBuff] = ADS868x_Manual_Conversion( IrProcess.iWave );
            }
            else {
                IrData[bHp].IrRaw[IrProcess.iWave][IrProcess.iBuff] = 0;
            }
            if( IrProcess.iWave < (IR_WL_MAX - 1) ){
                IrProcess.iWave++;
            }
            else {
                IrProcess.iWave = 0;
				if( IrProcess.iBuff < (IrProcess.BufSize - 1) ){
                    IrProcess.iBuff++;
                }
                else {
                    IrProcess.iBuff = 0;
                    IrProcess.State = IR_PROCESS_ADS868x_TEMP_CH_SET;
                }
            }
            break;
            
        case IR_PROCESS_ADS868x_TEMP_CH_SET:
            ADS868x_Channel_Set( 8 );
            IrProcess.State = IR_PROCESS_SENSOR_TEMP_SENSING;
            break;
            
        case IR_PROCESS_SENSOR_TEMP_SENSING:
            IrData[bHp].SenTempRaw = ADS868x_Temp_Read();
            IrProcess.State = IR_PROCESS_ADS868x_GAIN_CHECK_CH0;
            break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH0:
			GainErr.Bit.B0 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_0 );
			IrProcess.State++;
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH1:
			GainErr.Bit.B1 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_1 );
			IrProcess.State++;
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH2:
			GainErr.Bit.B2 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_2 );
			IrProcess.State++;
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH3:
			GainErr.Bit.B3 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_3 );
			if( SystemOption.Channels > 4 ){
				IrProcess.State++;
			}
			else {
				if( GainErr.Byte ){
					IrProcess.State = IR_PROCESS_ADS868x_INIT_AUTO_SEQ_EN;
				}
				else {
					IrProcess.State = IR_PROCESS_IDLE;
					IrProcess.FifoHp++;
				}
			}
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH4:
			GainErr.Bit.B4 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_4 );
			IrProcess.State = IR_PROCESS_ADS868x_GAIN_CHECK_CH5;
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH5:
			GainErr.Bit.B5 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_5 );
			IrProcess.State = IR_PROCESS_ADS868x_GAIN_CHECK_CH6;
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH6:
			GainErr.Bit.B6 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_6 );
			IrProcess.State = IR_PROCESS_ADS868x_GAIN_CHECK_CH7;
			break;
			
		case IR_PROCESS_ADS868x_GAIN_CHECK_CH7:
			GainErr.Bit.B7 = ADS868x_Gain_Check( ADS868x_PROG_REG_INPUT_RANGE_SELECT_7 );
			if( GainErr.Byte ){
				IrProcess.State = IR_PROCESS_ADS868x_INIT_AUTO_SEQ_EN;
			}
			else {
				IrProcess.State = IR_PROCESS_IDLE;
				IrProcess.FifoHp++;
			}
			break;
            
        case IR_PROCESS_ADS868x_INIT_AUTO_SEQ_EN:
			InitErr.Bit.Auto_Seq_En = ADS868x_Program_Register_Write( ADS868x_PROG_REG_AUTO_SEQ_EN, 0x00 );
			IrProcess.State++;
			break;
            
        case IR_PROCESS_ADS868x_INIT_CH_PWR_DOWN:
			InitErr.Bit.Ch_Pwr_Down = ADS868x_Program_Register_Write( ADS868x_PROG_REG_CH_PWR_DOWN, 0x00 );
			IrProcess.State++;
			break;
            
        case IR_PROCESS_ADS868x_INIT_FEATURE_SELECT:
			InitErr.Bit.Feature_Selection = ADS868x_Program_Register_Write( ADS868x_PROG_REG_FEATURE_SELECT, 0x00 );
			IrProcess.State++;
			break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_0:
			InitErr.Bit.Input_Range_Select_0 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_0, 0x06 );
			IrProcess.State++;
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_1:
			InitErr.Bit.Input_Range_Select_1 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_1, 0x06 );
			IrProcess.State++;
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_2:
            InitErr.Bit.Input_Range_Select_2 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_2, 0x06 );
			IrProcess.State++;
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_3:
            InitErr.Bit.Input_Range_Select_3 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_3, 0x06 );
			if( SystemOption.Channels > 4 ){
				IrProcess.State++;
			}
			else {
				if( InitErr.Word ){
					SystemError.Bit.eADC = 1;
				}
				else {
					SystemError.Bit.eADC = 0;
					IrProcess.State = IR_PROCESS_IDLE;
				}
			}
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_4:
            InitErr.Bit.Input_Range_Select_4 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_4, 0x06 );
			IrProcess.State++;
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_5:
            InitErr.Bit.Input_Range_Select_5 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_5, 0x06 );
			IrProcess.State++;
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_6:
            InitErr.Bit.Input_Range_Select_6 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_6, 0x06 );
			IrProcess.State++;
            break;
            
        case IR_PROCESS_ADS868x_INIT_INPUT_RANGE_7:
            InitErr.Bit.Input_Range_Select_7 = ADS868x_Program_Register_Write( ADS868x_PROG_REG_INPUT_RANGE_SELECT_7, 0x06 );
			if( InitErr.Word ){
				SystemError.Bit.eADC = 1;
			}
			else {
				SystemError.Bit.eADC = 0;
				IrProcess.State = IR_PROCESS_IDLE;
			}
            break;
    }
	
	TP_IR_SENSING_L();
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Task( void )
{                           
	U8				bHp = IrProcess.FifoHp & IR_FIFO_MASK;
	U8				bTp = IrProcess.FifoTp & IR_FIFO_MASK; 
	IR_DataType		*ir;
	
	if( bHp != bTp ){
		
        TP_IR_TASK_H();
		
		ir = &IrData[bTp];
		
		IR_Calculation( ir );
		MODBUS_InputRegister_Mapping( ir );
		IR_Log_Handler( bTp );
		
		IrProcess.FifoTp++;
		
		IrProcess.TaskComplete = true;
    
        TP_IR_TASK_L();
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Source_PWM_Start( void )
{
    if( HAL_TIM_PWM_Start_IT( &TIM1_Handle, TIM_CHANNEL_1 ) != HAL_OK ){ Error_Handler(); }
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IR_Source_PWM_Stop( void )
{
    if( HAL_TIM_PWM_Stop_IT( &TIM1_Handle, TIM_CHANNEL_1 ) != HAL_OK ){ Error_Handler(); }
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Sampling_Start( void )
{                   
    IrProcess.iWave = 0;
    IrProcess.iBuff = 0;
    IrProcess.State = IR_PROCESS_ADS868x_START_CH_SET;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Calculation( IR_DataType *ir )
{   
	IR_DataType	*pir = &IrData[IrProcess.FifoTp-1 & IR_FIFO_MASK];
	
	U16					w, i;
	
	U16					IrBuf, Hpeak, Lpeak;
	float				Average, Havr, Lavr;
	U16					Dv, RawDvMax;
	U8					Hcnt, Lcnt;
	
	U16					TargetOffs[IR_WL_MAX];
	
	if( ir->iADC_ConversionComplete == true ){
		ir->SrcPwr		= ir->SrcPwrRaw * iADC_LSB;
		ir->McuTemp.f	= MCU_Temp_Calculation( ir->McuTempRaw );
	}
	else {
		ir->SrcPwr	= pir->SrcPwr;	
		ir->McuTemp	= pir->McuTemp;
		dprintf( "  Internal ADC conversion have not completed.\n" );
	}
	
	ir->SenTemp.f = LMT84_Temperature_Calculation( ir->SenTempRaw * ADS868x_AUX_LSB );
		
	for( w=0; w<SystemOption.Channels; w++ ){
		
		Lpeak = 65535;
		Hpeak = 0;
		Average = 0;
		
		Havr = 0;
		Lavr = 0;
		Hcnt = 0;
		Lcnt = 0;
		
		Dv = 0;
		RawDvMax = 0;
		
		TargetOffs[w] = (U16)(IrAmpCal.TargetOffs[w] / ADS868x_LSB);
		
		
		for( i=0; i<IrProcess.BufSize; i++ ){
			
			IrBuf = ir->IrRaw[w][i];
			Average += IrBuf;
			if( i ){
				Dv = abs( IrBuf - ir->IrRaw[w][i-1] );
				if( RawDvMax < Dv ){
					RawDvMax = Dv;
				}
			}
			if( IrBuf > Hpeak ){
				Hpeak = IrBuf;
			}
			else if( IrBuf < Lpeak ){
				Lpeak = IrBuf;
			}
			
			if( IrBuf > TargetOffs[w] ){
				Havr += IrBuf;
				Hcnt++;
			}
			else {
				Lavr += IrBuf;
				Lcnt++;
			}
			
			ir->Wave[w][i] = IrBuf / 400; // 31.25mV scale
		}
		
		ir->HiPeak[w].f = (float)Hpeak * ADS868x_LSB;
		ir->LoPeak[w].f = (float)Lpeak * ADS868x_LSB;
		ir->PeakDv[w] = ( ir->HiPeak[w].f - IrAmpCal.TargetOffs[w] ) - ( IrAmpCal.TargetOffs[w] - ir->LoPeak[w].f );
		ir->OcPtoP[w].f = ir->HiPeak[w].f - ir->LoPeak[w].f;
		
		ir->HiTime[w] = Hcnt;
		ir->LoTime[w] = Lcnt;
		
		ir->RawDvMax[w] = RawDvMax;
		
		if( Hcnt ){
			ir->HiAvrg[w].f = Havr / Hcnt * ADS868x_LSB;
		}
		else {
			ir->HiAvrg[w].f = IrAmpCal.TargetOffs[w];
		}
		if( Lcnt ){
			ir->LoAvrg[w].f = Lavr / Lcnt * ADS868x_LSB;
		}
		else {
			ir->LoAvrg[w].f = IrAmpCal.TargetOffs[w];
		}
		ir->AvPtoP[w].f = ir->HiAvrg[w].f - ir->LoAvrg[w].f;
		ir->OcAvrg[w].f = Average / IrProcess.BufSize * ADS868x_LSB;
		
		IR_Amp_Cal_Handler( ir, w );
	}
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Log_Handler( U8 bTp )
{
	IR_DataType	*ir = &IrData[bTp];
	U8					w;
	U16					i;
	bool				Flag = false;
	                          
	switch( IrProcess.LogMode ){
		case IR_LOG_SERIALSCOPE:
		case IR_LOG_TERATERM_HALF:
			if( bTp & 1 ){
				Flag = true;
			}
			break;
			
		case IR_LOG_TERATERM_FULL:
			Flag = true;
			break;
			
		case IR_LOG_TERATERM_1SEC:
			if( IrProcess.LogHandleCnt < 10 ){
				IrProcess.LogHandleCnt++;
			}
			else {
				IrProcess.LogHandleCnt = 0;
				Flag = true;
			}
			break;
	}
	
	if( Flag  == false ){
		return;
	}
		
	switch( IrProcess.LogMode ){
		case IR_LOG_TERATERM_FULL:
		case IR_LOG_TERATERM_HALF:
		case IR_LOG_TERATERM_1SEC:
			printf( "," );
			for( w=0; w<SystemOption.Channels; w++ ){
			    printf( "%7.4f,%7.1f,%7.1f,    ", ir->OcPtoP[w].f, 5000.f-ir->HiPeak[w].f*1000, ir->LoPeak[w].f*1000 );
			}
			printf( "%6.2f,%6.2f,%5u\n", ir->SenTemp.f, ir->McuTemp.f, (U16)ir->SrcPwr );
			break;
			
		case IR_LOG_SERIALSCOPE:
			for( i=0; i<IrProcess.BufSize; i++ ){
				for( w=0; w<SystemOption.Channels; w++ ){
					printf( "%6.3f", ((float)ir->IrRaw[w][i] * ADS868x_LSB) );
					if( w == SystemOption.Channels-1 ){
						printf( "\r" );
					}
					else {
						printf( "," );
					}
				}
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Offs_Write( U16 i, U8 Data )
{
    TPL0102_WiperUpdate( i, 0, Data );
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Gain_Write( U16 i, U8 Data )
{
    TPL0102_WiperUpdate( i, 1, Data );
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool IR_Amp_Offs_Decrease( U16 i )
{
    bool    err = false;
    
    if( IrAmpCal.Offs[i] > AMP_CAL_PTM_OFFS_DEC_LIMIT ){
        IrAmpCal.Offs[i]--;
        
        IR_Amp_Offs_Write( i, IrAmpCal.Offs[i] );
    }
    else {
        err = true;
    }
    
    return err;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool IR_Amp_Offs_Increase( U16 i )
{
    bool    err = false;
    
    if( IrAmpCal.Offs[i] < AMP_CAL_PTM_OFFS_INC_LIMIT ){
        IrAmpCal.Offs[i]++;
        
        IR_Amp_Offs_Write( i, IrAmpCal.Offs[i] );
    }
    else {
        err = true;
    }
    
    return err;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool IR_Amp_Gain_Decrease( U16 i )
{
    bool    err = false;
    
    if( IrAmpCal.Gain[i] > AMP_CAL_PTM_GAIN_DEC_LIMIT ){
        IrAmpCal.Gain[i]--;
        
        IR_Amp_Gain_Write( i, IrAmpCal.Gain[i] );
    }
    else {
        err = true;
    }
    
    return err;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool IR_Amp_Gain_Increase( U16 i )
{
    bool    err = false;
    
    if( IrAmpCal.Gain[i] < AMP_CAL_PTM_GAIN_INC_LIMIT ){
        IrAmpCal.Gain[i]++;
        
        IR_Amp_Gain_Write( i, IrAmpCal.Gain[i] );
    }
    else {
        err = true;
    }
    
    return err;
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Cal_Handler( IR_DataType *ir, U16 i )
{   
	switch( IrAmpCal.Status[i] ){
	    case AMP_CAL_IDLE:
			IR_Manual_Amp_Set_Check( i );
			IR_Amp_Cal_Start( i );
	        break;
	        
	    case AMP_CAL_OFFS_CHECK_1ST:
	    case AMP_CAL_OFFS_CHECK_2ND:
	        if		( ir->PeakDv[i] >  AMP_CAL_OFFS_MARGIN ){ IrAmpCal.Status[i] +=  9; }
	        else if	( ir->PeakDv[i] < -AMP_CAL_OFFS_MARGIN ){ IrAmpCal.Status[i] += 19; }
			else											{ IrAmpCal.Status[i] += 20; }
	        break;
	        
	    case AMP_CAL_OFFS_DEC_1ST:
	    case AMP_CAL_OFFS_DEC_2ND:
	        if( ir->PeakDv[i] > AMP_CAL_OFFS_MARGIN ){
	            if( IR_Amp_Offs_Decrease( i ) ){ IrAmpCal.Status[i] = AMP_CAL_FAILURE; }
	            else { IrAmpCal.Status[i] -= AMP_CAL_READY; }
	        }
	        else {
	            IrAmpCal.Status[i] += 11;
	        }
	        break;
	        
	    case AMP_CAL_OFFS_INC_1ST:
	    case AMP_CAL_OFFS_INC_2ND:
	        if( ir->PeakDv[i] < -AMP_CAL_OFFS_MARGIN ){
	            if(  IR_Amp_Offs_Increase( i ) ){ IrAmpCal.Status[i] = AMP_CAL_FAILURE; }
	            else { IrAmpCal.Status[i] -= AMP_CAL_READY; }
	        }
	        else {
	            IrAmpCal.Status[i] += 1;
	        }
	        break;
	        
	    case AMP_CAL_GAIN_CHECK_1ST:
	    case AMP_CAL_GAIN_CHECK_2ND:
	        if		( ir->OcPtoP[i].f > (IrAmpCal.TargetGain[i] + AMP_CAL_GAIN_MARGIN) ){ IrAmpCal.Status[i] +=  9; }
	        else if	( ir->OcPtoP[i].f < (IrAmpCal.TargetGain[i] - AMP_CAL_GAIN_MARGIN) ){ IrAmpCal.Status[i] += 19; }
			else																		{ IrAmpCal.Status[i] += 20; }
	        break;
	        
	    case AMP_CAL_GAIN_DEC_1ST:
	    case AMP_CAL_GAIN_DEC_2ND:
	        if( ir->OcPtoP[i].f > (IrAmpCal.TargetGain[i] + AMP_CAL_GAIN_MARGIN) ){
	            if( IR_Amp_Gain_Decrease( i ) ){ IrAmpCal.Status[i] = AMP_CAL_FAILURE; }
	            else { IrAmpCal.Status[i] -= AMP_CAL_READY; }
	        }
	        else {
	            IrAmpCal.Status[i] += 1;
	        }
	        break;
	        
	    case AMP_CAL_GAIN_INC_1ST:
	    case AMP_CAL_GAIN_INC_2ND:
	        if( ir->OcPtoP[i].f < (IrAmpCal.TargetGain[i] - AMP_CAL_GAIN_MARGIN) ){
	            if( IR_Amp_Gain_Increase( i ) ){ IrAmpCal.Status[i] = AMP_CAL_FAILURE; }
	            else { IrAmpCal.Status[i] -= AMP_CAL_READY; }
	        }
	        else {
	            IrAmpCal.Status[i] += 1;
	        }
	        break;
	        
	    case AMP_CAL_SUCCESS:
	        IrAmpCal.ManualOffs[i] = IrAmpCal.Offs[i]; 
	        IrAmpCal.ManualGain[i] = IrAmpCal.Gain[i];
	        IrAmpCal.RunFlag &= ( ~(1 << i) );
	        IrAmpCal.Status[i] = AMP_CAL_IDLE;
			if( IrAmpCal.RunFlag == 0 ){ IR_Amp_Cal_All_Manual_Value_Eep_Update(); }
	        break;
	        
	    case AMP_CAL_FAILURE:
	        IrAmpCal.ManualOffs[i] = IrAmpCal.Offs[i]; 
	        IrAmpCal.ManualGain[i] = IrAmpCal.Gain[i];
	        IrAmpCal.RunFlag &= ( ~(1 << i) );
	        IrAmpCal.ErrFlag |=    (1 << i);
	        IrAmpCal.Status[i] = AMP_CAL_IDLE;
			if( IrAmpCal.RunFlag == 0 ){ IR_Amp_Cal_All_Manual_Value_Eep_Update(); }
	        break;
	        
	    default:
	        IrAmpCal.Status[i]++;
	        break;
	}
	
	TPL0102_WiperHandler( i );
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Cal_Start( U16 i )
{                 
	if( SystemState != SYSTEM_STATE_WARM_UP ){
		
		if( IrAmpCal.RunFlag &     (1 << i) ){
			IrAmpCal.ErrFlag &= ( ~(1 << i) );
			
			IrAmpCal.Offs[i] = 128;
			IrAmpCal.Status[i] = AMP_CAL_OFFS_CHECK_1ST;
			
			IR_Amp_Offs_Write( i, IrAmpCal.Offs[i] );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

static void IR_Manual_Amp_Set_Check( U16 i )
{
	if( IrAmpCal.Status[i] == AMP_CAL_IDLE ){
		
		if( IrAmpCal.Offs[i] != IrAmpCal.ManualOffs[i] ){
			IrAmpCal.Offs[i]  = IrAmpCal.ManualOffs[i];
			
			IR_Amp_Cal_Manual_Offs_Eep_Update( i );
			IR_Amp_Offs_Write( i, IrAmpCal.Offs[i] );
		}
		
		if( IrAmpCal.Gain[i] != IrAmpCal.ManualGain[i] ){
			IrAmpCal.Gain[i]  = IrAmpCal.ManualGain[i];
			
			IR_Amp_Cal_Manual_Gain_Eep_Update( i );
			IR_Amp_Gain_Write( i, IrAmpCal.ManualGain[i] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Cal_All_Manual_Value_Eep_Update( void )
{
	U16		eep[2];
	
	eep[0] = IrAmpCal.ManualOffs[IR_WL_1];
	eep[1] = IrAmpCal.ManualGain[IR_WL_1];
	EEPROM_Update( eep, EEP_IR1_AMP_CAL_PTM_MANUAL_OFFS, 2 );
	
	eep[0] = IrAmpCal.ManualOffs[IR_WL_2];
	eep[1] = IrAmpCal.ManualGain[IR_WL_2];
	EEPROM_Update( eep, EEP_IR2_AMP_CAL_PTM_MANUAL_OFFS, 2 );
	
	eep[0] = IrAmpCal.ManualOffs[IR_WL_3];
	eep[1] = IrAmpCal.ManualGain[IR_WL_3];
	EEPROM_Update( eep, EEP_IR3_AMP_CAL_PTM_MANUAL_OFFS, 2 );
	
	eep[0] = IrAmpCal.ManualOffs[IR_WL_4];
	eep[1] = IrAmpCal.ManualGain[IR_WL_4];
	EEPROM_Update( eep, EEP_IR4_AMP_CAL_PTM_MANUAL_OFFS, 2 );
	
	eep[0] = 0;
	EEPROM_Update( eep, EEP_SYS_AMP_CAL_RUN_FLAG, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Cal_Manual_Offs_Eep_Update( U16 i )
{
	U16		eep;
	
	eep = IrAmpCal.ManualOffs[i];
	EEPROM_Update( &eep, EEPROM_Block_Address( i ) + EEP_IR1_AMP_CAL_PTM_MANUAL_OFFS, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Amp_Cal_Manual_Gain_Eep_Update( U16 i )
{
	U16		eep;
	
	eep = IrAmpCal.ManualGain[i];
	EEPROM_Update( &eep, EEPROM_Block_Address( i ) + EEP_IR1_AMP_CAL_PTM_MANUAL_GAIN, 1 );
}


