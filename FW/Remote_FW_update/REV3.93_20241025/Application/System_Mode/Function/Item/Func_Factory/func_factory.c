#define __FUNC_FACTORY_C__
    #include "func_factory.h"
#undef  __FUNC_FACTORY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vfunc_factory.h"
#include "eeprom.h"
#include "adc.h"
#include "ma_output.h"
#include "poe.h"
#include "rs485.h"
#include "relay.h"
#include "func_flowset.h"
/* End Include */

/* Start Define */
#define SENSOR_ADC_LSB					( 5.12f / 65536.f )		// voltage
#define CAL_INJECTION_TIMEOUT			( 600 )					// seconds
/* End Define */

/* Start Enum */
enum {
	CONF_ITEM_CT0,
	CONF_ITEM_CT1,
	CONF_ITEM_CVT,
	CONF_ITEM_NONE
};
/* End Enum */

/* Start Struct */
typedef struct {
	float	Lo;
	float	Hi;
}FactoryCal_Limit;
/* End Struct */

/* Start Function */
static void Measuring_Config_Load( U16 SetNo, U8 Item );
static void Measuring_Cartridge_Config_Update( void );
static void Measuring_System_Config_Update( void );
static void Measuring_Option_Update( void );
static void Measuring_Filter_Update( void );
static void Maintenance_Data_Update( void );
static FactoryCal_Limit FactoryCal_Limit_Load( Measuring_GasInfo *pInf );
static void FactoryCal_Task( Func_HandleType *pFunc );
static void FactoryCal_Cartridge_Rgain_Update( U8 i );
static void FactoryCal_Cartridge_Rgain_Return( U8 i );
static void FactoryCal_Target_Set_Update( U8 i );
static void FactoryCal_Zero_Update( U8 i );
static void FactoryCal_Span_Update( U8 i );
static void FactoryCal_Base_Cal_Update( U8 i );
static void FactoryCal_Base_Set_Update( U8 i );
static void Factory_Sensor_Set_Init( U8 i );
static void Factory_Sensor_Cal_Init( U8 i );
static void Factory_Init( void );
static void KeyProcess( Func_HandleType *pFunc );
static void KeyProcess_Measuring( Func_HandleType *pFunc );
static void KeyProcess_Cal( Func_HandleType *pFunc );
static void KeyProcess_SysInit( Func_HandleType *pFunc );
static void KeyProcess_SysInfo( Func_HandleType *pFunc );
/* End Function */

