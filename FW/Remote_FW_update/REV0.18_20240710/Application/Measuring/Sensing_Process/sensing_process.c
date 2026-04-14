#define __SENSING_PROCESS_C__
	#include "sensing_process.h"
#undef  __SENSING_PROCESS_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "flash.h"
#include "sensor_list.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void Sensing_Parameter_Update( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Sensing_Process_Init( void )
{
	SensingProcess.Conf[0].SenNo		= Flash_Handle.Sram[FLASH_ADDR_SYS_CTG_SEN_TYPE_0];
	SensingProcess.Conf[0].DAC.VZERO	= Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_OFFS];
	SensingProcess.Conf[0].DAC.VBIAS	= Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_BIAS];
	SensingProcess.Conf[0].TIA.Word[0]	= Flash_Handle.Sram[FLASH_ADDR_CH0_PTM_mOFFS_TIA_0];
	SensingProcess.Conf[0].TIA.Word[1]	= Flash_Handle.Sram[FLASH_ADDR_CH0_PTM_mGAIN_TIA_1];
	SensingProcess.Conf[0].TargetGain	= Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_GAIN];
	
	LPDAC_Config( 0, &SensingProcess.Conf[0].DAC );
	LPTIA_Config( 0, &SensingProcess.Conf[0].TIA );
	
	SensingProcess.Conf[1].SenNo		= Flash_Handle.Sram[FLASH_ADDR_SYS_CTG_SEN_TYPE_1];
	SensingProcess.Conf[1].DAC.VZERO	= Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_OFFS];
	SensingProcess.Conf[1].DAC.VBIAS	= Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_BIAS];
	SensingProcess.Conf[1].TIA.Word[0]	= Flash_Handle.Sram[FLASH_ADDR_CH1_PTM_mOFFS_TIA_0];
	SensingProcess.Conf[1].TIA.Word[1]	= Flash_Handle.Sram[FLASH_ADDR_CH1_PTM_mGAIN_TIA_1];
	SensingProcess.Conf[1].TargetGain	= Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_GAIN];
	
	LPDAC_Config( 1, &SensingProcess.Conf[1].DAC );
	LPTIA_Config( 1, &SensingProcess.Conf[1].TIA );
	
	SensingProcess.pADC					= &ADC_Handle;
	SensingProcess.pHS300x				= &HS300x;
	
	SensingProcess.State				= SENSING_STATE_IDLE;
	
	SensingProcess.Hp					= 0;
	SensingProcess.Tp					= 0;
	
	SensingProcess.ADC_BuffSize			= Flash_Handle.Sram[FLASH_ADDR_SYS_ADC_BUFF_SIZE];
	SensingProcess.Period				= Flash_Handle.Sram[FLASH_ADDR_SYS_SENSING_PERIOD] - 1;
	SensingProcess.Period_Cnt			= SensingProcess.Period;
	
	SensingProcess.DAC_Reset_Cnt		= Flash_Handle.Sram[FLASH_ADDR_SYS_DAC_RESET_CNT];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Sensing_Process( void )
{
	U8		Hp = SensingProcess.Hp & SENSING_RAW_FIFO_MASK;
	
	if( SensingProcess.Period_Cnt ){
		SensingProcess.Period_Cnt--;
	}
	else {
		if( SensingProcess.State == SENSING_STATE_IDLE ){
			Sensing_Parameter_Update();
			SensingProcess.Period_Cnt = SensingProcess.Period;
			SensingProcess.State  = SENSING_STATE_HS300x_MEAS_REQUESTS;
		}
	}
	
	switch( SensingProcess.State ){
		case SENSING_STATE_HS300x_MEAS_REQUESTS:
			SensingProcess.pHS300x->CmdMR = true;
			SensingProcess.State = SENSING_STATE_ADC_CONVERSION_START;
			break;
			
		case SENSING_STATE_ADC_CONVERSION_START:
			if( SensingProcess.pHS300x->CmdMR == false ){
				SensingProcess.ADC_RawIndex = 0;
				SensingProcess.State = SENSING_STATE_LOAD_ADC;
				ADC_Conversion_Ready();
			}
			break;
			
		case SENSING_STATE_LOAD_ADC:
			if( SensingProcess.pADC->ConvComplete == true ){
				SensingProcess.pADC->ConvComplete = false;
				SensingProcess.Raw[Hp].ADC[SensingProcess.ADC_RawIndex] = SensingProcess.pADC->Raw;
				SensingProcess.ADC_RawIndex++;
				if( SensingProcess.ADC_RawIndex == SensingProcess.ADC_BuffSize ){
					SensingProcess.pHS300x->CmdDF = true;
					SensingProcess.State = SENSING_STATE_LOAD_HS300x;
				}
				else {
					ADC_Conversion_Ready();
				}
			}
			break;
			
		case SENSING_STATE_LOAD_HS300x:
			if( SensingProcess.pHS300x->CmdDF == false ){
				SensingProcess.Raw[Hp].HS300x_Humi = SensingProcess.pHS300x->Humi.w;
				SensingProcess.Raw[Hp].HS300x_Temp = SensingProcess.pHS300x->Temp.w;
				SensingProcess.State = SENSING_STATE_IDLE;
				SensingProcess.Hp++;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Sensing_Parameter_Update( void )
{
	if( SensingProcess.ADC_BuffSize != Flash_Handle.Sram[FLASH_ADDR_SYS_ADC_BUFF_SIZE] ){
		SensingProcess.ADC_BuffSize  = Flash_Handle.Sram[FLASH_ADDR_SYS_ADC_BUFF_SIZE];
	}
	if( SensingProcess.Period != (Flash_Handle.Sram[FLASH_ADDR_SYS_SENSING_PERIOD] - 1) ){
		SensingProcess.Period  = (Flash_Handle.Sram[FLASH_ADDR_SYS_SENSING_PERIOD] - 1);
	}
	
	if( SensingProcess.DAC_Reset_Cnt != Flash_Handle.Sram[FLASH_ADDR_SYS_DAC_RESET_CNT] ){
		SensingProcess.DAC_Reset_Cnt  = Flash_Handle.Sram[FLASH_ADDR_SYS_DAC_RESET_CNT];
		LPDAC_Config( 0, &SensingProcess.Conf[0].DAC );
		LPDAC_Config( 1, &SensingProcess.Conf[1].DAC );
	}
	
	if( SensingProcess.Conf[0].SenNo != Flash_Handle.Sram[FLASH_ADDR_SYS_CTG_SEN_TYPE_0] ){
		SensingProcess.Conf[0].SenNo  = Flash_Handle.Sram[FLASH_ADDR_SYS_CTG_SEN_TYPE_0];
	}
	if( SensingProcess.Conf[1].SenNo != Flash_Handle.Sram[FLASH_ADDR_SYS_CTG_SEN_TYPE_1] ){
		SensingProcess.Conf[1].SenNo  = Flash_Handle.Sram[FLASH_ADDR_SYS_CTG_SEN_TYPE_1];
	}
	
	if( SensingProcess.Conf[0].TargetGain != Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_GAIN] ){
		SensingProcess.Conf[0].TargetGain  = Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_GAIN];
	}
	if( SensingProcess.Conf[1].TargetGain != Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_GAIN] ){
		SensingProcess.Conf[1].TargetGain  = Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_GAIN];
	}
	if( (SensingProcess.Conf[0].DAC.VZERO != Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_OFFS]) ||
		(SensingProcess.Conf[0].DAC.VBIAS != Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_BIAS]) ){
		 SensingProcess.Conf[0].DAC.VZERO  = Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_OFFS];
		 SensingProcess.Conf[0].DAC.VBIAS  = Flash_Handle.Sram[FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_BIAS];
		 
		LPDAC_Config( 0, &SensingProcess.Conf[0].DAC );
	}
	if( (SensingProcess.Conf[1].DAC.VZERO != Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_OFFS]) ||
		(SensingProcess.Conf[1].DAC.VBIAS != Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_BIAS]) ){
		 SensingProcess.Conf[1].DAC.VZERO  = Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_OFFS];
		 SensingProcess.Conf[1].DAC.VBIAS  = Flash_Handle.Sram[FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_BIAS];
		 
		LPDAC_Config( 1, &SensingProcess.Conf[1].DAC );
	}
	
	if( (SensingProcess.Conf[0].TIA.Word[0] != Flash_Handle.Sram[FLASH_ADDR_CH0_PTM_mOFFS_TIA_0]) ||
		(SensingProcess.Conf[0].TIA.Word[1] != Flash_Handle.Sram[FLASH_ADDR_CH0_PTM_mGAIN_TIA_1]) ){
		 SensingProcess.Conf[0].TIA.Word[0]  = Flash_Handle.Sram[FLASH_ADDR_CH0_PTM_mOFFS_TIA_0];
		 SensingProcess.Conf[0].TIA.Word[1]  = Flash_Handle.Sram[FLASH_ADDR_CH0_PTM_mGAIN_TIA_1];
		 
		LPTIA_Config( 0, &SensingProcess.Conf[0].TIA );
	}
	if( (SensingProcess.Conf[1].TIA.Word[0] != Flash_Handle.Sram[FLASH_ADDR_CH1_PTM_mOFFS_TIA_0]) ||
		(SensingProcess.Conf[1].TIA.Word[1] != Flash_Handle.Sram[FLASH_ADDR_CH1_PTM_mGAIN_TIA_1]) ){
		 SensingProcess.Conf[1].TIA.Word[0]  = Flash_Handle.Sram[FLASH_ADDR_CH1_PTM_mOFFS_TIA_0];
		 SensingProcess.Conf[1].TIA.Word[1]  = Flash_Handle.Sram[FLASH_ADDR_CH1_PTM_mGAIN_TIA_1];
		 
		LPTIA_Config( 1, &SensingProcess.Conf[1].TIA );
	}
}


