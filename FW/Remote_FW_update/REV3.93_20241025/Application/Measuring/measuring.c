#define __MEASURING_C__
    #include "measuring.h"
#undef  __MEASURING_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "eeprom.h"
#include "gdi.h"
#include "data_log.h"
#include "modbus_map.h"
#include "function_mode.h"
#include "vmeasuring.h"
#include "option.h"
#include "power_control.h"
/* End Include */

/* Start Define */
#define CtSenLpfCurWeight				( MeasHandle.CtSenLpfWeight )
#define IrSenLpfCurWeight				( MeasHandle.IrSenLpfWeight )
#define OtherLpfCurWeight				( MeasHandle.OtherLpfWeight )
#define CtSenLpfAccWeight				( 1.f - CtSenLpfCurWeight )
#define IrSenLpfAccWeight				( 1.f - IrSenLpfCurWeight )
#define OtherLpfAccWeight				( 1.f - OtherLpfCurWeight )
#define CtSenLpf( Acc, Cur )			( Acc * CtSenLpfAccWeight + Cur * CtSenLpfCurWeight )
#define IrSenLpf( Acc, Cur )			( Acc * IrSenLpfAccWeight + Cur * IrSenLpfCurWeight )
#define OtherLpf( Acc, Cur )			( Acc * OtherLpfAccWeight + Cur * OtherLpfCurWeight )

#define	SecToMeasuringTime( sec )		( sec * CavityHandle.Inf[CAVITY_INF_SRC_FREQ] )

#define DAC_ERR_DEC_COUNTS				( 5 )
#define DAC_ERR_DEC_TIME				SecToMeasuringTime(   10 )
#define TEMP_ERR_DEC_TIME				SecToMeasuringTime(   30 )
#define SYS_PWR_ERR_DEC_TIME			SecToMeasuringTime(    5 )
#define SRC_PWR_ERR_DEC_TIME			SecToMeasuringTime(  300 )
#define OFFSET_DRF_DEC_TIME				SecToMeasuringTime(  600 )
#define OFFSET_INSTABILITY_CHK_TIME		SecToMeasuringTime(   30 )
#define OFFSET_INSTABILITY_DEC_TIME		SecToMeasuringTime(  600 )
#define EXC_SIGNAL_DEC_TIME				SecToMeasuringTime(  600 )
#define NOT_STABLE_CHK_TIME				SecToMeasuringTime(   30 )
#define PENDING_DEC_TIME				SecToMeasuringTime( 7200 )
#define OPT_PATH_ERR_DEC_TIME			SecToMeasuringTime(  600 )
#define EXC_CURRENT_DEC_TIME			SecToMeasuringTime(    5 )
#define GAS_DEV_DEC_TIME				SecToMeasuringTime(   30 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static bool Channel_Config_Load_Cartridge( Measuring_ConfType *pConf );
static bool Channel_Config_Load_Cavity( Measuring_ConfType *pConf );
static bool Channel_Config_Load_Combined( Measuring_ConfType *pConf );
static void Default_Gas_Info_Load_Cartridge( Measuring_GasInfo *pGas, Measuring_AlarmInfo *pAlr );
static void Default_Gas_Info_Load_Cavity( Measuring_GasInfo *pGas, Measuring_AlarmInfo *pAlr );
static void Gas_Info_Load( void );
static void Alr_Info_Load( void );
static void Gas_Info_Check( void );
static void Raw_Mapping( void );
static void Lowpass_Filtering( void );
static void Base_Zero_Handler( void );
static float CT_Gas_Calculation( Measuring_GasInfo *pInf, float d );
static void IR_Calculation_Paramter_Load( Measuring_GasInfo *pInf );
static float IR_Gas_Calculation( Measuring_GasInfo *pInf, float d, float r );
static void Gas_Calculation( void );
static void Measuring_Deviation_Calculation( void );
static void Interference_Gas_Filtering( Measuring_FilterItem *pf );
static void Measuring_Filter_Com( Measuring_FilterItem *pf );
static void Measuring_Filter_Ctg( Measuring_FilterItem *pf );
static void Measuring_Filter_Cvt( Measuring_FilterItem *pf );
static void Measuring_Filter_Decision( void );
static void Output_Gas_Decision( void );
static void Ctg_Dac_Reset( void );
/* End Function */