/* Start Variable */
static Factory_SetType	FaSet;
static U8				iBaseZeroSet;
static U8				iBaseZeroMax;
static U8				iBaseZeroBuf[MEASURING_GAS_SENSOR_MAX];
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Func_Factory( Func_HandleType *pFunc )
{
	KeyProcess( pFunc );
	if( pFunc->TaskEvt == true ){ FactoryCal_Task( pFunc ); }
	if( pFunc->ViewEvt == true ){ vFunc_Factory( pFunc, &FaSet ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Config_Load( U16 SetNo, U8 Item )
{
	Measuring_ConfNoType	Backup = FaSet.sConf.SetNo;
	U8						i;
	
	switch( Item ){
		case CONF_ITEM_CT0:
			FaSet.sConf.SetNo.Ctg[0] = SetNo;
			if( (FaSet.pMeas->pSen->pCvt->IsPresent == true) &&
				(FaSet.pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] > 1) ){
				FaSet.sConf.SetNo.Ctg[1] = SetNo;
			}
			break;
			
		case CONF_ITEM_CT1:
			FaSet.sConf.SetNo.Ctg[1] = SetNo;
			break;
			
		case CONF_ITEM_CVT:
			FaSet.sConf.SetNo.Cvt = SetNo;
			break;
	}
	
	if( Measuring_Channel_Config_Load( &FaSet.sConf ) == true ){
		FaSet.sConf.SetNo = Backup;
		Measuring_Channel_Config_Load( &FaSet.sConf );
	}
	else {
		for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
			
			if( MeasHandle.Gas[i].IsPresent == true ){
				FaSet.sGas[i].SenNo = FaSet.sConf.SenNo[i];
				FaSet.sGas[i].GasNo = FaSet.sConf.GasNo[i];
				Measuring_Default_Gas_Info_Load( &FaSet.sGas[i], &FaSet.sAlr[i] );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Cartridge_Config_Update( void )
{
	if( (FaSet.pMeas->Conf.SenComb == MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL) ||
		(FaSet.pMeas->Conf.SenComb == MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD) ){
		FaSet.sCtgSenType[1] = FaSet.sCtgSenType[0];
	}
	
	SenEEP_Write_Ctg( FaSet.sCtgSenType, CARTRIDGE_EEP_BLK_SYS, SENSOR_EEP_SYS_CTG_SEN_TYPE_0, 2 );
	
	FaSet.sConf.SetNo.Ctg[0] = 0;
	FaSet.sConf.SetNo.Ctg[1] = 0;
	
	Measuring_Config_Load( NULL, CONF_ITEM_NONE );
	Measuring_System_Config_Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_System_Config_Update( void )
{
	U8		i;
	
	FaSet.pMeas->Conf = FaSet.sConf;
	SenEEP_Write_Ctg( &FaSet.pMeas->Conf.SetNo.Ctg[0], CARTRIDGE_EEP_BLK_SYS, SENSOR_EEP_SYS_CTG_CONF_NO_0, 2 );
	SenEEP_Write_Cvt( &FaSet.pMeas->Conf.SetNo.Cvt, CAVITY_EEP_BLK_SYS, SENSOR_EEP_SYS_CVT_CONF_NO, 1 );
	
	//EEPROM_Update( &FaSet.pMeas->Conf.SetNo.Ctg[0], SYSTEM_EEP_RES_68, 2 );
	//EEPROM_Update( &FaSet.pMeas->Conf.SetNo.Cvt, SYSTEM_EEP_RES_70, 1 );
	
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		
		if( MeasHandle.Gas[i].IsPresent == true ){
			FaSet.pMeas->Gas[i].Inf = FaSet.sGas[i];
			FaSet.pMeas->Alr[i].Inf = FaSet.sAlr[i];
			FaSet.pMeas->Gas[i].Inf.Func = FaSet.pMeas->Conf.Func[i];
			Measuring_Gas_Info_Update( i );
		}
	}
	
	Measuring_Output_Channel_Mapping();
	Measuring_Gas_Function_Load();
	
	SystemError.Main.Bit.Measurement_Parameter = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Option_Update( void )
{
	U8		i;
	U16		eep[5];
	
	for( i=0; i<FaSet.pMeas->Conf.OutMax; i++ ){
		
		if( (FaSet.pMeas->pGas[i]->Inf.ZeroSkipPer != FaSet.sGas[i].ZeroSkipPer) ||
			(FaSet.pMeas->pGas[i]->Inf.SpanSkipPer != FaSet.sGas[i].SpanSkipPer) ||
			(FaSet.pMeas->pGas[i]->Inf.OutDelayPer != FaSet.sGas[i].OutDelayPer) ||
			(FaSet.pMeas->pGas[i]->Inf.OutDelaySec != FaSet.sGas[i].OutDelaySec) ||
			(FaSet.pMeas->pAlr[i]->Inf.IsUnderCheckEnabled != FaSet.sAlr[i].IsUnderCheckEnabled) ){
			
			FaSet.pMeas->pGas[i]->Inf.ZeroSkipPer = FaSet.sGas[i].ZeroSkipPer;
			FaSet.pMeas->pGas[i]->Inf.SpanSkipPer = FaSet.sGas[i].SpanSkipPer;
			FaSet.pMeas->pGas[i]->Inf.OutDelayPer = FaSet.sGas[i].OutDelayPer;
			FaSet.pMeas->pGas[i]->Inf.OutDelaySec = FaSet.sGas[i].OutDelaySec;
			FaSet.pMeas->pAlr[i]->Inf.IsUnderCheckEnabled = FaSet.sAlr[i].IsUnderCheckEnabled;
			
			eep[0] = FaSet.pMeas->pGas[i]->Inf.ZeroSkipPer;
			eep[1] = FaSet.pMeas->pGas[i]->Inf.SpanSkipPer;
			eep[2] = FaSet.pMeas->pGas[i]->Inf.OutDelayPer;
			eep[3] = FaSet.pMeas->pGas[i]->Inf.OutDelaySec;
			eep[4] = FaSet.pMeas->pAlr[i]->Inf.IsUnderCheckEnabled;
			
			SenEEP_Write_CHn( eep, FaSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_ZERO_SKIP_PERCENTAGE_x10, 5 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Measuring_Filter_Update( void )
{
	if( FaSet.pMeas->Filter.NotStableDecTime != FaSet.NotStableDecTime ){
		FaSet.pMeas->Filter.NotStableDecTime  = FaSet.NotStableDecTime;
		
		EEPROM_Update( &FaSet.pMeas->Filter.NotStableDecTime, SYSTEM_EEP_MEASURING_NOT_STABLE_DEC_TIME, 1 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Maintenance_Data_Update( void )
{
	U8		i;
	U16		uA;
	
	for( i=0; i<FaSet.pMeas->Conf.OutMax; i++ ){
		
		if( FaSet.pMeas->pGas[i]->Inf.MaintenanceGas != FaSet.sGas[i].MaintenanceGas ){
			FaSet.pMeas->pGas[i]->Inf.MaintenanceGas  = FaSet.sGas[i].MaintenanceGas;
			
			SenEEP_Write_CHn( &FaSet.sGas[i].MaintenanceGas, FaSet.pMeas->Conf.iOut[i], SENSOR_EEP_CHn_MAINTENANCE_GAS, 1 ); 
			
			uA = 16000 / FaSet.pMeas->pGas[i]->Inf.HighScale * FaSet.pMeas->pGas[i]->Inf.MaintenanceGas + 4000;
			
			if( i == 0 ){ EEPROM_Update( &uA, SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1, 1 ); }
			else		{ EEPROM_Update( &uA, SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2, 1 ); }
		}
	}
	
	if( FaSet.pMeas->Mode.Item.Maintenance_QuitMin != FaSet.sMode.Item.Maintenance_QuitMin ){
		FaSet.pMeas->Mode.Item.Maintenance_QuitMin  = FaSet.sMode.Item.Maintenance_QuitMin;
		
		EEPROM_Update( &FaSet.pMeas->Mode.Word, SYSTEM_EEP_MEASURING_MODE, 1 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static FactoryCal_Limit FactoryCal_Limit_Load( Measuring_GasInfo *pInf )
{
	float				Lx, Hx, C, Lo, Ho;
	FactoryCal_Limit	Lmt;
	
	if( pInf->Method == MEASURING_METHOD_IR ){
		switch( pInf->SenNo ){
			case IR_SEN_03_330:
				switch( pInf->GasNo ){
					case IR_GAS_03_330_LNG_100_LEL:
					case IR_GAS_03_330_CH4_100_LEL:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X025_LNG_100_LEL:	Lx =   -43.25f, Hx =    -86.98f, Lo =  3.00f, Ho =  -4.00f; break;
							case MODEL_CVT_QUAD_X025_CH4_100_LEL:	Lx =   -43.25f, Hx =    -86.98f, Lo =  3.00f, Ho =  -4.00f; break;
							case MODEL_CVT_DUAL_CH:					Lx =   -31.57f, Hx =    -63.48f, Lo =  3.00f, Ho =  -4.00f; break;
						}
						break;
						
					case IR_GAS_03_330_C4H10_100_LEL:				Lx =   -24.86f, Hx =    -55.94f, Lo =  4.00f, Ho =   0.00f; break;
					case IR_GAS_03_330_SFA_1_100_LEL:				Lx =   -33.13f, Hx =    -71.62f, Lo =  4.00f, Ho =  -0.73f; break;
					case IR_GAS_03_330_4MS_100_LEL:					Lx =   -35.09f, Hx =    -76.75f, Lo =  2.50f, Ho =  -5.00f; break;
					case IR_GAS_03_330_TOLUENE_100_LEL:				Lx =   -34.70f, Hx =    -63.10f, Lo =  0.60f, Ho =  -6.00f; break;
					case IR_GAS_03_330_CH4_100_VOL:					Lx = -6870.68f, Hx = -12546.47f, Lo =  6.40f, Ho =  -6.40f; break;
				}
				break;
				
			case IR_SEN_03_370:
				switch( pInf->GasNo ){
					case IR_GAS_03_370_GMP_02_3000_PPM:				Lx =   -50.60f, Hx =    -88.55f, Lo =  1.80f, Ho =  -6.00f; break;
					case IR_GAS_03_370_OMCTS_100_LEL:				Lx =   -61.78f, Hx =   -110.05f, Lo =  3.00f, Ho =  -6.17f; break;
					case IR_GAS_03_370_BTBAS_1000_PPM:				Lx =   -35.86f, Hx =    -66.00f, Lo =  0.60f, Ho =  -5.00f; break;
				}
				break;
				
			case IR_SEN_03_420:
				switch( pInf->GasNo ){
					case IR_GAS_03_420_ECH_100_LEL:					Lx =   -75.90f, Hx =   -100.83f, Lo =  3.00f, Ho =  -2.00f; break;
				}
				break;
				
			case IR_SEN_04_260:
				switch( pInf->GasNo ){
					case IR_GAS_04_260_CO2_5000_PPM:				Lx =   -41.81f, Hx =    -72.83f, Lo =  3.80f, Ho =  -2.80f; break;
					case IR_GAS_04_260_CO2_9999_PPM:				Lx =   -47.10f, Hx =    -75.59f, Lo =  1.40f, Ho =  -0.80f; break;
					case IR_GAS_04_260_CO2_1_00_VOL:				Lx =   -47.11f, Hx =    -75.59f, Lo =  1.40f, Ho =  -0.80f; break;
					case IR_GAS_04_260_CO2_5_0_VOL:					Lx =   -49.86f, Hx =    -83.37f, Lo = -2.20f, Ho =   2.60f; break;
				}
				break;
				
			case IR_SEN_04_520:
				switch( pInf->GasNo ){
					case IR_GAS_04_520_N2O_4000_PPM:				Lx =   -44.56f, Hx =    -91.91f, Lo =  3.60f, Ho =  -2.20f; break;
					case IR_GAS_04_520_N2O_1000_PPM:				Lx =   -40.24f, Hx =    -67.07f, Lo =  0.80f, Ho =  -7.00f; break;
				}
				break;
				
			case IR_SEN_04_660:
				switch( pInf->GasNo ){
					case IR_GAS_04_660_CO_9999_PPM:					Lx =   -20.97f, Hx =    -37.75f, Lo =  1.10f, Ho =  -7.60f; break;
					case IR_GAS_04_660_CO_1_00_VOL:					Lx =   -20.98f, Hx =    -37.76f, Lo =  1.10f, Ho =  -7.60f; break;
					case IR_GAS_04_660_CO_2_00_VOL:					Lx =   -38.28f, Hx =    -65.07f, Lo =  2.00f, Ho =  -6.60f; break;
					case IR_GAS_04_660_CO_5_00_VOL:					Lx =   -65.11f, Hx =   -104.33f, Lo =  5.60f, Ho =  -5.30f; break;
				}
				break;
				
			case IR_SEN_04_850:
				switch( pInf->GasNo ){
					case IR_GAS_04_850_COS_200_PPM:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X025_COS_200_PPM:	Lx =   -36.23f, Hx =    -60.74f, Lo =  0.07f, Ho =  -3.10f; break;
							case MODEL_CVT_DUAL_X044:				Lx =   -32.18f, Hx =    -53.95f, Lo =  0.06f, Ho =  -3.10f; break;
						}
						break;
						
					case IR_GAS_04_850_COS_75_0_PPM:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X025_COS_75_0_PPM:	Lx =   -30.35f, Hx =    -52.18f, Lo =  0.07f, Ho =  -3.48f; break;
						}
						break;
				}
				break;
				
			case IR_SEN_08_000:
			   switch( pInf->GasNo ){
					case IR_GAS_08_000_C4F8_2000_PPM:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X027_C4F8_2000_PPM:	Lx =   -52.34f, Hx =   -102.61f, Lo =  5.20f, Ho =  -8.00f; break;
							case MODEL_CVT_DUAL_X054:				Lx =   -49.57f, Hx =    -97.16f, Lo =  4.20f, Ho =  -8.00f; break;
						}
						break;
						
					case IR_GAS_08_000_FL300_200_PPM:				Lx =   -61.86f, Hx =   -103.07f, Lo =  4.04f, Ho =  -6.82f; break;
					case IR_GAS_08_000_CF4_500_PPM:					Lx =   -19.27f, Hx =    -37.47f, Lo =  1.20f, Ho =  -2.30f; break;
			   }
			   break;
			   
			case IR_SEN_08_925:
				switch( pInf->GasNo ){
					case IR_GAS_08_925_C5F8_2000_PPM:				Lx =   -35.36f, Hx =    -77.67f, Lo =  0.73f, Ho = -11.90f; break;
					case IR_GAS_08_925_C4F6_2000_PPM:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X027_C4F6_2000_PPM:	Lx =   -36.92f, Hx =    -86.77f, Lo =  2.40f, Ho =  -8.00f; break;
							case MODEL_CVT_DUAL_X021:				Lx =   -33.78f, Hx =    -79.37f, Lo =  2.41f, Ho =  -8.00f; break;
						}
						break;
						
					case IR_GAS_08_925_CH2F2_2000_PPM:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X027_CH2F2_2000_PPM:Lx =   -31.55f, Hx =    -81.85f, Lo =  0.51f, Ho =  -3.60f; break;
							case MODEL_CVT_DUAL_X021:				Lx =   -50.65f, Hx =   -131.42f, Lo =  0.48f, Ho =  -3.60f; break;
						}
						break;
						
					case IR_GAS_08_925_TEOS_100_PPM:				Lx =   -37.96f, Hx =    -69.93f, Lo = -0.04f, Ho =  -3.00f; break;
					case IR_GAS_08_925_TEOS_HT200_100_PPM:			Lx =   -37.96f, Hx =    -69.93f, Lo = -0.04f, Ho =  -3.00f; break;
					case IR_GAS_08_925_HFC_2000_PPM:				Lx =   -50.65f, Hx =   -131.42f, Lo =  0.48f, Ho =  -3.60f; break;
					case IR_GAS_08_925_IPA_3000_PPM:				Lx =   -85.06f, Hx =   -143.78f, Lo =  0.25f, Ho =  -7.00f; break;
					case IR_GAS_08_925_IPA_100_LEL:					Lx =   -64.08f, Hx =   -105.09f, Lo =  1.60f, Ho =  -2.50f; break;
					case IR_GAS_08_925_SELITANE_100_LEL:			Lx =   -62.04f, Hx =   -101.75f, Lo =  1.60f, Ho =  -2.50f; break;
					case IR_GAS_08_925_C4F6_300_PPM:				Lx =   -30.18f, Hx =    -87.89f, Lo =  0.00f, Ho =  -3.40f; break;
				}
				break;
				
			case IR_SEN_09_076:
				switch( pInf->GasNo ){
					case IR_GAS_09_076_C4F6S_2000_PPM:				Lx =   -34.31f, Hx =    -57.90f, Lo =  4.00f, Ho = -10.00f; break;
				}
				break;
				
			case IR_SEN_09_150:
				switch( pInf->GasNo ){
					case IR_GAS_09_150_C4H2F6_1000_PPM:				Lx =   -27.06f, Hx =    -45.63f, Lo =  1.60f, Ho =  -7.10f; break;
				}
				break;
				
			case IR_SEN_09_230:
				switch( pInf->GasNo ){
					case IR_GAS_09_230_OMCTS_75_0_PPM:				Lx =   -30.77f, Hx =    -52.95f, Lo =  0.83f, Ho =  -2.79f; break;
				}
				break;
				
			case IR_SEN_09_440:
				switch( pInf->GasNo ){
					case IR_GAS_09_440_CH3F_2000_PPM:
						switch( MeasHandle.Conf.CvtModel ){
						   case MODEL_CVT_QUAD_X027_CH3F_2000_PPM:	Lx =   -28.80f, Hx =    -50.12f, Lo =  0.94f, Ho =  -4.08f; break;
						   case MODEL_CVT_DUAL_X018:				Lx =   -27.06f, Hx =    -47.09f, Lo =  0.91f, Ho =  -4.08f; break;
						}
						break;
						
					case IR_GAS_09_440_ETHANOL_100_LEL:				Lx =   -76.26f, Hx =   -122.89f, Lo =  3.04f, Ho =   1.00f; break;
					case IR_GAS_09_440_CTFE_1000_PPM:				Lx =   -43.67f, Hx =    -70.97f, Lo =  0.46f, Ho =  -7.40f; break;
					case IR_GAS_09_440_C3F6_2000_PPM:				Lx =   -38.19f, Hx =    -72.92f, Lo =  5.74f, Ho =  -9.40f; break;
				}
				break;
				
			case IR_SEN_09_790:
				switch( pInf->GasNo ){
					case IR_GAS_09_790_C3F6_2000_PPM:				Lx =   -44.26f, Hx =   -104.14f, Lo =  3.40f, Ho =  -7.00f; break;
				}
				break;
				
			case IR_SEN_10_100:
				switch( pInf->GasNo ){
					case IR_GAS_10_100_METHANOL_3000_PPM:			Lx =   -43.90f, Hx =    -84.13f, Lo =  0.13f, Ho =  -9.20f; break;
				}
				break;
				
			case IR_SEN_10_990:
				switch( pInf->GasNo ){
					case IR_GAS_10_990_NF3_200_PPM:
						switch( MeasHandle.Conf.CvtModel ){
							case MODEL_CVT_QUAD_X042_NF3_200_PPM:	Lx =   -41.97f, Hx =    -70.36f, Lo =  0.12f, Ho =  -3.76f; break;
							case MODEL_CVT_DUAL_X127:				Lx =   -44.40f, Hx =    -74.45f, Lo = -0.04f, Ho =  -3.76f; break;
						}
						break;
					   
					case IR_GAS_10_990_HMDS_1000_PPM:				Lx =   -39.99f, Hx =    -76.65f, Lo =  0.54f, Ho =  -8.20f; break;
					case IR_GAS_10_990_HMDS_100_LEL:				Lx =   -67.70f, Hx =   -120.58f, Lo =  4.90f, Ho =  -6.20f; break;
					case IR_GAS_10_990_C3F6O_5000_PPM:				Lx =   -41.08f, Hx =    -72.49f, Lo =  0.28f, Ho = -13.00f; break;
					case IR_GAS_10_990_C3H6_100_LEL:				Lx =   -72.19f, Hx =   -115.51f, Lo =  6.00f, Ho =  -5.40f; break;
					case IR_GAS_10_990_C2H4_100_LEL:				Lx =   -61.07f, Hx =   -107.31f, Lo =  6.50f, Ho =  -6.80f; break;
				}
				break;
		}
		C = (float)FaSet.Cal.SpanGas / (float)pInf->HighScale * 100.f * (float)pInf->CrossScale / 100.f;
		Lmt.Lo = (Lx * (1.f - exp( -pInf->a * pow( C, pInf->n ) ))) + Lo;
		Lmt.Hi = (Hx * (1.f - exp( -pInf->a * pow( C, pInf->n ) ))) + Ho;
	}
	else {
		switch( pInf->SenNo ){
			case CT_SEN_EC_DC_CL2_3:
				switch( pInf->GasNo ){
					case EC_GAS_EC_DC_CL2_3_BR2_3_00_PPM:			Lmt.Lo =   -416.7f; Lmt.Hi =   -2989.1f; break;
					case EC_GAS_EC_DC_CL2_3_CL2_3_00_PPM:			Lmt.Lo =   -416.7f; Lmt.Hi =   -2989.1f; break;
				}
				break;
				
			case CT_SEN_EC_DC_H2_2000:
				switch( pInf->GasNo ){
					case EC_GAS_EC_DC_H2_2000_H2_2000_PPM:			Lmt.Lo =      6.3f; Lmt.Hi =      44.8f; break;
				}
				break;
				
			case CT_SEN_EC_DC_HCL_15:
				switch( pInf->GasNo ){
					case EC_GAS_EC_DC_HCL_15_BCL3_15_0_PPM:			Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_HCDS_20_0_PPM:			Lmt.Lo =    290.1f; Lmt.Hi =    1120.9f; break;
					case EC_GAS_EC_DC_HCL_15_HCL_10_0_PPM:			Lmt.Lo =    189.5f; Lmt.Hi =     732.0f; break;
					case EC_GAS_EC_DC_HCL_15_LTO520_50_0_PPM:		Lmt.Lo =     58.0f; Lmt.Hi =     224.2f; break;
					case EC_GAS_EC_DC_HCL_15_MCS_15_0_PPM:			Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_PCL3_15_0_PPM:			Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_POCL3_15_0_PPM:		Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_SiCL4_15_0_PPM:		Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_SiH2CL2_15_0_PPM:		Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_SiHCL3_15_0_PPM:		Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_TCS_15_0_PPM:			Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
					case EC_GAS_EC_DC_HCL_15_TICL4_15_0_PPM:		Lmt.Lo =    193.4f; Lmt.Hi =     747.3f; break;
				}
				break;
				
			case CT_SEN_EC_DC_HF_9:
				switch( pInf->GasNo ){
					case EC_GAS_EC_DC_HF_9_BBR3_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
					case EC_GAS_EC_DC_HF_9_BF3_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
					case EC_GAS_EC_DC_HF_9_C2F4O_200_PPM:			Lmt.Lo =     -1.6f; Lmt.Hi =     -28.0f; break;
					case EC_GAS_EC_DC_HF_9_CH2O2_50_0_PPM:			Lmt.Lo =     -6.3f; Lmt.Hi =    -112.1f; break;
					case EC_GAS_EC_DC_HF_9_CLF3_1_00_PPM:			Lmt.Lo =   -312.5f; Lmt.Hi =   -5604.6f; break;
					case EC_GAS_EC_DC_HF_9_F2_3_00_PPM:				Lmt.Lo =   -104.2f; Lmt.Hi =   -1868.2f; break;
					case EC_GAS_EC_DC_HF_9_GEF4_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
					case EC_GAS_EC_DC_HF_9_HBR_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
					case EC_GAS_EC_DC_HF_9_HCOOH_50_0_PPM:			Lmt.Lo =     -6.3f; Lmt.Hi =    -112.1f; break;
					case EC_GAS_EC_DC_HF_9_HF_6_00_PPM:				Lmt.Lo =    -52.1f; Lmt.Hi =    -934.1f; break;
					case EC_GAS_EC_DC_HF_9_HNO3_20_0_PPM:			Lmt.Lo =    -15.6f; Lmt.Hi =    -280.2f; break;
					case EC_GAS_EC_DC_HF_9_PF3_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
					case EC_GAS_EC_DC_HF_9_SIF4_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
					case EC_GAS_EC_DC_HF_9_WF6_9_00_PPM:			Lmt.Lo =    -34.7f; Lmt.Hi =    -622.7f; break;
				}
				break;
				
			case CT_SEN_EC_DC_O3_1:
				switch( pInf->GasNo ){
					case EC_GAS_EC_DC_O3_1_O3_1_00_PPM:				Lmt.Lo =   -703.1f; Lmt.Hi =   -2802.3f; break;
				}
				break;
				
			case CT_SEN_ETO_C_20:
				switch( pInf->GasNo ){
					case EC_GAS_ETO_C_20_C2H4O_EO_30_0_PPM:			Lmt.Lo =    555.6f; Lmt.Hi =    3502.4f; break;
					case EC_GAS_ETO_C_20_C3H7OH_500_PPM:			Lmt.Lo =    400.0f; Lmt.Hi =    2521.7f; break;
					case EC_GAS_ETO_C_20_C5H8O2_30_0_PPM:			Lmt.Lo =    555.6f; Lmt.Hi =    3502.4f; break;
					case EC_GAS_ETO_C_20_C7H8_150_PPM:				Lmt.Lo =    111.1f; Lmt.Hi =     700.5f; break;
					case EC_GAS_ETO_C_20_TEB_15_0_PPM:				Lmt.Lo =   1111.1f; Lmt.Hi =    7004.8f; break;
					case EC_GAS_ETO_C_20_TEOS_30_0_PPM:				Lmt.Lo =    555.6f; Lmt.Hi =    3502.4f; break;
					case EC_GAS_ETO_C_20_USN_01_20_0_PPM:			Lmt.Lo =   1000.0f; Lmt.Hi =    6304.3f; break;
				}
				break;
				
			case CT_SEN_H2_C_20000:
				switch( pInf->GasNo ){
					case EC_GAS_H2_C_20000_H2_4_000_VOL:			Lmt.Lo =  16667.0f; Lmt.Hi =  119565.2f; break;
				}
				break;
				
			case CT_SEN_H2O2_CB_100:
				switch( pInf->GasNo ){
					case EC_GAS_H2O2_CB_100_H2O2_100_PPM:			Lmt.Lo =    400.0f; Lmt.Hi =    1434.8f; break;
				}
				break;
				
			case CT_SEN_H2S_C_50:
				switch( pInf->GasNo ){
					case EC_GAS_H2S_C_50_H2S_30_0_PPM:				Lmt.Lo =    666.7f; Lmt.Hi =    2391.3f; break;
				}
				break;
				
			case CT_SEN_HCN_C_100:
				switch( pInf->GasNo ){
					case EC_GAS_HCN_C_100_HCN_30_0_PPM:				Lmt.Lo =    111.1f; Lmt.Hi =     398.6f; break;
				}
				break;
				
			case CT_SEN_NH3_CR_200:
				switch( pInf->GasNo ){
					case EC_GAS_NH3_CR_200_ACP_02_150_PPM:			Lmt.Lo =     11.1f; Lmt.Hi =      79.7f; break;
					case EC_GAS_NH3_CR_200_C5H5N_150_PPM:			Lmt.Lo =     11.1f; Lmt.Hi =      79.7f; break;
					case EC_GAS_NH3_CR_200_NH3_75_0_PPM:			Lmt.Lo =     22.2f; Lmt.Hi =     159.4f; break;
					case EC_GAS_NH3_CR_200_NH4OH_75_0_PPM:			Lmt.Lo =     22.2f; Lmt.Hi =     159.4f; break;
					case EC_GAS_NH3_CR_200_SAM24_75_0_PPM:			Lmt.Lo =     22.2f; Lmt.Hi =     159.4f; break;
				}
				break;
				
			case CT_SEN_NO_CF_100:
				switch( pInf->GasNo ){
					case EC_GAS_NO_CF_100_NO_100_PPM:				Lmt.Lo =     83.3f; Lmt.Hi =     525.4f; break;
				}
				break;
				
			case CT_SEN_NO2_C_20:
				switch( pInf->GasNo ){
					case EC_GAS_NO2_C_20_NO2_15_0_PPM:				Lmt.Lo =   -416.7f; Lmt.Hi =   -1494.6f; break;
				}
				break;
				
			case CT_SEN_PH3_C_5:
				switch( pInf->GasNo ){
					case EC_GAS_PH3_C_5_COS_100_PPM:				Lmt.Lo =     20.4f; Lmt.Hi =      73.2f; break;
					case EC_GAS_PH3_C_5_GEH4_1_00_PPM:				Lmt.Lo =   2040.8f; Lmt.Hi =    7320.3f; break;
					case EC_GAS_PH3_C_5_H2SO4_25_0_PPM:				Lmt.Lo =     81.6f; Lmt.Hi =     292.8f; break;
					case EC_GAS_PH3_C_5_PH3_1_00_PPM:				Lmt.Lo =   2040.8f; Lmt.Hi =    7320.3f; break;
				}
				break;
				
			case CT_SEN_SIH4_CD_50:
				switch( pInf->GasNo ){
					case EC_GAS_SIH4_CD_50_3MS_30_0_PPM:			Lmt.Lo =     41.7f; Lmt.Hi =     373.6f; break;
					case EC_GAS_SIH4_CD_50_C3H10SI_30_0_PPM:		Lmt.Lo =     41.7f; Lmt.Hi =     373.6f; break;
					case EC_GAS_SIH4_CD_50_GEH4_1_00_PPM:			Lmt.Lo =   1600.0f; Lmt.Hi =   14347.8f; break;
					case EC_GAS_SIH4_CD_50_SI2H6_15_0_PPM:			Lmt.Lo =    106.7f; Lmt.Hi =     956.5f; break;
					case EC_GAS_SIH4_CD_50_SI3H8_15_0_PPM:			Lmt.Lo =    106.7f; Lmt.Hi =     956.5f; break;
					case EC_GAS_SIH4_CD_50_SIH4_15_0_PPM:			Lmt.Lo =    106.7f; Lmt.Hi =     956.5f; break;
					case EC_GAS_SIH4_CD_50_TSA_15_0_PPM:			Lmt.Lo =    106.7f; Lmt.Hi =     956.5f; break;
				}
				break;
				
			case CT_SEN_SO2_C_20:
				switch( pInf->GasNo ){
					case EC_GAS_SO2_C_20_H2SO4_100_PPM:				Lmt.Lo =     40.0f; Lmt.Hi =     143.5f; break;
					case EC_GAS_SO2_C_20_SO2_10_0_PPM:				Lmt.Lo =    400.0f; Lmt.Hi =    1434.8f; break;
				}
				break;
				
			case CT_SEN_C2H4_C_1500:
				switch( pInf->GasNo ){
					case EC_GAS_C2H4_C_1500_C2H4_2000_PPM:			Lmt.Lo =      1.8f; Lmt.Hi =      17.9f; break;
					case EC_GAS_C2H4_C_1500_C3H6_2000_PPM:			Lmt.Lo =      1.5f; Lmt.Hi =      14.9f; break;
				}
				break;
				
			case CT_SEN_CH2O_C_10:
				switch( pInf->GasNo ){
					case EC_GAS_CH2O_C_10_CH2O_10_0_PPM:			Lmt.Lo =  13333.0f; Lmt.Hi =   47826.1f; break;
				}
				break;
				
			case CT_SEN_CO_CF_200:
				switch( pInf->GasNo ){
					case EC_GAS_CO_CF_200_CO_150_PPM:				Lmt.Lo =    106.7f; Lmt.Hi =     956.5f; break;
				}
				break;
				
			case CT_SEN_O2_MP_100:
				switch( pInf->GasNo ){
					case EC_GAS_O2_MP_100_O2_30_0_VOL:				Lmt.Lo = -11000.0f; Lmt.Hi = -166666.7f; break;
					case EC_GAS_O2_MP_100_O2_25_0_VOL:				Lmt.Lo = -13200.0f; Lmt.Hi = -200000.0f; break;
				}
				break;
				
			case CT_SEN_ASH3_3E_1:
				switch( pInf->GasNo ){
					case EC_GAS_ASH3_3E_1_ASH3_0_300_PPM:			Lmt.Lo =    390.6f; Lmt.Hi =    9341.0f; break;
					case EC_GAS_ASH3_3E_1_B2H6_0_300_PPM:			Lmt.Lo =    390.6f; Lmt.Hi =    9341.0f; break;
					case EC_GAS_ASH3_3E_1_GEH4_3_00_PPM:			Lmt.Lo =     39.1f; Lmt.Hi =     934.1f; break;
				}
				break;
				
			case CT_SEN_COCL2_3E_1:
				switch( pInf->GasNo ){
					case EC_GAS_COCL2_3E_1_COCL2_1_00_PPM:			Lmt.Lo =    195.3f; Lmt.Hi =    2802.3f; break;
				}
				break;
				
			case CT_SEN_NAP_100AM:
				switch( pInf->GasNo ){
					case CB_GAS_NAP_100AM_C2H6_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C3H6_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C3H6O3_100_LEL:			Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C3H7OH_100_LEL:			Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C3H8_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C5H10_100_LEL:			Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C5H12_100_LEL:			Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C7H8_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_CH4_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_COMB_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_H_C_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_H2_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_D2_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_C4H10_100_LEL:			Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_LNG_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_LPG_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_NAP_100AM_PGMEA_100_LEL:			Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
				}
				break;
				
			case CT_SEN_GS_100A:
				switch( pInf->GasNo ){
					case CB_GAS_GS_100A_C2H2_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_GS_100A_C2H4_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_GS_100A_C3H7OH_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_GS_100A_C5H10_100_LEL:				Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
					case CB_GAS_GS_100A_H2_100_LEL:					Lmt.Lo =      0.5f; Lmt.Hi =       5.8f; break;
				}
				break;
				
			case CT_SEN_GS_100SC:
				switch( pInf->GasNo ){
					case SC_GAS_GS_100SC_ORG_1000_PPM:				Lmt.Lo =     0.05f; Lmt.Hi =       0.6f; break;
				}
				break;
		}
	}
	
	return Lmt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Task( Func_HandleType *pFunc )
{
	U8					i;
	FactoryCal_Limit	CalLmt;
	float				CalRsp;
	Measuring_GasInfo	*pInf;
	
	if( FaSet.Cal.Tick & 0x8000 ){
		if( FaSet.Cal.Tick == 0x8000 ){
			FaSet.Cal.Tick = 0;
			
			switch( pFunc->Level ){
				case FUNC_LEVEL_4: // Zero
					switch( FaSet.Cal.Status ){
						case FACTORY_CAL_STATUS_INJECT_GAS:
							if( FaSet.Cal.InjectionTmr < CAL_INJECTION_TIMEOUT ){
								FaSet.Cal.InjectionTmr++;
								FaSet.Cal.Tick = 0x8000 + 24;
								if( FaSet.Cal.InjectionTmr == CAL_INJECTION_TIMEOUT ){
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										if( MeasHandle.Gas[i].IsPresent == true ){
											if( FaSet.Cal.Select[i] == true ){
												FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_TIMEOUT;
											}
										}
									}
									FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED;
								}
							}
							break;
							
						case FACTORY_CAL_STATUS_DECISION:
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( MeasHandle.Gas[i].IsPresent == true ){
									if( FaSet.Cal.Select[i] == true ){
										FactoryCal_Zero_Update( i );
										FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
									}
								}
							}
							FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED;
							break;
					}
					break;
					
				case FUNC_LEVEL_5:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 1: // Cavity
									switch( FaSet.Cal.Status ){
										case FACTORY_CAL_STATUS_PACKET_SEND_GAIN:
											FaSet.Cal.Tick = 0x8004;
											FaSet.Cal.Status = FACTORY_CAL_STATUS_PROGRESS_GAIN;
											break;
											
										case FACTORY_CAL_STATUS_PROGRESS_GAIN:
											if( FaSet.pMeas->pSen->pCvt->AmpCalRunFlag ){
												FaSet.Cal.Tick = 0x8004;
											}
											else {
												for( i=0; i<CAVITY_SEN_CH_MAX; i++ ){
													FaSet.pMeas->pCur->nDet[1+i] = FaSet.pMeas->Raw.nDet[1+i];
												}
												FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED_GAIN;
											}
											break;
									}
									break;
							}
							break;
							
						case 2: // Span
							switch( FaSet.Cal.Status ){
								case FACTORY_CAL_STATUS_INJECT_GAS:
									if( FaSet.Cal.InjectionTmr < CAL_INJECTION_TIMEOUT ){
										FaSet.Cal.InjectionTmr++;
										FaSet.Cal.Tick = 0x8000 + 24;
										if( FaSet.Cal.InjectionTmr == CAL_INJECTION_TIMEOUT ){
											FaSet.Cal.Result[pFunc->Item.Lv4] = FACTORY_CAL_RESULT_TIMEOUT;
											FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED;
										}
									}
									break;
									
								case FACTORY_CAL_STATUS_DECISION:
									i = pFunc->Item.Lv4;
									pInf = &FaSet.pMeas->Gas[i].Inf;
									CalLmt = FactoryCal_Limit_Load( pInf );
									CalRsp = Measuring_Gas_Response_Calculation( i, FaSet.Cal.SpanGas, FaSet.Cal.Volt[i] );
									/* Negative Sensors */
									if( ((pInf->Method == MEASURING_METHOD_CT) && (pInf->SenNo == CT_SEN_EC_DC_CL2_3)) ||
										((pInf->Method == MEASURING_METHOD_CT) && (pInf->SenNo == CT_SEN_EC_DC_HF_9	)) ||
										((pInf->Method == MEASURING_METHOD_CT) && (pInf->SenNo == CT_SEN_EC_DC_O3_1	)) ||
										((pInf->Method == MEASURING_METHOD_CT) && (pInf->SenNo == CT_SEN_ETO_C_20	)) ||
										((pInf->Method == MEASURING_METHOD_CT) && (pInf->SenNo == CT_SEN_NO2_C_20	)) ||
										((pInf->Method == MEASURING_METHOD_CT) && (pInf->SenNo == CT_SEN_O2_MP_100	)) ||
										( pInf->Method == MEASURING_METHOD_IR) )
									{
										if( CalRsp > CalLmt.Lo ){
											FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_LO_RESPONSE;
										}
										else if( CalRsp < CalLmt.Hi ){
											FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_HI_RESPONSE;
										}
										else {
											if( FaSet.Cal.Volt[i] > FaSet.pMeas->Gas[i].Inf.mZero.f ){
												FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_OUTPUT_REVERSE;
											}
											else {
												FactoryCal_Span_Update( i );
												FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
											}
										}
									}
									/* Positive Sensors */
									else {
										if( CalRsp < CalLmt.Lo ){
											FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_LO_RESPONSE;
										}
										else if( CalRsp > CalLmt.Hi ){
											FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_HI_RESPONSE;
										}
										else {
											if( FaSet.Cal.Volt[i] < FaSet.pMeas->Gas[i].Inf.mZero.f ){
												FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_OUTPUT_REVERSE;
											}
											else {
												FactoryCal_Span_Update( i );
												FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
											}
										}
									}
									FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED;
									break;
							}
							break;
							
						case 3: // Base Zero
							switch( FaSet.Cal.Status ){
								case FACTORY_CAL_STATUS_INJECT_GAS:
									if( FaSet.Cal.InjectionTmr < CAL_INJECTION_TIMEOUT ){
										FaSet.Cal.InjectionTmr++;
										FaSet.Cal.Tick = 0x8000 + 24;
										if( FaSet.Cal.InjectionTmr == CAL_INJECTION_TIMEOUT ){
											for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
												if( MeasHandle.Gas[i].IsPresent == true ){
													if( FaSet.Cal.Select[i] == true ){
														FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_TIMEOUT;
													}
												}
											}
											FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED;
										}
									}
									break;
									
								case FACTORY_CAL_STATUS_DECISION:
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										if( MeasHandle.Gas[i].IsPresent == true ){
											if( FaSet.Cal.Select[i] == true ){
												FactoryCal_Base_Cal_Update( i );
												FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
											}
										}
									}
									FaSet.Cal.Status = FACTORY_CAL_STATUS_FINISHED;
									break;
							}
							break;
					}
					break;
			}
		}
		else {
			FaSet.Cal.Tick--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Cartridge_Rgain_Update( U8 i )
{
	SenEEP_Write_CHn( FaSet.Cal.TIA[i].Word, i, SENSOR_EEP_CHn_PTM_mOFFS_TIA_0, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Cartridge_Rgain_Return( U8 i )
{
	if( FaSet.pMeas->Gas[i].Inf.TIA.Word[0] != FaSet.Cal.TIA[i].Word[0] ){
		SenEEP_Write_CHn( FaSet.pMeas->Gas[i].Inf.TIA.Word, i, SENSOR_EEP_CHn_PTM_mOFFS_TIA_0, 1 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Target_Set_Update( U8 i )
{
	U16		eep[3];
	S16		vZero;
	S16		vBias;
	
	if( (FaSet.pMeas->Gas[i].Inf.TargetOffs != FaSet.Cal.TargetOffs[i]) ||
		(FaSet.pMeas->Gas[i].Inf.TargetGain != FaSet.Cal.TargetGain[i]) )
	{
		if( i < CARTRIDGE_SEN_CH_MAX ){
			vZero = FaSet.Cal.TargetOffs[i];
			vBias = Measuring_CT_Sensor_Bias_Load( &FaSet.pMeas->Gas[i].Inf );
		
			eep[0] = FaSet.pMeas->Gas[i].Inf.TargetOffs = FaSet.Cal.TargetOffs[i];
			eep[1] = FaSet.pMeas->Gas[i].Inf.TargetGain = FaSet.Cal.TargetGain[i];
			eep[2] = FaSet.pMeas->Gas[i].Inf.TargetBias = (U16)(vZero - vBias);
		
			SenEEP_Write_CHn( eep, i, SENSOR_EEP_CHn_AMP_CAL_TARGET_mV_OFFS, 3 );
		}
		else {
			eep[0] = FaSet.pMeas->Gas[i].Inf.TargetOffs = FaSet.Cal.TargetOffs[i];
			eep[1] = FaSet.pMeas->Gas[i].Inf.TargetGain = FaSet.Cal.TargetGain[i];
		
			SenEEP_Write_CHn( eep, i, SENSOR_EEP_CHn_AMP_CAL_TARGET_mV_OFFS, 2 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Zero_Update( U8 i )
{
	Measuring_GasInfo	*pInf = &FaSet.pMeas->Gas[i].Inf;
	U16					wBuf[8];
		
	if( FaSet.pMeas->bZero.WarmUpEnd[i] == false ){
		FaSet.pMeas->bZero.WarmUpEnd[i] = true;
		FaSet.pMeas->bZero.WarmUpTmr[i] = 0;
	}
	
	pInf->mZero.f = FaSet.Cal.Volt[i];
	pInf->bZero.f = FaSet.Cal.Volt[i];
	pInf->mZeroTemp.f = FaSet.Cal.Temp[i];
	pInf->bZeroTemp.f = FaSet.Cal.Temp[i];
	wBuf[0] = pInf->mZero.w.l;
	wBuf[1] = pInf->mZero.w.h;
	wBuf[2] = pInf->mZeroTemp.w.l;
	wBuf[3] = pInf->mZeroTemp.w.h;
	
	switch( pInf->Func ){
		case MEASURING_GAS_FUNC_CT_INT:
		case MEASURING_GAS_FUNC_IR_INT:
			pInf->mSpan.f = pInf->mZero.f * FaSet.pMeas->Conf.iSpanScale[i];
			pInf->bSpan.f = pInf->bZero.f * FaSet.pMeas->Conf.iSpanScale[i];
			pInf->mSpanTemp.f = pInf->mZeroTemp.f;
			pInf->bSpanTemp.f = pInf->bZeroTemp.f;
			wBuf[4] = pInf->mSpan.w.l;
			wBuf[5] = pInf->mSpan.w.h;
			wBuf[6] = pInf->mSpanTemp.w.l;
			wBuf[7] = pInf->mSpanTemp.w.h;
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mZERO_L, 8 );
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 8 );
			break;
			
		case MEASURING_GAS_FUNC_IR_OUT_WITH_REF:
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mZERO_L, 4 );
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 4 );
			i = FaSet.pMeas->Conf.iRefCvt;
			pInf = &FaSet.pMeas->Gas[i].Inf;
			pInf->mZero.f = FaSet.Cal.Volt[i];
			pInf->bZero.f = FaSet.Cal.Volt[i];
			pInf->mZeroTemp.f = FaSet.Cal.Temp[i];
			pInf->bZeroTemp.f = FaSet.Cal.Temp[i];
			wBuf[0] = pInf->mZero.w.l;
			wBuf[1] = pInf->mZero.w.h;
			wBuf[2] = pInf->mZeroTemp.w.l;
			wBuf[3] = pInf->mZeroTemp.w.h;
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mZERO_L, 4 );
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 4 );
			break;
			
		default:
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mZERO_L, 4 );
			SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 4 );
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Span_Update( U8 i )
{
	Measuring_GasInfo	*pInf = &FaSet.pMeas->Gas[i].Inf;
	U16					wBuf[9];
	
	pInf->mSpan.f = FaSet.Cal.Volt[i];
	pInf->bSpan.f = FaSet.Cal.Volt[i];
	pInf->mSpanTemp.f = FaSet.Cal.Temp[i];
	pInf->bSpanTemp.f = FaSet.Cal.Temp[i];
	pInf->SpanGas = FaSet.Cal.SpanGas;
	wBuf[0] = pInf->mSpan.w.l;
	wBuf[1] = pInf->mSpan.w.h;
	wBuf[2] = pInf->mSpanTemp.w.l;
	wBuf[3] = pInf->mSpanTemp.w.h;
	wBuf[4] = pInf->SpanGas;
	SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mSPAN_L, 5 );
	SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bSPAN_L, 4 );
	
	if( pInf->Func == MEASURING_GAS_FUNC_CT_OUT_WITH_REF ){
		i = FaSet.pMeas->Conf.iRefCtg;
		pInf = &FaSet.pMeas->Gas[i].Inf;
		pInf->mSpan.f = FaSet.Cal.Volt[i];
		pInf->bSpan.f = FaSet.Cal.Volt[i];
		pInf->mSpanTemp.f = FaSet.Cal.Temp[i];
		pInf->bSpanTemp.f = FaSet.Cal.Temp[i];
		wBuf[0] = pInf->mSpan.w.l;
		wBuf[1] = pInf->mSpan.w.h;
		wBuf[2] = pInf->mSpanTemp.w.l;
		wBuf[3] = pInf->mSpanTemp.w.h;
		SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mSPAN_L, 4 );
		SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bSPAN_L, 4 );
	}
	else if( pInf->Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF ){
		i = FaSet.pMeas->Conf.iRefCvt;
		pInf = &FaSet.pMeas->Gas[i].Inf;
		pInf->mSpan.f = FaSet.Cal.Volt[i];
		pInf->bSpan.f = FaSet.Cal.Volt[i];
		pInf->mSpanTemp.f = FaSet.Cal.Temp[i];
		pInf->bSpanTemp.f = FaSet.Cal.Temp[i];
		wBuf[0] = pInf->mSpan.w.l;
		wBuf[1] = pInf->mSpan.w.h;
		wBuf[2] = pInf->mSpanTemp.w.l;
		wBuf[3] = pInf->mSpanTemp.w.h;
		SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_mSPAN_L, 4 );
		SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bSPAN_L, 4 );
	} 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Base_Cal_Update( U8 i )
{
	Measuring_GasInfo	*pInf = &FaSet.pMeas->Gas[i].Inf;
	U16					wBuf[8];
			
	if( FaSet.pMeas->bZero.WarmUpEnd[i] == false ){
		FaSet.pMeas->bZero.WarmUpEnd[i] = true;
		FaSet.pMeas->bZero.WarmUpTmr[i] = 0;
	}
	
	pInf->bZero.f = FaSet.Cal.Volt[i];
	pInf->bSpan.f = FaSet.Cal.Volt[i] * (pInf->mSpan.f / pInf->mZero.f);
	pInf->bZeroTemp.f = FaSet.Cal.Temp[i];
	pInf->bSpanTemp.f = FaSet.Cal.Temp[i];
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
		i = MeasHandle.Conf.iRefCvt;
		pInf = &MeasHandle.Gas[i].Inf;
		pInf->bZero.f = FaSet.Cal.Volt[i];
		pInf->bSpan.f = FaSet.Cal.Volt[i] * (pInf->mSpan.f / pInf->mZero.f);
		pInf->bZeroTemp.f = FaSet.Cal.Temp[i];
		pInf->bSpanTemp.f = FaSet.Cal.Temp[i];
		wBuf[0] = pInf->bZero.w.l;
		wBuf[1] = pInf->bZero.w.h;
		wBuf[2] = pInf->bZeroTemp.w.l;
		wBuf[3] = pInf->bZeroTemp.w.h;
		wBuf[4] = pInf->bSpan.w.l;
		wBuf[5] = pInf->bSpan.w.h;
		wBuf[6] = pInf->bSpanTemp.w.l;
		wBuf[7] = pInf->bSpanTemp.w.h;
		SenEEP_Write_CHn( wBuf, i, SENSOR_EEP_CHn_bZERO_L, 8 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FactoryCal_Base_Set_Update( U8 i )
{
	U16			eep[3];
	
	if( (FaSet.pMeas->Gas[i].Inf.bZeroUse != FaSet.Cal.bZeroUse[i]) ||
		(FaSet.pMeas->Gas[i].Inf.bZeroPer != FaSet.Cal.bZeroPer[i]) ||
		(FaSet.pMeas->Gas[i].Inf.bZeroSec != FaSet.Cal.bZeroSec[i]) )
	{
		eep[0] = FaSet.pMeas->Gas[i].Inf.bZeroUse = FaSet.Cal.bZeroUse[i];
		eep[1] = FaSet.pMeas->Gas[i].Inf.bZeroPer = FaSet.Cal.bZeroPer[i];
		eep[2] = FaSet.pMeas->Gas[i].Inf.bZeroSec = FaSet.Cal.bZeroSec[i];
		
		SenEEP_Write_CHn( eep, i, SENSOR_EEP_CHn_bZERO_USE, 3 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Factory_Sensor_Set_Init( U8 i )
{
	U16						*pDef;
	U16						wBuf[33];
	Measuring_GasType		*pGas = &FaSet.pMeas->Gas[i];
	Measuring_AlarmType		*pAlr = &FaSet.pMeas->Alr[i];
	U16						uA;
	
	Measuring_Default_Gas_Info_Load( &pGas->Inf, &pAlr->Inf );
	
	if( i < CARTRIDGE_SEN_CH_MAX ){
		pDef = (U16 *)SENSOR_EEP_CT_CHn_DEFAULT;
	}
	else {
		pDef = (U16 *)SENSOR_EEP_IR_CHn_DEFAULT;
	}
	
	
	wBuf[ 0] = pGas->Inf.TIA.Word[0];
	wBuf[ 1] = pGas->Inf.TIA.Word[1];
	wBuf[ 2] = pGas->Inf.TargetOffs;
	wBuf[ 3] = pGas->Inf.TargetGain;
	wBuf[ 4] = pGas->Inf.TargetBias;
	
	wBuf[ 5] = pGas->Inf.SpanGas;
	
	wBuf[ 6] = pGas->Inf.bZeroUse;
	wBuf[ 7] = pGas->Inf.bZeroPer;
	wBuf[ 8] = pGas->Inf.bZeroSec;
	wBuf[ 9] = pGas->Inf.bZeroCnt				= pDef[SENSOR_EEP_CHn_bZERO_COUNT];
	wBuf[10] = pGas->Inf.Method;
	wBuf[11] = pGas->Inf.SenNo;
	wBuf[12] = pGas->Inf.GasNo;
	wBuf[13] = pGas->Inf.Dp;
	wBuf[14] = pGas->Inf.HighScale;
	wBuf[15] = pGas->Inf.Unit;
	wBuf[16] = pGas->Inf.MaintenanceGas;
	wBuf[17] = pGas->Inf.CrossScale;
	wBuf[18] = pGas->Inf.ZeroSkipPer;
	wBuf[19] = pGas->Inf.SpanSkipPer;
	wBuf[20] = pGas->Inf.OutDelayPer			= pDef[SENSOR_EEP_CHn_OUT_DELAY_PERCENTAGE_x10];
	wBuf[21] = pGas->Inf.OutDelaySec			= pDef[SENSOR_EEP_CHn_OUT_DELAY_SEC];
	wBuf[22] = pAlr->Inf.IsUnderCheckEnabled	= pDef[SENSOR_EEP_CHn_ALARM_UNDER_CHECK];
	wBuf[23] = pAlr->Inf._1stLatchEnabled		= pDef[SENSOR_EEP_CHn_ALARM_1ST_LATCH];
	wBuf[24] = pAlr->Inf._1stTypeIncrease;
	wBuf[25] = pAlr->Inf._1stLevel;
	wBuf[26] = pAlr->Inf._1stDead				= pDef[SENSOR_EEP_CHn_ALARM_1ST_DEAD_BAND];
	wBuf[27] = pAlr->Inf._1stDelaySec			= pDef[SENSOR_EEP_CHn_ALARM_1ST_DELAY_SEC];
	wBuf[28] = pAlr->Inf._2ndLatchEnabled		= pDef[SENSOR_EEP_CHn_ALARM_2ND_LATCH];
	wBuf[29] = pAlr->Inf._2ndTypeIncrease;
	wBuf[30] = pAlr->Inf._2ndLevel;
	wBuf[31] = pAlr->Inf._2ndDead				= pDef[SENSOR_EEP_CHn_ALARM_2ND_DEAD_BAND];
	wBuf[32] = pAlr->Inf._2ndDelaySec			= pDef[SENSOR_EEP_CHn_ALARM_2ND_DELAY_SEC];
	
	pGas->Inf.bZeroTmr		= pGas->Inf.bZeroSec	* 10;
	pGas->Inf.OutDelayTmr	= pGas->Inf.OutDelaySec	* 10;
	
	Measuring_Gas_MSD_Load( &pGas->Inf );
	
	FaSet.pMeas->Gas[i].Per = (float)pGas->Inf.MaintenanceGas / (float)pGas->Inf.HighScale * 100.f;
	
	SenEEP_Write_CHn( &wBuf[0], i, SENSOR_EEP_CHn_PTM_mOFFS_TIA_0, 5 );
	SenEEP_Write_CHn( &wBuf[5], i, SENSOR_EEP_CHn_SPAN_GAS, 1 );
	SenEEP_Write_CHn( &wBuf[6], i, SENSOR_EEP_CHn_bZERO_USE, 27 );
	
	if( (pGas->Inf.Func == MEASURING_GAS_FUNC_CT_OUT) ||
		(pGas->Inf.Func == MEASURING_GAS_FUNC_CT_OUT_WITH_REF) ||
		(pGas->Inf.Func == MEASURING_GAS_FUNC_IR_OUT) ||
		(pGas->Inf.Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF) )
	{
		uA = 16000 / pGas->Inf.HighScale * pGas->Inf.MaintenanceGas + 4000;
	
		if( i == MeasHandle.Conf.iOut[0] ){
			EEPROM_Update( &uA, SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1, 1 );
		}
		else if( (i == MeasHandle.Conf.iOut[1]) && (MeasHandle.Conf.OutMax > 1) ){
			EEPROM_Update( &uA, SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2, 1 );
		}
	}
	
	Measuring_Gas_Name_Update( i );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Factory_Sensor_Cal_Init( U8 i )
{
	Measuring_GasInfo		*pInf = &FaSet.pMeas->Gas[i].Inf;
	U16						*pDef;
	
	if( i < CARTRIDGE_SEN_CH_MAX ){
		pDef = (U16 *)SENSOR_EEP_CT_CHn_DEFAULT;
	}
	else {
		pDef = (U16 *)SENSOR_EEP_IR_CHn_DEFAULT;
	}
	
	if( pInf->Func == MEASURING_GAS_FUNC_IR_REF ){
		pInf->mZero.w.l		= pDef[SENSOR_EEP_CHn_mZERO_L];
		pInf->mZero.w.h		= pDef[SENSOR_EEP_CHn_mZERO_H];
		pInf->mZeroTemp.w.l	= pDef[SENSOR_EEP_CHn_mZERO_TEMP_L];
		pInf->mZeroTemp.w.h	= pDef[SENSOR_EEP_CHn_mZERO_TEMP_H];
		pInf->mSpan.w.l		= pDef[SENSOR_EEP_CHn_mZERO_L];
		pInf->mSpan.w.h		= pDef[SENSOR_EEP_CHn_mZERO_H];
		pInf->mSpanTemp.w.l	= pDef[SENSOR_EEP_CHn_mZERO_TEMP_L];
		pInf->mSpanTemp.w.h	= pDef[SENSOR_EEP_CHn_mZERO_TEMP_H];
		pInf->bZero.w.l		= pDef[SENSOR_EEP_CHn_bZERO_L];
		pInf->bZero.w.h		= pDef[SENSOR_EEP_CHn_bZERO_H];
		pInf->bZeroTemp.w.l	= pDef[SENSOR_EEP_CHn_bZERO_TEMP_L];
		pInf->bZeroTemp.w.h	= pDef[SENSOR_EEP_CHn_bZERO_TEMP_H];
		pInf->bSpan.w.l		= pDef[SENSOR_EEP_CHn_bZERO_L];
		pInf->bSpan.w.h		= pDef[SENSOR_EEP_CHn_bZERO_H];
		pInf->bSpanTemp.w.l	= pDef[SENSOR_EEP_CHn_bZERO_TEMP_L];
		pInf->bSpanTemp.w.h	= pDef[SENSOR_EEP_CHn_bZERO_TEMP_H];
		SenEEP_Write_CHn( pDef+SENSOR_EEP_CHn_mZERO_L, i, SENSOR_EEP_CHn_mZERO_L, 4 );
		SenEEP_Write_CHn( pDef+SENSOR_EEP_CHn_mZERO_L, i, SENSOR_EEP_CHn_mSPAN_L, 4 );
		SenEEP_Write_CHn( pDef+SENSOR_EEP_CHn_bZERO_L, i, SENSOR_EEP_CHn_bZERO_L, 4 );
		SenEEP_Write_CHn( pDef+SENSOR_EEP_CHn_bZERO_L, i, SENSOR_EEP_CHn_bSPAN_L, 4 );
	}
	else {
		pInf->mZero.w.l		= pDef[SENSOR_EEP_CHn_mZERO_L];
		pInf->mZero.w.h		= pDef[SENSOR_EEP_CHn_mZERO_H];
		pInf->mZeroTemp.w.l	= pDef[SENSOR_EEP_CHn_mZERO_TEMP_L];
		pInf->mZeroTemp.w.h	= pDef[SENSOR_EEP_CHn_mZERO_TEMP_H];
		pInf->mSpan.w.l		= pDef[SENSOR_EEP_CHn_mSPAN_L];
		pInf->mSpan.w.h		= pDef[SENSOR_EEP_CHn_mSPAN_H];
		pInf->mSpanTemp.w.l	= pDef[SENSOR_EEP_CHn_mSPAN_TEMP_L];
		pInf->mSpanTemp.w.h	= pDef[SENSOR_EEP_CHn_mSPAN_TEMP_H];
		pInf->bZero.w.l		= pDef[SENSOR_EEP_CHn_bZERO_L];
		pInf->bZero.w.h		= pDef[SENSOR_EEP_CHn_bZERO_H];
		pInf->bZeroTemp.w.l	= pDef[SENSOR_EEP_CHn_bZERO_TEMP_L];
		pInf->bZeroTemp.w.h	= pDef[SENSOR_EEP_CHn_bZERO_TEMP_H];
		pInf->bSpan.w.l		= pDef[SENSOR_EEP_CHn_bSPAN_L];
		pInf->bSpan.w.h		= pDef[SENSOR_EEP_CHn_bSPAN_H];
		pInf->bSpanTemp.w.l	= pDef[SENSOR_EEP_CHn_bSPAN_TEMP_L];
		pInf->bSpanTemp.w.h	= pDef[SENSOR_EEP_CHn_bSPAN_TEMP_H];
		SenEEP_Write_CHn( pDef+SENSOR_EEP_CHn_mZERO_L, i, SENSOR_EEP_CHn_mZERO_L, 8 );
		SenEEP_Write_CHn( pDef+SENSOR_EEP_CHn_bZERO_L, i, SENSOR_EEP_CHn_bZERO_L, 8 );
	}
	
	FaSet.pMeas->Gas[i].Per = (float)pInf->MaintenanceGas / (float)pInf->HighScale * 100.f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Factory_Init( void )
{
	FaSet.Init.Err.Word = 0;
	
	if( FaSet.Init.Set.Bit.mA_1 == true ){
		EEPROM_Update( EepHandle.pDefault+SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH1, SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH1, 3 );
		mA_Output_ReInit_Ch1();
	}
	
	if( FaSet.Init.Set.Bit.mA_2 == true ){
		EEPROM_Update( EepHandle.pDefault+SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH2, SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH2, 3 );
		mA_Output_ReInit_Ch2();
	}
	
	if( FaSet.Init.Set.Bit.Relay == true ){
		EEPROM_Update( EepHandle.pDefault+SYSTEM_EEP_RELAY_OPTION_TRB, SYSTEM_EEP_RELAY_OPTION_TRB, 6 );
		Relay_ReInit();
	}
	
	if( FaSet.Init.Set.Bit.Flow == true ){
		EEPROM_Update( EepHandle.pDefault+SYSTEM_EEP_FLOW_CTRL_P_CALCULATION_SCALE, SYSTEM_EEP_FLOW_CTRL_P_CALCULATION_SCALE, 12 );
		Flow_Control_ReInit();
	}
	
	if( FaSet.Init.Set.Bit.S1_Set == true ){ Factory_Sensor_Set_Init( 0 ); }
	if( FaSet.Init.Set.Bit.S2_Set == true ){ Factory_Sensor_Set_Init( 1 ); }
	if( FaSet.Init.Set.Bit.S3_Set == true ){ Factory_Sensor_Set_Init( 2 ); }
	if( FaSet.Init.Set.Bit.S4_Set == true ){ Factory_Sensor_Set_Init( 3 ); }
	if( FaSet.Init.Set.Bit.S5_Set == true ){ Factory_Sensor_Set_Init( 4 ); }
	if( FaSet.Init.Set.Bit.S6_Set == true ){ Factory_Sensor_Set_Init( 5 ); }
	
	if( FaSet.Init.Set.Bit.S1_Cal == true ){ Factory_Sensor_Cal_Init( 0 ); }
	if( FaSet.Init.Set.Bit.S2_Cal == true ){ Factory_Sensor_Cal_Init( 1 ); }
	if( FaSet.Init.Set.Bit.S3_Cal == true ){ Factory_Sensor_Cal_Init( 2 ); }
	if( FaSet.Init.Set.Bit.S4_Cal == true ){ Factory_Sensor_Cal_Init( 3 ); }
	if( FaSet.Init.Set.Bit.S5_Cal == true ){ Factory_Sensor_Cal_Init( 4 ); }
	if( FaSet.Init.Set.Bit.S6_Cal == true ){ Factory_Sensor_Cal_Init( 5 ); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess( Func_HandleType *pFunc )
{
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
			FaSet.pMeas = &MeasHandle;
			break;
			
		case FUNC_LEVEL_2:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv2 ){
						case 1: // Calibration
							pFunc->State.Bit.Calibration = true;
							break;
							
						case 3: // System Info
							iADC_BatteryScan( true );
							break;
					}
					pFunc->Level = FUNC_LEVEL_3;
					pFunc->IsApproved = false;
					pFunc->Item.Lv3 = 0;
					pFunc->Item.Lv4 = 0;
					break;
					
				case Up:
					if( pFunc->Item.Lv2 ){
						pFunc->Item.Lv2--;
					}
					break;
					
				case Down:
					if( pFunc->Item.Lv2 < 3 ){
						pFunc->Item.Lv2++;
					}
					break;
					
				case Reset:
					MoveFuncTop();
					break;
			}
			break;
			
		default:
			switch( pFunc->Item.Lv2 ){
				case 0: KeyProcess_Measuring( pFunc ); break;
				case 1: KeyProcess_Cal		( pFunc ); break;
				case 2: KeyProcess_SysInit	( pFunc ); break;
				case 3: KeyProcess_SysInfo	( pFunc ); break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess_Measuring( Func_HandleType *pFunc )
{
	U8		i, sMin, sMax;
	U8		is;
	U8		ig;
	U16		Set[4] = { 1000, 100, 10, 1 };
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							if( FaSet.pMeas->pSen->pCtg->IsPresent == true ){
								pFunc->Item.Lv4 = 0;
							}
							else {
								pFunc->Item.Lv4 = 1;
							}
							break;
							
						case 1: // Option
							for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
								FaSet.sGas[i] = FaSet.pMeas->pGas[i]->Inf;
								FaSet.sAlr[i] = FaSet.pMeas->pAlr[i]->Inf;
							}
							pFunc->Item.Lv4 = 0;
							break;
					
						case 2: // Filter
							FaSet.NotStableDecTime = FaSet.pMeas->Filter.NotStableDecTime;
							pFunc->Item.Lv4 = 0;
							break;
							
						case 3: // Maintenance
							for( i=0; i<MEASURING_GAS_OUTPUT_MAX; i++ ){
								FaSet.sGas[i] = FaSet.pMeas->pGas[i]->Inf;
							}
							FaSet.sMode = FaSet.pMeas->Mode;
							pFunc->Item.Lv4 = FaSet.sGas[0].Msd;
							break;
							
						case 4: // View Status
							KeyScan.Tick[KEY_INDEX_UP  ].IsRepeatMode = false;
							KeyScan.Tick[KEY_INDEX_DOWN].IsRepeatMode = false;
							FaSet.vScroll = 0;
							pFunc->Item.Lv4 = 0;
							break;
					}
					pFunc->Level = FUNC_LEVEL_4;
					pFunc->IsApproved = false;
					break;
					
				case Up:
					if( pFunc->Item.Lv3 ){
						pFunc->Item.Lv3--;
					}
					break;
					
				case Down:
					if( pFunc->Item.Lv3 < 4 ){
						pFunc->Item.Lv3++;
					}
					break;
					
				case Reset:
					pFunc->Level = FUNC_LEVEL_2;
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										FaSet.sGas[i] = FaSet.pMeas->Gas[i].Inf;
										FaSet.sAlr[i] = FaSet.pMeas->Alr[i].Inf;
									}
									FaSet.sCtgSenType[0] = FaSet.pMeas->pSen->pCtg->Eep[0][SENSOR_EEP_SYS_CTG_SEN_TYPE_0];
									FaSet.sCtgSenType[1] = FaSet.pMeas->pSen->pCtg->Eep[0][SENSOR_EEP_SYS_CTG_SEN_TYPE_1];
									pFunc->Item.Lv5 = 0;
									break;
									
								case 1: // System
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										FaSet.sGas[i] = FaSet.pMeas->Gas[i].Inf;
										FaSet.sAlr[i] = FaSet.pMeas->Alr[i].Inf;
									}
									FaSet.sConf = FaSet.pMeas->Conf;
									Measuring_Config_Load( NULL, CONF_ITEM_NONE );
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING:
										case MEASURING_SEN_COMB_CTG_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
											pFunc->Item.Lv5 = 0;
											break;
											
										case MEASURING_SEN_COMB_CVT_DUAL:
										case MEASURING_SEN_COMB_CVT_QUAD:
											pFunc->Item.Lv5 = 2;
											break;
									}
									break;
							}
							pFunc->IsApproved = false;
							pFunc->Level = FUNC_LEVEL_5;
							break;
							
						case 1: // Option
							if( pFunc->Item.Lv4 < 16 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv4 = 16;
								}
								else {
									if( FaSet.pMeas->Conf.OutMax > 1 ){
										pFunc->Item.Lv4++;
									}
									else {
										if( pFunc->Item.Lv4 < 7 ){
											pFunc->Item.Lv4++;
										}
										else {
											pFunc->Item.Lv4 = 16;
										}
									}
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Measuring_Option_Update();
								}
								pFunc->Level = FUNC_LEVEL_3;
							}
							break;
					
						case 2: // Filter
							if( pFunc->Item.Lv4 < 1 ){
								pFunc->Item.Lv4++;
							}
							else {
								if( pFunc->IsApproved == true ){
									Measuring_Filter_Update();
								}
								pFunc->Level = FUNC_LEVEL_3;
							}
							break;
							
						case 3: // Maintenance
							if( pFunc->Item.Lv4 < 9 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv4 = 9;
								}
								else {
									if( pFunc->Item.Lv4 == 3 ){
										if( FaSet.pMeas->Conf.OutMax < 2 ){
											pFunc->Item.Lv4 = 8;
										}
										else {
											pFunc->Item.Lv4 = 4 + FaSet.sGas[1].Msd;
										}
									}
									else {
										pFunc->Item.Lv4++;
									}
								}
							}
							else {
								if( pFunc->IsApproved == true ){
									Maintenance_Data_Update();
								}
								pFunc->Level = FUNC_LEVEL_3;
							}
							break;
							
						case 4: // View Status
							switch( pFunc->Item.Lv4 ){
								case 0: // Measuring Status
									pFunc->pMsgBox->ClearMsg = true;
									pFunc->Item.Lv4 = 1;
									break;
									
								case 1: // Peak Monitor
									pFunc->pMsgBox->ClearMsg = true;
									pFunc->Item.Lv4 = 2;
									break;
									
								case 2: // Filtering Status
									GDI_Clear( RGB.BLACK );
									FaSet.iWave = 0;
									FaSet.pMeas->pSen->pCtg->iWaveSet = 0;
									FaSet.pMeas->pSen->pCvt->iWaveSet = 0;
									FaSet.pMeas->pSen->CtgWaveModeSet = true;
									FaSet.pMeas->pSen->CvtWaveModeSet = true;
									pFunc->Item.Lv4 = 3;
									break;
									
								case 3: // Scope
									GDI_Clear( RGB.BLACK );
									FaSet.pMeas->pSen->CtgWaveModeSet = false;
									FaSet.pMeas->pSen->CvtWaveModeSet = false;
									pFunc->pMsgBox->ClearTitle = true;
									pFunc->Item.Lv4 = 4;
									break;
									
								case 4: // System Error
									pFunc->Item.Lv4 = 5;
									break;
									
								case 5: // Peripheral Error
									pFunc->pMsgBox->ClearMsg = true;
									FaSet.vScroll = 0;
									pFunc->Item.Lv4 = 0;
									break;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							if( FaSet.pMeas->pSen->pCtg->IsPresent == true ){
								pFunc->Item.Lv4 = 0;
							}
							break;
							
						case 1: // Option
							switch( pFunc->Item.Lv4 ){
								case  0: Func_uInc( 300, 10, &FaSet.sGas[0].ZeroSkipPer ); break;
								case  1: Func_uInc( 300,  1, &FaSet.sGas[0].ZeroSkipPer ); break;
								case  2: Func_uInc( 300, 10, &FaSet.sGas[0].SpanSkipPer ); break;
								case  3: Func_uInc( 300,  1, &FaSet.sGas[0].SpanSkipPer ); break;
								case  4: Func_uInc( 500, 10, &FaSet.sGas[0].OutDelayPer ); break;
								case  5: Func_uInc( 500,  1, &FaSet.sGas[0].OutDelayPer ); break;
								case  6: Func_uInc(  60,  1, &FaSet.sGas[0].OutDelaySec ); break;
								case  7: FaSet.sAlr[0].IsUnderCheckEnabled ^= true; break;
								case  8: Func_uInc( 300, 10, &FaSet.sGas[1].ZeroSkipPer ); break;
								case  9: Func_uInc( 300,  1, &FaSet.sGas[1].ZeroSkipPer ); break;
								case 10: Func_uInc( 300, 10, &FaSet.sGas[1].SpanSkipPer ); break;
								case 11: Func_uInc( 300,  1, &FaSet.sGas[1].SpanSkipPer ); break;
								case 12: Func_uInc( 500, 10, &FaSet.sGas[1].OutDelayPer ); break;
								case 13: Func_uInc( 500,  1, &FaSet.sGas[1].OutDelayPer ); break;
								case 14: Func_uInc(  60,  1, &FaSet.sGas[1].OutDelaySec ); break;
								case 15: FaSet.sAlr[1].IsUnderCheckEnabled ^= true; break;
								case 16: pFunc->IsApproved ^= true;
							}
							break;
					
						case 2: // Filter
							switch( pFunc->Item.Lv4 ){
								case 0: Func_uInc( 60000, 600, &FaSet.NotStableDecTime ); break;
								case 1: pFunc->IsApproved ^= true;
							}
							break;
							
						case 3: // Maintenance
							switch( pFunc->Item.Lv4 ){
								case 0:
								case 1:
								case 2:
								case 3:
								case 4:
								case 5:
								case 6:
								case 7:
									ig = pFunc->Item.Lv4 / 4;
									is = pFunc->Item.Lv4 % 4;
									if( FaSet.sGas[ig].MaintenanceGas +  Set[is] < FaSet.sGas[ig].HighScale ){
										FaSet.sGas[ig].MaintenanceGas += Set[is];
									}
									else {
										FaSet.sGas[ig].MaintenanceGas = FaSet.sGas[ig].HighScale;
									}
									break;
									
								case 8:
									if( FaSet.sMode.Item.Maintenance_QuitMin < 240 ){
										FaSet.sMode.Item.Maintenance_QuitMin++;
									}
									break;
									
								case 9: pFunc->IsApproved ^= true;
							}
							break;
							
						case 4: // View Status
							switch( pFunc->Item.Lv4 ){
								case 0: // Measuring Status
									if( FaSet.vScroll ){
										FaSet.vScroll--;
									}
									break;
									
								case 1: // Peak Monitor
									if( pFunc->KeyType == Long ){
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											if( MeasHandle.Gas[i].IsPresent == true ){
												FaSet.pMeas->Gas[i].nPerMin = FaSet.pMeas->pCur->nPer[i];
												FaSet.pMeas->Gas[i].nPerMax = FaSet.pMeas->pCur->nPer[i];
												FaSet.pMeas->Gas[i].iPerMin = FaSet.pMeas->pCur->iPer[i];
												FaSet.pMeas->Gas[i].iPerMax = FaSet.pMeas->pCur->iPer[i];
											}
										}
									}
									break;
									
								case 2: // Filtering Status
									if( pFunc->KeyType == Long ){
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											FaSet.pMeas->Filter.NotStableTmrMax[i] = FaSet.pMeas->Filter.NotStableTmr[i];
										}
									}
									break;
									
								case 3: // Scope
									if( FaSet.iWave < (MEASURING_GAS_SENSOR_MAX - 1) ){
										FaSet.iWave++;
									}
									else {
										FaSet.iWave = 0;
									}
									if( FaSet.iWave < CARTRIDGE_SEN_CH_MAX ){
										FaSet.pMeas->pSen->pCtg->iWaveSet = FaSet.iWave;
									}
									else {
										FaSet.pMeas->pSen->pCvt->iWaveSet = FaSet.iWave - CARTRIDGE_SEN_CH_MAX;
									}
									break;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							pFunc->Item.Lv4 = 1;
							break;
							
						case 1: // Option
							switch( pFunc->Item.Lv4 ){
								case  0: Func_uDec( 0, 10, &FaSet.sGas[0].ZeroSkipPer ); break;
								case  1: Func_uDec( 0,  1, &FaSet.sGas[0].ZeroSkipPer ); break;
								case  2: Func_uDec( 0, 10, &FaSet.sGas[0].SpanSkipPer ); break;
								case  3: Func_uDec( 0,  1, &FaSet.sGas[0].SpanSkipPer ); break;
								case  4: Func_uDec( 0, 10, &FaSet.sGas[0].OutDelayPer ); break;
								case  5: Func_uDec( 0,  1, &FaSet.sGas[0].OutDelayPer ); break;
								case  6: Func_uDec( 0,  1, &FaSet.sGas[0].OutDelaySec ); break;
								case  7: FaSet.sAlr[0].IsUnderCheckEnabled ^= true; break;
								case  8: Func_uDec( 0, 10, &FaSet.sGas[1].ZeroSkipPer ); break;
								case  9: Func_uDec( 0,  1, &FaSet.sGas[1].ZeroSkipPer ); break;
								case 10: Func_uDec( 0, 10, &FaSet.sGas[1].SpanSkipPer ); break;
								case 11: Func_uDec( 0,  1, &FaSet.sGas[1].SpanSkipPer ); break;
								case 12: Func_uDec( 0, 10, &FaSet.sGas[1].OutDelayPer ); break;
								case 13: Func_uDec( 0,  1, &FaSet.sGas[1].OutDelayPer ); break;
								case 14: Func_uDec( 0,  1, &FaSet.sGas[1].OutDelaySec ); break;
								case 15: FaSet.sAlr[1].IsUnderCheckEnabled ^= true; break;
								case 16: pFunc->IsApproved ^= true;
							}
							break;
					
						case 2: // Filter
							switch( pFunc->Item.Lv4 ){
								case 0: Func_uDec( 9000, 600, &FaSet.NotStableDecTime ); break;
								case 1: pFunc->IsApproved ^= true;
							}
							break;
							
						case 3: // Maintenance
							switch( pFunc->Item.Lv4 ){
								case 0:
								case 1:
								case 2:
								case 3:
								case 4:
								case 5:
								case 6:
								case 7:
									ig = pFunc->Item.Lv4 / 4;
									is = pFunc->Item.Lv4 % 4;
									if( FaSet.sGas[ig].MaintenanceGas -  Set[is] > 0 ){
										FaSet.sGas[ig].MaintenanceGas -= Set[is];
									}
									else {
										FaSet.sGas[ig].MaintenanceGas = 0;
									}
									break;
									
								case 8:
									if( FaSet.sMode.Item.Maintenance_QuitMin ){
										FaSet.sMode.Item.Maintenance_QuitMin--;
									}
									break;
									
								case 9: pFunc->IsApproved ^= true;
							}
							break;
							
						case 4: // View Status
							switch( pFunc->Item.Lv4 ){
								case 0: // Measuring Status
									if( FaSet.vScroll < 3 ){
										FaSet.vScroll++;
									}
									break;
									
								case 1: // Peak Monitor
									if( pFunc->KeyType == Long ){
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											if( MeasHandle.Gas[i].IsPresent == true ){
												FaSet.pMeas->Gas[i].nPerMin = FaSet.pMeas->pCur->nPer[i];
												FaSet.pMeas->Gas[i].nPerMax = FaSet.pMeas->pCur->nPer[i];
												FaSet.pMeas->Gas[i].iPerMin = FaSet.pMeas->pCur->iPer[i];
												FaSet.pMeas->Gas[i].iPerMax = FaSet.pMeas->pCur->iPer[i];
											}
										}
									}
									break;
									
								case 2: // Filtering Status
									if( pFunc->KeyType == Long ){
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											FaSet.pMeas->Filter.NotStableTmrMax[i] = FaSet.pMeas->Filter.NotStableTmr[i];
										}
									}
									break;
									
								case 3: // Scope
									if( FaSet.iWave ){
										FaSet.iWave--;
									}
									else {
										FaSet.iWave = MEASURING_GAS_SENSOR_MAX - 1;
									}
									if( FaSet.iWave < CARTRIDGE_SEN_CH_MAX ){
										FaSet.pMeas->pSen->pCtg->iWaveSet = FaSet.iWave;
									}
									else {
										FaSet.pMeas->pSen->pCvt->iWaveSet = FaSet.iWave - CARTRIDGE_SEN_CH_MAX;
									}
									break;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							pFunc->Level = FUNC_LEVEL_3;
							break;
							
						case 1: // Option
							if( pFunc->Item.Lv4 ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv4 = 0;
								}
								else {
									if( FaSet.pMeas->Conf.OutMax > 1 ){
										pFunc->Item.Lv4--;
									}
									else {
										if( pFunc->Item.Lv4 == 16 ){
											pFunc->Item.Lv4 =   7;
										}
										else {
											pFunc->Item.Lv4--;
										}
									}
								}
							}
							else {
								pFunc->Level = FUNC_LEVEL_3;
							}
							break;
					
						case 2: // Filter
							if( pFunc->Item.Lv4 ){
								pFunc->Item.Lv4--;
							}
							else {
								pFunc->Level = FUNC_LEVEL_3;
							}
							break;
							
						case 3: // Maintenance
							if( pFunc->Item.Lv4 > FaSet.sGas[0].Msd ){
								if( pFunc->KeyType == Long ){
									pFunc->Item.Lv4 = FaSet.sGas[0].Msd;
								}
								else {
									switch( pFunc->Item.Lv4 ){
										case  5:
										case  6:
										case  7:
											if( FaSet.sGas[1].Msd == (pFunc->Item.Lv4 - 4) ){
												pFunc->Item.Lv4 = 3;
											}
											else {
												pFunc->Item.Lv4--;
											}
											break;
											
										case  8:
											if( FaSet.pMeas->Conf.OutMax < 2 ){
												pFunc->Item.Lv4 = 3;
											}
											else {
												pFunc->Item.Lv4--;
											}
											break;
											
										default:
											pFunc->Item.Lv4--;
											break;
									}
								}
							}
							else {
								pFunc->Level = FUNC_LEVEL_3;
							}
							break;
							
						case 4: // View Status
							KeyScan.Tick[KEY_INDEX_UP  ].IsRepeatMode = true;
							KeyScan.Tick[KEY_INDEX_DOWN].IsRepeatMode = true;
							FaSet.pMeas->pSen->CtgWaveModeSet = false;
							FaSet.pMeas->pSen->CvtWaveModeSet = false;
							pFunc->pMsgBox->ClearTitle = true;
							pFunc->Level = FUNC_LEVEL_3;
							break;
					}
					break;
			}
			break;
			
		case FUNC_LEVEL_5:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									switch( pFunc->Item.Lv5 ){
										case 0:
											if( FaSet.pMeas->Conf.SenComb == MEASURING_SEN_COMB_CTG_DUAL ){
												pFunc->Item.Lv5 = 1;
											}
											else {
												pFunc->Item.Lv5 = 2;
											}
											break;
											
										case 1:
											pFunc->Item.Lv5 = 2;
											break;
											
										case 2:
											if( pFunc->IsApproved == true ){
												Measuring_Cartridge_Config_Update();
											}
											pFunc->Level = FUNC_LEVEL_4;
											break;
									}
									break;
									
								case 1: // System
									if( pFunc->Item.Lv5 < 3 ){ 
										switch( FaSet.pMeas->Conf.SenComb ){
											case MEASURING_SEN_COMB_CTG_SING:			sMax = 0; break;
											case MEASURING_SEN_COMB_CTG_DUAL:			sMax = 1; break;
											case MEASURING_SEN_COMB_CVT_DUAL:			sMax = 2; break;
											case MEASURING_SEN_COMB_CVT_QUAD:			sMax = 2; break;
											case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMax = 2; break;
											case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMax = 2; break;
											case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMax = 2; break;
											case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMax = 2; break;
										}
										if( pFunc->Item.Lv5 < sMax ){
											switch( FaSet.pMeas->Conf.SenComb ){
												case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
												case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
													if( pFunc->Item.Lv5 == 0 ){
														pFunc->Item.Lv5 = 2;
													}
													else {
														pFunc->Item.Lv5++;
													}
													break;
													
												case MEASURING_SEN_COMB_CTG_SING:
												case MEASURING_SEN_COMB_CTG_DUAL:
												case MEASURING_SEN_COMB_CVT_DUAL:
												case MEASURING_SEN_COMB_CVT_QUAD:
													pFunc->Item.Lv5++;
													break;
											}
										}
										else {
											pFunc->Item.Lv5 = 3;
										}
									}
									else {
										if( pFunc->IsApproved == true ){
											Measuring_System_Config_Update();
										}
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									switch( pFunc->Item.Lv5 ){
										case 0:
										case 1:
											if( FaSet.sCtgSenType[pFunc->Item.Lv5] ){
												FaSet.sCtgSenType[pFunc->Item.Lv5]--;
											}
											break;
											
										case 2:
											pFunc->IsApproved ^= true;
											break;
									}
									break;
									
								case 1: // System
									switch( pFunc->Item.Lv5 ){
										case 0: Measuring_Config_Load( FaSet.sConf.SetNo.Ctg[0] + 1, CONF_ITEM_CT0 ); break;
										case 1: Measuring_Config_Load( FaSet.sConf.SetNo.Ctg[1] + 1, CONF_ITEM_CT1 ); break;
										case 2: Measuring_Config_Load( FaSet.sConf.SetNo.Cvt	+ 1, CONF_ITEM_CVT ); break;
										case 3: pFunc->IsApproved ^= true; break;
									}
									break;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									switch( pFunc->Item.Lv5 ){
										case 0:
										case 1:
											if( FaSet.sCtgSenType[pFunc->Item.Lv5] < (CT_SEN_MAX - 1) ){
												FaSet.sCtgSenType[pFunc->Item.Lv5]++;
											}
											break;
											
										case 2:
											pFunc->IsApproved ^= true;
											break;
									}
									break;
									
								case 1: // System
									switch( pFunc->Item.Lv5 ){
										case 0: Measuring_Config_Load( FaSet.sConf.SetNo.Ctg[0] - 1, CONF_ITEM_CT0 ); break;
										case 1: Measuring_Config_Load( FaSet.sConf.SetNo.Ctg[1] - 1, CONF_ITEM_CT1 ); break;
										case 2: Measuring_Config_Load( FaSet.sConf.SetNo.Cvt	- 1, CONF_ITEM_CVT ); break;
										case 3: pFunc->IsApproved ^= true; break;
									}
									break;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv3 ){
						case 0: // Config
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									switch( pFunc->Item.Lv5 ){
										case 0:
											pFunc->Level = FUNC_LEVEL_4;
											break;
											
										case 1:
											pFunc->Item.Lv5 = 0;
											break;
											
										case 2:
											if( FaSet.pMeas->Conf.SenComb == MEASURING_SEN_COMB_CTG_DUAL ){
												pFunc->Item.Lv5 = 1;
											}
											else {
												pFunc->Item.Lv5 = 0;
											}
											break;
									}
									break;
									
								case 1: // System
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax = 0; break;
										case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax = 1; break;
										case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 2; sMax = 2; break;
										case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 2; sMax = 2; break;
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax = 2; break;
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 2; break;
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax = 2; break;
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 2; break;
									}
									if( pFunc->Item.Lv5 > sMin ){
										if( pFunc->Item.Lv5 == 3 ){
											pFunc->Item.Lv5 = sMax;
										}
										else {
											switch( FaSet.pMeas->Conf.SenComb ){
												case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
												case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
													if( pFunc->Item.Lv5 == 2 ){
														pFunc->Item.Lv5 = 0;
													}
													else {
														pFunc->Item.Lv5--;
													}
													break;
													
												case MEASURING_SEN_COMB_CTG_SING:
												case MEASURING_SEN_COMB_CTG_DUAL:
												case MEASURING_SEN_COMB_CVT_DUAL:
												case MEASURING_SEN_COMB_CVT_QUAD:
													pFunc->Item.Lv5--;
													break;
											}
										}
									}
									else {
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
							}
							break;
					}
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess_Cal( Func_HandleType *pFunc )
{
	U16		GainSet;
	U8		i, sMin, sMax;
	float	SpanMax;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( FaSet.pMeas->Conf.SenComb ){
								case MEASURING_SEN_COMB_CTG_SING:
								case MEASURING_SEN_COMB_CTG_DUAL:
								case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
								case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
								case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
								case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
									pFunc->Item.Lv4 = 0; // Cartridge
									break;
									
								case MEASURING_SEN_COMB_CVT_DUAL:
								case MEASURING_SEN_COMB_CVT_QUAD:
									pFunc->Item.Lv4 = 1; // Cavity-IR
									break;
									
								default:
									pFunc->Item.Lv4 = 2; // Target Voltage
									break;
							}
							break;
							
						case 1: // Zero
							FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								FaSet.Cal.Select[i] = false;
								FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
							}
							FaSet.Cal.InjectionTmr = 0;
							switch( FaSet.pMeas->Conf.SenComb ){
								case MEASURING_SEN_COMB_CTG_SING:
								case MEASURING_SEN_COMB_CTG_DUAL:
								case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
								case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
								case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
								case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
									pFunc->Item.Lv4 = 0;
									break;
									
								case MEASURING_SEN_COMB_CVT_DUAL:
								case MEASURING_SEN_COMB_CVT_QUAD:
									pFunc->Item.Lv4 = 2;
									break;
							}
							break;
							
						case 2: // Span
							pFunc->Item.Lv4 = FaSet.pMeas->Conf.iOut[0];
							break;
							
						case 3: // Base Zero
						case 4: // View Info
							pFunc->Item.Lv4 = 0;
							break;
					}
					pFunc->IsApproved = false;
					pFunc->Level = FUNC_LEVEL_4;
					break;
					
				case Up:
					if( pFunc->Item.Lv3 ){
						pFunc->Item.Lv3--;
					}
					break;
					
				case Down:
					if( pFunc->Item.Lv3 < 4 ){
						pFunc->Item.Lv3++;
					}
					break;
					
				case Reset:
					pFunc->State.Bit.Calibration = false;
					pFunc->Level = FUNC_LEVEL_2;
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									FaSet.Cal.TIA[0] = FaSet.pMeas->Gas[0].Inf.TIA;
									FaSet.Cal.TIA[1] = FaSet.pMeas->Gas[1].Inf.TIA;
									pFunc->Item.Lv5 = 0;
									break;
									
								case 1: // Cavity-IR
									FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										FaSet.Cal.Select[i] = false;
									}
									pFunc->Item.Lv5 = 0;
									break;
									
								case 2: // Target Voltage
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										if( FaSet.pMeas->Gas[i].IsPresent == true ){
											FaSet.Cal.TargetOffs[i] = FaSet.pMeas->Gas[i].Inf.TargetOffs;
											FaSet.Cal.TargetGain[i] = FaSet.pMeas->Gas[i].Inf.TargetGain;
										}
									}
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING:
										case MEASURING_SEN_COMB_CTG_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
											pFunc->Item.Lv5 = 0;
											break;
											
										case MEASURING_SEN_COMB_CVT_DUAL:
										case MEASURING_SEN_COMB_CVT_QUAD:
											pFunc->Item.Lv5 = 4;
											break;
									}
									break;
							}
							pFunc->IsApproved = false;
							pFunc->Level = FUNC_LEVEL_5;
							break;
							
						case 1: // Zero
							if( pFunc->Item.Lv4 < 6 ){
								FaSet.Cal.Select[pFunc->Item.Lv4] ^= true;
							}
							else {
								switch( FaSet.Cal.Status ){
									case FACTORY_CAL_STATUS_READY:
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											if( FaSet.Cal.Select[i] == true ){
												FaSet.Cal.Status = FACTORY_CAL_STATUS_INJECT_GAS;
												FaSet.Cal.Tick = 0x8000 + 24;
												break;
											}
										}
										break;
										
									case FACTORY_CAL_STATUS_INJECT_GAS:
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											if( MeasHandle.Gas[i].IsPresent == true ){
												FaSet.Cal.Volt[i] = FaSet.pMeas->pCur->nDet[i];
												if( i < CARTRIDGE_SEN_CH_MAX ){
													FaSet.Cal.Temp[i] = FaSet.pMeas->pCur->CtgSenTemp;
												}
												else {
													FaSet.Cal.Temp[i] = FaSet.pMeas->pCur->CvtSenTemp;
												}
											}
										}
										FaSet.Cal.Status = FACTORY_CAL_STATUS_DECISION;
										FaSet.Cal.Tick = 0x8000 + 49;
										break;
										
									case FACTORY_CAL_STATUS_FINISHED:
										FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
										FaSet.Cal.InjectionTmr = 0;
										break;
								}
							}
							break;
							
						case 2: // Span
							FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
							}
							i = pFunc->Item.Lv4;
							FaSet.Cal.InjectionTmr = 0;
							SpanMax = FaSet.pMeas->Gas[i].Inf.HighScale * (100.f / (U32)FaSet.pMeas->Gas[i].Inf.CrossScale) + 0.5f;
							if( SpanMax > 9999.f ){ SpanMax = 9999.f; }
							FaSet.Cal.SpanMax = (U16)SpanMax;
							FaSet.Cal.SpanMin = FaSet.Cal.SpanMax / 10;
							FaSet.Cal.SpanGas = FaSet.pMeas->Gas[i].Inf.SpanGas;
							FaSet.Cal.SpanMsd = FaSet.pMeas->Gas[i].Inf.Msd;
							pFunc->Item.Lv5 = FaSet.Cal.SpanMsd;
							pFunc->IsApproved = false;
							pFunc->Level = FUNC_LEVEL_5;
							break;
							
						case 3: // Base Zero
							switch( pFunc->Item.Lv4 ){
								case 0: // Cal
									FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										FaSet.Cal.Select[i] = false;
										FaSet.Cal.Result[i] = FACTORY_CAL_RESULT_SUCCESS;
									}
									iBaseZeroSet = 0;
									iBaseZeroMax = 0;
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										if( MeasHandle.Gas[i].IsPresent == true ){
											if( FaSet.pMeas->Gas[i].Inf.bZeroUse == true ){
												iBaseZeroBuf[iBaseZeroMax] = i;
												iBaseZeroMax++;
											}
										}
									}
									if( iBaseZeroMax ){
										pFunc->Item.Lv5 = iBaseZeroBuf[0];
									}
									else {
										pFunc->Item.Lv5 = 6;
									}
									FaSet.Cal.InjectionTmr = 0;
									break;
									
								case 1: // Set
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										if( FaSet.pMeas->Gas[i].IsPresent == true ){
											FaSet.Cal.bZeroUse[i] = FaSet.pMeas->Gas[i].Inf.bZeroUse;
											FaSet.Cal.bZeroPer[i] = FaSet.pMeas->Gas[i].Inf.bZeroPer;
											FaSet.Cal.bZeroSec[i] = FaSet.pMeas->Gas[i].Inf.bZeroSec;
										}
									}
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING:
										case MEASURING_SEN_COMB_CTG_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
											pFunc->Item.Lv5 = 0;
											break;
											
										case MEASURING_SEN_COMB_CVT_DUAL:
										case MEASURING_SEN_COMB_CVT_QUAD:
											pFunc->Item.Lv5 = 6;
											break;
									}
									break;
							}
							pFunc->IsApproved = false;
							pFunc->Level = FUNC_LEVEL_5;
							break;
							
						case 4: // View Info
							if( pFunc->KeyType == Long ){
								i = pFunc->Item.Lv4;
								if( FaSet.pMeas->Gas[i].IsPresent == true ){
									FaSet.pMeas->Gas[i].Inf.bZeroCnt = 0;
									SenEEP_Write_CHn( &FaSet.pMeas->Gas[i].Inf.bZeroCnt, i, SENSOR_EEP_CHn_bZERO_COUNT, 1 );
								}
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 1: // Cavity-IR
									if		( FaSet.pMeas->pSen->pCtg->IsPresent == true ){ pFunc->Item.Lv4 = 0; } // Cartridge
									break;
									
								case 2: // Target Voltage
									if		( FaSet.pMeas->pSen->pCvt->IsPresent == true ){ pFunc->Item.Lv4 = 1; } // Cavity-IR
									else if	( FaSet.pMeas->pSen->pCtg->IsPresent == true ){ pFunc->Item.Lv4 = 0; } // Cartridge
									break;
							}
							break;
							
						case 1: // Zero
							if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){
								switch( FaSet.pMeas->Conf.SenComb ){
									case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax = 0; break;
									case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax = 1; break;
									case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 2; sMax = 2; break;
									case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 2; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax = 2; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax = 2; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 5; break;
								}
								if( pFunc->Item.Lv4 > sMin ){
									if( pFunc->Item.Lv4 == 6 ){
										pFunc->Item.Lv4 = sMax;
									}
									else {
										switch( FaSet.pMeas->Conf.SenComb ){
											case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
											case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
												if( pFunc->Item.Lv4 == 2 ){
													pFunc->Item.Lv4 = 0;
												}
												else {
													pFunc->Item.Lv4--;
												}
												break;
												
											case MEASURING_SEN_COMB_CTG_SING:
											case MEASURING_SEN_COMB_CTG_DUAL:
											case MEASURING_SEN_COMB_CVT_DUAL:
											case MEASURING_SEN_COMB_CVT_QUAD:
											case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
											case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
												pFunc->Item.Lv4--;
												break;
										}
									}
								}
								
							}
							break;
							
						case 2: // Span
							pFunc->Item.Lv4 = FaSet.pMeas->Conf.iOut[0];
							break;
							
						case 3: // Base Zero
							if( pFunc->Item.Lv4 ){
								pFunc->Item.Lv4--;
							}
							break;
							
						case 4: // View Info
							if( pFunc->Item.Lv4 < (MEASURING_GAS_SENSOR_MAX - 1) ){
								pFunc->Item.Lv4++;
							}
							else {
								pFunc->Item.Lv4 = 0;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									if( FaSet.pMeas->pSen->pCvt->IsPresent == true ){ pFunc->Item.Lv4 = 1; } // Cavity-IR
									else { pFunc->Item.Lv4 = 2; } // Target Voltage
									break;
									
								case 1: // Cavity-IR
									pFunc->Item.Lv4 = 2; // Target Voltage
									break;
							}
							break;
							
						case 1: // Zero
							if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){
								switch( FaSet.pMeas->Conf.SenComb ){
									case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax = 0; break;
									case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax = 1; break;
									case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 2; sMax = 2; break;
									case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 2; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax = 2; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax = 2; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 5; break;
								}
								if( pFunc->Item.Lv4 < sMax ){
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
											if( pFunc->Item.Lv4 == 0 ){
												pFunc->Item.Lv4 = 2;
											}
											else {
												pFunc->Item.Lv4++;
											}
											break;
											
										case MEASURING_SEN_COMB_CTG_SING:
										case MEASURING_SEN_COMB_CTG_DUAL:
										case MEASURING_SEN_COMB_CVT_DUAL:
										case MEASURING_SEN_COMB_CVT_QUAD:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
											pFunc->Item.Lv4++;
											break;
									}
								}
								else {
									pFunc->Item.Lv4 = 6;
								}
							}
							break;
							
						case 2: // Span
							pFunc->Item.Lv4 = FaSet.pMeas->Conf.iOut[FaSet.pMeas->Conf.OutMax-1];
							break;
							
						case 3: // Base Zero
							if( pFunc->Item.Lv4 < 1 ){
								pFunc->Item.Lv4++;
							}
							break;
							
						case 4: // View Info
							if( pFunc->Item.Lv4 ){
								pFunc->Item.Lv4--;
							}
							else {
								pFunc->Item.Lv4 = MEASURING_GAS_SENSOR_MAX - 1;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv3 ){
						case 1: // Zero
							switch( FaSet.Cal.Status ){
								case FACTORY_CAL_STATUS_READY:
								case FACTORY_CAL_STATUS_FINISHED:
									pFunc->Level = FUNC_LEVEL_3;
									break;
									
								case FACTORY_CAL_STATUS_INJECT_GAS:
									FaSet.Cal.Tick = 0;
									FaSet.Cal.InjectionTmr = 0;
									FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
									break;
							}
							break;
							
						case 0: // Gain
						case 2: // Span
						case 3: // Base Zero
						case 4: // View Info
							pFunc->Level = FUNC_LEVEL_3;
							break;
					}
					break;
			}
			break;
			
		case FUNC_LEVEL_5:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									if( pFunc->Item.Lv5 < 2 ){
										if( FaSet.pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] > 1 ){
											pFunc->Item.Lv5++;
										}
										else {
											pFunc->Item.Lv5 = 2;
										}
									}
									else {
										if( pFunc->IsApproved == true ){
											FaSet.pMeas->Gas[0].Inf.TIA.Word[0] = FaSet.Cal.TIA[0].Word[0];
											FaSet.pMeas->Gas[1].Inf.TIA.Word[0] = FaSet.Cal.TIA[1].Word[0];
										}
										else {
											FactoryCal_Cartridge_Rgain_Return( 0 );
											FactoryCal_Cartridge_Rgain_Return( 1 );
										}
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
									
								case 1: // Cavity
									if( pFunc->Item.Lv5 < 4 ){
										FaSet.Cal.Select[CARTRIDGE_SEN_CH_MAX + pFunc->Item.Lv5] ^= true;
									}
									else {
										switch( FaSet.Cal.Status ){
											case FACTORY_CAL_STATUS_READY:
												if( (FaSet.pMeas->pCur->CvtSenTemp >= 30.f) &&
													(FaSet.pMeas->pCur->CvtSenTemp <= 50.f) )
												{
													GainSet = 0;
													for( i=0; i<FaSet.pMeas->pSen->pCvt->Inf[CAVITY_INF_CHANNELS]; i++ ){
														if( FaSet.Cal.Select[CARTRIDGE_SEN_CH_MAX + i] == true ){
															GainSet |= (1 << i);
														}
													}
													if( GainSet ){
														FaSet.pMeas->pSen->pCvt->AmpCalRunFlag = GainSet;
														SenEEP_Write_Cvt( &GainSet, 0, SENSOR_EEP_SYS_AMP_CAL_RUN_FLAG, 1 );
														FaSet.Cal.Tick = 0x8000 + 24;
														FaSet.Cal.Status = FACTORY_CAL_STATUS_PACKET_SEND_GAIN;
													}
												}
												break;
												
											case FACTORY_CAL_STATUS_FINISHED_GAIN:
												FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
												break;
										}
									}
									break;
									
								case 2: // Target Voltage
									if( pFunc->Item.Lv5 < 12 ){
										if( pFunc->KeyType == Long ){
											pFunc->Item.Lv5 = 12;
										}
										else { 
											switch( FaSet.pMeas->Conf.SenComb ){
												case MEASURING_SEN_COMB_CTG_SING:			sMax =  1; break;
												case MEASURING_SEN_COMB_CTG_DUAL:			sMax =  3; break;
												case MEASURING_SEN_COMB_CVT_DUAL:			sMax =  7; break;
												case MEASURING_SEN_COMB_CVT_QUAD:			sMax = 11; break;
												case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMax =  7; break;
												case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMax = 11; break;
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMax =  7; break;
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMax = 11; break;
											}
											if( pFunc->Item.Lv5 < sMax ){
												switch( FaSet.pMeas->Conf.SenComb ){
													case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
														if( pFunc->Item.Lv5 == 1 ){
															pFunc->Item.Lv5 = 4;
														}
														else {
															pFunc->Item.Lv5++;
														}
														break;
														
													case MEASURING_SEN_COMB_CTG_SING:
													case MEASURING_SEN_COMB_CTG_DUAL:
													case MEASURING_SEN_COMB_CVT_DUAL:
													case MEASURING_SEN_COMB_CVT_QUAD:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
														pFunc->Item.Lv5++;
														break;
												}
											}
											else {
												pFunc->Item.Lv5 = 12;
											}
										}
									}
									else {
										if( pFunc->IsApproved == true ){
											for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
												if( MeasHandle.Gas[i].IsPresent == true ){ FactoryCal_Target_Set_Update( i ); }
											}
										}
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
							}
							break;
							
						case 2: // Span
							if( pFunc->Item.Lv5 < 4 ){
								pFunc->Item.Lv5++;
							}
							else {
								switch( FaSet.Cal.Status ){
									case FACTORY_CAL_STATUS_READY:
										if( pFunc->IsApproved == true ){
											FaSet.Cal.Status = FACTORY_CAL_STATUS_INJECT_GAS;
											FaSet.Cal.Tick = 0x8000 + 24;
										}
										else {
											pFunc->Level = FUNC_LEVEL_4;
										}
										break;
										
									case FACTORY_CAL_STATUS_INJECT_GAS:
										for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
											if( MeasHandle.Gas[i].IsPresent == true ){
												FaSet.Cal.Volt[i] = FaSet.pMeas->pCur->nDet[i];
												if( i < CARTRIDGE_SEN_CH_MAX ){
													FaSet.Cal.Temp[i] = FaSet.pMeas->pCur->CtgSenTemp;
												}
												else {
													FaSet.Cal.Temp[i] = FaSet.pMeas->pCur->CvtSenTemp;
												}
											}
										}
										FaSet.Cal.Status = FACTORY_CAL_STATUS_DECISION;
										FaSet.Cal.Tick = 0x8000 + 49;
										break;
										
									case FACTORY_CAL_STATUS_FINISHED:
										FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
										FaSet.Cal.InjectionTmr = 0;
										break;
								}
							}
							break;
							
						case 3: // Base Zero
							switch( pFunc->Item.Lv4 ){
								case 0: // Cal
									if( pFunc->Item.Lv5 < 6 ){
										if( FaSet.pMeas->Gas[pFunc->Item.Lv5].Inf.bZeroUse == true ){
											FaSet.Cal.Select[pFunc->Item.Lv5] ^= true;
										}
									}
									else {
										switch( FaSet.Cal.Status ){
											case FACTORY_CAL_STATUS_READY:
												for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
													if( MeasHandle.Gas[i].IsPresent == true ){
														if( FaSet.Cal.Select[i] == true ){
															FaSet.Cal.Status = FACTORY_CAL_STATUS_INJECT_GAS;
															FaSet.Cal.Tick = 0x8000 + 24;
															break;
														}
													}
												}
												break;
												
											case FACTORY_CAL_STATUS_INJECT_GAS:
												for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
													if( MeasHandle.Gas[i].IsPresent == true ){
														FaSet.Cal.Volt[i] = FaSet.pMeas->pCur->nDet[i];
														if( i < CARTRIDGE_SEN_CH_MAX ){
															FaSet.Cal.Temp[i] = FaSet.pMeas->pCur->CtgSenTemp;
														}
														else {
															FaSet.Cal.Temp[i] = FaSet.pMeas->pCur->CvtSenTemp;
														}
													}
												}
												FaSet.Cal.Status = FACTORY_CAL_STATUS_DECISION;
												FaSet.Cal.Tick = 0x8000 + 49;
												break;
												
											case FACTORY_CAL_STATUS_FINISHED:
												FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
												FaSet.Cal.InjectionTmr = 0;
												break;
										}
									}
									break;
									
								case 1: // Set
									if( pFunc->Item.Lv5 < 18 ){
										if( pFunc->KeyType == Long ){
											pFunc->Item.Lv5 = 18;
										}
										else { 
											switch( FaSet.pMeas->Conf.SenComb ){
												case MEASURING_SEN_COMB_CTG_SING:			sMax =  2; break;
												case MEASURING_SEN_COMB_CTG_DUAL:			sMax =  5; break;
												case MEASURING_SEN_COMB_CVT_DUAL:
												case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMax =  8; break;
												case MEASURING_SEN_COMB_CVT_QUAD:
												case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
												case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMax = 17; break;
											}
											if( pFunc->Item.Lv5 < sMax ){
												switch( FaSet.pMeas->Conf.SenComb ){
													case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
														if( pFunc->Item.Lv5 == 2 ){
															pFunc->Item.Lv5 = 6;
														}
														else {
															pFunc->Item.Lv5++;
														}
														break;
														
													case MEASURING_SEN_COMB_CTG_SING:
													case MEASURING_SEN_COMB_CTG_DUAL:
													case MEASURING_SEN_COMB_CVT_DUAL:
													case MEASURING_SEN_COMB_CVT_QUAD:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
														pFunc->Item.Lv5++;
														break;
												}
											}
											else {
												pFunc->Item.Lv5 = 18;
											}
										}
									}
									else {
										if( pFunc->IsApproved == true ){
											for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
												if( MeasHandle.Gas[i].IsPresent == true ){ FactoryCal_Base_Set_Update( i ); }
											}
										}
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
							}
							break;
					}
					break;
					
				case Up:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									if( pFunc->Item.Lv5 == 2 ){
										pFunc->IsApproved ^= true;
									}
									else {
										if( pFunc->KeyType == Short ){
											if( FaSet.Cal.TIA[pFunc->Item.Lv5].Item.RGAIN < LPTIA_RGAIN_512K ){
												FaSet.Cal.TIA[pFunc->Item.Lv5].Item.RGAIN++;
												FactoryCal_Cartridge_Rgain_Update( pFunc->Item.Lv5 );
											}
										}
									}
									break;
									
								case 1: // Cavity
									if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){
										if( pFunc->Item.Lv5 ){
											if( pFunc->Item.Lv5 == 4 ){
												pFunc->Item.Lv5 = (FaSet.pMeas->pSen->pCvt->Inf[CAVITY_INF_CHANNELS] - 1);
											}
											else {
												pFunc->Item.Lv5--;
											}
										}
									}
									break;
									
								case 2: // Target Voltage
									switch( pFunc->Item.Lv5 ){
										case  0: Func_uInc( 1900, 50, &FaSet.Cal.TargetOffs[0] ); break;
										case  2: Func_uInc( 1900, 50, &FaSet.Cal.TargetOffs[1] ); break;
										case  4: Func_uInc( 3500, 50, &FaSet.Cal.TargetOffs[2] ); break;
										case  6: Func_uInc( 3500, 50, &FaSet.Cal.TargetOffs[3] ); break;
										case  8: Func_uInc( 3500, 50, &FaSet.Cal.TargetOffs[4] ); break;
										case 10: Func_uInc( 3500, 50, &FaSet.Cal.TargetOffs[5] ); break;
										case  1: Func_uInc( 1900, 50, &FaSet.Cal.TargetGain[0] ); break;
										case  3: Func_uInc( 1900, 50, &FaSet.Cal.TargetGain[1] ); break;
										case  5: Func_uInc( 4000, 50, &FaSet.Cal.TargetGain[2] ); break;
										case  7: Func_uInc( 4000, 50, &FaSet.Cal.TargetGain[3] ); break;
										case  9: Func_uInc( 4000, 50, &FaSet.Cal.TargetGain[4] ); break;
										case 11: Func_uInc( 4000, 50, &FaSet.Cal.TargetGain[5] ); break;
										case 12: pFunc->IsApproved ^= true;
									}
									break;
							}
							break;
							
						case 2: // Span
							switch( pFunc->Item.Lv5 ){
								case 0: Func_uInc( FaSet.Cal.SpanMax, 1000, &FaSet.Cal.SpanGas ); break;
								case 1: Func_uInc( FaSet.Cal.SpanMax,  100, &FaSet.Cal.SpanGas ); break;
								case 2: Func_uInc( FaSet.Cal.SpanMax,   10, &FaSet.Cal.SpanGas ); break;
								case 3: Func_uInc( FaSet.Cal.SpanMax,    1, &FaSet.Cal.SpanGas ); break;
								case 4: if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){ pFunc->IsApproved ^= true; } break;
							}
							break;
							
						case 3: // Base Zero
							switch( pFunc->Item.Lv4 ){
								case 0: // Cal
									if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){
										if( pFunc->Item.Lv5 == 6 ){
											if( iBaseZeroMax ){
												iBaseZeroSet = iBaseZeroMax - 1;
												pFunc->Item.Lv5 = iBaseZeroBuf[iBaseZeroSet];
											}
										}
										else {
											if( iBaseZeroSet ){
												iBaseZeroSet--;
												pFunc->Item.Lv5 = iBaseZeroBuf[iBaseZeroSet];
											}
										}
									}
									break;
									
								case 1: // Set
									switch( pFunc->Item.Lv5 ){
										case  0: FaSet.Cal.bZeroUse[0] ^= true; break;
										case  3: FaSet.Cal.bZeroUse[1] ^= true; break;
										case  6: FaSet.Cal.bZeroUse[2] ^= true; break;
										case  9: FaSet.Cal.bZeroUse[3] ^= true; break;
										case 12: FaSet.Cal.bZeroUse[4] ^= true; break;
										case 15: FaSet.Cal.bZeroUse[5] ^= true; break;
										case  1: Func_uInc( 100,  5, &FaSet.Cal.bZeroPer[0] ); break;
										case  4: Func_uInc( 100,  5, &FaSet.Cal.bZeroPer[1] ); break;
										case  7: Func_uInc( 100,  5, &FaSet.Cal.bZeroPer[2] ); break;
										case 10: Func_uInc( 100,  5, &FaSet.Cal.bZeroPer[3] ); break;
										case 13: Func_uInc( 100,  5, &FaSet.Cal.bZeroPer[4] ); break;
										case 16: Func_uInc( 100,  5, &FaSet.Cal.bZeroPer[5] ); break;
										case  2: Func_uInc( 600, 10, &FaSet.Cal.bZeroSec[0] ); break;
										case  5: Func_uInc( 600, 10, &FaSet.Cal.bZeroSec[1] ); break;
										case  8: Func_uInc( 600, 10, &FaSet.Cal.bZeroSec[2] ); break;
										case 11: Func_uInc( 600, 10, &FaSet.Cal.bZeroSec[3] ); break;
										case 14: Func_uInc( 600, 10, &FaSet.Cal.bZeroSec[4] ); break;
										case 17: Func_uInc( 600, 10, &FaSet.Cal.bZeroSec[5] ); break;
										case 18: pFunc->IsApproved ^= true;
									}
									break;
							}
							break;
					}
					break;
					
				case Down:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									if( pFunc->Item.Lv5 == 2 ){
										pFunc->IsApproved ^= true;
									}
									else {
										if( pFunc->KeyType == Short ){
											if( FaSet.Cal.TIA[pFunc->Item.Lv5].Item.RGAIN > LPTIA_RGAIN_DISCONNECT ){
												FaSet.Cal.TIA[pFunc->Item.Lv5].Item.RGAIN--;
												FactoryCal_Cartridge_Rgain_Update( pFunc->Item.Lv5 );
											}
										}
									}
									break;
									
								case 1: // Cavity
									if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){
										if( pFunc->Item.Lv5 < (FaSet.pMeas->pSen->pCvt->Inf[CAVITY_INF_CHANNELS] - 1) ){
											pFunc->Item.Lv5++;
										}
										else {
											pFunc->Item.Lv5 = 4;
										}
									}
									break;
									
								case 2: // Target Voltage
									switch( pFunc->Item.Lv5 ){
										case  0: Func_uDec(  300, 50, &FaSet.Cal.TargetOffs[0] ); break;
										case  2: Func_uDec(  300, 50, &FaSet.Cal.TargetOffs[1] ); break;
										case  4: Func_uDec( 2000, 50, &FaSet.Cal.TargetOffs[2] ); break;
										case  6: Func_uDec( 2000, 50, &FaSet.Cal.TargetOffs[3] ); break;
										case  8: Func_uDec( 2000, 50, &FaSet.Cal.TargetOffs[4] ); break;
										case 10: Func_uDec( 2000, 50, &FaSet.Cal.TargetOffs[5] ); break;
										case  1: Func_uDec(  300, 50, &FaSet.Cal.TargetGain[0] ); break;
										case  3: Func_uDec(  300, 50, &FaSet.Cal.TargetGain[1] ); break;
										case  5: Func_uDec( 1400, 50, &FaSet.Cal.TargetGain[2] ); break;
										case  7: Func_uDec( 1400, 50, &FaSet.Cal.TargetGain[3] ); break;
										case  9: Func_uDec( 1400, 50, &FaSet.Cal.TargetGain[4] ); break;
										case 11: Func_uDec( 1400, 50, &FaSet.Cal.TargetGain[5] ); break;
										case 12: pFunc->IsApproved ^= true;
									}
									break;
							}
							break;
							
						case 2: // Span
							switch( pFunc->Item.Lv5 ){
								case 0: Func_uDec( FaSet.Cal.SpanMin, 1000, &FaSet.Cal.SpanGas ); break;
								case 1: Func_uDec( FaSet.Cal.SpanMin,  100, &FaSet.Cal.SpanGas ); break;
								case 2: Func_uDec( FaSet.Cal.SpanMin,   10, &FaSet.Cal.SpanGas ); break;
								case 3: Func_uDec( FaSet.Cal.SpanMin,    1, &FaSet.Cal.SpanGas ); break;
								case 4: if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){ pFunc->IsApproved ^= true; } break;
							}
							break;
							
						case 3: // Base Zero
							switch( pFunc->Item.Lv4 ){
								case 0: // Cal
									if( FaSet.Cal.Status == FACTORY_CAL_STATUS_READY ){
										if( iBaseZeroSet < (iBaseZeroMax - 1) ){
											iBaseZeroSet++;
											pFunc->Item.Lv5 = iBaseZeroBuf[iBaseZeroSet];
										}
										else {
											pFunc->Item.Lv5 = 6;
										}
									}
									break;
									
								case 1: // Set
									switch( pFunc->Item.Lv5 ){
										case  0: FaSet.Cal.bZeroUse[0] ^= true; break;
										case  3: FaSet.Cal.bZeroUse[1] ^= true; break;
										case  6: FaSet.Cal.bZeroUse[2] ^= true; break;
										case  9: FaSet.Cal.bZeroUse[3] ^= true; break;
										case 12: FaSet.Cal.bZeroUse[4] ^= true; break;
										case 15: FaSet.Cal.bZeroUse[5] ^= true; break;
										case  1: Func_uDec( 10,  5, &FaSet.Cal.bZeroPer[0] ); break;
										case  4: Func_uDec( 10,  5, &FaSet.Cal.bZeroPer[1] ); break;
										case  7: Func_uDec( 10,  5, &FaSet.Cal.bZeroPer[2] ); break;
										case 10: Func_uDec( 10,  5, &FaSet.Cal.bZeroPer[3] ); break;
										case 13: Func_uDec( 10,  5, &FaSet.Cal.bZeroPer[4] ); break;
										case 16: Func_uDec( 10,  5, &FaSet.Cal.bZeroPer[5] ); break;
										case  2: Func_uDec( 10, 10, &FaSet.Cal.bZeroSec[0] ); break;
										case  5: Func_uDec( 10, 10, &FaSet.Cal.bZeroSec[1] ); break;
										case  8: Func_uDec( 10, 10, &FaSet.Cal.bZeroSec[2] ); break;
										case 11: Func_uDec( 10, 10, &FaSet.Cal.bZeroSec[3] ); break;
										case 14: Func_uDec( 10, 10, &FaSet.Cal.bZeroSec[4] ); break;
										case 17: Func_uDec( 10, 10, &FaSet.Cal.bZeroSec[5] ); break;
										case 18: pFunc->IsApproved ^= true;
									}
									break;
							}
							break;
					}
					break;
					
				case Reset:
					switch( pFunc->Item.Lv3 ){
						case 0: // Gain
							switch( pFunc->Item.Lv4 ){
								case 0: // Cartridge
									if( pFunc->Item.Lv5 ){
										if( FaSet.pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] > 1 ){
											pFunc->Item.Lv5--;
										}
										else {
											pFunc->Item.Lv5 = 0;
										}
									}
									pFunc->Level = FUNC_LEVEL_4;
									break;
									
								case 1: // Cavity
									switch( FaSet.Cal.Status ){
										case FACTORY_CAL_STATUS_READY:
										case FACTORY_CAL_STATUS_FINISHED_GAIN:
											pFunc->Level = FUNC_LEVEL_4;
											break;
									}
									break;
									
								case 2: // Target Voltage
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax =  1; break;
										case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax =  3; break;
										case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 4; sMax =  7; break;
										case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 4; sMax = 11; break;
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax =  7; break;
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 11; break;
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax =  7; break;
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 11; break;
									}
									if( pFunc->Item.Lv5 > sMin ){
										if( pFunc->KeyType == Long ){
											pFunc->Item.Lv5 = sMin;
										}
										else {
											if( pFunc->Item.Lv5 == 12 ){
												pFunc->Item.Lv5 = sMax;
											}
											else {
												switch( FaSet.pMeas->Conf.SenComb ){
													case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
														if( pFunc->Item.Lv5 == 4 ){
															pFunc->Item.Lv5 = 1;
														}
														else {
															pFunc->Item.Lv5--;
														}
														break;
														
													case MEASURING_SEN_COMB_CTG_SING:
													case MEASURING_SEN_COMB_CTG_DUAL:
													case MEASURING_SEN_COMB_CVT_DUAL:
													case MEASURING_SEN_COMB_CVT_QUAD:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
														pFunc->Item.Lv5--;
														break;
												}
											}
										}
									}
									else {
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
							}
							break;
							
						case 2: // Span
							switch( FaSet.Cal.Status ){
								case FACTORY_CAL_STATUS_READY:
									if( pFunc->Item.Lv5 > FaSet.Cal.SpanMsd ){
										pFunc->Item.Lv5--;
									}
									else {
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
									
								case FACTORY_CAL_STATUS_INJECT_GAS:
									FaSet.Cal.Tick = 0;
									FaSet.Cal.InjectionTmr = 0;
									FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
									break;
									
								case FACTORY_CAL_STATUS_FINISHED:
									pFunc->Level = FUNC_LEVEL_4;
									break;
							}
							break;
							
						case 3: // Base Zero
							switch( pFunc->Item.Lv4 ){
								case 0: // Cal
									switch( FaSet.Cal.Status ){
										case FACTORY_CAL_STATUS_READY:
										case FACTORY_CAL_STATUS_FINISHED:
											pFunc->Level = FUNC_LEVEL_4;
											break;
											
										case FACTORY_CAL_STATUS_INJECT_GAS:
											FaSet.Cal.Tick = 0;
											FaSet.Cal.InjectionTmr = 0;
											FaSet.Cal.Status = FACTORY_CAL_STATUS_READY;
											break;
									}
									break;
									
								case 1: // Set
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax =  2; break;
										case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax =  5; break;
										case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 6; sMax =  8; break;
										case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 6; sMax = 17; break;
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax =  8; break;
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 17; break;
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax =  8; break;
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 17; break;
									}
									if( pFunc->Item.Lv5 > sMin ){
										if( pFunc->KeyType == Long ){
											pFunc->Item.Lv5 = sMin;
										}
										else {
											if( pFunc->Item.Lv5 == 18 ){
												pFunc->Item.Lv5 = sMax;
											}
											else {
												switch( FaSet.pMeas->Conf.SenComb ){
													case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
														if( pFunc->Item.Lv5 == 6 ){
															pFunc->Item.Lv5 = 2;
														}
														else {
															pFunc->Item.Lv5--;
														}
														break;
														
													case MEASURING_SEN_COMB_CTG_SING:
													case MEASURING_SEN_COMB_CTG_DUAL:
													case MEASURING_SEN_COMB_CVT_DUAL:
													case MEASURING_SEN_COMB_CVT_QUAD:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
													case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
														pFunc->Item.Lv5--;
														break;
												}
											}
										}
									}
									else {
										pFunc->Level = FUNC_LEVEL_4;
									}
									break;
							}
							break;
					}
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess_SysInit( Func_HandleType *pFunc )
{
	U8		sMin, sMax;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			switch( pFunc->KeyData ){
				case Function:
					FaSet.Init.IsConfirmMode = false;
					FaSet.Init.Set.Word = 0;
					switch( pFunc->Item.Lv3 ){
						case 0: // Main
							if( SystemOption.Item.IsPoeMode == true ){ pFunc->Item.Lv4 = 2; } // Relay
							else { pFunc->Item.Lv4 = 0; } // mA CH-1
							break;
							
						case 1: // Sensor Set
						case 2: // Sensor Cal
							switch( FaSet.pMeas->Conf.SenComb ){
								case MEASURING_SEN_COMB_CTG_SING:
								case MEASURING_SEN_COMB_CTG_DUAL:
								case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
								case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
								case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
								case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
									pFunc->Item.Lv4 = 0;
									break;
									
								case MEASURING_SEN_COMB_CVT_DUAL:
								case MEASURING_SEN_COMB_CVT_QUAD:
									pFunc->Item.Lv4 = 2;
									break;
							}
							break;
					}
					pFunc->IsApproved = false;
					pFunc->Level = FUNC_LEVEL_4;
					break;
					
				case Up:
					if( pFunc->Item.Lv3 ){
						pFunc->Item.Lv3--;
					}
					break;
					
				case Down:
					if( pFunc->Item.Lv3 < 3 ){
						pFunc->Item.Lv3++;
					}
					break;
					
				case Reset:
					pFunc->Level = FUNC_LEVEL_2;
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 0: // Main
							switch( pFunc->Item.Lv4 ){
								case 0: FaSet.Init.Set.Bit.mA_1		^= true; break;
								case 1: FaSet.Init.Set.Bit.mA_2		^= true; break;
								case 2: FaSet.Init.Set.Bit.Relay	^= true; break;
								case 3: FaSet.Init.Set.Bit.Flow		^= true; break;
								case 4:
									if( FaSet.Init.IsConfirmMode == true ){
										if( pFunc->IsApproved == true ){
											Factory_Init();
										}
										FaSet.Init.IsConfirmMode = false;
									}
									else {
										if( FaSet.Init.Set.Word ){
											pFunc->IsApproved = false;
											FaSet.Init.IsConfirmMode = true;
										}
									}
									break;
							}
							break;
							
						case 1: // Sensor Set
							switch( pFunc->Item.Lv4 ){
								case 0: FaSet.Init.Set.Bit.S1_Set ^= true; break;
								case 1: FaSet.Init.Set.Bit.S2_Set ^= true; break;
								case 2: FaSet.Init.Set.Bit.S3_Set ^= true; break;
								case 3: FaSet.Init.Set.Bit.S4_Set ^= true; break;
								case 4: FaSet.Init.Set.Bit.S5_Set ^= true; break;
								case 5: FaSet.Init.Set.Bit.S6_Set ^= true; break;
								case 6:
									if( FaSet.Init.IsConfirmMode == true ){
										if( pFunc->IsApproved == true ){
											Factory_Init();
										}
										FaSet.Init.IsConfirmMode = false;
									}
									else {
										if( FaSet.Init.Set.Word ){
											pFunc->IsApproved = false;
											FaSet.Init.IsConfirmMode = true;
										}
									}
									break;
							}
							break;
							
						case 2: // Sensor Cal
							switch( pFunc->Item.Lv4 ){
								case 0: FaSet.Init.Set.Bit.S1_Cal ^= true; break;
								case 1: FaSet.Init.Set.Bit.S2_Cal ^= true; break;
								case 2: FaSet.Init.Set.Bit.S3_Cal ^= true; break;
								case 3: FaSet.Init.Set.Bit.S4_Cal ^= true; break;
								case 4: FaSet.Init.Set.Bit.S5_Cal ^= true; break;
								case 5: FaSet.Init.Set.Bit.S6_Cal ^= true; break;
								case 6:
									if( FaSet.Init.IsConfirmMode == true ){
										if( pFunc->IsApproved == true ){
											Factory_Init();
										}
										FaSet.Init.IsConfirmMode = false;
									}
									else {
										if( FaSet.Init.Set.Word ){
											pFunc->IsApproved = false;
											FaSet.Init.IsConfirmMode = true;
										}
									}
									break;
							}
							break;
							
						case 3: // System All
							if( FaSet.Init.IsConfirmMode == true ){
								if( pFunc->IsApproved == true ){
									FaSet.Init.Set.Word = 0xFFFF;
									Factory_Init();
								}
								FaSet.Init.IsConfirmMode = false;
							}
							else {
								pFunc->IsApproved = false;
								FaSet.Init.IsConfirmMode = true;
							}
							break;
					}
					break;
					
				case Up:
					if( FaSet.Init.IsConfirmMode == true ){
						pFunc->IsApproved ^= true;
					}
					else {
						switch( pFunc->Item.Lv3 ){
							case 0: // Main
								if( SystemOption.Item.IsPoeMode == true ){
									if( pFunc->Item.Lv4 > 2 ){ pFunc->Item.Lv4--; }
								}
								else {
									if( pFunc->Item.Lv4 > 0 ){
										if((pFunc->Item.Lv4 == 2) && (FaSet.pMeas->Conf.OutMax == 1) ){
											pFunc->Item.Lv4 -= 2;
										}
										else {
											pFunc->Item.Lv4 -= 1;
										}
									}
								}
								break;
								
							case 1: // Sensor Set
							case 2: // Sensor Cal
								switch( FaSet.pMeas->Conf.SenComb ){
									case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax = 0; break;
									case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax = 1; break;
									case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 2; sMax = 3; break;
									case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 2; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax = 3; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax = 3; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 5; break;
								}
								if( pFunc->Item.Lv4 == 6 ){
									pFunc->Item.Lv4 = sMax;
								}
								else {
									if( pFunc->Item.Lv4 > sMin ){
										switch( FaSet.pMeas->Conf.SenComb ){
											case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
											case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
												if( pFunc->Item.Lv4 == 2 ){
													pFunc->Item.Lv4 = 0;
												}
												else {
													pFunc->Item.Lv4--;
												}
												break;
												
											case MEASURING_SEN_COMB_CTG_SING:
											case MEASURING_SEN_COMB_CTG_DUAL:
											case MEASURING_SEN_COMB_CVT_DUAL:
											case MEASURING_SEN_COMB_CVT_QUAD:
											case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
											case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
												pFunc->Item.Lv4--;
												break;
										}
									}
								}
								break;
						}
					}
					break;
					
				case Down:
					if( FaSet.Init.IsConfirmMode == true ){
						pFunc->IsApproved ^= true;
					}
					else {
						switch( pFunc->Item.Lv3 ){
							case 0: // Main
								if( pFunc->Item.Lv4 < 4 ){
									if((pFunc->Item.Lv4 == 0) && (FaSet.pMeas->Conf.OutMax == 1) ){
										pFunc->Item.Lv4 += 2;
									}
									else {
										pFunc->Item.Lv4 += 1;
									}
								}
								break;
								
							case 1: // Sensor Set
							case 2: // Sensor Cal
								switch( FaSet.pMeas->Conf.SenComb ){
									case MEASURING_SEN_COMB_CTG_SING:			sMin = 0; sMax = 0; break;
									case MEASURING_SEN_COMB_CTG_DUAL:			sMin = 0; sMax = 1; break;
									case MEASURING_SEN_COMB_CVT_DUAL:			sMin = 2; sMax = 3; break;
									case MEASURING_SEN_COMB_CVT_QUAD:			sMin = 2; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:	sMin = 0; sMax = 3; break;
									case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:	sMin = 0; sMax = 5; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:	sMin = 0; sMax = 3; break;
									case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:	sMin = 0; sMax = 5; break;
								}
								if( pFunc->Item.Lv4 < sMax ){
									switch( FaSet.pMeas->Conf.SenComb ){
										case MEASURING_SEN_COMB_CTG_SING_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_SING_CVT_QUAD:
											if( pFunc->Item.Lv4 == 0 ){
												pFunc->Item.Lv4 = 2;
											}
											else {
												pFunc->Item.Lv4++;
											}
											break;
											
										case MEASURING_SEN_COMB_CTG_SING:
										case MEASURING_SEN_COMB_CTG_DUAL:
										case MEASURING_SEN_COMB_CVT_DUAL:
										case MEASURING_SEN_COMB_CVT_QUAD:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_DUAL:
										case MEASURING_SEN_COMB_CTG_DUAL_CVT_QUAD:
											pFunc->Item.Lv4++;
											break;
									}
								}
								else {
									pFunc->Item.Lv4 = 6;
								}
								break;
						}
					}
					break;
					
				case Reset:
					if( FaSet.Init.IsConfirmMode == true ){
						FaSet.Init.IsConfirmMode = false;
					}                
					else {
						pFunc->Level = FUNC_LEVEL_3;
					}
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void KeyProcess_SysInfo( Func_HandleType *pFunc )
{
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			switch( pFunc->KeyData ){
				case Function:
					switch( pFunc->Item.Lv3 ){
						case 1:
							FaSet.pMeas->pCur->VBACKUP = FaSet.pMeas->Raw.VBACKUP;
							pFunc->Item.Lv3 = 2;
							break;
							
						case 4:
							pFunc->Item.Lv3 = 6;
							break;
							
						case 6:
							pFunc->Item.Lv3 = 0;
							break;
							
						default:
							pFunc->Item.Lv3++;
							break;
					}
					pFunc->pMsgBox->ClearMsg = true;
					break;
					
				case Reset:
					iADC_BatteryScan( false );
					pFunc->Level = FUNC_LEVEL_2;
					break;
			}
			break;
	}
}


