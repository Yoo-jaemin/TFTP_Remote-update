#define __EEPROM_C__
    #include "eeprom.h"
#undef  __EEPROM_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "AT24C32.h"
#include "crc16.h"
#include "data_log.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static U16 EEPROM_Read( U16 addr );
static void EEPROM_Sys_Write( U16 addr, U16 data );
static void EEPROM_Load( void );
static void EEPROM_CRC_Write( void );
static void EEPROM_CRC_Check( void );
/* End Function */
                                     
/* Start Variable */
static const U16    SYSTEM_EEP_DEFAULT[SYSTEM_EEP_MAX] =
{
	0,									// (000) SYSTEM_EEP_CRC
	EEPROM_SYS_PREFIX,					// (001) SYSTEM_EEP_PREFIX
	0,									// (002) SYSTEM_EEP_RES_02
	0,									// (003) SYSTEM_EEP_RES_03
	160,								// (004) SYSTEM_EEP_SENSOR_CT_SEN_LPF_WEIGHT_x1000
	20,									// (005) SYSTEM_EEP_SENSOR_IR_SEN_LPF_WEIGHT_x1000
	50,									// (006) SYSTEM_EEP_SENSOR_OTHERS_LPF_WEIGHT_x1000
	0,									// (007) SYSTEM_EEP_RES_07
	0,									// (008) SYSTEM_EEP_RES_08
	0,									// (009) SYSTEM_EEP_RES_09
	0,									// (010) SYSTEM_EEP_PASSWORD
	0x8032,								// (011) SYSTEM_EEP_LCD_BRIGHTNESS_LEVEL
	0,									// (012) SYSTEM_EEP_RES_12
	0,									// (013) SYSTEM_EEP_RES_13
	1,									// (014) SYSTEM_EEP_MODBUS_UNIT_ID
	2,									// (015) SYSTEM_EEP_RS485_BAUD_RATE
	2,									// (016) SYSTEM_EEP_RS485_DATA_TYPE
	0,									// (017) SYSTEM_EEP_RES_17
	0,									// (018) SYSTEM_EEP_RES_18
	0,									// (019) SYSTEM_EEP_RES_19
	0x6CE9,								// (020) SYSTEM_EEP_POE_MC_0_1
	0x8300,								// (021) SYSTEM_EEP_POE_MC_2_3
	0x0000,								// (022) SYSTEM_EEP_POE_MC_4_5
	0xC0A8,								// (023) SYSTEM_EEP_POE_IP_0_1
	0x01C9,								// (024) SYSTEM_EEP_POE_IP_2_3
	0xFFFF,								// (025) SYSTEM_EEP_POE_NM_0_1
	0xFF00,								// (026) SYSTEM_EEP_POE_NM_2_3
	0xC0A8,								// (027) SYSTEM_EEP_POE_GW_0_1
	0x01FE,								// (028) SYSTEM_EEP_POE_GW_2_3
	0,									// (029) SYSTEM_EEP_RES_29
	0,									// (030) SYSTEM_EEP_RES_30
	0,									// (031) SYSTEM_EEP_RES_31
	4000,								// (032) SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1
	4000,								// (033) SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH1
	20000,								// (034) SYSTEM_EEP_mA_OUTPUT_CAL_SPAN_uA_CH1
	0,									// (035) SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH1
	1,									// (036) SYSTEM_EEP_RES_36
	2,									// (037) SYSTEM_EEP_RES_37
	0,									// (038) SYSTEM_EEP_RES_38
	4000,								// (039) SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2
	4000,								// (040) SYSTEM_EEP_mA_OUTPUT_CAL_ZERO_uA_CH2
	20000,								// (041) SYSTEM_EEP_mA_OUTPUT_CAL_SPAN_uA_CH2
	0,									// (042) SYSTEM_EEP_mA_OUTPUT_CAL_OFFS_uA_CH2
	0,									// (043) SYSTEM_EEP_RES_43
	0,									// (044) SYSTEM_EEP_RES_44
	0,									// (045) SYSTEM_EEP_RES_45
	7500,								// (046) SYSTEM_EEP_FLOW_CTRL_P_CALCULATION_SCALE
	70,									// (047) SYSTEM_EEP_FLOW_CTRL_PTM_OFFS
	128,								// (048) SYSTEM_EEP_FLOW_CTRL_PTM_GAIN
	500,								// (049) SYSTEM_EEP_FLOW_CTRL_SPAN_mL
	6000,								// (050) SYSTEM_EEP_FLOW_CTRL_SPAN_FREQ_x100
	900,								// (051) SYSTEM_EEP_FLOW_CTRL_SPAN_DUTY_x100
	0,									// (052) SYSTEM_EEP_FLOW_CTRL_SPAN_L
	17455,								// (053) SYSTEM_EEP_FLOW_CTRL_SPAN_H
	0,									// (054) SYSTEM_EEP_FLOW_CTRL_ZERO_L
	17224,								// (055) SYSTEM_EEP_FLOW_CTRL_ZERO_H
	500,								// (056) SYSTEM_EEP_FLOW_CTRL_TARG_mL
	30,									// (057) SYSTEM_EEP_FLOW_TRB_CHECK_SEC
	1,									// (058) SYSTEM_EEP_RES_58
	0,									// (059) SYSTEM_EEP_RES_59
	0,									// (060) SYSTEM_EEP_RES_60
	1,									// (061) SYSTEM_EEP_RELAY_OPTION_TRB
	1,									// (062) SYSTEM_EEP_RELAY_OPTION_AL1
	1,									// (063) SYSTEM_EEP_RELAY_OPTION_AL2
	0x0101,								// (064) SYSTEM_EEP_RELAY_BLINK_TIME_TRB
	0x0101,								// (065) SYSTEM_EEP_RELAY_BLINK_TIME_AL1
	0x0101,								// (066) SYSTEM_EEP_RELAY_BLINK_TIME_AL2
	0,									// (067) SYSTEM_EEP_RES_67
	0,									// (068) SYSTEM_EEP_RES_68
	0,									// (069) SYSTEM_EEP_RES_69
	0,									// (070) SYSTEM_EEP_RES_70
	0x0000,								// (071) SYSTEM_EEP_MEASURING_MODE
	18000,								// (072) SYSTEM_EEP_MEASURING_NOT_STABLE_DEC_TIME
	0,									// (073) SYSTEM_EEP_RES_73
	0,									// (074) SYSTEM_EEP_RES_74
	0,									// (075) SYSTEM_EEP_RES_75
	0,									// (076) SYSTEM_EEP_RES_76
	0,									// (077) SYSTEM_EEP_RES_77
	0,									// (078) SYSTEM_EEP_RES_78
	1,									// (079) SYSTEM_EEP_LOG_INTERVAL_100MSEC
	0,									// (080) SYSTEM_EEP_RES_80
	0,									// (081) SYSTEM_EEP_RES_81
	0,									// (082) SYSTEM_EEP_RES_82
	0,									// (083) SYSTEM_EEP_RES_83
	0,									// (084) SYSTEM_EEP_RES_84
	0,									// (085) SYSTEM_EEP_RES_85
	0,									// (086) SYSTEM_EEP_RES_86
	0,									// (087) SYSTEM_EEP_RES_87
	0,									// (088) SYSTEM_EEP_RES_88
	0,									// (089) SYSTEM_EEP_RES_89
	1,									// (090) SYSTEM_EEP_BUZZER_VOLUME
	0,									// (091) SYSTEM_EEP_RES_91
	0,									// (092) SYSTEM_EEP_RES_92
	0,									// (093) SYSTEM_EEP_RES_93
	0,									// (094) SYSTEM_EEP_RES_94
	0,									// (095) SYSTEM_EEP_RES_95
	0,									// (096) SYSTEM_EEP_RES_96
	0,									// (097) SYSTEM_EEP_RES_97
	0,									// (098) SYSTEM_EEP_RES_98
	0									// (099) SYSTEM_EEP_RES_99
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool EEPROM_Init( void )
{
	U16		i;
	
	AT24C32_Init();
	
	EepHandle.UpdateHp = 0;
	EepHandle.UpdateTp = 0;
	EepHandle.pDefault = (U16 *)SYSTEM_EEP_DEFAULT;
	
	EEPROM_Load();
	
	if( EepHandle.Sram[SYSTEM_EEP_PREFIX] != EEPROM_SYS_PREFIX ){
		for( i=SYSTEM_EEP_PREFIX; i<SYSTEM_EEP_MAX; i++ ){
			EEPROM_Sys_Write( i, EepHandle.pDefault[i] );
		}
		EEPROM_CRC_Write();
	}
	
	if( EepHandle.Tag.Prefix != EEPROM_TAG_PREFIX ){
		EepHandle.Tag.Prefix  = EEPROM_TAG_PREFIX;
		
		EEPROM_Tag_Write( &EepHandle.Tag.Prefix, SYSTEM_TAG_PREFIX, 1 );
		
		strcpy( (char *)EepHandle.Tag.SerialNumber,	"GTD-5x00(F)" );
		strcpy( (char *)EepHandle.Tag.Name,			"GASTRON Co., Ltd.    " );
		strcpy( (char *)EepHandle.Tag.Location,		"Factory              " );
		strcpy( (char *)EepHandle.Tag.Message,		"Sampling Type        " );
		
		EEPROM_Tag_Write( EepHandle.Tag.SerialNumber,	SYSTEM_TAG_SERIAL_NUM_00,	 6 );
		EEPROM_Tag_Write( EepHandle.Tag.Name,			SYSTEM_TAG_NAME_00,			11 );
		EEPROM_Tag_Write( EepHandle.Tag.Location,		SYSTEM_TAG_LOCATION_00,		11 );
		EEPROM_Tag_Write( EepHandle.Tag.Message,		SYSTEM_TAG_MESSAGE_00,		11 );
	}
	
	EEPROM_CRC_Check();
	
	return EepHandle.CRC_Error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EEPROM_Task( void )
{
	U8		Hp = EepHandle.UpdateHp & EEPROM_UPDATE_FIFO_MASK;
	U8		Tp = EepHandle.UpdateTp & EEPROM_UPDATE_FIFO_MASK;
	U16		i;
	
	if( Hp != Tp ){
		
		EepHandle.UpdateTp++;
		
		for( i=0; i<EepHandle.UpdateSize[Tp]; i++ ){
			EEPROM_Sys_Write( EepHandle.UpdateStartAddress[Tp] + i, EepHandle.UpdateData[Tp][i] );
		}
		
		if( (EepHandle.UpdateHp & EEPROM_UPDATE_FIFO_MASK) == (EepHandle.UpdateTp & EEPROM_UPDATE_FIFO_MASK) ){
			EEPROM_CRC_Write();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void EEPROM_Update( U16 *pData, U16 Address, U16 Size )
{
	U8		Hp = EepHandle.UpdateHp & EEPROM_UPDATE_FIFO_MASK;
	U8		i;
	
	for( i=0; i<Size; i++ ){ EepHandle.UpdateData[Hp][i] = pData[i]; }
	EepHandle.UpdateStartAddress[Hp] = Address;
	EepHandle.UpdateSize[Hp] = Size;
	EepHandle.UpdateHp++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EEPROM_Tag_Write( U16 *pData, U16 Address, U16 Size )
{
	U8		i;
	
	for( i=0; i<Size; i++ ){
		if( Address + i < EEPROM_SIZE ){
			AT24C32_WriteWord( Address + i, pData[i] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static U16 EEPROM_Read( U16 addr )
{
	return AT24C32_ReadWord( addr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_Sys_Write( U16 addr, U16 data )
{
	if( addr < EEPROM_SIZE ){
		
		if( EepHandle.Sram[addr] != data ){
			EepHandle.Sram[addr]  = data;
			AT24C32_WriteWord( addr, EepHandle.Sram[addr] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_Load( void )
{
	U16		i;
	
	for( i=SYSTEM_EEP_CRC; i<SYSTEM_EEP_MAX; i++ ){
		EepHandle.Sram[i] = EEPROM_Read( i );
	}
	
	EepHandle.Tag.Prefix = EEPROM_Read( SYSTEM_TAG_PREFIX );
	
	for( i=0; i<6; i++ ){
		EepHandle.Tag.SerialNumber[i]	= EEPROM_Read( SYSTEM_TAG_SERIAL_NUM_00	+ i );
	}
	
	for( i=0; i<11; i++ ){
		EepHandle.Tag.Name[i]			= EEPROM_Read( SYSTEM_TAG_NAME_00		+ i );
		EepHandle.Tag.Location[i]		= EEPROM_Read( SYSTEM_TAG_LOCATION_00	+ i );
		EepHandle.Tag.Message[i]		= EEPROM_Read( SYSTEM_TAG_MESSAGE_00	+ i );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_CRC_Write( void )
{
	U16		wCRC;
	
	wCRC = CRC16( (U8*)( &EepHandle.Sram[SYSTEM_EEP_PREFIX] ), ( SYSTEM_EEP_MAX-SYSTEM_EEP_PREFIX ) * 2 );
	EEPROM_Sys_Write( SYSTEM_EEP_CRC, wCRC );
	
	EEPROM_CRC_Check();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_CRC_Check( void )
{
	U16		wCRC;
	
	wCRC = CRC16( (U8*)( &EepHandle.Sram[SYSTEM_EEP_PREFIX] ), ( SYSTEM_EEP_MAX-SYSTEM_EEP_PREFIX ) * 2 );
	
	if( EEPROM_Read( SYSTEM_EEP_CRC ) != wCRC ){
		EepHandle.CRC_Error = true;
	}
	else {
		EepHandle.CRC_Error = false;
	}
}