/* Start Variable */
static U16			Maintenance_QuitSec;
static const float	LPTIA_Resistance[LPTIA_RGAIN_MAX] = {
	1000000000, //LPTIA_RGAIN_DISCONNECT = 0U,
	       200, //LPTIA_RGAIN_200,
	      1000, //LPTIA_RGAIN_1K,
	      2000, //LPTIA_RGAIN_2K,
	      3000, //LPTIA_RGAIN_3K,
	      4000, //LPTIA_RGAIN_4K,
	      6000, //LPTIA_RGAIN_6K,
	      8000, //LPTIA_RGAIN_8K,
	     10000, //LPTIA_RGAIN_10K,
	     12000, //LPTIA_RGAIN_12K,
	     16000, //LPTIA_RGAIN_16K,
	     20000, //LPTIA_RGAIN_20K,
	     24000, //LPTIA_RGAIN_24K,
	     30000, //LPTIA_RGAIN_30K,
	     32000, //LPTIA_RGAIN_32K,
	     40000, //LPTIA_RGAIN_40K,
	     48000, //LPTIA_RGAIN_48K,
	     64000, //LPTIA_RGAIN_64K,
	     85000, //LPTIA_RGAIN_85K,
	     96000, //LPTIA_RGAIN_96K,
	    100000, //LPTIA_RGAIN_100K,
	    120000, //LPTIA_RGAIN_120K,
	    128000, //LPTIA_RGAIN_128K,
	    160000, //LPTIA_RGAIN_160K,
	    196000, //LPTIA_RGAIN_196K,
	    256000, //LPTIA_RGAIN_256K,
	    512000, //LPTIA_RGAIN_512K,
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Config( void )
{
	U8		i;
	U16		eep;
	
	MeasHandle.pSen = &SensingProcess;
	
	for( i=0; i<CARTRIDGE_SEN_CH_MAX; i++ ){
		if( i < MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] ){
			MeasHandle.Gas[i].IsPresent =  true;
		}
		else {
			MeasHandle.Gas[i].IsPresent = false;
		}
	}
	for( i=0; i<CAVITY_SEN_CH_MAX; i++ ){
		if( i < MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] ){
			MeasHandle.Gas[CARTRIDGE_SEN_CH_MAX + i].IsPresent =  true;
		}
		else {
			MeasHandle.Gas[CARTRIDGE_SEN_CH_MAX + i].IsPresent = false;
		}
	}
	
	MeasHandle.bZero.NotStable = true;
	MeasHandle.bZero.StableTmr = SecToMeasuringTime( 600 );
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){ 
			MeasHandle.bZero.WarmUpEnd[i] = false;
			MeasHandle.bZero.WarmUpTmr[i] = SecToMeasuringTime( 3600 );
		}
		else { 
			MeasHandle.bZero.WarmUpEnd[i] = true;
			MeasHandle.bZero.WarmUpTmr[i] = 0;
		}
	}
	
	if( EepHandle.Sram[SYSTEM_EEP_SENSOR_CT_SEN_LPF_WEIGHT_x1000] == 0 ){
		eep = 160;
		EEPROM_Update( &eep, SYSTEM_EEP_SENSOR_CT_SEN_LPF_WEIGHT_x1000, 1 );
		MeasHandle.CtSenLpfWeight = 0.16f;
	}
	else {
		MeasHandle.CtSenLpfWeight = EepHandle.Sram[SYSTEM_EEP_SENSOR_CT_SEN_LPF_WEIGHT_x1000] * 0.001f;
	}
	MeasHandle.IrSenLpfWeight = EepHandle.Sram[SYSTEM_EEP_SENSOR_IR_SEN_LPF_WEIGHT_x1000] * 0.001f;
	MeasHandle.OtherLpfWeight = EepHandle.Sram[SYSTEM_EEP_SENSOR_OTHERS_LPF_WEIGHT_x1000] * 0.001f;
	
	for( i=0; i<MEASURING_LPF_BUF_SIZE; i++ ){
		MeasHandle.Lpf[i].VBAT = PwrCtrl.BQ.ADCs.VBAT.w * 0.001f;
	}
	MeasHandle.LpfIndex = 0;
	
	MeasHandle.Conf.SetNo.Ctg[0] = MeasHandle.pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_SYS][SENSOR_EEP_SYS_CTG_CONF_NO_0];
	MeasHandle.Conf.SetNo.Ctg[1] = MeasHandle.pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_SYS][SENSOR_EEP_SYS_CTG_CONF_NO_1];
	MeasHandle.Conf.SetNo.Cvt = MeasHandle.pSen->pCvt->Eep[CAVITY_EEP_BLK_SYS][SENSOR_EEP_SYS_CVT_CONF_NO];
	//MeasHandle.Conf.SetNo.Ctg[0] = EepHandle.Sram[SYSTEM_EEP_RES_68];
	//MeasHandle.Conf.SetNo.Ctg[1] = EepHandle.Sram[SYSTEM_EEP_RES_69];
	//MeasHandle.Conf.SetNo.Cvt = EepHandle.Sram[SYSTEM_EEP_RES_70];
	
	if( Measuring_Channel_Config_Load( &MeasHandle.Conf ) == true ){
		MeasHandle.Conf.SetNo.Ctg[0] = 0;
		MeasHandle.Conf.SetNo.Ctg[1] = 0;
		MeasHandle.Conf.SetNo.Cvt = 0;
		Measuring_Channel_Config_Load( &MeasHandle.Conf );
		SenEEP_Write_Ctg( &MeasHandle.Conf.SetNo.Ctg[0], CARTRIDGE_EEP_BLK_SYS, SENSOR_EEP_SYS_CTG_CONF_NO_0, 2 );
		SenEEP_Write_Cvt( &MeasHandle.Conf.SetNo.Cvt, CAVITY_EEP_BLK_SYS, SENSOR_EEP_SYS_CVT_CONF_NO, 1 );
		//EEPROM_Update( &MeasHandle.Conf.SetNo.Ctg[0], SYSTEM_EEP_RES_68, 2 );
		//EEPROM_Update( &MeasHandle.Conf.SetNo.Cvt, SYSTEM_EEP_RES_70, 1 );
	}
	
	Measuring_Output_Channel_Mapping();
	Measuring_Gas_Function_Load();
	
	Gas_Info_Load();
	Alr_Info_Load();
	Gas_Info_Check();
	
	Data_Logger_Config();
	
	MeasHandle.Filter.Idx = 0;
	MeasHandle.Filter.pFlow = &FlowCtrl;
	MeasHandle.Filter.CtgDacErrCnt = 0;
	MeasHandle.Filter.CtgDacErrTmr = 0;
	MeasHandle.Filter.CtgGasDevTmr.Word = 0;
	MeasHandle.Filter.CtgTempErrTmr = 0;
	MeasHandle.Filter.CtgExcCurrentTmr[0] = 0;
	MeasHandle.Filter.CtgExcCurrentTmr[1] = 0;
	MeasHandle.Filter.CvtTempErrTmr = 0;
	MeasHandle.Filter.SysPwrErrTmr = 0;
	MeasHandle.Filter.SrcPwrErrTmr = 0;
	MeasHandle.Filter.CvtOffsDriftTmr[0] = 0;
	MeasHandle.Filter.CvtOffsDriftTmr[1] = 0;
	MeasHandle.Filter.CvtOffsDriftTmr[2] = 0;
	MeasHandle.Filter.CvtOffsDriftTmr[3] = 0;
	MeasHandle.Filter.CvtOptPthErrTmr[0] = 0;
	MeasHandle.Filter.CvtOptPthErrTmr[1] = 0;
	MeasHandle.Filter.CvtOptPthErrTmr[2] = 0;
	MeasHandle.Filter.CvtOptPthErrTmr[3] = 0;
	MeasHandle.Filter.CvtOffsInstabilityTmr = 0;
	MeasHandle.Filter.ExcSignalTmr[0] = 0;
	MeasHandle.Filter.ExcSignalTmr[1] = 0;
	MeasHandle.Filter.ExcSignalTmr[2] = 0;
	MeasHandle.Filter.ExcSignalTmr[3] = 0;
	MeasHandle.Filter.ExcSignalTmr[4] = 0;
	MeasHandle.Filter.ExcSignalTmr[5] = 0;
	MeasHandle.Filter.NotStableTmr[0] = 0;
	MeasHandle.Filter.NotStableTmr[1] = 0;
	MeasHandle.Filter.NotStableTmr[2] = 0;
	MeasHandle.Filter.NotStableTmr[3] = 0;
	MeasHandle.Filter.NotStableTmr[4] = 0;
	MeasHandle.Filter.NotStableTmr[5] = 0;
	MeasHandle.Filter.PendingTmr = 0;
	
	if( EepHandle.Sram[SYSTEM_EEP_MEASURING_NOT_STABLE_DEC_TIME] == 0 ){
		MeasHandle.Filter.NotStableDecTime = 18000;
		EEPROM_Update( &MeasHandle.Filter.NotStableDecTime, SYSTEM_EEP_MEASURING_NOT_STABLE_DEC_TIME, 1 );
	}
	else {
		MeasHandle.Filter.NotStableDecTime = EepHandle.Sram[SYSTEM_EEP_MEASURING_NOT_STABLE_DEC_TIME];
	}
	
	MeasHandle.Mode.Word = EepHandle.Sram[SYSTEM_EEP_MEASURING_MODE];
	MeasHandle.Test.Word = 0;
	
	Measuring_Maintenance_Quit_Time_Refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Measuring_Channel_Config_Load( Measuring_ConfType *pConf )
{
	bool				err = false;
	U8					SensorCombination = 0;
	U8					i;
	
	if( MeasHandle.pSen->pCtg->IsPresent == true ){ SensorCombination |= 1; }
	if( MeasHandle.pSen->pCvt->IsPresent == true ){ SensorCombination |= 2; }
	
	switch( SensorCombination ){
		case 0:
			err = true;
			break;
			
		case 1:
			switch( MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] ){
				case 1: pConf->SenComb = MEASURING_SEN_COMB_CTG_SING; break;
				case 2: pConf->SenComb = MEASURING_SEN_COMB_CTG_DUAL; break;
				default: err = true; break;
			}
			break;
			
		case 2:
			switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] ){
				case  2: pConf->SenComb = MEASURING_SEN_COMB_CVT_DUAL; break;
				case  4: pConf->SenComb = MEASURING_SEN_COMB_CVT_QUAD; break;
				default: err = true; break;
			}
			break;
			
		case 3:
			switch( MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] ){
				case  1:
					switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] ){
						case  2: pConf->SenComb = MEASURING_SEN_COMB_CTG_SING_CVT_DUAL; break;
						case  4: pConf->SenComb = MEASURING_SEN_COMB_CTG_SING_CVT_QUAD; break;
						default: err = true; break;
					}
					break;
					
				case  2:
					switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] ){
						case  2: pConf->SenComb = MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL; break;
						case  4: pConf->SenComb = MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD; break;
						default: err = true; break;
					}
					break;
					
				default:
					err = true;
					break;
			}
			break;
	}
	
	if( err == false ){
	
		for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
			pConf->SenNo[i] = 0;
			pConf->GasNo[i] = 0;
			pConf->iSpanScale[i] = 0.1f;
			if( i < CARTRIDGE_SEN_CH_MAX ){
				pConf->Func[i] = MEASURING_GAS_FUNC_CT_INT;
			}
			else {
				pConf->Func[i] = MEASURING_GAS_FUNC_IR_INT;
			}
		}
	
		switch( pConf->SenComb ){
			case MEASURING_SEN_COMB_CTG_SING:
			case MEASURING_SEN_COMB_CTG_DUAL:
				err = Channel_Config_Load_Cartridge( pConf );
				break;
				
			case MEASURING_SEN_COMB_CVT_DUAL:
			case MEASURING_SEN_COMB_CVT_QUAD:
				err = Channel_Config_Load_Cavity( pConf );
				break;
				
			case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
			case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
			case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
			case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
				err = Channel_Config_Load_Combined( pConf );
				break;
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Output_Channel_Mapping( void )
{
	MeasHandle.pGas[0] = &MeasHandle.Gas[MeasHandle.Conf.iOut[0]];
	MeasHandle.pGas[1] = &MeasHandle.Gas[MeasHandle.Conf.iOut[1]];
	MeasHandle.pAlr[0] = &MeasHandle.Alr[MeasHandle.Conf.iOut[0]];
	MeasHandle.pAlr[1] = &MeasHandle.Alr[MeasHandle.Conf.iOut[1]];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Gas_Function_Load( void )
{
	U16		i;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		MeasHandle.Gas[i].Inf.Func = MeasHandle.Conf.Func[i];
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Gas_MSD_Load( Measuring_GasInfo *pInf )
{	
	if		( pInf->HighScale > 999 ){ pInf->Msd = 0; }
	else if	( pInf->HighScale >  99 ){ pInf->Msd = 1; }
	else if	( pInf->HighScale >   9 ){ pInf->Msd = 2; }
	else							 { pInf->Msd = 3; }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Default_Gas_Name_Load( Measuring_GasInfo *pInf )
{
	strcpy( (char *)pInf->GasName, "UNDEFINED " );
	
	switch( pInf->Method ){
		case MEASURING_METHOD_IR:
			switch( pInf->SenNo ){
				case IR_SEN_03_330:
					switch( pInf->GasNo ){
						case IR_GAS_03_330_INT:						strcpy( (char *)pInf->GasName, "INT-3330  " ); break;
						case IR_GAS_03_330_LNG_100_LEL:				strcpy( (char *)pInf->GasName, "LNG       " ); break;
						case IR_GAS_03_330_CH4_100_LEL:
						case IR_GAS_03_330_CH4_100_VOL:				strcpy( (char *)pInf->GasName, "CH4       " ); break;
						case IR_GAS_03_330_C4H10_100_LEL:			strcpy( (char *)pInf->GasName, "i-C4H10   " ); break;
						case IR_GAS_03_330_SFA_1_100_LEL:			strcpy( (char *)pInf->GasName, "SFA-1     " ); break;
						case IR_GAS_03_330_4MS_100_LEL:				strcpy( (char *)pInf->GasName, "4MS       " ); break;
						case IR_GAS_03_330_TOLUENE_100_LEL:			strcpy( (char *)pInf->GasName, "TOLUENE   " ); break;
					}
					break;
					
				case IR_SEN_03_370:
					switch( pInf->GasNo ){
						case IR_GAS_03_370_INT:						strcpy( (char *)pInf->GasName, "INT-3370  " ); break;
						case IR_GAS_03_370_GMP_02_3000_PPM:			strcpy( (char *)pInf->GasName, "GMP-02    " ); break;
						case IR_GAS_03_370_OMCTS_100_LEL:			strcpy( (char *)pInf->GasName, "OMCTS     " ); break;
						case IR_GAS_03_370_BTBAS_1000_PPM:			strcpy( (char *)pInf->GasName, "BTBAS     " ); break;
					}
					break;
					
				case IR_SEN_03_420:
					switch( pInf->GasNo ){
						case IR_GAS_03_420_INT:						strcpy( (char *)pInf->GasName, "INT-3420  " ); break;
						case IR_GAS_03_420_ECH_100_LEL:				strcpy( (char *)pInf->GasName, "ECH       " ); break;
					}
					break;
					
				case IR_SEN_03_720:
					switch( pInf->GasNo ){
						case IR_GAS_03_720_INT:						strcpy( (char *)pInf->GasName, "INT-3720  " ); break;
					}
					break;
					
				case IR_SEN_03_900:
					switch( pInf->GasNo ){
						case IR_GAS_03_900_REF:						strcpy( (char *)pInf->GasName, "REF-3900  " ); break;
					}
					break;
					
				case IR_SEN_03_950:
					switch( pInf->GasNo ){
						case IR_GAS_03_950_REF:						strcpy( (char *)pInf->GasName, "REF-3950  " ); break;
					}
					break;
					
				case IR_SEN_04_260:
					switch( pInf->GasNo ){
						case IR_GAS_04_260_INT:						strcpy( (char *)pInf->GasName, "INT-4260  " ); break;
						case IR_GAS_04_260_CO2_5000_PPM:
						case IR_GAS_04_260_CO2_9999_PPM:
						case IR_GAS_04_260_CO2_1_00_VOL:
						case IR_GAS_04_260_CO2_5_0_VOL:				strcpy( (char *)pInf->GasName, "CO2       " ); break;
					}
					break;
					
				case IR_SEN_04_520:
					switch( pInf->GasNo ){
						case IR_GAS_04_520_INT:						strcpy( (char *)pInf->GasName, "INT-4520  " ); break;
						case IR_GAS_04_520_N2O_4000_PPM:
						case IR_GAS_04_520_N2O_1000_PPM:			strcpy( (char *)pInf->GasName, "N2O       " ); break;
					}
					break;
					
				case IR_SEN_04_660:
					switch( pInf->GasNo ){
						case IR_GAS_04_660_INT:						strcpy( (char *)pInf->GasName, "INT-4660  " ); break;
						case IR_GAS_04_660_CO_9999_PPM:
						case IR_GAS_04_660_CO_1_00_VOL:
						case IR_GAS_04_660_CO_2_00_VOL:
						case IR_GAS_04_660_CO_5_00_VOL:				strcpy( (char *)pInf->GasName, "CO        " ); break;
					}
					break;
					
				case IR_SEN_04_850:
					switch( pInf->GasNo ){
						case IR_GAS_04_850_INT:						strcpy( (char *)pInf->GasName, "INT-4850  " ); break;
						case IR_GAS_04_850_COS_200_PPM:
						case IR_GAS_04_850_COS_75_0_PPM:			strcpy( (char *)pInf->GasName, "COS       " ); break;
					}
					break;
					
				case IR_SEN_08_000:
					switch( pInf->GasNo ){
						case IR_GAS_08_000_INT:						strcpy( (char *)pInf->GasName, "INT-8000  " ); break;
						case IR_GAS_08_000_C4F8_2000_PPM:			strcpy( (char *)pInf->GasName, "C4F8      " ); break;
						case IR_GAS_08_000_FL300_200_PPM:			strcpy( (char *)pInf->GasName, "FL300     " ); break;
						case IR_GAS_08_000_CF4_500_PPM:				strcpy( (char *)pInf->GasName, "CF4       " ); break;
					}
					break;
					
				case IR_SEN_08_925:
					switch( pInf->GasNo ){
						case IR_GAS_08_925_INT:						strcpy( (char *)pInf->GasName, "INT-8925  " ); break;
						case IR_GAS_08_925_C5F8_2000_PPM:			strcpy( (char *)pInf->GasName, "C5F8      " ); break;
						case IR_GAS_08_925_C4F6_300_PPM:
						case IR_GAS_08_925_C4F6_2000_PPM:			strcpy( (char *)pInf->GasName, "C4F6      " ); break;
						case IR_GAS_08_925_CH2F2_2000_PPM:			strcpy( (char *)pInf->GasName, "CH2F2     " ); break;
						case IR_GAS_08_925_TEOS_100_PPM:			strcpy( (char *)pInf->GasName, "TEOS      " ); break;
						case IR_GAS_08_925_TEOS_HT200_100_PPM:		strcpy( (char *)pInf->GasName, "TEOS-HT200" ); break;
						case IR_GAS_08_925_HFC_2000_PPM:			strcpy( (char *)pInf->GasName, "HFC       " ); break;
						case IR_GAS_08_925_IPA_3000_PPM:
						case IR_GAS_08_925_IPA_100_LEL:				strcpy( (char *)pInf->GasName, "IPA       " ); break;
						case IR_GAS_08_925_SELITANE_100_LEL:		strcpy( (char *)pInf->GasName, "SELITANE  " ); break;
					}
					break;
					
				case IR_SEN_09_076:
					switch( pInf->GasNo ){
						case IR_GAS_09_076_INT:						strcpy( (char *)pInf->GasName, "INT-9076  " ); break;
						case IR_GAS_09_076_C4F6S_2000_PPM:			strcpy( (char *)pInf->GasName, "C4F6S     " ); break;
					}
					break;
					
				case IR_SEN_09_150:
					switch( pInf->GasNo ){
						case IR_GAS_09_150_INT:						strcpy( (char *)pInf->GasName, "INT-9150  " ); break;
						case IR_GAS_09_150_C4H2F6_1000_PPM:			strcpy( (char *)pInf->GasName, "C4H2F6    " ); break;
					}
					break;
					
				case IR_SEN_09_230:
					switch( pInf->GasNo ){
						case IR_GAS_09_230_INT:						strcpy( (char *)pInf->GasName, "INT-9230  " ); break;
						case IR_GAS_09_230_OMCTS_75_0_PPM:			strcpy( (char *)pInf->GasName, "OMCTS     " ); break;
					}
					break;
					
				case IR_SEN_09_440:
					switch( pInf->GasNo ){
						case IR_GAS_09_440_INT:						strcpy( (char *)pInf->GasName, "INT-9440  " ); break;
						case IR_GAS_09_440_CH3F_2000_PPM:			strcpy( (char *)pInf->GasName, "CH3F      " ); break;
						case IR_GAS_09_440_ETHANOL_100_LEL:			strcpy( (char *)pInf->GasName, "ETHANOL   " ); break;
						case IR_GAS_09_440_CTFE_1000_PPM:			strcpy( (char *)pInf->GasName, "CTFE      " ); break;
						case IR_GAS_09_440_C3F6_2000_PPM:			strcpy( (char *)pInf->GasName, "C3F6      " ); break;
					}
					break;
					
				case IR_SEN_09_790:
					switch( pInf->GasNo ){
						case IR_GAS_09_790_INT:						strcpy( (char *)pInf->GasName, "INT-9790  " ); break;
						case IR_GAS_09_790_C3F6_2000_PPM:			strcpy( (char *)pInf->GasName, "C3F6      " ); break;
					}
					break;
					
				case IR_SEN_10_100:
					switch( pInf->GasNo ){
						case IR_GAS_10_100_INT:						strcpy( (char *)pInf->GasName, "INT-1010  " ); break;
						case IR_GAS_10_100_METHANOL_3000_PPM:		strcpy( (char *)pInf->GasName, "METHANOL  " ); break;
					}
					break;
					
				case IR_SEN_10_990:
					switch( pInf->GasNo ){
						case IR_GAS_10_990_INT:						strcpy( (char *)pInf->GasName, "INT-1099  " ); break;
						case IR_GAS_10_990_NF3_200_PPM:				strcpy( (char *)pInf->GasName, "NF3       " ); break;
						case IR_GAS_10_990_HMDS_1000_PPM:
						case IR_GAS_10_990_HMDS_100_LEL:			strcpy( (char *)pInf->GasName, "HMDS      " ); break;
						case IR_GAS_10_990_C3F6O_5000_PPM:			strcpy( (char *)pInf->GasName, "C3F6O     " ); break;
						case IR_GAS_10_990_C3H6_100_LEL:			strcpy( (char *)pInf->GasName, "C3H6      " ); break;
						case IR_GAS_10_990_C2H4_100_LEL:			strcpy( (char *)pInf->GasName, "C2H4      " ); break;
					}
					break;
			}
			break;
			
		case MEASURING_METHOD_CT:
			switch( pInf->SenNo ){
				case CT_SEN_EC_DC_CL2_3:
					switch( pInf->GasNo ){
						case EC_GAS_EC_DC_CL2_3_INT:				strcpy( (char *)pInf->GasName, "INT-CL2   " ); break;
						case EC_GAS_EC_DC_CL2_3_BR2_3_00_PPM:		strcpy( (char *)pInf->GasName, "Br2       " ); break;
						case EC_GAS_EC_DC_CL2_3_CL2_3_00_PPM:		strcpy( (char *)pInf->GasName, "Cl2       " ); break;
					}
					break;
					
				case CT_SEN_EC_DC_H2_2000:
					switch( pInf->GasNo ){
						case EC_GAS_EC_DC_H2_2000_INT:				strcpy( (char *)pInf->GasName, "INT-H2-2K " ); break;
						case EC_GAS_EC_DC_H2_2000_H2_2000_PPM:		strcpy( (char *)pInf->GasName, "H2        " ); break;
					}
					break;
					
				case CT_SEN_EC_DC_HCL_15:
					switch( pInf->GasNo ){
						case EC_GAS_EC_DC_HCL_15_INT:				strcpy( (char *)pInf->GasName, "INT-HCl   " ); break;
						case EC_GAS_EC_DC_HCL_15_BCL3_15_0_PPM:		strcpy( (char *)pInf->GasName, "BCl3      " ); break;
						case EC_GAS_EC_DC_HCL_15_HCDS_20_0_PPM:		strcpy( (char *)pInf->GasName, "HCDS      " ); break;
						case EC_GAS_EC_DC_HCL_15_HCL_10_0_PPM:		strcpy( (char *)pInf->GasName, "HCl       " ); break;
						case EC_GAS_EC_DC_HCL_15_LTO520_50_0_PPM:	strcpy( (char *)pInf->GasName, "LTO520    " ); break;
						case EC_GAS_EC_DC_HCL_15_MCS_15_0_PPM:		strcpy( (char *)pInf->GasName, "MCS       " ); break;
						case EC_GAS_EC_DC_HCL_15_PCL3_15_0_PPM:		strcpy( (char *)pInf->GasName, "PCl3      " ); break;
						case EC_GAS_EC_DC_HCL_15_POCL3_15_0_PPM:	strcpy( (char *)pInf->GasName, "POCl3     " ); break;
						case EC_GAS_EC_DC_HCL_15_SiCL4_15_0_PPM:	strcpy( (char *)pInf->GasName, "SiCl4     " ); break;
						case EC_GAS_EC_DC_HCL_15_SiH2CL2_15_0_PPM:	strcpy( (char *)pInf->GasName, "SiH2Cl2   " ); break;
						case EC_GAS_EC_DC_HCL_15_SiHCL3_15_0_PPM:	strcpy( (char *)pInf->GasName, "SiHCl3    " ); break;
						case EC_GAS_EC_DC_HCL_15_TCS_15_0_PPM:		strcpy( (char *)pInf->GasName, "TCS       " ); break;
						case EC_GAS_EC_DC_HCL_15_TICL4_15_0_PPM:	strcpy( (char *)pInf->GasName, "TiCl4     " ); break;
					}
					break;
					
				case CT_SEN_EC_DC_HF_9:
					switch( pInf->GasNo ){
						case EC_GAS_EC_DC_HF_9_INT:					strcpy( (char *)pInf->GasName, "INT-HF    " ); break;
						case EC_GAS_EC_DC_HF_9_BBR3_9_00_PPM:		strcpy( (char *)pInf->GasName, "BBr3      " ); break;
						case EC_GAS_EC_DC_HF_9_BF3_9_00_PPM:		strcpy( (char *)pInf->GasName, "BF3       " ); break;
						case EC_GAS_EC_DC_HF_9_C2F4O_200_PPM:		strcpy( (char *)pInf->GasName, "C2F4O     " ); break;
						case EC_GAS_EC_DC_HF_9_CH2O2_50_0_PPM:		strcpy( (char *)pInf->GasName, "CH2O2     " ); break;
						case EC_GAS_EC_DC_HF_9_CLF3_1_00_PPM:		strcpy( (char *)pInf->GasName, "ClF3      " ); break;
						case EC_GAS_EC_DC_HF_9_F2_3_00_PPM:			strcpy( (char *)pInf->GasName, "F2        " ); break;
						case EC_GAS_EC_DC_HF_9_GEF4_9_00_PPM:		strcpy( (char *)pInf->GasName, "GeF4      " ); break;
						case EC_GAS_EC_DC_HF_9_HBR_9_00_PPM:		strcpy( (char *)pInf->GasName, "HBr       " ); break;
						case EC_GAS_EC_DC_HF_9_HCOOH_50_0_PPM:		strcpy( (char *)pInf->GasName, "HCOOH     " ); break;
						case EC_GAS_EC_DC_HF_9_HF_6_00_PPM:			strcpy( (char *)pInf->GasName, "HF        " ); break;
						case EC_GAS_EC_DC_HF_9_HNO3_20_0_PPM:		strcpy( (char *)pInf->GasName, "HNO3      " ); break;
						case EC_GAS_EC_DC_HF_9_PF3_9_00_PPM:		strcpy( (char *)pInf->GasName, "PF3       " ); break;
						case EC_GAS_EC_DC_HF_9_SIF4_9_00_PPM:		strcpy( (char *)pInf->GasName, "SiF4      " ); break;
						case EC_GAS_EC_DC_HF_9_WF6_9_00_PPM:		strcpy( (char *)pInf->GasName, "WF6       " ); break;
					}
					break;
					
				case CT_SEN_EC_DC_O3_1:
					switch( pInf->GasNo ){
						case EC_GAS_EC_DC_O3_1_INT:					strcpy( (char *)pInf->GasName, "INT-O3    " ); break;
						case EC_GAS_EC_DC_O3_1_O3_1_00_PPM:			strcpy( (char *)pInf->GasName, "O3        " ); break;
					}
					break;
					
				case CT_SEN_ETO_C_20:
					switch( pInf->GasNo ){
						case EC_GAS_ETO_C_20_INT:					strcpy( (char *)pInf->GasName, "INT-ETO   " ); break;
						case EC_GAS_ETO_C_20_C2H4O_EO_30_0_PPM:		strcpy( (char *)pInf->GasName, "C2H4O     " ); break;
						case EC_GAS_ETO_C_20_C3H7OH_500_PPM:		strcpy( (char *)pInf->GasName, "C3H7OH    " ); break;
						case EC_GAS_ETO_C_20_C5H8O2_30_0_PPM:		strcpy( (char *)pInf->GasName, "C5H8O2    " ); break;
						case EC_GAS_ETO_C_20_C7H8_150_PPM:			strcpy( (char *)pInf->GasName, "C7H8      " ); break;
						case EC_GAS_ETO_C_20_TEB_15_0_PPM:			strcpy( (char *)pInf->GasName, "TEB       " ); break;
						case EC_GAS_ETO_C_20_TEOS_30_0_PPM:			strcpy( (char *)pInf->GasName, "TEOS      " ); break;
						case EC_GAS_ETO_C_20_USN_01_20_0_PPM:		strcpy( (char *)pInf->GasName, "USN-01    " ); break;
					}
					break;
					
				case CT_SEN_H2_C_20000:
					switch( pInf->GasNo ){
						case EC_GAS_H2_C_20000_INT:					strcpy( (char *)pInf->GasName, "INT-H2-20K" ); break;
						case EC_GAS_H2_C_20000_H2_4_000_VOL:		strcpy( (char *)pInf->GasName, "H2        " ); break;
					}
					break;
					
				case CT_SEN_H2O2_CB_100:
					switch( pInf->GasNo ){
						case EC_GAS_H2O2_CB_100_INT:				strcpy( (char *)pInf->GasName, "INT-H2O2  " ); break;
						case EC_GAS_H2O2_CB_100_H2O2_100_PPM:		strcpy( (char *)pInf->GasName, "H2O2      " ); break;
					}
					break;
					
				case CT_SEN_H2S_C_50:
					switch( pInf->GasNo ){
						case EC_GAS_H2S_C_50_INT:					strcpy( (char *)pInf->GasName, "INT-H2S   " ); break;
						case EC_GAS_H2S_C_50_H2S_30_0_PPM:			strcpy( (char *)pInf->GasName, "H2S       " ); break;
					}
					break;
					
				case CT_SEN_HCN_C_100:
					switch( pInf->GasNo ){
						case EC_GAS_HCN_C_100_INT:					strcpy( (char *)pInf->GasName, "INT-HCN   " ); break;
						case EC_GAS_HCN_C_100_HCN_30_0_PPM:			strcpy( (char *)pInf->GasName, "HCN       " ); break;
					}
					break;
					
				case CT_SEN_NH3_CR_200:
					switch( pInf->GasNo ){
						case EC_GAS_NH3_CR_200_INT:					strcpy( (char *)pInf->GasName, "INT-NH3   " ); break;
						case EC_GAS_NH3_CR_200_ACP_02_150_PPM:		strcpy( (char *)pInf->GasName, "ACP-02    " ); break;
						case EC_GAS_NH3_CR_200_C5H5N_150_PPM:		strcpy( (char *)pInf->GasName, "C5H5N     " ); break;
						case EC_GAS_NH3_CR_200_NH3_75_0_PPM:		strcpy( (char *)pInf->GasName, "NH3       " ); break;
						case EC_GAS_NH3_CR_200_NH4OH_75_0_PPM:		strcpy( (char *)pInf->GasName, "NH4OH     " ); break;
						case EC_GAS_NH3_CR_200_SAM24_75_0_PPM:		strcpy( (char *)pInf->GasName, "SAM24     " ); break;
					}
					break;
					
				case CT_SEN_NO_CF_100:
					switch( pInf->GasNo ){
						case EC_GAS_NO_CF_100_INT:					strcpy( (char *)pInf->GasName, "INT-NO    " ); break;
						case EC_GAS_NO_CF_100_NO_100_PPM:			strcpy( (char *)pInf->GasName, "NO        " ); break;
					}
					break;
					
				case CT_SEN_NO2_C_20:
					switch( pInf->GasNo ){
						case EC_GAS_NO2_C_20_INT:					strcpy( (char *)pInf->GasName, "INT-NO2   " ); break;
						case EC_GAS_NO2_C_20_NO2_15_0_PPM:			strcpy( (char *)pInf->GasName, "NO2       " ); break;
					}
					break;
					
				case CT_SEN_PH3_C_5:
					switch( pInf->GasNo ){
						case EC_GAS_PH3_C_5_INT:					strcpy( (char *)pInf->GasName, "INT-PH3   " ); break;
						case EC_GAS_PH3_C_5_COS_100_PPM:			strcpy( (char *)pInf->GasName, "COS       " ); break;
						case EC_GAS_PH3_C_5_GEH4_1_00_PPM:			strcpy( (char *)pInf->GasName, "GeH4      " ); break;
						case EC_GAS_PH3_C_5_H2SO4_25_0_PPM:			strcpy( (char *)pInf->GasName, "H2SO4     " ); break;
						case EC_GAS_PH3_C_5_PH3_1_00_PPM:			strcpy( (char *)pInf->GasName, "PH3       " ); break;
					}
					break;
					
				case CT_SEN_SIH4_CD_50:
					switch( pInf->GasNo ){
						case EC_GAS_SIH4_CD_50_INT:					strcpy( (char *)pInf->GasName, "INT-SiH4  " ); break;
						case EC_GAS_SIH4_CD_50_3MS_30_0_PPM:		strcpy( (char *)pInf->GasName, "3MS       " ); break;
						case EC_GAS_SIH4_CD_50_C3H10SI_30_0_PPM:	strcpy( (char *)pInf->GasName, "C3H10Si   " ); break;
						case EC_GAS_SIH4_CD_50_GEH4_1_00_PPM:		strcpy( (char *)pInf->GasName, "GeH4      " ); break;
						case EC_GAS_SIH4_CD_50_SI2H6_15_0_PPM:		strcpy( (char *)pInf->GasName, "Si2H6     " ); break;
						case EC_GAS_SIH4_CD_50_SI3H8_15_0_PPM:		strcpy( (char *)pInf->GasName, "Si3H8     " ); break;
						case EC_GAS_SIH4_CD_50_SIH4_15_0_PPM:		strcpy( (char *)pInf->GasName, "SiH4      " ); break;
						case EC_GAS_SIH4_CD_50_TSA_15_0_PPM:		strcpy( (char *)pInf->GasName, "TSA       " ); break;
					}
					break;
					
				case CT_SEN_SO2_C_20:
					switch( pInf->GasNo ){
						case EC_GAS_SO2_C_20_INT:					strcpy( (char *)pInf->GasName, "INT-SO2   " ); break;
						case EC_GAS_SO2_C_20_H2SO4_100_PPM:			strcpy( (char *)pInf->GasName, "H2SO4     " ); break;
						case EC_GAS_SO2_C_20_SO2_10_0_PPM:			strcpy( (char *)pInf->GasName, "SO2       " ); break;
					}
					break;
					
				case CT_SEN_C2H4_C_1500:
					switch( pInf->GasNo ){
						case EC_GAS_C2H4_C_1500_INT:				strcpy( (char *)pInf->GasName, "INT-C2H4  " ); break;
						case EC_GAS_C2H4_C_1500_C2H4_2000_PPM:		strcpy( (char *)pInf->GasName, "C2H4      " ); break;
						case EC_GAS_C2H4_C_1500_C3H6_2000_PPM:		strcpy( (char *)pInf->GasName, "C3H6      " ); break;
					}
					break;
					
				case CT_SEN_CH2O_C_10:
					switch( pInf->GasNo ){
						case EC_GAS_CH2O_C_10_INT:					strcpy( (char *)pInf->GasName, "INT-CH2O  " ); break;
						case EC_GAS_CH2O_C_10_CH2O_10_0_PPM:		strcpy( (char *)pInf->GasName, "CH20      " ); break;
					}
					break;
					
				case CT_SEN_CO_CF_200:
					switch( pInf->GasNo ){
						case EC_GAS_CO_CF_200_INT:					strcpy( (char *)pInf->GasName, "INT-CO    " ); break;
						case EC_GAS_CO_CF_200_CO_150_PPM:			strcpy( (char *)pInf->GasName, "CO        " ); break;
					}
					break;
					
				case CT_SEN_O2_MP_100:
					switch( pInf->GasNo ){
						case EC_GAS_O2_MP_100_INT:					strcpy( (char *)pInf->GasName, "INT-O2    " ); break;
						case EC_GAS_O2_MP_100_O2_30_0_VOL:
						case EC_GAS_O2_MP_100_O2_25_0_VOL:			strcpy( (char *)pInf->GasName, "O2        " ); break;
					}
					break;
					
				case CT_SEN_ASH3_3E_1:
					switch( pInf->GasNo ){
						case EC_GAS_ASH3_3E_1_INT:					strcpy( (char *)pInf->GasName, "INT-AsH3  " ); break;
						case EC_GAS_ASH3_3E_1_ASH3_0_300_PPM:		strcpy( (char *)pInf->GasName, "AsH3      " ); break;
						case EC_GAS_ASH3_3E_1_B2H6_0_300_PPM:		strcpy( (char *)pInf->GasName, "B2H6      " ); break;
						case EC_GAS_ASH3_3E_1_GEH4_3_00_PPM:		strcpy( (char *)pInf->GasName, "GeH4      " ); break;
					}
					break;
					
				case CT_SEN_COCL2_3E_1:
					switch( pInf->GasNo ){
						case EC_GAS_COCL2_3E_1_INT:					strcpy( (char *)pInf->GasName, "INT-COCl2 " ); break;
						case EC_GAS_COCL2_3E_1_COCL2_1_00_PPM:		strcpy( (char *)pInf->GasName, "COCl2     " ); break;
					}
					break;
					
				case CT_SEN_NAP_100AM:
					switch( pInf->GasNo ){
						case CB_GAS_NAP_100AM_INT:					strcpy( (char *)pInf->GasName, "INT-100AM " ); break;
						case CB_GAS_NAP_100AM_C2H6_100_LEL:			strcpy( (char *)pInf->GasName, "C2H6      " ); break;
						case CB_GAS_NAP_100AM_C3H6_100_LEL:			strcpy( (char *)pInf->GasName, "C3H6      " ); break;
						case CB_GAS_NAP_100AM_C3H6O3_100_LEL:		strcpy( (char *)pInf->GasName, "C3H6O3    " ); break;
						case CB_GAS_NAP_100AM_C3H7OH_100_LEL:		strcpy( (char *)pInf->GasName, "C3H7OH    " ); break;
						case CB_GAS_NAP_100AM_C3H8_100_LEL:			strcpy( (char *)pInf->GasName, "C3H8      " ); break;
						case CB_GAS_NAP_100AM_C5H10_100_LEL:		strcpy( (char *)pInf->GasName, "C5H10     " ); break;
						case CB_GAS_NAP_100AM_C5H12_100_LEL:		strcpy( (char *)pInf->GasName, "C5H12     " ); break;
						case CB_GAS_NAP_100AM_C7H8_100_LEL:			strcpy( (char *)pInf->GasName, "C7H8      " ); break;
						case CB_GAS_NAP_100AM_CH4_100_LEL:			strcpy( (char *)pInf->GasName, "CH4       " ); break;
						case CB_GAS_NAP_100AM_COMB_100_LEL:			strcpy( (char *)pInf->GasName, "COMB.     " ); break;
						case CB_GAS_NAP_100AM_H_C_100_LEL:			strcpy( (char *)pInf->GasName, "H.C.      " ); break;
						case CB_GAS_NAP_100AM_H2_100_LEL:			strcpy( (char *)pInf->GasName, "H2        " ); break;
						case CB_GAS_NAP_100AM_D2_100_LEL:			strcpy( (char *)pInf->GasName, "D2        " ); break;
						case CB_GAS_NAP_100AM_C4H10_100_LEL:		strcpy( (char *)pInf->GasName, "i-C4H10   " ); break;
						case CB_GAS_NAP_100AM_LNG_100_LEL:			strcpy( (char *)pInf->GasName, "LNG       " ); break;
						case CB_GAS_NAP_100AM_LPG_100_LEL:			strcpy( (char *)pInf->GasName, "LPG       " ); break;
						case CB_GAS_NAP_100AM_PGMEA_100_LEL:		strcpy( (char *)pInf->GasName, "PGMEA     " ); break;
					}
					break;
					
				case CT_SEN_GS_100A:
					switch( pInf->GasNo ){
						case CB_GAS_GS_100A_INT:					strcpy( (char *)pInf->GasName, "INT-100A  " ); break;
						case CB_GAS_GS_100A_C2H2_100_LEL:			strcpy( (char *)pInf->GasName, "C2H2      " ); break;
						case CB_GAS_GS_100A_C2H4_100_LEL:			strcpy( (char *)pInf->GasName, "C2H4      " ); break;
						case CB_GAS_GS_100A_C3H7OH_100_LEL:			strcpy( (char *)pInf->GasName, "C3H7OH    " ); break;
						case CB_GAS_GS_100A_C5H10_100_LEL:			strcpy( (char *)pInf->GasName, "C5H10     " ); break;
						case CB_GAS_GS_100A_H2_100_LEL:				strcpy( (char *)pInf->GasName, "H2        " ); break;
					}
					break;
					
				case CT_SEN_GS_100SC:
					switch( pInf->GasNo ){
						case SC_GAS_GS_100SC_INT:					strcpy( (char *)pInf->GasName, "INT-100SC " ); break;
						case SC_GAS_GS_100SC_ORG_1000_PPM:			strcpy( (char *)pInf->GasName, "ORG       " ); break;
					}
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Default_Gas_Info_Load( Measuring_GasInfo *pGas, Measuring_AlarmInfo *pAlr )
{
	
	switch( pGas->Method ){
		case MEASURING_METHOD_CT: Default_Gas_Info_Load_Cartridge( pGas, pAlr ); break;
		case MEASURING_METHOD_IR: Default_Gas_Info_Load_Cavity   ( pGas, pAlr ); break;
	}
	
	Measuring_Default_Gas_Name_Load( pGas );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Gas_Name_Update( U8 i )
{
	SenEEP_Write_CHn( MeasHandle.Gas[i].Inf.GasName, i, SENSOR_EEP_CHn_GAS_NAME_0, 6 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Gas_Info_Update( U8 i )
{
	U16		eep[24];
	
	Measuring_Gas_MSD_Load( &MeasHandle.Gas[i].Inf );
	
	eep[ 0] = MeasHandle.Gas[i].Inf.TIA.Word[0];
	eep[ 1] = MeasHandle.Gas[i].Inf.TIA.Word[1];
	eep[ 2] = MeasHandle.Gas[i].Inf.TargetOffs;
	eep[ 3] = MeasHandle.Gas[i].Inf.TargetGain;
	eep[ 4] = MeasHandle.Gas[i].Inf.TargetBias;
	
	eep[ 5] = MeasHandle.Gas[i].Inf.SpanGas;
	
	eep[ 6] = MeasHandle.Gas[i].Inf.bZeroUse;
	eep[ 7] = MeasHandle.Gas[i].Inf.bZeroPer;
	eep[ 8] = MeasHandle.Gas[i].Inf.bZeroSec;
	
	eep[ 9] = MeasHandle.Gas[i].Inf.SenNo;
	eep[10] = MeasHandle.Gas[i].Inf.GasNo;
	eep[11] = MeasHandle.Gas[i].Inf.Dp;
	eep[12] = MeasHandle.Gas[i].Inf.HighScale;
	eep[13] = MeasHandle.Gas[i].Inf.Unit;
	eep[14] = MeasHandle.Gas[i].Inf.MaintenanceGas;
	eep[15] = MeasHandle.Gas[i].Inf.CrossScale;
	eep[16] = MeasHandle.Gas[i].Inf.ZeroSkipPer;
	eep[17] = MeasHandle.Gas[i].Inf.SpanSkipPer;
	eep[18] = MeasHandle.Gas[i].Inf.OutDelayPer;
	eep[19] = MeasHandle.Gas[i].Inf.OutDelaySec;
	
	eep[20] = MeasHandle.Alr[i].Inf._1stTypeIncrease;
	eep[21] = MeasHandle.Alr[i].Inf._1stLevel;
	
	eep[22] = MeasHandle.Alr[i].Inf._2ndTypeIncrease;
	eep[23] = MeasHandle.Alr[i].Inf._2ndLevel;
	
	if( i < CARTRIDGE_SEN_CH_MAX ){
		SenEEP_Write_CHn( &eep[ 0], i, SENSOR_EEP_CHn_PTM_mOFFS_TIA_0, 5 );
	}
	else {
		SenEEP_Write_CHn( &eep[ 2], i, SENSOR_EEP_CHn_AMP_CAL_TARGET_mV_OFFS, 3 );
	}
	
	SenEEP_Write_CHn( &eep[ 5], i, SENSOR_EEP_CHn_SPAN_GAS, 1 );
	SenEEP_Write_CHn( &eep[ 6], i, SENSOR_EEP_CHn_bZERO_USE, 3 );
	SenEEP_Write_CHn( &eep[ 9], i, SENSOR_EEP_CHn_SEN_NO, 11 );
	SenEEP_Write_CHn( &eep[20], i, SENSOR_EEP_CHn_ALARM_1ST_INCREASE_TYPE, 2 );
	SenEEP_Write_CHn( &eep[22], i, SENSOR_EEP_CHn_ALARM_2ND_INCREASE_TYPE, 2 );
	
	Measuring_Gas_Name_Update( i );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Task( void )
{
	if( Sensing_Process() ){
		
		MeasHandle.pPrv = &MeasHandle.Lpf[MeasHandle.LpfIndex - 1 & MEASURING_LPF_BUF_MASK];
		MeasHandle.pCur = &MeasHandle.Lpf[MeasHandle.LpfIndex     & MEASURING_LPF_BUF_MASK];
			
		Raw_Mapping();
		Lowpass_Filtering();
		Base_Zero_Handler();
		Gas_Calculation();
		Measuring_Deviation_Calculation();
		Measuring_Filter_Decision();
		Output_Gas_Decision();
		Data_Logger( &MeasHandle );
		
		MeasHandle.LpfIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Maintenance_Auto_Quit_Handler( void )
{
	if( MeasHandle.Mode.Item.Maintenance == true ){
		if( MeasHandle.Mode.Item.Maintenance_QuitMin ){
			if( Maintenance_QuitSec ){
				Maintenance_QuitSec--;
			}
			else {
				MeasHandle.Mode.Item.Maintenance = false;
				EEPROM_Update( &MeasHandle.Mode.Word, SYSTEM_EEP_MEASURING_MODE, 1 );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Measuring_Maintenance_Quit_Time_Refresh( void )
{
	if( MeasHandle.Mode.Item.Maintenance == true ){
		Maintenance_QuitSec = (U16)MeasHandle.Mode.Item.Maintenance_QuitMin * 60;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float Measuring_Gas_Response_Calculation( U8 i, U16 SpanGas, float det )
{
	float				ret;
	float				zts;
	float				UnitScale;
	Measuring_GasInfo	*pInf = &MeasHandle.Gas[i].Inf;
	
	if( i < CARTRIDGE_SEN_CH_MAX ){
		UnitScale = (float)SpanGas / pow( 10, pInf->Dp ) * pInf->CrossScale / 100.f;
		zts = det - pInf->bZero.f;
		if( pInf->SenNo < CT_SEN_NAP_100AM ){
			ret = zts / LPTIA_Resistance[pInf->TIA.Item.RGAIN] * 1000000000.f / UnitScale;
		}
		else {
			ret = zts * 1000.f / UnitScale;
		}
	}
	else {
		ret = det / pInf->bZero.f * 100.f - 100.f;
	}
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S16 Measuring_CT_Sensor_Bias_Load( Measuring_GasInfo *pGas )
{
	S16		vBias;
	
	switch( pGas->SenNo ){
		case CT_SEN_ETO_C_20:		vBias =  300; break;
		case CT_SEN_H2O2_CB_100:	vBias =   50; break;
		case CT_SEN_NO_CF_100:		vBias =  300; break;
		case CT_SEN_O2_MP_100:		vBias = -600; break;
		default:					vBias =    0; break;
	}
	
	return vBias;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool Channel_Config_Load_Cartridge( Measuring_ConfType *pConf )
{
	bool	err = false;
	U8		i;
	
	for( i=0, pConf->OutMax = 0; i<MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS]; i++ ){
		
		pConf->iRefCtg = 1;
		pConf->iOut[i] = i;
		pConf->Func[i] = MEASURING_GAS_FUNC_CT_OUT;
		pConf->OutMax++;
	    pConf->SenNo[i] = MeasHandle.pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_SYS][SENSOR_EEP_SYS_CTG_SEN_TYPE_0 + i];
		
		pConf->IsAndMode = false;
		
		switch( pConf->SenNo[i] ){
			case CT_SEN_UNDEFINED:
				switch( pConf->SetNo.Ctg[i] ){
					case 0:
						pConf->GasNo[i] = 0;
						pConf->CtgModel[i] = MODEL_CTG_UNDEFINED;
						break;
						
					default: err = true; break;
				}
				break;
				
			case CT_SEN_EC_DC_CL2_3:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // BR2 3.00 PPM
					case 1: // CL2 3.00 PPM
						pConf->GasNo[i] = EC_GAS_EC_DC_CL2_3_BR2_3_00_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_EC_DC_CL2_3_BR2_3_00_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_EC_DC_CL2_3
				}
				break;
				
			case CT_SEN_EC_DC_H2_2000:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // H2 2000 PPM
						pConf->GasNo[i] = EC_GAS_EC_DC_H2_2000_H2_2000_PPM;
						pConf->CtgModel[i] = MODEL_CTG_EC_DC_H2_2000_H2_2000_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_EC_DC_H2_2000
				}
				break;
				
			case CT_SEN_EC_DC_HCL_15:
				switch( pConf->SetNo.Ctg[i] ){
					case  0: // BCL3 15.0_PPM
					case  1: // HCDS 20.0 PPM
					case  2: // HCL 10.0 PPM
					case  3: // LTO520 50.0 PPM
					case  4: // MCS 15.0 PPM
					case  5: // PCL3 15.0 PPM
					case  6: // POCL3 15.0 PPM
					case  7: // SiCL4 15.0 PPM
					case  8: // SiH2CL2 15.0 PPM
					case  9: // SiHCL3 15.0 PPM
					case 10: // TCS 15.0 PPM
					case 11: // TICL4 15.0 PPM
						pConf->GasNo[i] = EC_GAS_EC_DC_HCL_15_BCL3_15_0_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_EC_DC_HCL_15_BCL3_15_0_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_EC_DC_HCL_15
				}
				break;
				
			case CT_SEN_EC_DC_HF_9:
				switch( pConf->SetNo.Ctg[i] ){
					case  0: // BBR3 9.00 PPM
					case  1: // BF3 9.00 PPM
					case  2: // C2F4O 200 PPM
					case  3: // CH2O2 50.0 PPM
					case  4: // CLF3 1.00 PPM
					case  5: // F2 3.00 PPM
					case  6: // GEF4 9.00 PPM
					case  7: // HBR 9.00 PPM
					case  8: // HCOOH 50.0 PPM
					case  9: // HF 6.00 PPM
					case 10: // HNO3 20.0 PPM
					case 11: // PF3 9.00 PPM
					case 12: // SIF4 9.00 PPM
					case 13: // WF6 9.00 PPM
						pConf->GasNo[i] = EC_GAS_EC_DC_HF_9_BBR3_9_00_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_EC_DC_HF_9_BBR3_9_00_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_EC_DC_HF_9
				}
				break;
				
			case CT_SEN_EC_DC_O3_1:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // O3 1.00 PPM
						pConf->GasNo[i] = EC_GAS_EC_DC_O3_1_O3_1_00_PPM;
						pConf->CtgModel[i] = MODEL_CTG_EC_DC_O3_1_O3_1_00_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_EC_DC_O3_1
				}
				break;
				
			case CT_SEN_ETO_C_20:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // C2H4O(E.O) 30.0 PPM
					case 1: // C3H7OH 500 PPM
					case 2: // C5H8O2 30.0 PPM
					case 3: // C7H8 150 PPM
					case 4: // TEB 15.0 PPM
					case 5: // TEOS 30.0 PPM
					case 6: // USN-01 20.0 PPM
						pConf->GasNo[i] = EC_GAS_ETO_C_20_C2H4O_EO_30_0_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_ETO_C_20_C2H4O_EO_30_0_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_ETO_C_20
				}
				break;
				
			case CT_SEN_H2_C_20000:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ORG 1000 PPM
						pConf->GasNo[i] = EC_GAS_H2_C_20000_H2_4_000_VOL;
						pConf->CtgModel[i] = MODEL_CTG_H2_C_20000_H2_4_000_VOL;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_H2_C_20000
				}
				break;
				
			case CT_SEN_H2O2_CB_100:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ORG 1000 PPM
						pConf->GasNo[i] = EC_GAS_H2O2_CB_100_H2O2_100_PPM;
						pConf->CtgModel[i] = MODEL_CTG_H2O2_CB_100_H2O2_100_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_H2O2_CB_100
				}
				break;
				
			case CT_SEN_H2S_C_50:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ORG 1000 PPM
						pConf->GasNo[i] = EC_GAS_H2S_C_50_H2S_30_0_PPM;
						pConf->CtgModel[i] = MODEL_CTG_H2S_C_50_H2S_30_0_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_H2S_C_50
				}
				break;
				
			case CT_SEN_HCN_C_100:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ORG 1000 PPM
						pConf->GasNo[i] = EC_GAS_HCN_C_100_HCN_30_0_PPM;
						pConf->CtgModel[i] = MODEL_CTG_HCN_C_100_HCN_30_0_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_HCN_C_100
				}
				break;
				
			case CT_SEN_NH3_CR_200:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ACP-02 150 PPM
					case 1: // C5H5N 150 PPM
					case 2: // N2O 5.00 VOL
					case 3: // NH3 75.0 PPM
					case 4: // NH4OH 75.0 PPM
					case 5: // SAM24 75.0 PPM
						pConf->GasNo[i] = EC_GAS_NH3_CR_200_ACP_02_150_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_NH3_CR_200_ACP_02_150_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_NH3_CR_200
				}
				break;
				
			case CT_SEN_NO_CF_100:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // NO 100 PPM
						pConf->GasNo[i] = EC_GAS_NO_CF_100_NO_100_PPM;
						pConf->CtgModel[i] = MODEL_CTG_NO_CF_100_NO_100_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_NO_CF_100
				}
				break;
				
			case CT_SEN_NO2_C_20:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // NO2 15.0 PPM
						pConf->GasNo[i] = EC_GAS_NO2_C_20_NO2_15_0_PPM;
						pConf->CtgModel[i] = MODEL_CTG_NO2_C_20_NO2_15_0_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_NO_CF_100
				}
				break;
				
			case CT_SEN_PH3_C_5:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // COS 100 PPM
					case 1: // GEH4 1.00 PPM
					case 2: // H2SO4 25.0 PPM
					case 3: // PH3 1.00 PPM
						pConf->GasNo[i] = EC_GAS_PH3_C_5_COS_100_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_PH3_C_5_COS_100_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_PH3_C_5
				}
				break;
				
			case CT_SEN_SIH4_CD_50:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // 3MS 30.0 PPM
					case 1: // C3H10Si 30.0 PPM
					case 2: // GEH4 1.00 PPM
					case 3: // SI2H6 15.0 PPM
					case 4: // SI3H8 15.0 PPM
					case 5: // SIH4 15.0 PPM
					case 6: // TSA 15.0 PPM
						pConf->GasNo[i] = EC_GAS_SIH4_CD_50_3MS_30_0_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_SIH4_CD_50_3MS_30_0_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of SIH4_CD_50
				}
				break;
				
			case CT_SEN_SO2_C_20:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // H2SO4 100 PPM
					case 1: // SO2 10.0 PPM
						pConf->GasNo[i] = EC_GAS_SO2_C_20_H2SO4_100_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_SO2_C_20_H2SO4_100_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_SO2_C_20
				}
				break;
				
			case CT_SEN_C2H4_C_1500:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // C2H4 2000 PPM
					case 1: // C3H6 2000 PPM
						pConf->GasNo[i] = EC_GAS_C2H4_C_1500_C2H4_2000_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_C2H4_C_1500_C2H4_2000_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_C2H4_C_1500
				}
				break;
				
			case CT_SEN_CH2O_C_10:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // CH2O 10.0 PPM
						pConf->GasNo[i] = EC_GAS_CH2O_C_10_CH2O_10_0_PPM;
						pConf->CtgModel[i] = MODEL_CTG_CH2O_C_10_CH2O_10_0_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_CH2O_C_10
				}
				break;
				
			case CT_SEN_CO_CF_200:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // CO 150 PPM
						pConf->GasNo[i] = EC_GAS_CO_CF_200_CO_150_PPM;
						pConf->CtgModel[i] = MODEL_CTG_CO_CF_200_CO_150_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_CO_CF_200
				}
				break;
				
			case CT_SEN_O2_MP_100:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // O2 30.0 %VOL
						pConf->GasNo[i] = EC_GAS_O2_MP_100_O2_30_0_VOL;
						pConf->CtgModel[i] = MODEL_CTG_O2_MP_100_O2_30_0_VOL;
						break;
						
					case 1:
						pConf->GasNo[i] = EC_GAS_O2_MP_100_O2_25_0_VOL;
						pConf->CtgModel[i] = MODEL_CTG_O2_MP_100_O2_25_0_VOL;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_O2_MP_100
				}
				break;
				
			case CT_SEN_ASH3_3E_1:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ASH3 0.300 PPM
					case 1: // B2H6 0.300 PPM
					case 2: // GEH4 3.00 PPM
						pConf->GasNo[i] = EC_GAS_ASH3_3E_1_ASH3_0_300_PPM + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_ASH3_3E_1_ASH3_0_300_PPM + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_ASH3_3E_1
				}
				break;
				
			case CT_SEN_COCL2_3E_1:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // COCL2 1.00 PPM
						pConf->GasNo[i] = EC_GAS_COCL2_3E_1_COCL2_1_00_PPM;
						pConf->CtgModel[i] = MODEL_CTG_COCL2_3E_1_COCL2_1_00_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_O2_MP_100
				}
				break;
				
			case CT_SEN_NAP_100AM:
				switch( pConf->SetNo.Ctg[i] ){
					case  0: // C2H6 100 %LEL
					case  1: // C3H6 100 %LEL
					case  2: // C3H6O 100 %LEL
					case  3: // C3H7O 100 %LEL
					case  4: // C3H8 100 %LEL
					case  5: // C5H10 100 %LEL
					case  6: // C5H12 100 %LEL
					case  7: // C7H8 100 %LEL
					case  8: // CH4 100 %LEL
					case  9: // COMB. 100 %LEL
					case 10: // H.C 100 %LEL
					case 11: // H2 100 %LEL
					case 12: // D2 100 %LEL
					case 13: // C4H10 100 %LEL
					case 14: // LNG 100 %LEL
					case 15: // LPG 100 %LEL
					case 16: // PGMEA 100 %LEL
						pConf->GasNo[i] = CB_GAS_NAP_100AM_C2H6_100_LEL + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_NAP_100AM_C2H6_100_LEL + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of NAP_100AM
				}
				break;
				
			case CT_SEN_GS_100A:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // C2H2 100 LEL
					case 1: // C2H4 100 LEL
					case 2: // C3H7OH 100 LEL
					case 3: // C5H10 100 LEL
					case 4: // H2 100 LEL
						pConf->GasNo[i] = CB_GAS_GS_100A_C2H2_100_LEL + pConf->SetNo.Ctg[i];
						pConf->CtgModel[i] = MODEL_CTG_GS_100A_C2H2_100_LEL + pConf->SetNo.Ctg[i];
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_GS_100A
				}
				break;
				
			case CT_SEN_GS_100SC:
				switch( pConf->SetNo.Ctg[i] ){
					case 0: // ORG 1000 PPM
						pConf->GasNo[i] = SC_GAS_GS_100SC_ORG_1000_PPM;
						pConf->CtgModel[i] = MODEL_CTG_GS_100SC_ORG_1000_PPM;
						break;
						
					default: err = true; break; // Undefined config number of CT_SEN_GS_100SC
				}
				break;
				
			default: err = true; break; // Undefined sensor of Cartridge
		}
	}
	
	if( err == false ){
		
		if( pConf->OutMax == MEASURING_GAS_OUTPUT_MAX ){
			if( pConf->CtgModel[0] == pConf->CtgModel[1] ){
				pConf->IsAndMode = true;
				pConf->Func[0] = MEASURING_GAS_FUNC_CT_OUT_WITH_REF;
				pConf->Func[1] = MEASURING_GAS_FUNC_CT_REF;
				pConf->OutMax = 1;
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool Channel_Config_Load_Cavity( Measuring_ConfType *pConf )
{
	bool	err = false;
	
	switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_DET_TYPE] ){
		case 970:
			switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE] ){
				case CAVITY_SEN_TYPE_LMM_274_X025:
					pConf->SenNo[2] = IR_SEN_03_330;
					pConf->SenNo[3] = IR_SEN_09_440;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_04_850;
					switch( pConf->SetNo.Cvt ){
						case 0: // LNG 100 %LEL
						case 1: // CH4 100 %LEL
							pConf->iOut[0] = 2;
							pConf->CvtModel = MODEL_CVT_QUAD_X025_LNG_100_LEL + pConf->SetNo.Cvt;
							pConf->GasNo[2] = IR_GAS_03_330_LNG_100_LEL + pConf->SetNo.Cvt;
							pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.4f;
							pConf->iSpanScale[3] = 0.85f;
							pConf->iSpanScale[4] = 0.1f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						case 2: // COS 200 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X025_COS_200_PPM;
							pConf->GasNo[5] = IR_GAS_04_850_COS_200_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.3f;
							pConf->iSpanScale[3] = 0.9f;
							pConf->iSpanScale[4] = 0.65f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X025
					}
					break;
					
				case CAVITY_SEN_TYPE_LMM_274_X027:
					pConf->SenNo[2] = IR_SEN_03_330;
					pConf->SenNo[3] = IR_SEN_09_440;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_08_925;
					switch( pConf->SetNo.Cvt ){
						case 0: // C4F8 2000 PPM
							pConf->iOut[0] = 4;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_C4F8_2000_PPM;
							pConf->GasNo[4] = IR_GAS_08_000_C4F8_2000_PPM;
							pConf->Func[4] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.6f;
							pConf->iSpanScale[3] = 0.8f;
							pConf->iSpanScale[4] = 0.1f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						case 1: // C5F8 2000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_C5F8_2000_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_C5F8_2000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.85f;
							pConf->iSpanScale[3] = 0.9f;
							pConf->iSpanScale[4] = 0.3f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 2: // C4F6 2000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_C4F6_2000_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_C4F6_2000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.85f;
							pConf->iSpanScale[3] = 0.8f;
							pConf->iSpanScale[4] = 0.8f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						case 3: // CH2F2 2000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_CH2F2_2000_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_CH2F2_2000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.85f;
							pConf->iSpanScale[3] = 0.1f;
							pConf->iSpanScale[4] = 0.85f;
							pConf->iSpanScale[5] = 0.7f;
							break;
							
						case 4: // TEOS 100 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_TEOS_100_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_TEOS_100_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.85f;
							pConf->iSpanScale[3] = 0.6f;
							pConf->iSpanScale[4] = 0.6f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						case 5: // CH3F 2000 PPM
							pConf->iOut[0] = 3;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_CH3F_2000_PPM;
							pConf->GasNo[3] = IR_GAS_09_440_CH3F_2000_PPM;
							pConf->Func[3] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.5f;
							pConf->iSpanScale[3] = 0.15f;
							pConf->iSpanScale[4] = 0.9f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						case 6: // SFA-1 100 %LEL
							pConf->iOut[0] = 2;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_SFA_1_100_LEL;
							pConf->GasNo[2] = IR_GAS_03_330_SFA_1_100_LEL;
							pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.3f;
							pConf->iSpanScale[3] = 0.8f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						case 7: // IPA 3000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_IPA_3000_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_IPA_3000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.3f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.65f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 8: // IPA 100 %LEL
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_IPA_100_LEL;
							pConf->GasNo[5] = IR_GAS_08_925_IPA_100_LEL;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.4f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.6f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 9: // 4MS 100 %LEL
							pConf->iOut[0] = 2;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_4MS_100_LEL;
							pConf->GasNo[2] = IR_GAS_03_330_4MS_100_LEL;
							pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.6f;
							pConf->iSpanScale[3] = 0.85f;
							pConf->iSpanScale[4] = 0.1f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						case 10: // SELITANE 100 %LEL
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_SELITANE_100_LEL;
							pConf->GasNo[5] = IR_GAS_08_925_SELITANE_100_LEL;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.4f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.6f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 11: // ETHANOL 100 %LEL
							pConf->iOut[0] = 3;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_ETHANOL_100_LEL;
							pConf->GasNo[3] = IR_GAS_09_440_ETHANOL_100_LEL;
							pConf->Func[3] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.45f;
							pConf->iSpanScale[3] = 0.15f;
							pConf->iSpanScale[4] = 0.35f;
							pConf->iSpanScale[5] = 0.5f;
							break;
								
						case 12: // FL300 200 PPM
							pConf->iOut[0] = 4;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_FL300_200_PPM;
							pConf->GasNo[4] = IR_GAS_08_000_FL300_200_PPM;
							pConf->Func[4] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.8f;
							pConf->iSpanScale[3] = 0.8f;
							pConf->iSpanScale[4] = 0.4f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 13: // TEOS_HT200 100 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_TEOS_HT200_100_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_TEOS_HT200_100_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.85f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.6f;
							pConf->iSpanScale[5] = 0.5f;
							break;
								
						case 14: // CF4 500 PPM
							pConf->iOut[0] = 4;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_CF4_500_PPM;
							pConf->GasNo[4] = IR_GAS_08_000_CF4_500_PPM;
							pConf->Func[4] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.5f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X027
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_CH:
					pConf->SenNo[2] = IR_SEN_03_330;
					pConf->SenNo[3] = IR_SEN_03_950;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_CH;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // LNG 100 %LEL
						case 1: // CH4 100 %LEL
						case 2: // C4H10 100 %LEL
							pConf->GasNo[2] = IR_GAS_03_330_LNG_100_LEL + pConf->SetNo.Cvt;
							break;
							
						case 3: // Toluene 100 %LEL
							pConf->GasNo[2] = IR_GAS_03_330_TOLUENE_100_LEL;
							break;
							
						case 4: // LNG 100 %VOL
							pConf->GasNo[2] = IR_GAS_03_330_CH4_100_VOL;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_CH
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_DH:
					pConf->SenNo[2] = IR_SEN_04_260;
					pConf->SenNo[3] = IR_SEN_03_950;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_DH;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // CO2 5000 PPM
						case 1: // CO2 9999 PPM
						case 2: // CO2 1 %VOL
						case 3: // CO2 5 %VOL
							pConf->GasNo[2] = IR_GAS_04_260_CO2_5000_PPM + pConf->SetNo.Cvt;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_DH
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_IH:
					pConf->SenNo[2] = IR_SEN_04_660;
					pConf->SenNo[3] = IR_SEN_03_950;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_IH;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // CO 9999 PPM
						case 1: // CO 1 %VOL
						case 2: // CO 2 %VOL
						case 3: // CO 5 %VOL
							pConf->GasNo[2] = IR_GAS_04_660_CO_9999_PPM + pConf->SetNo.Cvt;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_IH
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_X018:
					pConf->SenNo[2] = IR_SEN_09_440;
					pConf->SenNo[3] = IR_SEN_03_900;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_X018;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // CH3F 2000 PPM
							pConf->GasNo[2] = IR_GAS_09_440_CH3F_2000_PPM;
							break;
							
						case 1: // CTFE 1000 PPM
							pConf->GasNo[2] = IR_GAS_09_440_CTFE_1000_PPM;
							break;
							
						case 2: // C3F6 2000 PPM
							pConf->GasNo[2] = IR_GAS_09_440_C3F6_2000_PPM;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_X018
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_X021:
					pConf->SenNo[2] = IR_SEN_08_925;
					pConf->SenNo[3] = IR_SEN_03_900;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_X021;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // C4F6 2000 PPM
						case 1: // CH2F2 2000 PPM
							pConf->GasNo[2] = IR_GAS_08_925_C4F6_2000_PPM + pConf->SetNo.Cvt;
							break;
							
						case 2: // HFC 2000 PPM
							pConf->GasNo[2] = IR_GAS_08_925_HFC_2000_PPM;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_X021
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_X054:
					pConf->SenNo[2] = IR_SEN_08_000;
					pConf->SenNo[3] = IR_SEN_03_900;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_X054;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // C4F8 2000 PPM
							pConf->GasNo[2] = IR_GAS_08_000_C4F8_2000_PPM;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_X022
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_X040:
					pConf->SenNo[2] = IR_SEN_04_520;
					pConf->SenNo[3] = IR_SEN_03_720;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_X040;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // N2O 4000 PPM
							pConf->GasNo[2] = IR_GAS_04_520_N2O_4000_PPM;
							break;
							
						case 1: // N2O 1000 PPM
							pConf->GasNo[2] = IR_GAS_04_520_N2O_1000_PPM;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_X040
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_X044:
					pConf->SenNo[2] = IR_SEN_04_850;
					pConf->SenNo[3] = IR_SEN_03_900;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_X044;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // COS 200 PPM
							pConf->GasNo[2] = IR_GAS_04_850_COS_200_PPM;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_X044
					}
					break;
					
				case CAVITY_SEN_TYPE_LMM_274_X033:
					pConf->SenNo[2] = IR_SEN_09_076;
					pConf->SenNo[3] = IR_SEN_09_440;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_10_100;
					switch( pConf->SetNo.Cvt ){
						case 0: // C4F6S 2000 PPM
							pConf->iOut[0] = 2;
							pConf->CvtModel = MODEL_CVT_QUAD_X033_C4F6S_2000_PPM;
							pConf->GasNo[2] = IR_GAS_09_076_C4F6S_2000_PPM;
							pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.75f;
							pConf->iSpanScale[4] = 0.85f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X033
					}
					break;
					
				case CAVITY_SEN_TYPE_LMM_274_X040:
					pConf->SenNo[2] = IR_SEN_03_370;
					pConf->SenNo[3] = IR_SEN_03_420;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_09_230;
					switch( pConf->SetNo.Cvt ){
						case 0: // ECH 100 %LEL
							pConf->iOut[0] = 3;
							pConf->CvtModel = MODEL_CVT_QUAD_X040_ECH_100_LEL;
							pConf->GasNo[3] = IR_GAS_03_420_ECH_100_LEL;
							pConf->Func[3] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.2f;
							pConf->iSpanScale[3] = 0.3f;
							pConf->iSpanScale[4] = 0.7f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						case 1: // OMCTS 75 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X040_OMCTS_75_0_PPM;
							pConf->GasNo[5] = IR_GAS_09_230_OMCTS_75_0_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.85f;
							pConf->iSpanScale[3] = 0.85f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.4f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X040
					}
					break;
					
				case CAVITY_SEN_TYPE_LMM_274_X042:
					pConf->SenNo[2] = IR_SEN_03_370;
					pConf->SenNo[3] = IR_SEN_09_150;
					pConf->SenNo[4] = IR_SEN_09_790;
					pConf->SenNo[5] = IR_SEN_10_990;
					switch( pConf->SetNo.Cvt ){
						case 0: // NF3 200 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_NF3_200_PPM;
							pConf->GasNo[5] = IR_GAS_10_990_NF3_200_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.8f;
							pConf->iSpanScale[3] = 0.7f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						case 1: // GMP-02 3000 PPM
							pConf->iOut[0] = 2;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_GMP_02_3000_PPM;
							pConf->GasNo[2] = IR_GAS_03_370_GMP_02_3000_PPM;
							pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.9f;
							pConf->iSpanScale[4] = 0.7f;
							pConf->iSpanScale[5] = 0.75f;
							break;
							
						case 2: // C4H2F6 1000 PPM
							pConf->iOut[0] = 3;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_C4H2F6_1000_PPM;
							pConf->GasNo[3] = IR_GAS_09_150_C4H2F6_1000_PPM;
							pConf->Func[3] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.8f;
							pConf->iSpanScale[3] = 0.2f;
							pConf->iSpanScale[4] = 0.85f;
							pConf->iSpanScale[5] = 0.7f;
							break;
							
						case 3: // HMDS 1000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_HMDS_1000_PPM;
							pConf->GasNo[5] = IR_GAS_10_990_HMDS_1000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						case 4: // C3F6 2000 PPM
							pConf->iOut[0] = 4;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_C3F6_2000_PPM;
							pConf->GasNo[4] = IR_GAS_09_790_C3F6_2000_PPM;
							pConf->Func[4] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.4f;
							pConf->iSpanScale[3] = 0.85f;
							pConf->iSpanScale[4] = 0.1f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						case 5: // HMDS 100 %LEL
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_HMDS_100_LEL;
							pConf->GasNo[5] = IR_GAS_10_990_HMDS_100_LEL;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						case 6: // C3F6O 5000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_C3F6O_5000_PPM;
							pConf->GasNo[5] = IR_GAS_10_990_C3F6O_5000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.75f;
							pConf->iSpanScale[3] = 0.5f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 7: // C3H6 100 %LEL
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_C3H6_100_LEL;
							pConf->GasNo[5] = IR_GAS_10_990_C3H6_100_LEL;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.3f;
							pConf->iSpanScale[3] = 0.85f;
							pConf->iSpanScale[4] = 0.75f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 8: // ETHYLENE 100 %LEL
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_C2H4_100_LEL;
							pConf->GasNo[5] = IR_GAS_10_990_C2H4_100_LEL;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.6f;
							pConf->iSpanScale[3] = 0.85f;
							pConf->iSpanScale[4] = 0.35f;
							pConf->iSpanScale[5] = 0.6f;
							break;
							
						case 9: // BTBAS 1000 PPM
							pConf->iOut[0] = 2;
							pConf->CvtModel = MODEL_CVT_QUAD_X042_BTBAS_1000_PPM;
							pConf->GasNo[2] = IR_GAS_03_370_BTBAS_1000_PPM;
							pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.55f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X042
					}
					break;
					
				case CAVITY_SEN_TYPE_LMM_274_X044:
					pConf->SenNo[2] = IR_SEN_09_076;
					pConf->SenNo[3] = IR_SEN_03_370;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_10_100;
					switch( pConf->SetNo.Cvt ){
						case 0: // METHANOL 3000 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X044_METHANOL_3000_PPM;
							pConf->GasNo[5] = IR_GAS_10_100_METHANOL_3000_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.5f;
							pConf->iSpanScale[3] = 0.4f;
							pConf->iSpanScale[4] = 0.9f;
							pConf->iSpanScale[5] = 0.8f;
							break;
							
						case 1: // OMCTS 100 %LEL
							pConf->iOut[0] = 3;
							pConf->CvtModel = MODEL_CVT_QUAD_X044_OMCTS_100_LEL;
							pConf->GasNo[3] = IR_GAS_03_370_OMCTS_100_LEL;
							pConf->Func[3] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.3f;
							pConf->iSpanScale[3] = 0.7f;
							pConf->iSpanScale[4] = 0.5f;
							pConf->iSpanScale[5] = 0.8f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X044
					}
					break;
					
				case CAVITY_SEN_TYPE_LIM_272_X127:
					pConf->SenNo[2] = IR_SEN_10_990;
					pConf->SenNo[3] = IR_SEN_03_950;
					pConf->Func[2] = MEASURING_GAS_FUNC_IR_OUT_WITH_REF;
					pConf->Func[3] = MEASURING_GAS_FUNC_IR_REF;
					pConf->iRefCvt = 3;
					pConf->iOut[0] = 2;
					pConf->CvtModel = MODEL_CVT_DUAL_X127;
					pConf->OutMax = 1;
					switch( pConf->SetNo.Cvt ){
						case 0: // NF3 200 PPM
							pConf->GasNo[2] = IR_GAS_10_990_NF3_200_PPM;
							break;
							
						default: err = true; break; // Undefined config number of LIM_272_X127
					}
					break;
					
				default: err = true; break; // Undefined sensor of 970
			}
			break;
			
		case 972:
			switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE] ){
				case CAVITY_SEN_TYPE_LMM_274_X025:
					pConf->SenNo[2] = IR_SEN_03_330;
					pConf->SenNo[3] = IR_SEN_09_440;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_04_850;
					switch( pConf->SetNo.Cvt ){
						case 0: // COS 200 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X025_COS_75_0_PPM;
							pConf->GasNo[5] = IR_GAS_04_850_COS_75_0_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.55f;
							pConf->iSpanScale[4] = 0.65f;
							pConf->iSpanScale[5] = 0.85f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X025
					}
					break;
					
				case CAVITY_SEN_TYPE_LMM_274_X027:
					pConf->SenNo[2] = IR_SEN_03_330;
					pConf->SenNo[3] = IR_SEN_09_440;
					pConf->SenNo[4] = IR_SEN_08_000;
					pConf->SenNo[5] = IR_SEN_08_925;
					switch( pConf->SetNo.Cvt ){
						case 0: // C4F6 300 PPM
							pConf->iOut[0] = 5;
							pConf->CvtModel = MODEL_CVT_QUAD_X027_C4F6_300_PPM;
							pConf->GasNo[5] = IR_GAS_08_925_C4F6_300_PPM;
							pConf->Func[5] = MEASURING_GAS_FUNC_IR_OUT;
							pConf->OutMax = 1;
							pConf->iSpanScale[2] = 0.7f;
							pConf->iSpanScale[3] = 0.7f;
							pConf->iSpanScale[4] = 0.55f;
							pConf->iSpanScale[5] = 0.5f;
							break;
							
						default: err = true; break; // Undefined config number of LMM_274_X027
					}
					break;
					
				default: err = true; break; // Undefined sensor of 972
			}
			break;
			
		default: err = true; break; // Undefined detector type of Cavity
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool Channel_Config_Load_Combined( Measuring_ConfType *pConf )
{
	bool					err = false;
	Measuring_ConfType		Ctg = *pConf;
	Measuring_ConfType		Cvt = *pConf;
	
	err |= Channel_Config_Load_Cartridge( &Ctg );
	err |= Channel_Config_Load_Cavity( &Cvt );
	
	if( err == false ){
		if( (Ctg.OutMax + Cvt.OutMax) > MEASURING_GAS_OUTPUT_MAX ){
			err = true;
		}
		else {
			pConf->iRefCtg = Ctg.iRefCtg;
			pConf->iRefCvt = Cvt.iRefCvt;
			pConf->iOut[0] = Ctg.iOut[0];
			pConf->iOut[1] = Cvt.iOut[0];
			pConf->SetNo.Ctg[0] = Ctg.SetNo.Ctg[0];
			pConf->SetNo.Ctg[1] = Ctg.SetNo.Ctg[1];
			pConf->SetNo.Cvt = Cvt.SetNo.Cvt;
			pConf->CtgModel[0] = Ctg.CtgModel[0];
			pConf->CtgModel[1] = Ctg.CtgModel[1];
			pConf->CvtModel = Cvt.CvtModel;
			pConf->SenNo[0] = Ctg.SenNo[0];
			pConf->SenNo[1] = Ctg.SenNo[1];
			pConf->SenNo[2] = Cvt.SenNo[2];
			pConf->SenNo[3] = Cvt.SenNo[3];
			pConf->SenNo[4] = Cvt.SenNo[4];
			pConf->SenNo[5] = Cvt.SenNo[5];
			pConf->GasNo[0] = Ctg.GasNo[0];
			pConf->GasNo[1] = Ctg.GasNo[1];
			pConf->GasNo[2] = Cvt.GasNo[2];
			pConf->GasNo[3] = Cvt.GasNo[3];
			pConf->GasNo[4] = Cvt.GasNo[4];
			pConf->GasNo[5] = Cvt.GasNo[5];
			pConf->Func[0] = Ctg.Func[0];
			pConf->Func[1] = Ctg.Func[1];
			pConf->Func[2] = Cvt.Func[2];
			pConf->Func[3] = Cvt.Func[3];
			pConf->Func[4] = Cvt.Func[4];
			pConf->Func[5] = Cvt.Func[5];
			pConf->iSpanScale[0] = Ctg.iSpanScale[0];
			pConf->iSpanScale[1] = Ctg.iSpanScale[1];
			pConf->iSpanScale[2] = Cvt.iSpanScale[2];
			pConf->iSpanScale[3] = Cvt.iSpanScale[3];
			pConf->iSpanScale[4] = Cvt.iSpanScale[4];
			pConf->iSpanScale[5] = Cvt.iSpanScale[5];
			pConf->OutMax = Ctg.OutMax + Cvt.OutMax;
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Default_Gas_Info_Load_Cartridge( Measuring_GasInfo *pGas, Measuring_AlarmInfo *pAlr )
{
	Cartridge_TiaType	TIA = {
		.Item.RLOAD		= LPTIA_RLOAD_10,
		.Item.RFILTER	= LPTIA_RFILTER_20K,
		.Item.RGAIN		= LPTIA_RGAIN_10K,
		.Item.SWMODE	= LPTIA_SWMODE_NORM
	};
	S16		vZero = 400;
	S16		vBias = Measuring_CT_Sensor_Bias_Load( pGas );
	U16		vTarg = 1800;
	
	U16		Dp = 0;
	U16		HighScale = 100;
	U16		Unit = MEASURING_UNIT_PPM;
	U16		MaintenanceGas = 0;
	U16		CrossScale = 100;
	U16		SpanGas = 50;
	bool	bzUse = false;
	U16		bzPer = 30;
	U16		bzSec = 60;
	U16		ZeroSkip = 70;
	U16		SpanSkip = 30;
	U16		OutDelayPer = 70;
	U16		OutDelaySec = 5;
	bool	AL1_Increase = true;
	bool	AL2_Increase = true;
	S16		AL1 = 20;
	S16		AL2 = 40;
	
	switch( pGas->SenNo ){
		case CT_SEN_UNDEFINED:
			TIA.Word[0] = 0;
			TIA.Word[1] = 0;
			break;
			
		case CT_SEN_EC_DC_CL2_3:
			vZero = 1800;
			vTarg = 400;
			switch( pGas->GasNo ){
				case EC_GAS_EC_DC_CL2_3_BR2_3_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_160K;
					Dp = 2;
					HighScale = 300;
					SpanGas = 200;
					AL1 = 30;
					AL2 = 200;
					break;
					
				case EC_GAS_EC_DC_CL2_3_CL2_3_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_160K;
					Dp = 2;
					HighScale = 300;
					SpanGas = 200;
					AL1 = 25;
					AL2 = 50;
					break;
			}
			break;
			
		case CT_SEN_EC_DC_H2_2000:
			switch( pGas->GasNo ){
				case EC_GAS_EC_DC_H2_2000_H2_2000_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_16K;
					HighScale = 2000;
					SpanGas = 1000;
					AL1 = 500;
					AL2 = 1000;
					break;
			}
			break;
			
		case CT_SEN_EC_DC_HCL_15:
			switch( pGas->GasNo ){
				case EC_GAS_EC_DC_HCL_15_BCL3_15_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_128K;
					Dp = 1;
					HighScale = 150;
					SpanGas = 100;
					ZeroSkip = 90;
					AL1 = 30;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HCL_15_HCDS_20_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_64K;
					Dp = 1;
					HighScale = 200;
					CrossScale = 55;
					SpanGas = 100;
					ZeroSkip = 90;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HCL_15_HCL_10_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_196K;
					Dp = 1;
					SpanGas = 100;
					ZeroSkip = 90;
					AL1 = 10;
					AL2 = 20;
					break;
					
				case EC_GAS_EC_DC_HCL_15_LTO520_50_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_128K;
					Dp = 1;
					HighScale = 500;
					CrossScale = 300;
					SpanGas = 100;
					ZeroSkip = 90;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HCL_15_POCL3_15_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_128K;
					Dp = 1;
					HighScale = 150;
					SpanGas = 100;
					ZeroSkip = 90;
					AL1 = 10;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HCL_15_MCS_15_0_PPM:
				case EC_GAS_EC_DC_HCL_15_PCL3_15_0_PPM:
				case EC_GAS_EC_DC_HCL_15_SiCL4_15_0_PPM:
				case EC_GAS_EC_DC_HCL_15_SiH2CL2_15_0_PPM:
				case EC_GAS_EC_DC_HCL_15_SiHCL3_15_0_PPM:
				case EC_GAS_EC_DC_HCL_15_TCS_15_0_PPM:
				case EC_GAS_EC_DC_HCL_15_TICL4_15_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_128K;
					Dp = 1;
					HighScale = 150;
					SpanGas = 100;
					ZeroSkip = 90;
					AL1 = 50;
					AL2 = 100;
					break;
			}
			break;
			
		case CT_SEN_EC_DC_HF_9:
			vZero = 1800;
			vTarg = 400;
			switch( pGas->GasNo ){
				case EC_GAS_EC_DC_HF_9_BBR3_9_00_PPM:
				case EC_GAS_EC_DC_HF_9_BF3_9_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 900;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 100;
					AL2 = 600;
					break;
					
				case EC_GAS_EC_DC_HF_9_C2F4O_200_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					HighScale = 200;
					CrossScale = 1300;
					SpanGas = 10;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HF_9_CH2O2_50_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 1;
					HighScale = 500;
					CrossScale = 400;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HF_9_CLF3_1_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					CrossScale = 6;
					SpanGas = 1000;
					AL1 = 10;
					AL2 = 60;
					break;
					
				case EC_GAS_EC_DC_HF_9_F2_3_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 300;
					CrossScale = 20;
					SpanGas = 1000;
					AL1 = 30;
					AL2 = 200;
					break;
					
				case EC_GAS_EC_DC_HF_9_GEF4_9_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 900;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 300;
					AL2 = 600;
					break;
					
				case EC_GAS_EC_DC_HF_9_HBR_9_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 900;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 100;
					AL2 = 600;
					break;
					
				case EC_GAS_EC_DC_HF_9_HCOOH_50_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 1;
					HighScale = 500;
					CrossScale = 400;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 200;
					break;
					
				case EC_GAS_EC_DC_HF_9_HF_6_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 600;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 100;
					AL2 = 200;
					break;
					
				case EC_GAS_EC_DC_HF_9_HNO3_20_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 1;
					HighScale = 200;
					CrossScale = 150;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_EC_DC_HF_9_PF3_9_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 900;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 300;
					AL2 = 600;
					break;
					
				case EC_GAS_EC_DC_HF_9_SIF4_9_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 900;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 300;
					AL2 = 600;
					break;
					
				case EC_GAS_EC_DC_HF_9_WF6_9_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_256K;
					Dp = 2;
					HighScale = 900;
					CrossScale = 60;
					SpanGas = 1000;
					AL1 = 50;
					AL2 = 600;
					break;
			}
			break;
			
		case CT_SEN_EC_DC_O3_1:
			vZero = 1800;
			vTarg = 400;
			switch( pGas->GasNo ){
				case EC_GAS_EC_DC_O3_1_O3_1_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_512K;
					Dp = 2;
					AL1 = 8;
					AL2 = 20;
					break;
			}
			break;
			
		case CT_SEN_ETO_C_20:
			vZero = 600;
			vTarg = 1800;
			switch( pGas->GasNo ){
				case EC_GAS_ETO_C_20_C2H4O_EO_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_12K;
					Dp = 1;
					HighScale = 300;
					SpanGas = 200;
					AL2 = 200;
					break;
					
				case EC_GAS_ETO_C_20_C3H7OH_500_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_1K;
					HighScale = 500;
					CrossScale = 66;
					SpanGas = 300;
					AL1 = 100;
					AL2 = 200;
					break;
					
				case EC_GAS_ETO_C_20_C5H8O2_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_12K;
					Dp = 1;
					HighScale = 300;
					SpanGas = 200;
					AL1 = 100;
					AL2 = 200;
					break;
					
				case EC_GAS_ETO_C_20_C7H8_150_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_12K;
					HighScale = 150;
					CrossScale = 500;
					SpanGas = 20;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_ETO_C_20_TEB_15_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_12K;
					Dp = 1;
					HighScale = 150;
					CrossScale = 50;
					SpanGas = 200;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_ETO_C_20_TEOS_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_12K;
					Dp = 1;
					HighScale = 300;
					SpanGas = 200;
					AL1 = 100;
					AL2 = 200;
					break;
					
				case EC_GAS_ETO_C_20_USN_01_20_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_10K;
					Dp = 1;
					HighScale = 200;
					CrossScale = 60;
					SpanGas = 200;
					AL1 = 40;
					AL2 = 80;
					break;
			}
			break;
			
		case CT_SEN_H2_C_20000:
			switch( pGas->GasNo ){
				case EC_GAS_H2_C_20000_H2_4_000_VOL:
					TIA.Item.RGAIN = LPTIA_RGAIN_3K;
					Dp = 3;
					HighScale = 4000;
					Unit = MEASURING_UNIT_VOL;
					SpanGas = 2000;
					AL1 = 800;
					AL2 = 1600;
					break;
			}
			break;
			
		case CT_SEN_H2O2_CB_100:
			switch( pGas->GasNo ){
				case EC_GAS_H2O2_CB_100_H2O2_100_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_10K;
					SpanGas = 10;
					AL1 = 10;
					break;
			}
			break;
			
		case CT_SEN_H2S_C_50:
			switch( pGas->GasNo ){
				case EC_GAS_H2S_C_50_H2S_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_20K;
					Dp = 1;
					HighScale = 300;
					SpanGas = 250;
					AL1 = 100;
					AL2 = 200;
					break;
			}
			break;
			
		case CT_SEN_HCN_C_100:
			switch( pGas->GasNo ){
				case EC_GAS_HCN_C_100_HCN_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_120K;
					Dp = 1;
					HighScale = 300;
					CrossScale = 200;
					SpanGas = 100;
					AL1 = 30;
					AL2 = 200;
					break;
			}
			break;
			
		case CT_SEN_NH3_CR_200:
			switch( pGas->GasNo ){
				case EC_GAS_NH3_CR_200_ACP_02_150_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_120K;
					HighScale = 150;
					CrossScale = 200;
					SpanGas = 50;
					AL1 = 30;
					AL2 = 60;
					break;
					
				case EC_GAS_NH3_CR_200_C5H5N_150_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_120K;
					HighScale = 150;
					CrossScale = 200;
					SpanGas = 50;
					break;
					
				case EC_GAS_NH3_CR_200_NH3_75_0_PPM:
				case EC_GAS_NH3_CR_200_NH4OH_75_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_120K;
					Dp = 1;
					HighScale = 750;
					SpanGas = 500;
					AL1 = 125;
					AL2 = 250;
					break;
					
				case EC_GAS_NH3_CR_200_SAM24_75_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_120K;
					Dp = 1;
					HighScale = 750;
					SpanGas = 500;
					AL1 = 250;
					AL2 = 500;
					break;
			}
			break;
			
		case CT_SEN_NO_CF_100:
			vZero = 600;
			vTarg = 1800;
			switch( pGas->GasNo ){
				case EC_GAS_NO_CF_100_NO_100_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_24K;
					SpanGas = 50;
					AL1 = 25;
					AL2 = 50;
					break;
			}
			break;
			
		case CT_SEN_NO2_C_20:
			vZero = 1800;
			vTarg = 400;
			switch( pGas->GasNo ){
				case EC_GAS_NO2_C_20_NO2_15_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_64K;
					Dp = 1;
					HighScale = 150;
					SpanGas = 100;
					AL1 = 30;
					AL2 = 60;
					break;
			}
			break;
			
		case CT_SEN_PH3_C_5:
			switch( pGas->GasNo ){
				case EC_GAS_PH3_C_5_COS_100_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_196K;
					SpanGas = 100;
					AL1 = 25;
					AL2 = 60;
					break;
					
				case EC_GAS_PH3_C_5_GEH4_1_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_196K;
					Dp = 2;
					SpanGas = 100;
					AL2 = 60;
					break;
					
				case EC_GAS_PH3_C_5_H2SO4_25_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_196K;
					Dp = 1;
					HighScale = 250;
					CrossScale = 2500;
					SpanGas = 10;
					AL1 = 100;
					AL2 = 250;
					break;
					
				case EC_GAS_PH3_C_5_PH3_1_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_196K;
					Dp = 2;
					SpanGas = 100;
					AL1 = 30;
					AL2 = 60;
					break;
			}
			break;
			
		case CT_SEN_SIH4_CD_50:
			switch( pGas->GasNo ){
				case EC_GAS_SIH4_CD_50_3MS_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_128K;
					Dp = 1;
					HighScale = 300;
					CrossScale = 250;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_SIH4_CD_50_C3H10SI_30_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_128K;
					Dp = 1;
					HighScale = 300;
					CrossScale = 250;
					SpanGas = 100;
					AL1 = 100;
					AL2 = 200;
					break;
					
				case EC_GAS_SIH4_CD_50_GEH4_1_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_100K;
					Dp = 2;
					CrossScale = 6;
					SpanGas = 1000;
					AL2 = 60;
					break;
					
				case EC_GAS_SIH4_CD_50_SI2H6_15_0_PPM:
				case EC_GAS_SIH4_CD_50_SI3H8_15_0_PPM:
				case EC_GAS_SIH4_CD_50_SIH4_15_0_PPM:
				case EC_GAS_SIH4_CD_50_TSA_15_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_100K;
					Dp = 1;
					HighScale = 150;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 100;
					break;
			}
			break;
			
		case CT_SEN_SO2_C_20:
			switch( pGas->GasNo ){
				case EC_GAS_SO2_C_20_H2SO4_100_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_100K;
					CrossScale = 1000;
					SpanGas = 10;
					AL1 = 10;
					break;
					
				case EC_GAS_SO2_C_20_SO2_10_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_100K;
					Dp = 1;
					SpanGas = 100;
					AL2 = 60;
					break;
			}
			break;
			
		case CT_SEN_C2H4_C_1500:
			switch( pGas->GasNo ){
				case EC_GAS_C2H4_C_1500_C2H4_2000_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_40K;
					HighScale = 2000;
					SpanGas = 1000;
					AL1 = 200;
					AL2 = 1000;
					break;
					
				case EC_GAS_C2H4_C_1500_C3H6_2000_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_48K;
					HighScale = 2000;
					CrossScale = 110;
					SpanGas = 1000;
					AL1 = 500;
					AL2 = 1000;
					break;
			}
			break;
			
		case CT_SEN_CH2O_C_10:
			switch( pGas->GasNo ){
				case EC_GAS_CH2O_C_10_CH2O_10_0_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_3K;
					Dp = 1;
					CrossScale = 13;
					SpanGas = 750;
					AL1 = 10;
					AL2 = 60;
					break;
			}
			break;
			
		case CT_SEN_CO_CF_200:
			switch( pGas->GasNo ){
				case EC_GAS_CO_CF_200_CO_150_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_10K;
					HighScale = 150;
					SpanGas = 75;
					AL1 = 30;
					AL2 = 50;
					break;
			}
			break;
			
		case CT_SEN_O2_MP_100:
			vZero = 1300;
			vTarg = 400;
			switch( pGas->GasNo ){
				case EC_GAS_O2_MP_100_O2_30_0_VOL:
					TIA.Item.RGAIN = LPTIA_RGAIN_200;
					Dp = 1;
					HighScale = 300;
					Unit = MEASURING_UNIT_VOL;
					MaintenanceGas = 209;
					SpanGas = 209;
					SpanSkip = 0;
					AL1 = 180;
					AL2 = 230;
					AL1_Increase = false;
					break;
					
				case EC_GAS_O2_MP_100_O2_25_0_VOL:
					TIA.Item.RGAIN = LPTIA_RGAIN_200;
					Dp = 1;
					HighScale = 250;
					Unit = MEASURING_UNIT_VOL;
					MaintenanceGas = 209;
					SpanGas = 209;
					SpanSkip = 0;
					AL1 = 195;
					AL2 = 190;
					AL1_Increase = false;
					AL2_Increase = false;
					break;
			}
			break;
			
		case CT_SEN_ASH3_3E_1:
			TIA.Item.RLOAD = LPTIA_RLOAD_1600;
			switch( pGas->GasNo ){
				case EC_GAS_ASH3_3E_1_ASH3_0_300_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_512K;
					Dp = 3;
					HighScale = 300;
					CrossScale = 150;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_ASH3_3E_1_B2H6_0_300_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_512K;
					Dp = 3;
					HighScale = 300;
					SpanGas = 100;
					AL1 = 50;
					AL2 = 100;
					break;
					
				case EC_GAS_ASH3_3E_1_GEH4_3_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_512K;
					Dp = 2;
					HighScale = 300;
					CrossScale = 1500;
					SpanGas = 10;
					AL2 = 60;
					break;
			}
			break;
			
		case CT_SEN_COCL2_3E_1:
			TIA.Item.RLOAD = LPTIA_RLOAD_1600;
			switch( pGas->GasNo ){
				case EC_GAS_COCL2_3E_1_COCL2_1_00_PPM:
					TIA.Item.RGAIN = LPTIA_RGAIN_512K;
					Dp = 2;
					SpanGas = 50;
					AL1 = 10;
					AL2 = 20;
					break;
			}
			break;
			
		case CT_SEN_NAP_100AM:
			vZero = 1350;
			vTarg = 1800;
			TIA.Item.RLOAD = LPTIA_RLOAD_0;
			TIA.Item.RGAIN = LPTIA_RGAIN_DISCONNECT;
			TIA.Item.SWMODE = LPTIA_SWMODE_VOTAGE_FOLLOWER;
			ZeroSkip = 30;
			OutDelayPer = 30;
			switch( pGas->GasNo ){
				case CB_GAS_NAP_100AM_C2H6_100_LEL:
				case CB_GAS_NAP_100AM_C5H12_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 120;
					break;
				
				case CB_GAS_NAP_100AM_C3H6_100_LEL:
				case CB_GAS_NAP_100AM_C3H7OH_100_LEL:
				case CB_GAS_NAP_100AM_CH4_100_LEL:
				case CB_GAS_NAP_100AM_COMB_100_LEL:
				case CB_GAS_NAP_100AM_H_C_100_LEL:
				case CB_GAS_NAP_100AM_H2_100_LEL:
				case CB_GAS_NAP_100AM_D2_100_LEL:
				case CB_GAS_NAP_100AM_C4H10_100_LEL:
				case CB_GAS_NAP_100AM_LNG_100_LEL:
				case CB_GAS_NAP_100AM_LPG_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					break;
				
				case CB_GAS_NAP_100AM_C3H6O3_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 150;
					break;
				
				case CB_GAS_NAP_100AM_C3H8_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 110;
					break;
				
				case CB_GAS_NAP_100AM_C5H10_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 208;
					SpanGas = 25;
					break;
				
				case CB_GAS_NAP_100AM_C7H8_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 250;
					SpanGas = 25;
					break;
					
				case CB_GAS_NAP_100AM_PGMEA_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 200;
					SpanGas = 25;
					break;
			}
			break;
			
		case CT_SEN_GS_100A:
			vZero = 1350;
			vTarg = 1800;
			TIA.Item.RLOAD = LPTIA_RLOAD_0;
			TIA.Item.RGAIN = LPTIA_RGAIN_DISCONNECT;
			TIA.Item.SWMODE = LPTIA_SWMODE_VOTAGE_FOLLOWER;
			ZeroSkip = 30;
			OutDelayPer = 30;
			switch( pGas->GasNo ){
				case CB_GAS_GS_100A_C2H2_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 140;
					break;
					
				case CB_GAS_GS_100A_C2H4_100_LEL:
				case CB_GAS_GS_100A_C3H7OH_100_LEL:
				case CB_GAS_GS_100A_H2_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					break;
					
				case CB_GAS_GS_100A_C5H10_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 220;
					SpanGas = 25;
					break;
			}
			break;
			
		case CT_SEN_GS_100SC:
			vZero = 1350;
			vTarg = 1800;
			TIA.Item.RLOAD = LPTIA_RLOAD_0;
			TIA.Item.RGAIN = LPTIA_RGAIN_DISCONNECT;
			TIA.Item.SWMODE = LPTIA_SWMODE_VOTAGE_FOLLOWER;
			ZeroSkip = 30;
			OutDelayPer = 30;
			switch( pGas->GasNo ){
				case SC_GAS_GS_100SC_ORG_1000_PPM:
					HighScale = 1000;
					CrossScale = 40;
					SpanGas = 1000;
					AL1 = 200;
					AL2 = 400;
					break;
			}
			break;
	}
	
	pGas->TIA.Word[0] = TIA.Word[0];
	pGas->TIA.Word[1] = TIA.Word[1];
	pGas->TargetOffs = vZero;
	pGas->TargetGain = vTarg;
	pGas->TargetBias = (U16)(vZero - vBias);
	
	pGas->SpanGas = SpanGas;
	pGas->bZeroUse = bzUse;
	pGas->bZeroPer = bzPer;
	pGas->bZeroSec = bzSec;
	pGas->Dp = Dp;
	pGas->HighScale = HighScale;
	pGas->Unit = Unit;
	pGas->MaintenanceGas = MaintenanceGas;
	pGas->CrossScale = CrossScale;
	pGas->ZeroSkipPer = ZeroSkip;
	pGas->SpanSkipPer = SpanSkip;
	pGas->OutDelayPer = OutDelayPer;
	pGas->OutDelaySec = OutDelaySec;
	pAlr->_1stTypeIncrease = AL1_Increase;
	pAlr->_2ndTypeIncrease = AL2_Increase;
	pAlr->_1stLevel = AL1;
	pAlr->_2ndLevel = AL2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Default_Gas_Info_Load_Cavity( Measuring_GasInfo *pGas, Measuring_AlarmInfo *pAlr )
{
	U16		TargetOffs = 3300;
	U16		TargetGain = 2200;
	U16		SpanGas = 50;
	bool	bzUse = true;
	U16		bzPer = 30;
	U16		bzSec = 60;
	U16		Dp = 0;
	U16		HighScale = 100;
	U16		Unit = MEASURING_UNIT_VOL;
	U16		MaintenanceGas = 0;
	U16		CrossScale = 100;
	U16		ZeroSkip = 30;
	U16		SpanSkip = 30;
	U16		OutDelayPer = 50;
	U16		OutDelaySec = 5;
	S16		AL1 = 20;
	S16		AL2 = 40;
	bool	AL1_Increase = true;
	bool	AL2_Increase = true;
	
	switch( pGas->SenNo ){
		case IR_SEN_03_330:
			switch( pGas->GasNo ){
				case IR_GAS_03_330_LNG_100_LEL:
				case IR_GAS_03_330_CH4_100_LEL:
				case IR_GAS_03_330_C4H10_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					break;
					
				case IR_GAS_03_330_SFA_1_100_LEL:
					SpanGas = 57;
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 132;
					AL1 = 25;
					AL2 = 50;
					break;
					
				case IR_GAS_03_330_4MS_100_LEL:
					SpanGas = 26;
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 289;
					AL1 = 25;
					AL2 = 50;
					break;
					
				case IR_GAS_03_330_TOLUENE_100_LEL:
					SpanGas = 75;
					bzUse = false;
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 66;
					ZeroSkip = 0;
					break;
					
				case IR_GAS_03_330_CH4_100_VOL:
					SpanGas = 10;
					bzUse = false;
					break;
			}
			break;
			
		case IR_SEN_03_370:
			switch( pGas->GasNo ){
				case IR_GAS_03_370_GMP_02_3000_PPM:
					SpanGas = 1500;
					HighScale = 3000;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 100;
					AL2 = 200;
					break;
					
				case IR_GAS_03_370_OMCTS_100_LEL:
					SpanGas = 16;
					HighScale = 100;
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 156;
					ZeroSkip = 50;
					AL1 = 25;
					AL2 = 100;
					break;
					
				case IR_GAS_03_370_BTBAS_1000_PPM:
					SpanGas = 3100;
					HighScale = 1000;
					Unit = MEASURING_UNIT_PPM;
					CrossScale = 24;
					ZeroSkip = 80;
					AL1 = 200;
					AL2 = 400;
					break;
			}
			break;
			
		case IR_SEN_03_420:
			switch( pGas->GasNo ){
				case IR_GAS_03_420_ECH_100_LEL:
					SpanGas = 69;
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 145;
					AL1 = 25;
					AL2 = 50;
					break;
			}
			break;
			
		case IR_SEN_03_720:
		case IR_SEN_03_900:
		case IR_SEN_03_950:
			bzUse = false;
			break;
			
		case IR_SEN_04_260:
			switch( pGas->GasNo ){
				case IR_GAS_04_260_CO2_5000_PPM:
					SpanGas = 2500;
					bzUse = false;
					HighScale = 5000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 10;
					SpanSkip = 20;
					AL1 = 1000;
					AL2 = 2000;
					break;
					
				case IR_GAS_04_260_CO2_9999_PPM:
					SpanGas = 5000;
					bzUse = false;
					HighScale = 9999;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 10;
					SpanSkip = 20;
					AL1 = 2500;
					AL2 = 5000;
					break;
					
				case IR_GAS_04_260_CO2_1_00_VOL:
					bzUse = false;
					Dp = 2;
					ZeroSkip = 10;
					SpanSkip = 20;
					AL1 = 25;
					AL2 = 50;
					break;
					
				case IR_GAS_04_260_CO2_5_0_VOL:
					bzUse = false;
					Dp = 1;
					HighScale = 50;
					SpanSkip = 50;
					AL1 = 10;
					AL2 = 20;
					break;
			}
			break;
			
		case IR_SEN_04_520:
			switch( pGas->GasNo ){
				case IR_GAS_04_520_N2O_4000_PPM:
					SpanGas = 2000;
					HighScale = 4000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 50;
					SpanSkip = 20;
					AL1 = 500;
					AL2 = 2000;
					break;
					
				case IR_GAS_04_520_N2O_1000_PPM:
					SpanGas = 500;
					HighScale = 1000;
					Unit = MEASURING_UNIT_PPM;
					SpanSkip = 20;
					AL1 = 50;
					AL2 = 100;
					break;
			}
			break;
			
		case IR_SEN_04_660:
			switch( pGas->GasNo ){
				case IR_GAS_04_660_CO_9999_PPM:
					SpanGas = 5000;
					HighScale = 9999;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 2500;
					AL2 = 5000;
					break;
					
				case IR_GAS_04_660_CO_1_00_VOL:
					Dp = 2;
					break;
					
				case IR_GAS_04_660_CO_2_00_VOL:
					SpanGas = 100;
					Dp = 2;
					HighScale = 200;
					Unit = MEASURING_UNIT_VOL;
					AL1 = 40;
					AL2 = 80;
					break;
					
				case IR_GAS_04_660_CO_5_00_VOL:
					SpanGas = 200;
					Dp = 2;
					HighScale = 500;
					Unit = MEASURING_UNIT_VOL;
					AL1 = 100;
					AL2 = 200;
					break;
			}
			break;
			
		case IR_SEN_04_850:
			switch( pGas->GasNo ){
				case IR_GAS_04_850_COS_200_PPM:
					SpanGas = 100;
					HighScale = 200;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 25;
					AL2 = 100;
					break;
					
				case IR_GAS_04_850_COS_75_0_PPM:
					SpanGas = 300;
					Dp = 1;
					HighScale = 750;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 50;
					AL2 = 100;
					break;
			}
			break;
			
		case IR_SEN_08_000:
			switch( pGas->GasNo ){
				case IR_GAS_08_000_C4F8_2000_PPM:
					SpanGas = 1500;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 500;
					AL2 = 1000;
					break;
					
				case IR_GAS_08_000_FL300_200_PPM:
					SpanGas = 620;
					HighScale = 200;
					Unit = MEASURING_UNIT_PPM;
					CrossScale = 16;
					AL1 = 40;
					AL2 = 80;
					break;
					
				case IR_GAS_08_000_CF4_500_PPM:
					SpanGas = 250;
					HighScale = 500;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 100;
					AL2 = 200;
					break;
			}
			break;
			
		case IR_SEN_08_925:
			switch( pGas->GasNo ){
				case IR_GAS_08_925_C5F8_2000_PPM:
					SpanGas = 1500;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 70;
					SpanSkip = 70;
					AL1 = 500;
					AL2 = 1000;
					break;
					
				case IR_GAS_08_925_C4F6_2000_PPM:
				case IR_GAS_08_925_CH2F2_2000_PPM:
				case IR_GAS_08_925_HFC_2000_PPM:
					SpanGas = 1500;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 500;
					AL2 = 1000;
					break;
					
				case IR_GAS_08_925_TEOS_100_PPM:
				case IR_GAS_08_925_TEOS_HT200_100_PPM:
					SpanGas = 30;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 10;
					AL2 = 20;
					break;
					
				case IR_GAS_08_925_IPA_3000_PPM:
					SpanGas = 2250;
					HighScale = 3000;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 200;
					AL2 = 400;
					break;
					
				case IR_GAS_08_925_IPA_100_LEL:
					SpanGas = 30;
					Unit = MEASURING_UNIT_LEL;
					ZeroSkip = 50;
					break;
					
				case IR_GAS_08_925_SELITANE_100_LEL:
					SpanGas = 30;
					Unit = MEASURING_UNIT_LEL;
					CrossScale = 110;
					ZeroSkip = 50;
					break;
					
				case IR_GAS_08_925_C4F6_300_PPM:
					SpanGas = 240;
					HighScale = 300;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 50;
					AL2 = 100;
					break;
			}
			break;
			
		case IR_SEN_09_076:
			switch( pGas->GasNo ){
				case IR_GAS_09_076_C4F6S_2000_PPM:
					SpanGas = 1500;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					CrossScale = 66;
					AL1 = 500;
					AL2 = 1000;
					break;
			}
			break;
			
		case IR_SEN_09_150:
			switch( pGas->GasNo ){
				case IR_GAS_09_150_C4H2F6_1000_PPM:
					SpanGas = 500;
					HighScale = 1000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 70;
					AL1 = 140;
					AL2 = 280;
					break;
			}
			break;
			
		case IR_SEN_09_230:
			switch( pGas->GasNo ){
				case IR_GAS_09_230_OMCTS_75_0_PPM:
					SpanGas = 500;
					Dp = 1;
					HighScale = 750;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 50;
					AL1 = 100;
					AL2 = 200;
					break;
			}
			break;
			
		case IR_SEN_09_440:
			switch( pGas->GasNo ){
				case IR_GAS_09_440_CH3F_2000_PPM:
					SpanGas = 1000;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 70;
					AL1 = 500;
					AL2 = 1000;
					break;
					
				case IR_GAS_09_440_ETHANOL_100_LEL:
					SpanGas = 20;
					Unit = MEASURING_UNIT_LEL;
					ZeroSkip = 50;
					break;
					
				case IR_GAS_09_440_CTFE_1000_PPM:
					SpanGas = 926;
					HighScale = 1000;
					Unit = MEASURING_UNIT_PPM;
					CrossScale = 54;
					AL1 = 250;
					AL2 = 500;
					break;
					
				case IR_GAS_09_440_C3F6_2000_PPM:
					SpanGas = 1000;
					Dp = 0;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 500;
					AL2 = 1000;
					break;
			}
			break;
			
		case IR_SEN_09_790:
			switch( pGas->GasNo ){
				case IR_GAS_09_790_C3F6_2000_PPM:
					SpanGas = 1500;
					HighScale = 2000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 50;
					AL1 = 300;
					AL2 = 600;
					break;
			}
			break;
			
		case IR_SEN_10_100:
			switch( pGas->GasNo ){
				case IR_GAS_10_100_METHANOL_3000_PPM:
					SpanGas = 1500;
					HighScale = 3000;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 200;
					AL2 = 400;
					break;
			}
			break;
			
		case IR_SEN_10_990:
			switch( pGas->GasNo ){
				case IR_GAS_10_990_NF3_200_PPM:
					if( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE] == CAVITY_SEN_TYPE_LIM_272_X127 ){
						TargetGain = 1800;
					}
					SpanGas = 100;
					HighScale = 200;
					Unit = MEASURING_UNIT_PPM;
					AL1 = 10;
					AL2 = 20;
					break;
					
				case IR_GAS_10_990_HMDS_1000_PPM:
					SpanGas = 500;
					HighScale = 1000;
					Unit = MEASURING_UNIT_PPM;
					ZeroSkip = 50;
					AL1 = 250;
					AL2 = 500;
					break;
					
				case IR_GAS_10_990_HMDS_100_LEL:
					SpanGas = 12;
					Unit = MEASURING_UNIT_LEL;
					ZeroSkip = 50;
					break;
					
				case IR_GAS_10_990_C3F6O_5000_PPM:
					SpanGas = 2600;
					HighScale = 5000;
					Unit = MEASURING_UNIT_PPM;
					CrossScale = 115;
					ZeroSkip = 70;
					AL1 = 800;
					AL2 = 1600;
					break;
					
				case IR_GAS_10_990_C3H6_100_LEL:
				case IR_GAS_10_990_C2H4_100_LEL:
					Unit = MEASURING_UNIT_LEL;
					break;
			}
			break;
	}
	
	pGas->TargetOffs = TargetOffs;
	pGas->TargetGain = TargetGain;
	pGas->SpanGas = SpanGas;
	pGas->bZeroUse = bzUse;
	pGas->bZeroPer = bzPer;
	pGas->bZeroSec = bzSec;
	pGas->Dp = Dp;
	pGas->HighScale = HighScale;
	pGas->Unit = Unit;
	pGas->MaintenanceGas = MaintenanceGas;
	pGas->CrossScale = CrossScale;
	pGas->ZeroSkipPer = ZeroSkip;
	pGas->SpanSkipPer = SpanSkip;
	pGas->OutDelayPer = OutDelayPer;
	pGas->OutDelaySec = OutDelaySec;
	pAlr->_1stTypeIncrease = AL1_Increase;
	pAlr->_2ndTypeIncrease = AL2_Increase;
	pAlr->_1stLevel = AL1;
	pAlr->_2ndLevel = AL2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Gas_Info_Load( void )
{
	U8			i;
	U16			*pSensorEep;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ )
	{
		if( i < CARTRIDGE_SEN_CH_MAX ){
			pSensorEep = &MeasHandle.pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_CH1 + i][0];
		}
		else {
			pSensorEep = &MeasHandle.pSen->pCvt->Eep[CAVITY_EEP_BLK_CH1 + i - CARTRIDGE_SEN_CH_MAX][0];
		}
		
		MeasHandle.Gas[i].Inf.TIA.Word[0]		= pSensorEep[SENSOR_EEP_CHn_PTM_mOFFS_TIA_0];
		MeasHandle.Gas[i].Inf.TIA.Word[1]		= pSensorEep[SENSOR_EEP_CHn_PTM_mGAIN_TIA_1];
		MeasHandle.Gas[i].Inf.TargetOffs		= pSensorEep[SENSOR_EEP_CHn_AMP_CAL_TARGET_mV_OFFS];
		MeasHandle.Gas[i].Inf.TargetGain		= pSensorEep[SENSOR_EEP_CHn_AMP_CAL_TARGET_mV_GAIN];
		MeasHandle.Gas[i].Inf.TargetBias		= pSensorEep[SENSOR_EEP_CHn_VBIAS];
		MeasHandle.Gas[i].Inf.StableTime		= pSensorEep[SENSOR_EEP_CHn_STABLE_HOLD_TIME_SEC];
		MeasHandle.Gas[i].Inf.mZero.w.l			= pSensorEep[SENSOR_EEP_CHn_mZERO_L];
		MeasHandle.Gas[i].Inf.mZero.w.h			= pSensorEep[SENSOR_EEP_CHn_mZERO_H];
		MeasHandle.Gas[i].Inf.mZeroTemp.w.l		= pSensorEep[SENSOR_EEP_CHn_mZERO_TEMP_L];
		MeasHandle.Gas[i].Inf.mZeroTemp.w.h		= pSensorEep[SENSOR_EEP_CHn_mZERO_TEMP_H];
		MeasHandle.Gas[i].Inf.mSpan.w.l			= pSensorEep[SENSOR_EEP_CHn_mSPAN_L];
		MeasHandle.Gas[i].Inf.mSpan.w.h			= pSensorEep[SENSOR_EEP_CHn_mSPAN_H];
		MeasHandle.Gas[i].Inf.mSpanTemp.w.l		= pSensorEep[SENSOR_EEP_CHn_mSPAN_TEMP_L];
		MeasHandle.Gas[i].Inf.mSpanTemp.w.h		= pSensorEep[SENSOR_EEP_CHn_mSPAN_TEMP_H];
		MeasHandle.Gas[i].Inf.SpanGas			= pSensorEep[SENSOR_EEP_CHn_SPAN_GAS];
		MeasHandle.Gas[i].Inf.bZero.w.l			= pSensorEep[SENSOR_EEP_CHn_bZERO_L];
		MeasHandle.Gas[i].Inf.bZero.w.h			= pSensorEep[SENSOR_EEP_CHn_bZERO_H];
		MeasHandle.Gas[i].Inf.bZeroTemp.w.l		= pSensorEep[SENSOR_EEP_CHn_bZERO_TEMP_L];
		MeasHandle.Gas[i].Inf.bZeroTemp.w.h		= pSensorEep[SENSOR_EEP_CHn_bZERO_TEMP_H];
		MeasHandle.Gas[i].Inf.bSpan.w.l			= pSensorEep[SENSOR_EEP_CHn_bSPAN_L];
		MeasHandle.Gas[i].Inf.bSpan.w.h			= pSensorEep[SENSOR_EEP_CHn_bSPAN_H];
		MeasHandle.Gas[i].Inf.bSpanTemp.w.l		= pSensorEep[SENSOR_EEP_CHn_bSPAN_TEMP_L];
		MeasHandle.Gas[i].Inf.bSpanTemp.w.h		= pSensorEep[SENSOR_EEP_CHn_bSPAN_TEMP_H];
		MeasHandle.Gas[i].Inf.rZero				= 1.f;
		MeasHandle.Gas[i].Inf.rSpan				= 1.f;
		
		MeasHandle.Gas[i].Inf.GasName[0]		= pSensorEep[SENSOR_EEP_CHn_GAS_NAME_0];
		MeasHandle.Gas[i].Inf.GasName[1]		= pSensorEep[SENSOR_EEP_CHn_GAS_NAME_1];
		MeasHandle.Gas[i].Inf.GasName[2]		= pSensorEep[SENSOR_EEP_CHn_GAS_NAME_2];
		MeasHandle.Gas[i].Inf.GasName[3]		= pSensorEep[SENSOR_EEP_CHn_GAS_NAME_3];
		MeasHandle.Gas[i].Inf.GasName[4]		= pSensorEep[SENSOR_EEP_CHn_GAS_NAME_4];
		MeasHandle.Gas[i].Inf.GasName[5]		= pSensorEep[SENSOR_EEP_CHn_GAS_NAME_5];
		
		MeasHandle.Gas[i].Inf.bZeroUse			= pSensorEep[SENSOR_EEP_CHn_bZERO_USE];
		MeasHandle.Gas[i].Inf.bZeroPer			= pSensorEep[SENSOR_EEP_CHn_bZERO_PERCENTAGE_x10];
		MeasHandle.Gas[i].Inf.bZeroSec			= pSensorEep[SENSOR_EEP_CHn_bZERO_SEC];
		MeasHandle.Gas[i].Inf.bZeroCnt			= pSensorEep[SENSOR_EEP_CHn_bZERO_COUNT];
		MeasHandle.Gas[i].Inf.bZeroTmr			= SecToMeasuringTime( 10 );
		
		MeasHandle.Gas[i].Inf.Method			= pSensorEep[SENSOR_EEP_CHn_METHOD];
		MeasHandle.Gas[i].Inf.SenNo				= pSensorEep[SENSOR_EEP_CHn_SEN_NO];
		MeasHandle.Gas[i].Inf.GasNo				= pSensorEep[SENSOR_EEP_CHn_GAS_NO];
		MeasHandle.Gas[i].Inf.Dp				= pSensorEep[SENSOR_EEP_CHn_DP];
		MeasHandle.Gas[i].Inf.HighScale			= pSensorEep[SENSOR_EEP_CHn_HIGH_SCALE];
		MeasHandle.Gas[i].Inf.Unit				= pSensorEep[SENSOR_EEP_CHn_UNIT];
		MeasHandle.Gas[i].Inf.MaintenanceGas	= pSensorEep[SENSOR_EEP_CHn_MAINTENANCE_GAS];
		MeasHandle.Gas[i].Inf.CrossScale		= pSensorEep[SENSOR_EEP_CHn_CROSS_SENS_SCALE_x100];
		
		MeasHandle.Gas[i].Inf.ZeroSkipPer		= pSensorEep[SENSOR_EEP_CHn_ZERO_SKIP_PERCENTAGE_x10];
		MeasHandle.Gas[i].Inf.SpanSkipPer		= pSensorEep[SENSOR_EEP_CHn_SPAN_SKIP_PERCENTAGE_x10];
		MeasHandle.Gas[i].Inf.OutDelayPer		= pSensorEep[SENSOR_EEP_CHn_OUT_DELAY_PERCENTAGE_x10];
		MeasHandle.Gas[i].Inf.OutDelaySec		= pSensorEep[SENSOR_EEP_CHn_OUT_DELAY_SEC];
		MeasHandle.Gas[i].Inf.OutDelayTmr		= SecToMeasuringTime( MeasHandle.Gas[i].Inf.OutDelaySec );
		
		Measuring_Gas_MSD_Load( &MeasHandle.Gas[i].Inf );
		
		MeasHandle.Gas[i].Per = (float)MeasHandle.Gas[i].Inf.MaintenanceGas / (float)MeasHandle.Gas[i].Inf.HighScale * 100.f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Alr_Info_Load( void )
{
	U16		*pSensorEep;
	U8		i;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		if( i < CARTRIDGE_SEN_CH_MAX ){
			pSensorEep = &MeasHandle.pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_CH1 + i][0];
		}
		else {
			pSensorEep = &MeasHandle.pSen->pCvt->Eep[CAVITY_EEP_BLK_CH1 + i - CARTRIDGE_SEN_CH_MAX][0];
		}
		
		MeasHandle.Alr[i].Inf.IsUnderCheckEnabled	= pSensorEep[SENSOR_EEP_CHn_ALARM_UNDER_CHECK];
		MeasHandle.Alr[i].Inf._1stLatchEnabled		= pSensorEep[SENSOR_EEP_CHn_ALARM_1ST_LATCH];
		MeasHandle.Alr[i].Inf._1stTypeIncrease		= pSensorEep[SENSOR_EEP_CHn_ALARM_1ST_INCREASE_TYPE];
		MeasHandle.Alr[i].Inf._1stLevel				= (S16)pSensorEep[SENSOR_EEP_CHn_ALARM_1ST_LEVEL];
		MeasHandle.Alr[i].Inf._1stDead				= pSensorEep[SENSOR_EEP_CHn_ALARM_1ST_DEAD_BAND];
		MeasHandle.Alr[i].Inf._1stDelaySec			= pSensorEep[SENSOR_EEP_CHn_ALARM_1ST_DELAY_SEC];
		MeasHandle.Alr[i].Inf._2ndLatchEnabled		= pSensorEep[SENSOR_EEP_CHn_ALARM_2ND_LATCH];
		MeasHandle.Alr[i].Inf._2ndTypeIncrease		= pSensorEep[SENSOR_EEP_CHn_ALARM_2ND_INCREASE_TYPE];
		MeasHandle.Alr[i].Inf._2ndLevel				= (S16)pSensorEep[SENSOR_EEP_CHn_ALARM_2ND_LEVEL];
		MeasHandle.Alr[i].Inf._2ndDead				= pSensorEep[SENSOR_EEP_CHn_ALARM_2ND_DEAD_BAND];
		MeasHandle.Alr[i].Inf._2ndDelaySec			= pSensorEep[SENSOR_EEP_CHn_ALARM_2ND_DELAY_SEC];
		MeasHandle.Alr[i].Inf.Det._1st_Pre			= false;
		MeasHandle.Alr[i].Inf.Det._1st_Dly			= MeasHandle.Alr[i].Inf._1stDelaySec;
		MeasHandle.Alr[i].Inf.Det._2nd_Pre			= false;
		MeasHandle.Alr[i].Inf.Det._2nd_Dly			= MeasHandle.Alr[i].Inf._2ndDelaySec;
		MeasHandle.Alr[i].Latched					= false;
		MeasHandle.Alr[i].Item.Byte					= 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Gas_Info_Check( void )
{
	U8		i;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
			if( (MeasHandle.Gas[i].Inf.SenNo != MeasHandle.Conf.SenNo[i]) ||
				(MeasHandle.Gas[i].Inf.GasNo != MeasHandle.Conf.GasNo[i]) )
			{
				MeasHandle.Gas[i].Inf.SenNo = MeasHandle.Conf.SenNo[i];
				MeasHandle.Gas[i].Inf.GasNo = MeasHandle.Conf.GasNo[i];
				Measuring_Default_Gas_Info_Load( &MeasHandle.Gas[i].Inf, &MeasHandle.Alr[i].Inf );
				Measuring_Gas_Info_Update( i );
			}
			else if( MeasHandle.Gas[i].Inf.GasName[0] == 0 ){
				Measuring_Default_Gas_Name_Load( &MeasHandle.Gas[i].Inf );
				Measuring_Gas_Name_Update( i );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Raw_Mapping( void )
{
	U8		i;
	
	MeasHandle.Raw.SysSenTemp	= MeasHandle.pSen->Raw.Temp.LmtTemp;
	MeasHandle.Raw.SysMcuTemp	= MeasHandle.pSen->Raw.Temp.McuTemp;
	MeasHandle.Raw.BatSenTemp	= MeasHandle.pSen->Raw.Temp.BatTemp;
	MeasHandle.Raw.BatSenJper	= MeasHandle.pSen->Raw.Temp.BatJper;
	
	MeasHandle.Raw.Psen			= MeasHandle.pSen->Raw.Pressure.AmpOut;
	MeasHandle.Raw.FlowRate		= FlowCtrl.FlowRate;
	if( SystemOption.Item.IsBatMode == true ){
		MeasHandle.Raw.VSUP		= MeasHandle.pSen->Raw.Power.VSYS;
	}
	else {
		MeasHandle.Raw.VSUP		= MeasHandle.pSen->Raw.Power.VSUP;
		MeasHandle.Raw.P48		= MeasHandle.pSen->Raw.Power.P48;
		MeasHandle.Raw.P24		= MeasHandle.pSen->Raw.Power.P24;
	}
	MeasHandle.Raw.VBACKUP		= MeasHandle.pSen->Raw.Power.VBACKUP;
	
	MeasHandle.Raw.IBUS			= MeasHandle.pSen->Raw.Power.IBUS;
	MeasHandle.Raw.IBAT			= MeasHandle.pSen->Raw.Power.IBAT;
	MeasHandle.Raw.VBUS			= MeasHandle.pSen->Raw.Power.VBUS;
	MeasHandle.Raw.VBAT			= MeasHandle.pSen->Raw.Power.VBAT;
	
	MeasHandle.Raw.CtgSenHumi	= MeasHandle.pSen->Raw.CT.SenHumi.f;
	MeasHandle.Raw.CtgSenTemp	= MeasHandle.pSen->Raw.CT.SenTemp.f;
	MeasHandle.Raw.CtgMcuTemp	= MeasHandle.pSen->Raw.CT.McuTemp.f;
	
	MeasHandle.Raw.CtgRawDvSum	= MeasHandle.pSen->Raw.CT.RawDvSum;
	
	MeasHandle.Raw.CtgAVDD		= MeasHandle.pSen->Raw.CT.AVDD.f;
	MeasHandle.Raw.CtgDVDD		= MeasHandle.pSen->Raw.CT.DVDD.f;
	MeasHandle.Raw.CtgVCB		= MeasHandle.pSen->Raw.CT.VCB.f;
	
	for( i=0; i<MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS]; i++ ){
		MeasHandle.Raw.CtgPeakDv.LPTIA_LPF[i]	= MeasHandle.pSen->Raw.CT.PeakDv.LPTIA_LPF[i];
		MeasHandle.Raw.CtgPeakDv.VZERO[i]		= MeasHandle.pSen->Raw.CT.PeakDv.VZERO[i];
		MeasHandle.Raw.CtgPeakDv.VBIAS[i]		= MeasHandle.pSen->Raw.CT.PeakDv.VBIAS[i];
		MeasHandle.Raw.CtgVZERO[i]				= MeasHandle.pSen->Raw.CT.VZERO[i].f;
		MeasHandle.Raw.CtgVBIAS[i]				= MeasHandle.pSen->Raw.CT.VBIAS[i].f;
		MeasHandle.Raw.CtgICB[i]				= MeasHandle.pSen->Raw.CT.ICB[i].f;
	}
	
	MeasHandle.Raw.CvtSenTemp	= MeasHandle.pSen->Raw.IR.SenTemp.f;
	MeasHandle.Raw.CvtMcuTemp	= MeasHandle.pSen->Raw.IR.McuTemp.f;
	
	MeasHandle.Raw.CvtSrcPwr	= MeasHandle.pSen->Raw.IR.SrcPwr;
	
	for( i=0; i<MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; i++ ){
		
		MeasHandle.Raw.CvtHiPeak[i] = MeasHandle.pSen->Raw.IR.HiPeak[i].f;
		MeasHandle.Raw.CvtLoPeak[i] = MeasHandle.pSen->Raw.IR.LoPeak[i].f;
		MeasHandle.Raw.CvtOcAvrg[i] = MeasHandle.pSen->Raw.IR.OcAvrg[i].f;
		MeasHandle.Raw.CvtHiTime[i] = MeasHandle.pSen->Raw.IR.HiTime[i];
		MeasHandle.Raw.CvtLoTime[i] = MeasHandle.pSen->Raw.IR.LoTime[i];
		MeasHandle.Raw.CvtRawDvMax[i] = MeasHandle.pSen->Raw.IR.RawDvMax[i];
	}
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
			if( i < CARTRIDGE_SEN_CH_MAX ){
				MeasHandle.Raw.nDet[i] = MeasHandle.pSen->Raw.CT.LPTIA_LPF[i].f;
			}
			else {
				MeasHandle.Raw.nDet[i] = MeasHandle.pSen->Raw.IR.OcPtoP[i-CARTRIDGE_SEN_CH_MAX].f;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Lowpass_Filtering( void )
{
	U8		i, iCvt;
	
	// System
	MeasHandle.pCur->SysSenTemp		= OtherLpf( MeasHandle.pPrv->SysSenTemp, MeasHandle.Raw.SysSenTemp );
	MeasHandle.pCur->SysMcuTemp		= OtherLpf( MeasHandle.pPrv->SysMcuTemp, MeasHandle.Raw.SysMcuTemp );
	MeasHandle.pCur->BatSenTemp		= OtherLpf( MeasHandle.pPrv->BatSenTemp, MeasHandle.Raw.BatSenTemp );
	MeasHandle.pCur->BatSenJper		= OtherLpf( MeasHandle.pPrv->BatSenJper, MeasHandle.Raw.BatSenJper );
	
	MeasHandle.pCur->Psen			= OtherLpf( MeasHandle.pPrv->Psen, MeasHandle.Raw.Psen );
	MeasHandle.pCur->FlowRate		= OtherLpf( MeasHandle.pPrv->FlowRate, MeasHandle.Raw.FlowRate );
	
	if( SystemOption.Item.IsUniMode == true ){
		MeasHandle.pCur->P48 = OtherLpf( MeasHandle.pPrv->P48, MeasHandle.Raw.P48 );
		MeasHandle.pCur->P24 = OtherLpf( MeasHandle.pPrv->P24, MeasHandle.Raw.P24 );
		if( MeasHandle.pCur->P24  > MeasHandle.pCur->P48 ){
			MeasHandle.pCur->VSUP = MeasHandle.pCur->P24;
		}
		else {
			MeasHandle.pCur->VSUP = MeasHandle.pCur->P48;
		}
	}
	else {
		MeasHandle.pCur->VSUP = OtherLpf( MeasHandle.pPrv->VSUP, MeasHandle.Raw.VSUP );
	}
	MeasHandle.pCur->VBACKUP		= OtherLpf( MeasHandle.pPrv->VBACKUP, MeasHandle.Raw.VBACKUP );
	
	MeasHandle.pCur->IBUS			= OtherLpf( MeasHandle.pPrv->IBUS, MeasHandle.Raw.IBUS );
	MeasHandle.pCur->IBAT			= OtherLpf( MeasHandle.pPrv->IBAT, MeasHandle.Raw.IBAT );
	MeasHandle.pCur->VBUS			= OtherLpf( MeasHandle.pPrv->VBUS, MeasHandle.Raw.VBUS );
	MeasHandle.pCur->VBAT			= OtherLpf( MeasHandle.pPrv->VBAT, MeasHandle.Raw.VBAT );
	
	// Cartirdge
	MeasHandle.pCur->CtgSenHumi		= OtherLpf( MeasHandle.pPrv->CtgSenHumi, MeasHandle.Raw.CtgSenHumi );
	MeasHandle.pCur->CtgSenTemp		= OtherLpf( MeasHandle.pPrv->CtgSenTemp, MeasHandle.Raw.CtgSenTemp );
	MeasHandle.pCur->CtgMcuTemp		= OtherLpf( MeasHandle.pPrv->CtgMcuTemp, MeasHandle.Raw.CtgMcuTemp );
	
	MeasHandle.pCur->CtgRawDvSum	= MeasHandle.Raw.CtgRawDvSum;
	
	MeasHandle.pCur->CtgAVDD		= OtherLpf( MeasHandle.pPrv->CtgAVDD, MeasHandle.Raw.CtgAVDD );
	MeasHandle.pCur->CtgDVDD		= OtherLpf( MeasHandle.pPrv->CtgDVDD, MeasHandle.Raw.CtgDVDD );
	MeasHandle.pCur->CtgVCB			= OtherLpf( MeasHandle.pPrv->CtgVCB, MeasHandle.Raw.CtgVCB );
	
	for( i=0; i<MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS]; i++ ){
		MeasHandle.pCur->CtgPeakDv.LPTIA_LPF[i]	= MeasHandle.Raw.CtgPeakDv.LPTIA_LPF[i];
		MeasHandle.pCur->CtgPeakDv.VZERO[i]		= MeasHandle.Raw.CtgPeakDv.VZERO[i];
		MeasHandle.pCur->CtgPeakDv.VBIAS[i]		= MeasHandle.Raw.CtgPeakDv.VBIAS[i];
		MeasHandle.pCur->CtgVZERO[i]			= OtherLpf( MeasHandle.pPrv->CtgVZERO[i], MeasHandle.Raw.CtgVZERO[i] );
		MeasHandle.pCur->CtgVBIAS[i]			= OtherLpf( MeasHandle.pPrv->CtgVBIAS[i], MeasHandle.Raw.CtgVBIAS[i] );
		MeasHandle.pCur->CtgICB[i]				= OtherLpf( MeasHandle.pPrv->CtgICB[i], MeasHandle.Raw.CtgICB[i] );
		if( MeasHandle.Gas[i].IsPresent == true ){
			MeasHandle.pCur->nDet[i] = CtSenLpf( MeasHandle.pPrv->nDet[i], MeasHandle.Raw.nDet[i] );
		}
	}
	
	// Cavity
	MeasHandle.pCur->CvtSenTemp		= OtherLpf( MeasHandle.pPrv->CvtSenTemp, MeasHandle.Raw.CvtSenTemp );
	MeasHandle.pCur->CvtMcuTemp		= OtherLpf( MeasHandle.pPrv->CvtMcuTemp, MeasHandle.Raw.CvtMcuTemp );
	
	MeasHandle.pCur->CvtSrcPwr		= OtherLpf( MeasHandle.pPrv->CvtSrcPwr, MeasHandle.Raw.CvtSrcPwr );
	
	for( i=0, iCvt=CARTRIDGE_SEN_CH_MAX; i<MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; i++, iCvt++ ){
		MeasHandle.pCur->CvtHiPeak[i] = IrSenLpf( MeasHandle.pPrv->CvtHiPeak[i], MeasHandle.Raw.CvtHiPeak[i] );
		MeasHandle.pCur->CvtLoPeak[i] = IrSenLpf( MeasHandle.pPrv->CvtLoPeak[i], MeasHandle.Raw.CvtLoPeak[i] );
		MeasHandle.pCur->CvtOcAvrg[i] = IrSenLpf( MeasHandle.pPrv->CvtOcAvrg[i], MeasHandle.Raw.CvtOcAvrg[i] );
		MeasHandle.pCur->CvtHiTime[i] = MeasHandle.Raw.CvtHiTime[i];
		MeasHandle.pCur->CvtLoTime[i] = MeasHandle.Raw.CvtLoTime[i];
		MeasHandle.pCur->CvtRawDvMax[i] = MeasHandle.Raw.CvtRawDvMax[i];
		if( MeasHandle.Gas[iCvt].IsPresent == true ){
			MeasHandle.pCur->nDet[iCvt] = IrSenLpf( MeasHandle.pPrv->nDet[iCvt], MeasHandle.Raw.nDet[iCvt] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Base_Zero_Handler( void )
{
	float				Temp;
	float				TempDv;
	U16					wBuf[8];
	bool				IsDrifted;
	float				nDet;
	float				nPer;
	float				bPer;
	float				bSpanScale;
	U16					bTmr;
	U8					i;
	U8					iRef;
	Measuring_GasInfo	*pInf;
	
	if( MeasHandle.bZero.NotStable == true ){
		if( MeasHandle.bZero.StableTmr ){
			MeasHandle.bZero.StableTmr--;
			if( MeasHandle.bZero.StableTmr == 0 ){
				MeasHandle.bZero.NotStable = false;
			}
		}
	}
	
	// Warm up timer
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
			if( MeasHandle.bZero.WarmUpEnd[i] == false ){
				if( MeasHandle.bZero.WarmUpTmr[i] ){
					MeasHandle.bZero.WarmUpTmr[i]--;
					if( MeasHandle.bZero.WarmUpTmr[i] == 0 ){
						MeasHandle.bZero.WarmUpEnd[i] = true;
					}
				}
			}
		}
	}
	
	if( (FuncHandle.State.Bit.Calibration == false) &&
		(MeasHandle.bZero.NotStable == false) &&
		(MeasHandle.Filter.Res.Bit.InterfGas_1 == false) &&
		(MeasHandle.Filter.Res.Bit.InterfGas_2 == false) )
	{
		for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
			
			if( MeasHandle.Gas[i].IsPresent == true ){
				
				pInf = &MeasHandle.Gas[i].Inf;
				
				if( pInf->bZeroUse == true ){
					
					if( i < CARTRIDGE_SEN_CH_MAX ){
						Temp = MeasHandle.pCur->CtgSenTemp;
					}
					else {
						Temp = MeasHandle.pCur->CvtSenTemp;
					}
					
					nPer = MeasHandle.pPrv->nPer[i];
					if( MeasHandle.bZero.WarmUpEnd[i] == true ){
						bPer = pInf->bZeroPer * 0.1f;
						bTmr = SecToMeasuringTime( pInf->bZeroSec );
					}
					else {
						bPer = pInf->bZeroPer * 0.2f;
						bTmr = SecToMeasuringTime( 10 );
					}
					TempDv = fabs( Temp - pInf->bZeroTemp.f );
					
					if( (TempDv >= 0.5f) && (nPer >= -bPer) && (nPer <= bPer) ){ IsDrifted = true; }
					else if( (nPer <= -0.5f) && (nPer >= -bPer) ){ IsDrifted = true; }
					else if( (nPer >=  0.5f) && (nPer <=  bPer) ){ IsDrifted = true; }
					else { IsDrifted = false; }
					
					if( IsDrifted == true ){
						if( pInf->bZeroTmr ){
							pInf->bZeroTmr--;
							if( pInf->bZeroTmr == 0 ){
								pInf->bZeroTmr = bTmr;
								if( MeasHandle.bZero.WarmUpEnd[i] == false ){
									MeasHandle.bZero.WarmUpEnd[i] = true;
									MeasHandle.bZero.WarmUpTmr[i] = 0;
								}
								nDet = (pInf->bZero.f - MeasHandle.pCur->nDet[i]) * 0.3f + MeasHandle.pCur->nDet[i];
								bSpanScale = pInf->mSpan.f / pInf->mZero.f;
								pInf->bZero.f = nDet;
								pInf->bSpan.f = nDet * bSpanScale;
								pInf->bZeroTemp.f = Temp;
								pInf->bSpanTemp.f = Temp;
								wBuf[0] = pInf->bZero.w.l;
								wBuf[1] = pInf->bZero.w.h;
								wBuf[2] = pInf->bZeroTemp.w.l;
								wBuf[3] = pInf->bZeroTemp.w.h;
								wBuf[4] = pInf->bSpan.w.l;
								wBuf[5] = pInf->bSpan.w.h;
								wBuf[6] = pInf->bSpanTemp.w.l;
								wBuf[7] = pInf->bSpanTemp.w.h;
								SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 8 );
								if( pInf->bZeroCnt < 65535 ){
									pInf->bZeroCnt++;
									SenEEP_Write_CHn( &pInf->bZeroCnt, i, SENSOR_EEP_CHn_bZERO_COUNT, 1 );
								}
								if( pInf->Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF ){
									iRef = MeasHandle.Conf.iRefCvt;
									pInf = &MeasHandle.Gas[iRef].Inf;
									nDet = (pInf->bZero.f - MeasHandle.pCur->nDet[iRef]) * 0.3f + MeasHandle.pCur->nDet[iRef];
									bSpanScale = pInf->mSpan.f / pInf->mZero.f;
									pInf->bZero.f = nDet;
									pInf->bSpan.f = nDet * bSpanScale;
									pInf->bZeroTemp.f = Temp;
									pInf->bSpanTemp.f = Temp;
									wBuf[0] = pInf->bZero.w.l;
									wBuf[1] = pInf->bZero.w.h;
									wBuf[2] = pInf->bZeroTemp.w.l;
									wBuf[3] = pInf->bZeroTemp.w.h;
									wBuf[4] = pInf->bSpan.w.l;
									wBuf[5] = pInf->bSpan.w.h;
									wBuf[6] = pInf->bSpanTemp.w.l;
									wBuf[7] = pInf->bSpanTemp.w.h;
									SenEEP_Write_CHn( wBuf, iRef, SENSOR_EEP_CHn_bZERO_L, 8 );
								}
								dprintf( "BZ%u,", i );
							}
						}
					}
					else {
						pInf->bZeroTmr = bTmr;
					}
				}
			}
		}
	}
	else {
		for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
			if( MeasHandle.Gas[i].IsPresent == true ){
				
				pInf = &MeasHandle.Gas[i].Inf;
				
				if( MeasHandle.bZero.WarmUpEnd[i] == true ){
					pInf->bZeroTmr = SecToMeasuringTime( pInf->bZeroSec );
				}
				else {
					pInf->bZeroTmr = SecToMeasuringTime( 10 );
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float CT_Gas_Calculation( Measuring_GasInfo *pInf, float d )
{
	float	SpanPer = (float)pInf->SpanGas / (float)pInf->HighScale * 100.f * (float)pInf->CrossScale / 100.f;
	float	CurTemp = MeasHandle.pCur->CtgSenTemp;
	float	Slope;
	float	Result;
	
	float   K0;
	float   K1 = -0.287f;
	float   K2 =  0.0018f;
	float   K3 =  0.000006f;
	float	TCx;
	float   L_Comp;
	float   T_Comp;
	
	float   SkipPer;
	float   LoSkipPer;
	float   HiSkipPer;
	float   LoSlope;
	float   HiSlope;
	
	switch( pInf->SenNo){
		case CT_SEN_UNDEFINED:
			Result = 0.f;
			break;
			
		case CT_SEN_O2_MP_100: // Membrapor O2 application note 2, 7
			K0  = (100.f - (K1 * pInf->bSpanTemp.f + K2 * pow( pInf->bSpanTemp.f, 2 ) + K3 * pow( pInf->bSpanTemp.f, 3 )));
			TCx = (K0 + K1 * CurTemp + K2 * pow( CurTemp, 2 ) + K3 * pow( CurTemp, 3 ));
			L_Comp = ( pInf->bZero.f - pInf->bSpan.f ) / log( 1 / ( 1.f - (float)pInf->SpanGas / 1000.f ) );
			T_Comp = ((pInf->bZero.f - d) * TCx) / 100.f;
			Result = ( 1 - exp( ( -1 * T_Comp ) / L_Comp ) ) * 1000.f;
			Result = Result / (float)pInf->HighScale * 100.f;
			// Below is the lightning slope formula 
			SkipPer = 2.f;
			LoSkipPer = SpanPer - SkipPer;
			HiSkipPer = SpanPer + SkipPer;
			LoSlope = SpanPer / LoSkipPer;
			HiSlope = (100.f - SpanPer) / (100.f - HiSkipPer);
			if( Result < LoSkipPer ) {
			      Result *= LoSlope;
			}
			else if( Result > HiSkipPer ){
			      Result = SpanPer + (Result - HiSkipPer) * HiSlope;
			}
			else {
			      Result = SpanPer;
			}
			break;
			
		default: 
			Slope = (pInf->bSpan.f - pInf->bZero.f) / SpanPer;
			Result = (d - pInf->bZero.f) / Slope;
			break;
	}
	
	if		( Result < -MEASURING_GAS_PERCENTAGE_MAX ){ Result = -MEASURING_GAS_PERCENTAGE_MAX; }
	else if	( Result >  MEASURING_GAS_PERCENTAGE_MAX ){ Result =  MEASURING_GAS_PERCENTAGE_MAX; }
	
	return Result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void IR_Calculation_Paramter_Load( Measuring_GasInfo *pInf )
{
	pInf->a = 0.0100f;
	pInf->n = 1.0000f;
	pInf->Alpha = 0.f;
	pInf->Beta  = 0.f;
	
	switch( pInf->SenNo ){
		case IR_SEN_03_330:
			switch( pInf->GasNo ){
				case IR_GAS_03_330_INT:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X027_C5F8_2000_PPM	: pInf->n = 1.1000f; break;
						case MODEL_CVT_QUAD_X027_SFA_1_100_LEL	: pInf->n = 0.8000f; break;
					}
					break;
					
				case IR_GAS_03_330_LNG_100_LEL:
				case IR_GAS_03_330_CH4_100_LEL:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X025_LNG_100_LEL	:
						case MODEL_CVT_QUAD_X025_CH4_100_LEL	: pInf->a = 0.0366f, pInf->n = 0.5332f; break;
						case MODEL_CVT_DUAL_CH					: pInf->a = 0.0469f, pInf->n = 0.5666f; break;
					}
					break;
					
				case IR_GAS_03_330_C4H10_100_LEL				: pInf->a = 0.0597f, pInf->n = 0.8456f; break;
				case IR_GAS_03_330_SFA_1_100_LEL				: pInf->a = 0.0247f, pInf->n = 0.9821f; break;
				case IR_GAS_03_330_4MS_100_LEL					: pInf->a = 0.0150f, pInf->n = 0.9184f; break;
				case IR_GAS_03_330_TOLUENE_100_LEL				: pInf->a = 0.0067f, pInf->n = 1.0940f; break;
				case IR_GAS_03_330_CH4_100_VOL					: pInf->a = 0.0010f, pInf->n = 0.4284f; break;
			}
			break;
			
		case IR_SEN_03_370:
			switch( pInf->GasNo ){
				case IR_GAS_03_370_GMP_02_3000_PPM				: pInf->a = 0.0080f, pInf->n = 0.9546f; break;
				case IR_GAS_03_370_OMCTS_100_LEL				: pInf->a = 0.0158f, pInf->n = 0.8353f; break;
				case IR_GAS_03_370_BTBAS_1000_PPM				: pInf->a = 0.0048f, pInf->n = 1.0473f; break;
			}
			break;
			
		case IR_SEN_03_420:
			switch( pInf->GasNo ){
				case IR_GAS_03_420_ECH_100_LEL					: pInf->a = 0.0684f, pInf->n = 0.7861f; break;
			}
			break;
			
		case IR_SEN_04_260:
			switch( pInf->GasNo ){
				case IR_GAS_04_260_CO2_5000_PPM					: pInf->a = 0.0513f, pInf->n = 0.7321f, pInf->Beta = 0.2439f; break;
				case IR_GAS_04_260_CO2_9999_PPM					:
				case IR_GAS_04_260_CO2_1_00_VOL					: pInf->a = 0.0796f, pInf->n = 0.7160f, pInf->Beta = 0.3482f; break;
				case IR_GAS_04_260_CO2_5_0_VOL					: pInf->a = 0.2868f, pInf->n = 0.5641f, pInf->Beta = 0.4761f; break;
			}
			break;
			
		case IR_SEN_04_520:
			switch( pInf->GasNo ){
				case IR_GAS_04_520_N2O_4000_PPM					: pInf->a = 0.0255f, pInf->n = 0.8977f; break;
				case IR_GAS_04_520_N2O_1000_PPM					: pInf->a = 0.0084f, pInf->n = 0.9244f; break;
			}
			break;
			
		case IR_SEN_04_660:
			switch( pInf->GasNo ){
				case IR_GAS_04_660_CO_9999_PPM					: pInf->a = 0.0226f, pInf->n = 0.7813f; break;
				case IR_GAS_04_660_CO_1_00_VOL					: pInf->a = 0.0226f, pInf->n = 0.7813f; break;
				case IR_GAS_04_660_CO_2_00_VOL					: pInf->a = 0.0274f, pInf->n = 0.6696f; break;
				case IR_GAS_04_660_CO_5_00_VOL					: pInf->a = 0.0330f, pInf->n = 0.6117f; break;
			}
			break;
			
		case IR_SEN_04_850:
			switch( pInf->GasNo ){
				case IR_GAS_04_850_COS_200_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X025_COS_200_PPM	: pInf->a = 0.0042f, pInf->n = 0.9974f; break;
						case MODEL_CVT_DUAL_X044				: pInf->a = 0.0040f, pInf->n = 1.0438f; break;
					}
					break;
					
				case IR_GAS_04_850_COS_75_0_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X025_COS_75_0_PPM	: pInf->a = 0.0047f, pInf->n = 1.0767f; break;
					}
					break;
			}
			break;
			
		case IR_SEN_08_000:
			switch( pInf->GasNo ){
				case IR_GAS_08_000_INT:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X025_LNG_100_LEL	:
						case MODEL_CVT_QUAD_X025_CH4_100_LEL	: pInf->n = 0.7500f; break;
						case MODEL_CVT_QUAD_X027_CH3F_2000_PPM	: pInf->n = 1.1000f; break;
						case MODEL_CVT_QUAD_X027_C4F8_2000_PPM	:
						case MODEL_CVT_QUAD_X027_SFA_1_100_LEL	:
						case MODEL_CVT_QUAD_X027_4MS_100_LEL	: pInf->n = 0.8000f; break;
						case MODEL_CVT_QUAD_X027_IPA_3000_PPM	: pInf->n = 0.9000f; break;
						case MODEL_CVT_QUAD_X027_C4F6_300_PPM	: pInf->n = 1.0800f; break;
					}
					break;
					
				case IR_GAS_08_000_C4F8_2000_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X027_C4F8_2000_PPM	: pInf->a = 0.0192f, pInf->n = 0.9410f; break;
						case MODEL_CVT_DUAL_X054				: pInf->a = 0.0143f, pInf->n = 1.0333f, pInf->Beta = -0.25f; break;
					}
					break;
					
				case IR_GAS_08_000_FL300_200_PPM				: pInf->a = 0.0092f, pInf->n = 1.0264f; break;
				case IR_GAS_08_000_CF4_500_PPM					: pInf->a = 0.0182f, pInf->n = 0.9230f; break;
			}
			break;
			
		case IR_SEN_08_925:
			switch( pInf->GasNo ){
				case IR_GAS_08_925_INT:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X027_C4F8_2000_PPM	:
						case MODEL_CVT_QUAD_X027_SFA_1_100_LEL	: pInf->n = 0.8000f; break;
					}
					break;
					
				case IR_GAS_08_925_C5F8_2000_PPM				: pInf->a = 0.0052f, pInf->n = 0.9945f; break;
				
				case IR_GAS_08_925_C4F6_2000_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X027_C4F6_2000_PPM	: pInf->a = 0.0140f, pInf->n = 0.9312f; break;
						case MODEL_CVT_DUAL_X021				: pInf->a = 0.0150f, pInf->n = 0.9475f; break;
					}
					break;
					
				case IR_GAS_08_925_CH2F2_2000_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X027_CH2F2_2000_PPM	: pInf->a = 0.0080f, pInf->n = 1.0125f; break;
						case MODEL_CVT_DUAL_X021				: pInf->a = 0.0051f, pInf->n = 0.9765f; break;
					}
					break;
					
				case IR_GAS_08_925_TEOS_100_PPM					:
				case IR_GAS_08_925_TEOS_HT200_100_PPM			: pInf->a = 0.0031f, pInf->n = 1.0370f; break;
				case IR_GAS_08_925_HFC_2000_PPM					: pInf->a = 0.0051f, pInf->n = 0.9765f; break;
				case IR_GAS_08_925_IPA_3000_PPM					: pInf->a = 0.0018f, pInf->n = 0.9995f; break;
				case IR_GAS_08_925_IPA_100_LEL					: pInf->a = 0.0151f, pInf->n = 1.0258f; break;
				case IR_GAS_08_925_SELITANE_100_LEL				: pInf->a = 0.0160f, pInf->n = 0.9933f; break;
				case IR_GAS_08_925_C4F6_300_PPM					: pInf->a = 0.0081f, pInf->n = 0.9820f; break;
			}
			break;
			
		case IR_SEN_09_076:
			switch( pInf->GasNo ){
				case IR_GAS_09_076_C4F6S_2000_PPM				: pInf->a = 0.0216f, pInf->n = 0.9973f; break;
			}
			break;
			
		case IR_SEN_09_150:
			switch( pInf->GasNo ){
				case IR_GAS_09_150_C4H2F6_1000_PPM				: pInf->a = 0.0101f, pInf->n = 0.9871f; break;
			}
			break;
			
		case IR_SEN_09_230:
			switch( pInf->GasNo ){
				case IR_GAS_09_230_OMCTS_75_0_PPM				: pInf->a = 0.0063f, pInf->n = 1.0782f; break;
			}
			break;
			
		case IR_SEN_09_440:
			switch( pInf->GasNo ){
				case IR_GAS_09_440_CH3F_2000_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X027_CH3F_2000_PPM	: pInf->a = 0.0063f, pInf->n = 1.0083f; break;
						case MODEL_CVT_DUAL_X018				: pInf->a = 0.0064f, pInf->n = 1.0232f; break;
					}
					break;
					
				case IR_GAS_09_440_ETHANOL_100_LEL				: pInf->a = 0.0880f, pInf->n = 0.7495f; break;
				case IR_GAS_09_440_CTFE_1000_PPM				: pInf->a = 0.0041f, pInf->n = 1.0489f; break;
				case IR_GAS_09_440_C3F6_2000_PPM				: pInf->a = 0.0521f, pInf->n = 0.6321f; break;
			}
			break;
			
		case IR_SEN_09_790:
			switch( pInf->GasNo ){
				case IR_GAS_09_790_C3F6_2000_PPM				: pInf->a = 0.0213f, pInf->n = 0.7800f; break;
			}
			break;
			
		case IR_SEN_10_100:
			switch( pInf->GasNo ){
				case IR_GAS_10_100_METHANOL_3000_PPM			: pInf->a = 0.0035f, pInf->n = 1.0060f; break;
			}
			break;
			
		case IR_SEN_10_990:
			switch( pInf->GasNo ){
				case IR_GAS_10_990_NF3_200_PPM:
					switch( MeasHandle.Conf.CvtModel ){
						case MODEL_CVT_QUAD_X042_NF3_200_PPM	: pInf->a = 0.0035f, pInf->n = 1.0225f; break;
						case MODEL_CVT_DUAL_X127				: pInf->a = 0.0027f, pInf->n = 1.0730f, pInf->Alpha = 0.0036f, pInf->Beta = -0.720f; break;
					}
					break;
					
				case IR_GAS_10_990_HMDS_1000_PPM				: pInf->a = 0.0049f, pInf->n = 0.9462f; break;
				case IR_GAS_10_990_HMDS_100_LEL					: pInf->a = 0.0258f, pInf->n = 0.8430f; break;
				case IR_GAS_10_990_C3F6O_5000_PPM				: pInf->a = 0.0024f, pInf->n = 1.0558f; break;
				case IR_GAS_10_990_C3H6_100_LEL					: pInf->a = 0.0244f, pInf->n = 0.8545f; break;
				case IR_GAS_10_990_C2H4_100_LEL					: pInf->a = 0.0185f, pInf->n = 0.8950f; break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float IR_Gas_Calculation( Measuring_GasInfo *pInf, float d, float r )
{
	float	C = (float)pInf->SpanGas / (float)pInf->HighScale * 100.f * (float)pInf->CrossScale / 100.f;
	float	Zero;
	float	Span;
	float	Span_Comp;
	float	Normalised_Transmittance;
	float	Normalised_Transmittance_Comp;
	float	Normalised_Absorbance_Comp;
	bool	IsNegative = false;
	float	LogParameter;
	float	Result;
	float	bzTemp = pInf->bZeroTemp.f + 273.15;
	float	mzTemp = pInf->mZeroTemp.f + 273.15;
	float	crTemp = MeasHandle.pCur->CvtSenTemp + 273.15;
	
	IR_Calculation_Paramter_Load( pInf );
	
	Zero = pInf->bZero.f / pInf->rZero;
	Normalised_Transmittance = d / (Zero * r);
	Span = (1.f - pInf->bSpan.f / (Zero * pInf->rSpan)) / (1.f - exp( -pInf->a * pow( C, pInf->n )));
	Normalised_Transmittance_Comp = Normalised_Transmittance * (1 + pInf->Alpha * (crTemp - bzTemp));
	Normalised_Absorbance_Comp = 1 - Normalised_Transmittance_Comp;
	Span_Comp = Span + pInf->Beta * (crTemp - mzTemp) / mzTemp;
	
	if( Normalised_Absorbance_Comp  <  0.f ){
		Normalised_Absorbance_Comp *= -1.f;
		IsNegative = true;
	}
	
	LogParameter = 1.f - Normalised_Absorbance_Comp / Span_Comp;
	
	if( IsNegative == true ){
		if( LogParameter < 0.f ){
			return -MEASURING_GAS_PERCENTAGE_MAX;
		}
		else {
			Result = pow( -log( LogParameter ) / pInf->a, 1.f / pInf->n ) * -1.f;
		}
	}
	else {
		if( LogParameter < 0.f ){
			return  MEASURING_GAS_PERCENTAGE_MAX;
		}
		else {
			Result = pow( -log( LogParameter ) / pInf->a, 1.f / pInf->n );
		}
	}
	
	if		( Result < -MEASURING_GAS_PERCENTAGE_MAX ){ Result = -MEASURING_GAS_PERCENTAGE_MAX; }
	else if	( Result >  MEASURING_GAS_PERCENTAGE_MAX ){ Result =  MEASURING_GAS_PERCENTAGE_MAX; }
	
	return Result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Gas_Calculation( void )
{
	U8							i;
	U8							iRef;
	static Measuring_GasInfo	Inf;
	float						res;
	float						det;
	float						zts;
	float						ref;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
		
			det = MeasHandle.pCur->nDet[i];
			ref = 1.f;
			MeasHandle.Gas[i].Inf.rZero = 1.f;
			MeasHandle.Gas[i].Inf.rSpan = 1.f;
			
			//MeasHandle.pCur->nRat[i] = Measuring_Gas_Response_Calculation( i, det );
			
			if( i < CARTRIDGE_SEN_CH_MAX ){
				zts = det - MeasHandle.Gas[i].Inf.bZero.f;
				if( MeasHandle.Gas[i].Inf.SenNo < CT_SEN_NAP_100AM ){
					MeasHandle.pCur->nRat[i] = zts / LPTIA_Resistance[MeasHandle.Gas[i].Inf.TIA.Item.RGAIN] * 1000000000.f;
				}
				else {
					MeasHandle.pCur->nRat[i] = zts * 1000.f;
				}
			}
			else {
				MeasHandle.pCur->nRat[i] = det / MeasHandle.Gas[i].Inf.bZero.f * 100.f - 100.f;
			}
			
			switch( MeasHandle.Gas[i].Inf.Func ){
				case MEASURING_GAS_FUNC_CT_OUT:
					res = CT_Gas_Calculation( &MeasHandle.Gas[i].Inf, det );
					if( isnan( res ) == true ){
						MeasHandle.pCur->nPer[i] = MeasHandle.pPrv->nPer[i];
						MeasHandle.pCur->iPer[i] = MeasHandle.pPrv->iPer[i];
					}
					else {
						MeasHandle.pCur->nPer[i] = res;
						MeasHandle.pCur->iPer[i] = res;
					}
					break;
					
				case MEASURING_GAS_FUNC_CT_OUT_WITH_REF:
					iRef = MeasHandle.Conf.iRefCtg;
					res = CT_Gas_Calculation( &MeasHandle.Gas[i].Inf, det );
					ref = CT_Gas_Calculation( &MeasHandle.Gas[iRef].Inf, MeasHandle.pCur->nDet[iRef] );
					if( (isnan( res ) == true) || (isnan( ref ) == true) ){
						MeasHandle.pCur->nPer[i] = MeasHandle.pPrv->nPer[i];
						MeasHandle.pCur->iPer[i] = MeasHandle.pPrv->iPer[i];
						MeasHandle.pCur->nPer[iRef] = MeasHandle.pPrv->nPer[iRef];
						MeasHandle.pCur->iPer[iRef] = MeasHandle.pPrv->iPer[iRef];
					}
					else {
						MeasHandle.pCur->nPer[i] = res;
						MeasHandle.pCur->iPer[i] = res;
						MeasHandle.pCur->nPer[MeasHandle.Conf.iRefCtg] = ref;
						MeasHandle.pCur->iPer[MeasHandle.Conf.iRefCtg] = ref;
					}
					break;
					
				case MEASURING_GAS_FUNC_IR_OUT:
					res = IR_Gas_Calculation( &MeasHandle.Gas[i].Inf, det, ref );
					if( isnan( res ) == true ){
						MeasHandle.pCur->nPer[i] = MeasHandle.pPrv->nPer[i];
						MeasHandle.pCur->iPer[i] = MeasHandle.pPrv->iPer[i];
					}
					else {
						MeasHandle.pCur->nPer[i] = res;
						Inf = MeasHandle.Gas[i].Inf;
						Inf.GasNo = 0;
						Inf.HighScale = 100;
						Inf.CrossScale = 100;
						Inf.SpanGas = 50;
						Inf.bSpan.f = Inf.bZero.f * MeasHandle.Conf.iSpanScale[i];
						res = IR_Gas_Calculation( &Inf, det, ref );
						if( isnan( res ) == true ){
							MeasHandle.pCur->iPer[i] = MeasHandle.pPrv->iPer[i];
						}
						else {
							MeasHandle.pCur->iPer[i] = res;
						}
					}
					break;
					
				case MEASURING_GAS_FUNC_IR_OUT_WITH_REF:
					res = IR_Gas_Calculation( &MeasHandle.Gas[i].Inf, det, ref );
					if( isnan( res ) == true ){
						MeasHandle.pCur->iPer[i] = MeasHandle.pPrv->iPer[i];
						MeasHandle.pCur->nPer[i] = MeasHandle.pPrv->nPer[i];
					}
					else {
						MeasHandle.pCur->iPer[i] = res;
						ref = MeasHandle.pCur->nDet[MeasHandle.Conf.iRefCvt];
						MeasHandle.Gas[i].Inf.rZero = MeasHandle.Gas[MeasHandle.Conf.iRefCvt].Inf.bZero.f;
						MeasHandle.Gas[i].Inf.rSpan = MeasHandle.Gas[MeasHandle.Conf.iRefCvt].Inf.bSpan.f;
						res = IR_Gas_Calculation( &MeasHandle.Gas[i].Inf, det, ref );
						if( isnan( res ) == true ){
							MeasHandle.pCur->nPer[i] = MeasHandle.pPrv->nPer[i];
						}
						else {
							MeasHandle.pCur->nPer[i] = res;
						}
					}
					break;
					
				case MEASURING_GAS_FUNC_IR_INT:
					res = IR_Gas_Calculation( &MeasHandle.Gas[i].Inf, det, ref );
					if( isnan( res ) == true ){
						MeasHandle.pCur->nPer[i] = MeasHandle.pPrv->nPer[i];
						MeasHandle.pCur->iPer[i] = MeasHandle.pPrv->iPer[i];
					}
					else {
						MeasHandle.pCur->nPer[i] = res;
						MeasHandle.pCur->iPer[i] = res;
					}
					break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Deviation_Calculation( void )
{
	U8		i, iLpf;
	U32		DvSum;
	float	AvSum;
	
	for( i=0; i<MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; i++ ){
		MeasHandle.Dev.CvtHiPeak[i] = fabs( MeasHandle.pCur->CvtHiPeak[i] - MeasHandle.pPrv->CvtHiPeak[i] );
		MeasHandle.Dev.CvtLoPeak[i] = fabs( MeasHandle.pCur->CvtLoPeak[i] - MeasHandle.pPrv->CvtLoPeak[i] );
		MeasHandle.Dev.CvtOcAvrg[i] = fabs( MeasHandle.pCur->CvtOcAvrg[i] - MeasHandle.pPrv->CvtOcAvrg[i] );
		MeasHandle.Dev.CvtHiTime[i] = abs( MeasHandle.pCur->CvtHiTime[i] - MeasHandle.pPrv->CvtHiTime[i] );
		MeasHandle.Dev.CvtLoTime[i] = abs( MeasHandle.pCur->CvtLoTime[i] - MeasHandle.pPrv->CvtLoTime[i] );
		MeasHandle.Dev.CvtRawDvMax[i] = abs( MeasHandle.pCur->CvtRawDvMax[i] - MeasHandle.pPrv->CvtRawDvMax[i] );
		for( iLpf=1, DvSum=0, AvSum=0; iLpf<MEASURING_LPF_BUF_SIZE; iLpf++ ){
			DvSum += abs( MeasHandle.Lpf[iLpf].CvtRawDvMax[i] - MeasHandle.Lpf[iLpf-1].CvtRawDvMax[i] );
			if( DvSum > 65535 ){ DvSum = 65535; }
			AvSum += fabs( MeasHandle.Lpf[iLpf].CvtOcAvrg[i] - MeasHandle.Lpf[iLpf-1].CvtOcAvrg[i] );
			
		}
		MeasHandle.Dev.CvtRawDvMaxSum[i] = DvSum;
		AvSum *= 10000.f;
		if( AvSum > 65535.f ){ AvSum = 65535.f; }
		MeasHandle.Dev.CvtOcAvrgSum[i] = (U16)AvSum;
	}
	
	MeasHandle.Dev.CvtSrcPwr	= fabs(	MeasHandle.pCur->CvtSrcPwr	- MeasHandle.pPrv->CvtSrcPwr	);
	
	MeasHandle.Dev.CtgSenTemp	= fabs(	MeasHandle.pCur->CtgSenTemp	- MeasHandle.pPrv->CtgSenTemp	);
	MeasHandle.Dev.CvtSenTemp	= fabs(	MeasHandle.pCur->CvtSenTemp	- MeasHandle.pPrv->CvtSenTemp	);
	MeasHandle.Dev.SysSenTemp	= fabs(	MeasHandle.pCur->SysSenTemp	- MeasHandle.pPrv->SysSenTemp	);
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
			
			MeasHandle.Dev.nDet[i] = fabs( MeasHandle.pCur->nDet[i] - MeasHandle.pPrv->nDet[i] );
			MeasHandle.Dev.nPer[i] = fabs( MeasHandle.pCur->nPer[i] - MeasHandle.pPrv->nPer[i] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Interference_Gas_Filtering( Measuring_FilterItem *pf )
{
	bool	Det = false;
	
	// Interference Gas Detection Check
	switch( MeasHandle.Conf.CvtModel ){
		case MODEL_CVT_QUAD_X025_LNG_100_LEL:
		case MODEL_CVT_QUAD_X025_CH4_100_LEL:
		case MODEL_CVT_QUAD_X027_4MS_100_LEL:
		case MODEL_CVT_QUAD_X033_C4F6S_2000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 3.f) ||
				(MeasHandle.pCur->nPer[3] >= 3.f) ||
				(MeasHandle.pCur->nPer[4] >= 3.f) ||
				(MeasHandle.pCur->nPer[5] >= 3.f) )
			{
				if( (MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[2]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X044_OMCTS_100_LEL:
			if( (MeasHandle.pCur->nPer[2] >= 5.f) ||
				(MeasHandle.pCur->nPer[3] >= 5.f) ||
				(MeasHandle.pCur->nPer[4] >= 5.f) ||
				(MeasHandle.pCur->nPer[5] >= 5.f) )
			{
				if( (MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[2]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X025_COS_200_PPM:
		case MODEL_CVT_QUAD_X027_C4F6_2000_PPM:
		case MODEL_CVT_QUAD_X027_CH2F2_2000_PPM:
		case MODEL_CVT_QUAD_X027_TEOS_100_PPM:
		case MODEL_CVT_QUAD_X027_SFA_1_100_LEL:
		case MODEL_CVT_QUAD_X027_IPA_3000_PPM:
		case MODEL_CVT_QUAD_X042_NF3_200_PPM:
		case MODEL_CVT_QUAD_X025_COS_75_0_PPM:
		case MODEL_CVT_QUAD_X042_C3H6_100_LEL:
		case MODEL_CVT_QUAD_X042_C2H4_100_LEL:
		case MODEL_CVT_QUAD_X027_C4F6_300_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 3.f) ||
				(MeasHandle.pCur->nPer[3] >= 3.f) ||
				(MeasHandle.pCur->nPer[4] >= 3.f) ||
				(MeasHandle.pCur->nPer[5] >= 3.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[5]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X027_TEOS_HT200_100_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 3.f) ||
				(MeasHandle.pCur->nPer[3] >= 3.f) ||
				(MeasHandle.pCur->nPer[4] >= 3.f) ||
				(MeasHandle.pCur->nPer[5] >= 3.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[5]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X027_IPA_100_LEL:
		case MODEL_CVT_QUAD_X027_SELITANE_100_LEL:
		case MODEL_CVT_QUAD_X040_OMCTS_75_0_PPM:
		case MODEL_CVT_QUAD_X044_METHANOL_3000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 5.f) ||
				(MeasHandle.pCur->nPer[3] >= 5.f) ||
				(MeasHandle.pCur->nPer[4] >= 5.f) ||
				(MeasHandle.pCur->nPer[5] >= 5.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[5]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X027_C4F8_2000_PPM:
		case MODEL_CVT_QUAD_X027_FL300_200_PPM:
		case MODEL_CVT_QUAD_X027_CF4_500_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 3.f) ||
				(MeasHandle.pCur->nPer[3] >= 3.f) ||
				(MeasHandle.pCur->nPer[4] >= 3.f) ||
				(MeasHandle.pCur->nPer[5] >= 3.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[4]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[4]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[4]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X027_C5F8_2000_PPM:
		case MODEL_CVT_QUAD_X042_C3F6O_5000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 7.f) ||
				(MeasHandle.pCur->nPer[3] >= 7.f) ||
				(MeasHandle.pCur->nPer[4] >= 7.f) ||
				(MeasHandle.pCur->nPer[5] >= 7.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[5]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X027_CH3F_2000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 7.f) ||
				(MeasHandle.pCur->nPer[3] >= 7.f) ||
				(MeasHandle.pCur->nPer[4] >= 7.f) ||
				(MeasHandle.pCur->nPer[5] >= 7.f) )
			{
				if( (MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[3]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X040_ECH_100_LEL:
			if( (MeasHandle.pCur->nPer[2] >= 3.f) ||
				(MeasHandle.pCur->nPer[3] >= 3.f) ||
				(MeasHandle.pCur->nPer[4] >= 3.f) ||
				(MeasHandle.pCur->nPer[5] >= 3.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[3]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X042_GMP_02_3000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 3.f) ||
				(MeasHandle.pCur->nPer[3] >= 3.f) ||
				(MeasHandle.pCur->nPer[4] >= 3.f) ||
				(MeasHandle.pCur->nPer[5] >= 3.f) )
			{
				if( (MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[5]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X042_C4H2F6_1000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 7.f) ||
				(MeasHandle.pCur->nPer[3] >= 7.f) ||
				(MeasHandle.pCur->nPer[4] >= 7.f) ||
				(MeasHandle.pCur->nPer[5] >= 7.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[3]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X042_HMDS_1000_PPM:
		case MODEL_CVT_QUAD_X042_HMDS_100_LEL:
			if( (MeasHandle.pCur->nPer[2] >= 5.f) ||
				(MeasHandle.pCur->nPer[3] >= 5.f) ||
				(MeasHandle.pCur->nPer[4] >= 5.f) ||
				(MeasHandle.pCur->nPer[5] >= 5.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[5]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[2]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X027_ETHANOL_100_LEL:
			if( (MeasHandle.pCur->nPer[2] >= 5.f) ||
				(MeasHandle.pCur->nPer[3] >= 5.f) ||
				(MeasHandle.pCur->nPer[4] >= 5.f) ||
				(MeasHandle.pCur->nPer[5] >= 5.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[3]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X042_C3F6_2000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 5.f) ||
				(MeasHandle.pCur->nPer[3] >= 5.f) ||
				(MeasHandle.pCur->nPer[4] >= 5.f) ||
				(MeasHandle.pCur->nPer[5] >= 5.f) )
			{
				if( (MeasHandle.pCur->iPer[2] >= MeasHandle.pCur->iPer[4]) ||
					(MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[4]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[4]) )
				{
					Det = true;
				}
			}
			break;
			
		case MODEL_CVT_QUAD_X042_BTBAS_1000_PPM:
			if( (MeasHandle.pCur->nPer[2] >= 8.f) ||
				(MeasHandle.pCur->nPer[3] >= 8.f) ||
				(MeasHandle.pCur->nPer[4] >= 8.f) ||
				(MeasHandle.pCur->nPer[5] >= 8.f) )
			{
				if( (MeasHandle.pCur->iPer[3] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[2]) ||
					(MeasHandle.pCur->iPer[4] >= MeasHandle.pCur->iPer[3]) ||
					(MeasHandle.pCur->iPer[5] >= MeasHandle.pCur->iPer[3]) )
				{
					Det = true;
				}
			}
			break;
	}
	
	if( Det == true ){
		switch( MeasHandle.Conf.SenComb ){
			case MEASURING_SEN_COMB_CVT_QUAD:			pf->Bit.InterfGas_1 = true; break;
			case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
			case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	pf->Bit.InterfGas_2 = true; break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Filter_Com( Measuring_FilterItem *pf )
{
	float	Vmin, Vmax;
	
	// Supply Power
	if( SystemOption.Item.IsBatMode == true ){
		if( MeasHandle.pCur->VSUP < 13.f ){
			pf->Bit.SystemPwr = true;
		}
	}
	else {
		if( SystemOption.Item.IsUniMode == true ){
			if( ((MeasHandle.pCur->P48 > 58.f) || (MeasHandle.pCur->P24 > 33.f)) ||
				((MeasHandle.pCur->P48 < 36.f) && (MeasHandle.pCur->P24 < 15.f)) )
			{
				pf->Bit.SystemPwr = true;
			}
		}
		else {
			if( SystemOption.Item.IsPoeMode == true){
				Vmin = 36.f; Vmax = 58.f;
			}
			else {
				Vmin = 15.f; Vmax = 33.f;
			}
			if((MeasHandle.pCur->VSUP < Vmin) || (MeasHandle.pCur->VSUP > Vmax)){
				pf->Bit.SystemPwr = true;
			}
		}
		if( pf->Bit.SystemPwr == true ){
			if( MeasHandle.Filter.SysPwrErrTmr < SYS_PWR_ERR_DEC_TIME ){
				MeasHandle.Filter.SysPwrErrTmr++;
			}
			else {
				SystemError.Main.Bit.Power = true;
			}
		}
		else {
			MeasHandle.Filter.SysPwrErrTmr = 0;
			SystemError.Main.Bit.Power = false;
		}
	}
	
	// Target Flow Check
	if( MeasHandle.pCur->FlowRate < (FlowCtrl.Cal.Targ_mL * 0.8f) ){
		pf->Bit.FlowRate = true;
		if( MeasHandle.Filter.pFlow->TroubleTmr ){
			MeasHandle.Filter.pFlow->TroubleTmr--;
		}
		else {
			SystemError.Main.Bit.FlowRate_Udr = true;
		}
	}
	else if	( MeasHandle.pCur->FlowRate > (FlowCtrl.Cal.Targ_mL * 1.2f) ){
		pf->Bit.FlowRate = true;
		if( MeasHandle.Filter.pFlow->TroubleTmr ){
			MeasHandle.Filter.pFlow->TroubleTmr--;
		}
		else {
			SystemError.Main.Bit.FlowRate_Ovr = true;
		}
	}
	else {
		MeasHandle.Filter.pFlow->TroubleTmr = SecToMeasuringTime( MeasHandle.Filter.pFlow->TroubleSec );
		SystemError.Main.Bit.FlowRate_Udr = false;
		SystemError.Main.Bit.FlowRate_Ovr = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Filter_Ctg( Measuring_FilterItem *pf )
{
	U8		iPrv = MeasHandle.Filter.Idx-1 & MEASURING_FILTER_BUF_MASK;
	U8		iSen;
	U16		RawDvSumLmt_VZB;
	U16		RawDvSumLmt_TIA;
	U16		PeakDvLmt_VZB;
	U16		PeakDvLmt_TIA;
	float	nPerDvLmt;
	bool	IsDualMode = false;
	
	if( MeasHandle.pSen->pCtg->IsPresent == true ){
		
		// Operating Temperature Check
		if( (MeasHandle.pCur->CtgSenTemp < -20.f) || (MeasHandle.pCur->CtgSenTemp >  85.f) ){
			pf->Bit.CtgTemp = true;
			if( MeasHandle.Filter.CtgTempErrTmr < TEMP_ERR_DEC_TIME ){
				MeasHandle.Filter.CtgTempErrTmr++;
			}
			else {
				SystemError.Main.Bit.CtgTemp = true;
			}
		}
		else {
			MeasHandle.Filter.CtgTempErrTmr = 0;
			SystemError.Main.Bit.CtgTemp = false;
		}
		
		if( MeasHandle.pSen->pCtg->State == 0 ){
			pf->Bit.CtgWarmUp = true;
		}
			
		// DAC output voltage error
		if( MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] == 2 ){ IsDualMode = true; }
		if( (fabs(MeasHandle.pCur->CtgVZERO[0] - ((float)MeasHandle.Gas[0].Inf.TargetOffs * 0.001f)) > 0.03f) ||
			(fabs(MeasHandle.pCur->CtgVBIAS[0] - ((float)MeasHandle.Gas[0].Inf.TargetBias * 0.001f)) > 0.03f) ||
			(IsDualMode && (fabs(MeasHandle.pCur->CtgVZERO[1] - ((float)MeasHandle.Gas[1].Inf.TargetOffs * 0.001f)) > 0.03f)) ||
			(IsDualMode && (fabs(MeasHandle.pCur->CtgVBIAS[1] - ((float)MeasHandle.Gas[1].Inf.TargetBias * 0.001f)) > 0.03f)) )
		{
			pf->Bit.CtgDacError = true;
			if( MeasHandle.Filter.CtgDacErrTmr < DAC_ERR_DEC_TIME ){
				MeasHandle.Filter.CtgDacErrTmr++;
			}
			else {
				MeasHandle.Filter.CtgDacErrTmr = 0;
				if( MeasHandle.Filter.CtgDacErrCnt < DAC_ERR_DEC_COUNTS ){
					MeasHandle.Filter.CtgDacErrCnt++;
					
					Ctg_Dac_Reset();
				}
				else {
					SystemError.Main.Bit.CtgDac = true;
				}
			}
		}
		else {
			MeasHandle.Filter.CtgDacErrTmr = 0;
		}
		
		// Dual Cartridge Gas Deviation Check
		if( MeasHandle.Conf.IsAndMode == true ){
			
			if( fabs( MeasHandle.pCur->nPer[0] - MeasHandle.pCur->nPer[1] ) > 6.0f ){
				if( MeasHandle.Filter.CtgGasDevTmr.Item.Cnt < GAS_DEV_DEC_TIME ){
					MeasHandle.Filter.CtgGasDevTmr.Item.Cnt++;
				}
				else {
					if( SystemError.Main.Bit.CT_Gas_Deviation != true ){
						SystemError.Main.Bit.CT_Gas_Deviation  = true;
						if( MeasHandle.pCur->nPer[0] > MeasHandle.pCur->nPer[1] ){
							MeasHandle.Filter.CtgGasDevTmr.Item.IsCh1Higher = true;
						}
					}
				}
			}
			else {
				MeasHandle.Filter.CtgGasDevTmr.Item.Cnt = 0;
			}
		}
		
		for( iSen=0; iSen<MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS]; iSen++ ){
			
			// ADC sensing range of ADuCM355 is exceeded
			if( (MeasHandle.pCur->nDet[iSen] < 0.15f) || (MeasHandle.pCur->nDet[iSen] > 2.05f) ){
				pf->Bit.CtgExcSignal = true;
				if( MeasHandle.Filter.ExcSignalTmr[iSen] < EXC_SIGNAL_DEC_TIME ){
					MeasHandle.Filter.ExcSignalTmr[iSen]++;
				}
				else {
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.CT_ExcSignal_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.CT_ExcSignal_CH2 = true;
					}
				}
			}
			else {
				SystemError.Main.Bit.CT_ExcSignal_CH1 = false;
				SystemError.Main.Bit.CT_ExcSignal_CH2 = false;
				MeasHandle.Filter.ExcSignalTmr[iSen] = 0;
			}
			
			// CB & SC ensor current range exceeded
			if( (MeasHandle.Gas[iSen].Inf.SenNo >= CT_SEN_NAP_100AM) &&
			    ((MeasHandle.pCur->CtgICB[iSen] < 0.1f) || (MeasHandle.pCur->CtgICB[iSen] > 0.4f)) )
			{
				pf->Bit.CtgExcCurrent = true;
				if( MeasHandle.Filter.CtgExcCurrentTmr[iSen] < EXC_CURRENT_DEC_TIME ){
					MeasHandle.Filter.CtgExcCurrentTmr[iSen]++;
				}
				else {
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.CT_ExcCurrent_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.CT_ExcCurrent_CH2 = true;
					}
				}
			}
			else {
				MeasHandle.Filter.CtgExcCurrentTmr[iSen] = 0;
			}
			
			// Raw Data Check
			switch( MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_MODEL] ){
				case CARTRIDGE_MODEL_PROTO:
				case CARTRIDGE_MODEL_5100FN:
					RawDvSumLmt_VZB	= 300;
					PeakDvLmt_VZB = 30;
					switch( MeasHandle.Gas[iSen].Inf.SenNo ){
						case CT_SEN_CO_CF_200		:
						case CT_SEN_EC_DC_HF_9		: RawDvSumLmt_TIA	= 4000; PeakDvLmt_TIA = 2000; break;
						case CT_SEN_EC_DC_O3_1		:
						case CT_SEN_EC_DC_HCL_15	: RawDvSumLmt_TIA	= 4000; PeakDvLmt_TIA = 3000; break;
						case CT_SEN_NO_CF_100		:
						case CT_SEN_SIH4_CD_50		: RawDvSumLmt_TIA	= 5000; PeakDvLmt_TIA = 3000; break;
						case CT_SEN_C2H4_C_1500		: RawDvSumLmt_TIA	= 5000; PeakDvLmt_TIA = 2000; break;
						case CT_SEN_NH3_CR_200		: RawDvSumLmt_TIA	= 4000; PeakDvLmt_TIA = 4000; break;
						case CT_SEN_O2_MP_100		: RawDvSumLmt_TIA	=  500; PeakDvLmt_TIA =  500; break;
						case CT_SEN_NAP_100AM		: RawDvSumLmt_TIA	= 1000; PeakDvLmt_TIA = 1000; break;
						default						: RawDvSumLmt_TIA	= 5000; PeakDvLmt_TIA = 4000; break;
					}
					break;
					
				case CARTRIDGE_MODEL_5000N:
					RawDvSumLmt_VZB	= 300;
					PeakDvLmt_VZB = 30;
					switch( MeasHandle.Gas[iSen].Inf.SenNo ){
						case CT_SEN_CO_CF_200		: RawDvSumLmt_TIA	= 1500; PeakDvLmt_TIA = 1500; break;
						case CT_SEN_C2H4_C_1500		:
						case CT_SEN_NAP_100AM		:
						case CT_SEN_EC_DC_O3_1		:
						case CT_SEN_SIH4_CD_50		: RawDvSumLmt_TIA	= 1000; PeakDvLmt_TIA = 1000; break;
						case CT_SEN_EC_DC_HF_9		:
						case CT_SEN_NH3_CR_200		:
						case CT_SEN_EC_DC_HCL_15	: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
						case CT_SEN_NO_CF_100		: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 1000; break;
						case CT_SEN_O2_MP_100		: RawDvSumLmt_TIA	=  500; PeakDvLmt_TIA =  500; break;
						default						: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
					}
					break;
					
				case CARTRIDGE_MODEL_5200N:
				case CARTRIDGE_MODEL_5200N_DUAL:
					RawDvSumLmt_VZB	= 300;
					PeakDvLmt_VZB = 30;
					switch( MeasHandle.Gas[iSen].Inf.SenNo ){
						case CT_SEN_CO_CF_200		: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
						default						: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
					}
					break;
					
				case CARTRIDGE_MODEL_5200FN:
				case CARTRIDGE_MODEL_5200FN_DUAL:
					RawDvSumLmt_VZB	= 400;
					PeakDvLmt_VZB = 50;
					switch( MeasHandle.Gas[iSen].Inf.SenNo ){
						case CT_SEN_CO_CF_200		: RawDvSumLmt_TIA	= 3000; PeakDvLmt_TIA = 2000; break;
						default						: RawDvSumLmt_TIA	= 3000; PeakDvLmt_TIA = 2000; break;
					}
					break;
			}
			
			if( MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_BUF_SIZE] == 200 ){
				RawDvSumLmt_VZB = (U16)((float)RawDvSumLmt_VZB * 1.111f);
				RawDvSumLmt_TIA = (U16)((float)RawDvSumLmt_TIA * 1.111f);
				PeakDvLmt_VZB   = (U16)((float)PeakDvLmt_VZB   * 1.111f);
				PeakDvLmt_TIA   = (U16)((float)PeakDvLmt_TIA   * 1.111f);
			}
			switch( MeasHandle.pSen->pCtg->Inf[CARTRIDGE_INF_MODEL] ){
				case CARTRIDGE_MODEL_PROTO:
				case CARTRIDGE_MODEL_5000N:
				case CARTRIDGE_MODEL_5100FN:
					if( (MeasHandle.pCur->CtgRawDvSum.VZERO[iSen] > RawDvSumLmt_VZB) ||
						(MeasHandle.pCur->CtgRawDvSum.VBIAS[iSen] > RawDvSumLmt_VZB) ||
						(MeasHandle.pCur->CtgPeakDv.VZERO[iSen] > PeakDvLmt_VZB) ||
						(MeasHandle.pCur->CtgPeakDv.VBIAS[iSen] > PeakDvLmt_VZB) )
					{
						pf->Bit.CtgRandomNoise = true;
					}
					break;
					
				case CARTRIDGE_MODEL_5200N:
				case CARTRIDGE_MODEL_5200FN:
				case CARTRIDGE_MODEL_5200N_DUAL:
				case CARTRIDGE_MODEL_5200FN_DUAL:
					if( (MeasHandle.pCur->CtgRawDvSum.LPTIA_LPF[iSen] > RawDvSumLmt_TIA) ||
						(MeasHandle.pCur->CtgRawDvSum.VZERO[iSen] > RawDvSumLmt_VZB) ||
						(MeasHandle.pCur->CtgRawDvSum.VBIAS[iSen] > RawDvSumLmt_VZB) ||
						(MeasHandle.pCur->CtgPeakDv.LPTIA_LPF[iSen] > PeakDvLmt_TIA) ||
						(MeasHandle.pCur->CtgPeakDv.VZERO[iSen] > PeakDvLmt_VZB) ||
						(MeasHandle.pCur->CtgPeakDv.VBIAS[iSen] > PeakDvLmt_VZB) )
					{
						pf->Bit.CtgRandomNoise = true;
					}
					break;
			}
			
			if( (pf->Bit.CtgRandomNoise == true) ||
				(MeasHandle.pCur->CtgRawDvSum.LPTIA_LPF[iSen] > RawDvSumLmt_TIA) ||
				(MeasHandle.pCur->CtgPeakDv.LPTIA_LPF[iSen] > PeakDvLmt_TIA) )
			{
				if( MeasHandle.Filter.NotStableTmr[iSen] < 0xFFFF ){
					MeasHandle.Filter.NotStableTmr[iSen]++;
				}
				if( MeasHandle.Filter.NotStableTmr[iSen] == SecToMeasuringTime(MeasHandle.Filter.NotStableDecTime / 10) ){
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.CT_NotStable_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.CT_NotStable_CH2 = true;
					}
				}
				if( MeasHandle.Filter.NotStableTmrMax[iSen] < MeasHandle.Filter.NotStableTmr[iSen] ){
					MeasHandle.Filter.NotStableTmrMax[iSen] = MeasHandle.Filter.NotStableTmr[iSen];
				}
			}
			else {
				if( MeasHandle.Filter.NotStableTmr[iSen] ){
					MeasHandle.Filter.NotStableTmr[iSen]--;
				}
			}
			if( MeasHandle.Filter.NotStableTmr[iSen] > NOT_STABLE_CHK_TIME ){ pf->Bit.CtgNotStable = true; }
			
			// Deviation Check
			switch( MeasHandle.Gas[iSen].Inf.SenNo ){
				case CT_SEN_EC_DC_HCL_15:	nPerDvLmt = 15.f; break;
				case CT_SEN_EC_DC_HF_9:		nPerDvLmt = 25.f; break;
				case CT_SEN_EC_DC_O3_1:		nPerDvLmt = 30.f; break;
				case CT_SEN_NH3_CR_200:		nPerDvLmt = 25.f; break;
				case CT_SEN_NO_CF_100:		nPerDvLmt = 20.f; break;
				case CT_SEN_SIH4_CD_50:		nPerDvLmt = 20.f; break;
				case CT_SEN_C2H4_C_1500:	nPerDvLmt = 15.f; break;
				case CT_SEN_CO_CF_200:		nPerDvLmt = 25.f; break;
				case CT_SEN_O2_MP_100:		nPerDvLmt =  5.f; break;
				case CT_SEN_NAP_100AM:		nPerDvLmt = 10.f; break;
				default:					nPerDvLmt = 30.f; break;
			}
			if( MeasHandle.Dev.nPer[iSen] > nPerDvLmt ){ pf->Bit.CtgDeviation = true; }
			
			// Waiting for sensor signal stable after detect the signal error
			if( (MeasHandle.Filter.Buf[iPrv].Bit.SystemPwr == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.FlowRate == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CtgExcCurrent == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CtgRandomNoise == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CtgNotStable == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CtgDeviation == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CtgExcSignal == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CtgDacError == true) )
			{
				if( MeasHandle.Dev.nDet[iSen] > 0.0001f ){
					pf->Bit.CtgNotStable = true;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Filter_Cvt( Measuring_FilterItem *pf )
{
	U8		iPrv = MeasHandle.Filter.Idx-1 & MEASURING_FILTER_BUF_MASK;
	U8		iSen;
	U8		iCvt;
	U8		RandomNoiseCnt = 0;
	U8		DeviationCnt = 0;
	U16		TimeDvLmt;
	U16		RawDvMaxLmt;
	U16		RawDvMaxSumLmt;
	U16		AvDvSumLmt;
	bool	OptCheckEnabled;
	float	OptCheckVoltage;
	
	if( MeasHandle.pSen->pCvt->IsPresent == true ){
		// Operating Temperature Check
		if( (MeasHandle.pCur->CvtSenTemp < -20.f) || (MeasHandle.pCur->CvtSenTemp >  85.f) ){
			pf->Bit.CvtTemp = true;
			if( MeasHandle.Filter.CvtTempErrTmr < TEMP_ERR_DEC_TIME ){
				MeasHandle.Filter.CvtTempErrTmr++;
			}
			else {
				SystemError.Main.Bit.CvtTemp = true;
			}
		}
		else {
			MeasHandle.Filter.CvtTempErrTmr = 0;
			SystemError.Main.Bit.CvtTemp = false;
		}
		
		if( MeasHandle.pSen->pCvt->State == 0 ){
			pf->Bit.CvtWarmUp = true;
		}
		
		// IR Source Power Check
		if( (MeasHandle.pCur->CvtSrcPwr < 400.f) || (MeasHandle.pCur->CvtSrcPwr > 500.f) ){
			pf->Bit.CvtSrcPwr = true;
			
			if( MeasHandle.Filter.SrcPwrErrTmr < SRC_PWR_ERR_DEC_TIME ){
				MeasHandle.Filter.SrcPwrErrTmr++;
			}
			else {
				SystemError.Main.Bit.IR_SourcePower = true;
			}
		}
		else {
			MeasHandle.Filter.SrcPwrErrTmr = 0;
		}
		
		// Offset Stability Check
		AvDvSumLmt = 500;
		for( iCvt=0; iCvt<MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; iCvt++ ){
			if( MeasHandle.Dev.CvtOcAvrgSum[iCvt] < AvDvSumLmt ){
				break;
			}
		}
		if( iCvt == MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] ){
			pf->Bit.CvtOffsInstability = true;                   
			if( MeasHandle.Filter.CvtOffsInstabilityTmr < OFFSET_INSTABILITY_DEC_TIME ){
				MeasHandle.Filter.CvtOffsInstabilityTmr++;
			}
			else {
				SystemError.Main.Bit.IR_OffsNotStable = true;
			}
		}
		else {
			if( MeasHandle.Filter.CvtOffsInstabilityTmr ){
				MeasHandle.Filter.CvtOffsInstabilityTmr--;
			}
			if( MeasHandle.Filter.CvtOffsInstabilityTmr > OFFSET_INSTABILITY_CHK_TIME ){ pf->Bit.CvtOffsInstability = true; }
		}
		
		
		for( iCvt=0; iCvt<MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; iCvt++ ){
			
			iSen = CARTRIDGE_SEN_CH_MAX + iCvt;
			
			// Offset Drift Check
			if( (MeasHandle.pCur->CvtHiPeak[iCvt] > 4.9f) && (MeasHandle.pCur->CvtLoPeak[iCvt] > 0.1f) ){
				pf->Bit.CvtOffsetDrift = true;
				if( MeasHandle.Filter.CvtOffsDriftTmr[iCvt] < OFFSET_DRF_DEC_TIME ){
					MeasHandle.Filter.CvtOffsDriftTmr[iCvt]++;
				}
				else {
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.IR_OffsDrift_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.IR_OffsDrift_CH2 = true;
					}
					else {
						SystemError.Main.Bit.IR_OffsDrift_REF = true;
					}
				}
			}
			else if( (MeasHandle.pCur->CvtLoPeak[iCvt] < 0.1f) && (MeasHandle.pCur->CvtHiPeak[iCvt] < 4.9f) ){
				pf->Bit.CvtOffsetDrift = true;
				if( MeasHandle.Filter.CvtOffsDriftTmr[iCvt] < OFFSET_DRF_DEC_TIME ){
					MeasHandle.Filter.CvtOffsDriftTmr[iCvt]++;
				}
				else {
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.IR_OffsDrift_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.IR_OffsDrift_CH2 = true;
					}
					else {
						SystemError.Main.Bit.IR_OffsDrift_REF = true;
					}
				}
			}
			else {
				MeasHandle.Filter.CvtOffsDriftTmr[iCvt] = 0;
			}
			
			// Peak to peak is exceeded
			if( ((MeasHandle.pCur->nDet[iSen] < 0.01f) && (pf->Bit.CvtSrcPwr == false)) ||
				 (MeasHandle.pCur->nDet[iSen] > 4.99f) )
			{
				pf->Bit.CvtExcSignal = true;
				if( MeasHandle.Filter.ExcSignalTmr[iSen] < EXC_SIGNAL_DEC_TIME ){
					MeasHandle.Filter.ExcSignalTmr[iSen]++;
				}
				else {
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.IR_ExcSignal_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.IR_ExcSignal_CH2 = true;
					}
					else {
						SystemError.Main.Bit.IR_ExcSignal_REF = true;
					}
				}
			}
			else {
				MeasHandle.Filter.ExcSignalTmr[iSen] = 0;
			}
			
			// Signal Stability
			switch( MeasHandle.pSen->pCvt->Inf[CAVITY_INF_DET_TYPE] ){
				case 970:
					TimeDvLmt = 3; // 300 usec
					RawDvMaxLmt = 38; // 2.968750 mV
					RawDvMaxSumLmt = (U16)( 777.f - (3.8f - MeasHandle.Gas[iSen].Inf.bZero.f) * 130.f); // 60.703125 mV
					break;
					
				case 972:
					TimeDvLmt = 5; // 500 usec
					RawDvMaxLmt = 53; // 4.140625 mV
					RawDvMaxSumLmt = (U16)(1100.f - (3.8f - MeasHandle.Gas[iSen].Inf.bZero.f) * 130.f); // 78.437500 mV
					break;
					
				default:
					TimeDvLmt = 3; // 300 usec
					RawDvMaxLmt = 38; // 2.968750 mV
					RawDvMaxSumLmt = (U16)( 777.f - (3.8f - MeasHandle.Gas[iSen].Inf.bZero.f) * 130.f); // 60.703125 mV
					break;
			}
			if( (MeasHandle.Dev.CvtHiTime[iCvt] > TimeDvLmt) ||
				(MeasHandle.Dev.CvtLoTime[iCvt] > TimeDvLmt) ||
				(MeasHandle.Dev.CvtRawDvMax[iCvt] > RawDvMaxLmt) ||
				(MeasHandle.Dev.CvtRawDvMaxSum[iCvt] > RawDvMaxSumLmt) )
			{
				RandomNoiseCnt++;
				if( MeasHandle.Filter.NotStableTmr[iSen] < 0xFFFF ){
					MeasHandle.Filter.NotStableTmr[iSen]++;
				}
				if( MeasHandle.Filter.NotStableTmr[iSen] == SecToMeasuringTime(MeasHandle.Filter.NotStableDecTime / 10) ){
					if( iSen == MeasHandle.Conf.iOut[0] ){
						SystemError.Main.Bit.IR_NotStable_CH1 = true;
					}
					else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
						SystemError.Main.Bit.IR_NotStable_CH2 = true;
					}
					else {
						SystemError.Main.Bit.IR_NotStable_REF = true;
					}
				}
				if( MeasHandle.Filter.NotStableTmrMax[iSen] < MeasHandle.Filter.NotStableTmr[iSen] ){
					MeasHandle.Filter.NotStableTmrMax[iSen] = MeasHandle.Filter.NotStableTmr[iSen];
				}
			}
			else {
				if( MeasHandle.Filter.NotStableTmr[iSen] ){
					MeasHandle.Filter.NotStableTmr[iSen]--;
				}
			}
			if( MeasHandle.Filter.NotStableTmr[iSen] > NOT_STABLE_CHK_TIME ){ pf->Bit.CvtNotStable = true; }
			
			// Deviation Check
			if( (MeasHandle.Dev.CvtOcAvrg[iCvt] > 0.010f) || (MeasHandle.Dev.nPer[iSen] > 5.f) ){ DeviationCnt++; }
			
			// Waiting for sensor signal stable after detect the signal error
			if( (MeasHandle.Filter.Buf[iPrv].Bit.SystemPwr == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.FlowRate == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtSrcPwr == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtOffsetDrift == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtRandomNoise == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtNotStable == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtDeviation == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtExcSignal == true) ||
				(MeasHandle.Filter.Buf[iPrv].Bit.CvtOffsInstability == true) )
			{
				if( (MeasHandle.Dev.nDet[iSen] > 0.0001f) ||
					(MeasHandle.Dev.CvtHiPeak[iCvt] > 0.0001f) ||
					(MeasHandle.Dev.CvtLoPeak[iCvt] > 0.0001f) ||
					(MeasHandle.Dev.CvtOcAvrg[iCvt] > 0.00005f) )
				{
					pf->Bit.CvtNotStable = true;
				}
			}
			
			// IR optical path contamination check
			OptCheckEnabled = false;
			if( MeasHandle.Gas[iSen].Inf.bZeroUse == true ){
				OptCheckEnabled = true;
				OptCheckVoltage = MeasHandle.Gas[iSen].Inf.bZero.f;
			}
			else {
				if( (MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_CH) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_DH) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_IH) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_X018) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_X021) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_X054) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_X040) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_X044) ||
					(MeasHandle.Conf.CvtModel == MODEL_CVT_DUAL_X127) )
				{
					if( iSen == MeasHandle.Conf.iRefCvt ){
						OptCheckEnabled = true;
						OptCheckVoltage = MeasHandle.pCur->nDet[iSen];
					}
				}
			}
			if( OptCheckEnabled == true ){
				if( OptCheckVoltage < (MeasHandle.Gas[iSen].Inf.mZero.f * 0.85f) ){
					pf->Bit.CvtOptPath = true;
					if( MeasHandle.Filter.CvtOptPthErrTmr[iCvt] < OPT_PATH_ERR_DEC_TIME ){
						MeasHandle.Filter.CvtOptPthErrTmr[iCvt]++;
					}
					else {
						if( iSen == MeasHandle.Conf.iOut[0] ){
							SystemError.Main.Bit.IR_OptPathContaminated_CH1 = true;
						}
						else if( (iSen == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
							SystemError.Main.Bit.IR_OptPathContaminated_CH2 = true;
						}
						else {
							SystemError.Main.Bit.IR_OptPathContaminated_REF = true;
						}
					}
				}
				else {
					MeasHandle.Filter.CvtOptPthErrTmr[iCvt] = 0;
				}
			}
			else {
				MeasHandle.Filter.CvtOptPthErrTmr[iCvt] = 0;
			}
		}
		
		// Noise Check
		if( RandomNoiseCnt > (MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] / 2) ){ pf->Bit.CvtRandomNoise = true; }
		if( DeviationCnt   > (MeasHandle.pSen->pCvt->Inf[CAVITY_INF_CHANNELS] / 2) ){ pf->Bit.CvtDeviation   = true; }
		
		Interference_Gas_Filtering( pf );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Filter_Decision( void )
{
	Measuring_FilterItem	ft = { 0 };
	U8						i;
	
	Measuring_Filter_Com( &ft );
	Measuring_Filter_Ctg( &ft );
	Measuring_Filter_Cvt( &ft );
	
	if( ft.Word ){
		if( MeasHandle.Filter.PendingTmr < PENDING_DEC_TIME ){
			MeasHandle.Filter.PendingTmr++;
		}
		else {
			SystemError.Main.Bit.Measurement_Pending = true;
		}
	}
	else {
		SystemError.Main.Bit.Measurement_Pending = false;
		MeasHandle.Filter.PendingTmr = 0;
	}
	
	MeasHandle.Filter.Buf[MeasHandle.Filter.Idx & MEASURING_FILTER_BUF_MASK] = ft;
	for( ft.Word=0, i=0; i<MEASURING_FILTER_BUF_SIZE; i++ ){ ft.Word |= MeasHandle.Filter.Buf[i].Word; }
	MeasHandle.Filter.Res = ft;
	MeasHandle.Filter.Idx++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Output_Gas_Decision( void )
{
	U8						i;
	bool					IsOutputCh;
	U8						iOut;
	bool					IsRemTest[2];
	bool					IsWebTest[2];
	Measuring_FilterItem	Filter;
	S16						Result;
	S16						CtgOut;
	S16						CtgRef;
	S16						ZskipLo;
	S16						ZskipHi;
	S16						CrsSpan;
	S16						SskipLo;
	S16						SskipHi;
	S16						Over;
	float					HiScalePer;
	float					Actual;
	bool					IsWarmingUp;
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( i<CARTRIDGE_SEN_CH_MAX ){ IsWarmingUp = MeasHandle.Filter.Res.Bit.CtgWarmUp; }
		else						{ IsWarmingUp = MeasHandle.Filter.Res.Bit.CvtWarmUp; }
		
		if( (MeasHandle.Gas[i].IsPresent == true) && (IsWarmingUp == false) ){
		
			if		(MeasHandle.pCur->nPer[i] < MeasHandle.Gas[i].nPerMin){MeasHandle.Gas[i].nPerMin = MeasHandle.pCur->nPer[i];}
			else if	(MeasHandle.pCur->nPer[i] > MeasHandle.Gas[i].nPerMax){MeasHandle.Gas[i].nPerMax = MeasHandle.pCur->nPer[i];}
			if		(MeasHandle.pCur->iPer[i] < MeasHandle.Gas[i].iPerMin){MeasHandle.Gas[i].iPerMin = MeasHandle.pCur->iPer[i];}
			else if	(MeasHandle.pCur->iPer[i] > MeasHandle.Gas[i].iPerMax){MeasHandle.Gas[i].iPerMax = MeasHandle.pCur->iPer[i];}
			
			IsRemTest[0] = MeasHandle.Test.Item.RemoteTest_CH1;
			IsRemTest[1] = MeasHandle.Test.Item.RemoteTest_CH2;
			IsWebTest[0] = MeasHandle.Test.Item.WebTest_CH1;
			IsWebTest[1] = MeasHandle.Test.Item.WebTest_CH2;
			HiScalePer = MeasHandle.Gas[i].Inf.HighScale * 0.01f;
			
			Filter.Word = MeasHandle.Filter.Res.Word;
			
			if( i == MeasHandle.Conf.iOut[0] ){
				IsOutputCh = true;
				iOut = 0;
				Filter.Bit.InterfGas_2 = false;
			}
			else if( (i == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
				IsOutputCh = true;
				iOut = 1;
				Filter.Bit.InterfGas_1 = false;
			}
			else {
				IsOutputCh = false;
				Filter.Word = 0;
			}
			
			if( MeasHandle.Gas[i].Inf.Method == MEASURING_METHOD_IR ){
				Filter.Bit.CtgWarmUp		= false;
				Filter.Bit.CtgTemp			= false;
				Filter.Bit.CtgExcCurrent	= false;
				Filter.Bit.CtgRandomNoise	= false;
				Filter.Bit.CtgNotStable		= false;
				Filter.Bit.CtgDeviation		= false;
				Filter.Bit.CtgExcSignal		= false;
			}
			else {
				Filter.Bit.CvtWarmUp		= false;
				Filter.Bit.CvtTemp			= false;
				Filter.Bit.CvtSrcPwr		= false;
				Filter.Bit.CvtOffsetDrift	= false;
				Filter.Bit.CvtRandomNoise	= false;
				Filter.Bit.CvtNotStable		= false;
				Filter.Bit.CvtDeviation		= false;
				Filter.Bit.CvtExcSignal		= false;
				Filter.Bit.CvtOptPath		= false;
			}
			
			if( (Filter.Word == 0) || (FuncHandle.State.Bit.Calibration == true) || (SystemState == SYSTEM_STATE_ALLBASE) ){
				MeasHandle.Gas[i].Per = MeasHandle.pCur->nPer[i];
			}
			
			// Percentage to Concentration
			Actual = HiScalePer * MeasHandle.Gas[i].Per;
			if		( Actual >  9999 ){ Actual =  9999; }
			else if	( Actual < -9999 ){ Actual = -9999; }
			
			MeasHandle.Gas[i].Act = (S16)Actual;
			
			// Min & Max Update
			if( MeasHandle.Alr[i].Latched == true ){
				if		( MeasHandle.Gas[i].Min > MeasHandle.Gas[i].Act ){ MeasHandle.Gas[i].Min = MeasHandle.Gas[i].Act; }
				else if	( MeasHandle.Gas[i].Max < MeasHandle.Gas[i].Act ){ MeasHandle.Gas[i].Max = MeasHandle.Gas[i].Act; }
			}
			else {
				MeasHandle.Gas[i].Min = MeasHandle.Gas[i].Act;
				MeasHandle.Gas[i].Max = MeasHandle.Gas[i].Act;
			}
			
			// Result Mapping
			if( IsOutputCh == true ){
				
				if( (MeasHandle.Test.Item.GasSimulation == true) || (IsRemTest[iOut] == true) || (IsWebTest[iOut] == true) ){
					MeasHandle.Gas[i].Res = MeasHandle.Gas[i].Set;
				}
				else if( MeasHandle.Mode.Item.Engineering == true ){
					MeasHandle.Gas[i].Res = MeasHandle.Gas[i].Act;
				}
				else {
					if( MeasHandle.Alr[i].Latched == true ){
						if( MeasHandle.Alr[i].Item.Bit.a1st == true ){
							if( MeasHandle.Alr[i].Inf._1stTypeIncrease == true ){
								Result = MeasHandle.Gas[i].Max;
							}
							else {
								Result = MeasHandle.Gas[i].Min;
							}
						}
						if( MeasHandle.Alr[i].Item.Bit.a2nd == true ){
							if( MeasHandle.Alr[i].Inf._2ndTypeIncrease == true ){
								Result = MeasHandle.Gas[i].Max;
							}
							else {
								Result = MeasHandle.Gas[i].Min;
							}
						}
					}
					else {
						Result = MeasHandle.Gas[i].Act;
					}
					
					ZskipLo = (S16)(-HiScalePer * (MeasHandle.Gas[i].Inf.ZeroSkipPer * 0.1f) );
					ZskipHi = (S16)( HiScalePer * (MeasHandle.Gas[i].Inf.ZeroSkipPer * 0.1f) );
					CrsSpan = (S16)(MeasHandle.Gas[i].Inf.SpanGas * MeasHandle.Gas[i].Inf.CrossScale / 100.f);
					SskipLo = (S16)(CrsSpan - HiScalePer * (MeasHandle.Gas[i].Inf.SpanSkipPer * 0.1f) );
					SskipHi = (S16)(CrsSpan + HiScalePer * (MeasHandle.Gas[i].Inf.SpanSkipPer * 0.1f) );
					
					if( MeasHandle.Conf.Func[i] == MEASURING_GAS_FUNC_CT_OUT_WITH_REF ){
						CtgOut = MeasHandle.Gas[i].Act;
						Actual = HiScalePer * MeasHandle.Gas[MeasHandle.Conf.iRefCtg].Per;
						if		( Actual >  9999 ){ Actual =  9999; }
						else if	( Actual < -9999 ){ Actual = -9999; }
						CtgRef = (S16)Actual;
						if( MeasHandle.Gas[i].Inf.SenNo == CT_SEN_O2_MP_100 ){
							if		( ((CtgOut >= SskipLo) && (CtgOut <= SskipHi)) ||
									  ((CtgRef >= SskipLo) && (CtgRef <= SskipHi)) ){ Result = CrsSpan; }
							else if	(  (CtgOut >= ZskipLo) && (CtgOut <= ZskipHi)  ){ Result = 0; }
						}
						else {
							if		( ((CtgOut >= ZskipLo) && (CtgOut <= ZskipHi)) ||
									  ((CtgRef >= ZskipLo) && (CtgRef <= ZskipHi)) ){ Result = 0; }
							else if	(  (CtgOut >= SskipLo) && (CtgOut <= SskipHi)  ){ Result = CrsSpan; }
						}
					}
					else {
						if		( (Result >= ZskipLo) && (Result <= ZskipHi) ){ Result = 0; }
						else if	( (Result >= SskipLo) && (Result <= SskipHi) ){ Result = CrsSpan; }
					}
					
					if( MeasHandle.Gas[i].Inf.OutDelaySec ){
						if( Result >= (HiScalePer * MeasHandle.Gas[i].Inf.OutDelayPer * 0.1f) ){
							if( MeasHandle.Gas[i].Inf.OutDelayTmr ){
								MeasHandle.Gas[i].Inf.OutDelayTmr--;
							}
							else {
								MeasHandle.Gas[i].Res = Result;
							}
						}
						else {
							MeasHandle.Gas[i].Inf.OutDelayTmr = SecToMeasuringTime( MeasHandle.Gas[i].Inf.OutDelaySec );
							MeasHandle.Gas[i].Res = Result;
						}
					}
					else {
						MeasHandle.Gas[i].Res = Result;
					}
				}
			}
			else {
				MeasHandle.Gas[i].Res = MeasHandle.Gas[i].Act;
			}
			
			Over = (S16)(MeasHandle.Gas[i].Inf.HighScale * 1.1f);
			if( MeasHandle.Gas[i].Res < 0 ){
				MeasHandle.Gas[i].Dsp = 0;
			}
			else if( MeasHandle.Gas[i].Res > Over ){
				MeasHandle.Gas[i].Dsp = Over;
			}
			else {
				MeasHandle.Gas[i].Dsp = MeasHandle.Gas[i].Res;
			}
			
			if( MeasHandle.Mode.Item.Maintenance == true ){
				MeasHandle.Gas[i].Out = MeasHandle.Gas[i].Inf.MaintenanceGas;
			}
			else {
				MeasHandle.Gas[i].Out = MeasHandle.Gas[i].Dsp;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Ctg_Dac_Reset( void )
{
	U16		RstCnt = SensingProcess.pCtg->Eep[CARTRIDGE_EEP_BLK_SYS][SENSOR_EEP_SYS_CTG_DAC_RESET_CNT] + 1;
	
	SenEEP_Write_Ctg( &RstCnt, CARTRIDGE_EEP_BLK_SYS, SENSOR_EEP_SYS_CTG_DAC_RESET_CNT, 1 );
	
	
}



