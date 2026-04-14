#define __FLASH_C__
    #include "flash.h"
#undef  __FLASH_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "DmaLib.h"
#include "crc16.h"
#include "main.h"
#include "gpt.h"
#include "tia.h"
/* End Include */

/* Start Define */
#define FLASH_ERR_CNT			( 1000 )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void FLASH_Write_DMA( U16 *pData, U16 Addr, U16 Size );
static U16 FLASH_Read( U16 Addr );
static void FLASH_Load( void );
static U16 FLASH_Make_CRC( void );
static void FLASH_CRC_Check( void );
/* End Function */
                                     
/* Start Variable */
static const U16    SYSTEM_FLASH_DEFAULT[FLASH_ADDR_MAX] =
{
	0,						// (40001) FLASH_ADDR_SYS_CRC
	FLASH_SYS_PREFIX,		// (40002) FLASH_ADDR_SYS_PREFIX
	0,						// (40003) FLASH_ADDR_SYS_MODBUS_TIMEOUT_TICK_100uS
	0,						// (40004) FLASH_ADDR_SYS_LOG_MODE
	0,						// (40005) FLASH_ADDR_SYS_RES_05
	0,						// (40006) FLASH_ADDR_SYS_RES_06
	0,						// (40007) FLASH_ADDR_SYS_MODBUS_ADDRESS
	0,						// (40008) FLASH_ADDR_SYS_RES_08
	0,						// (40009) FLASH_ADDR_SYS_RES_09
	0,						// (40010) FLASH_ADDR_SYS_RES_10
	0,						// (40011) FLASH_ADDR_SYS_RES_11
	0,						// (40012) FLASH_ADDR_SYS_RES_12
	0,						// (40013) FLASH_ADDR_SYS_RES_13
	0,						// (40014) FLASH_ADDR_SYS_RES_14
	0,						// (40015) FLASH_ADDR_SYS_RES_15
	0,						// (40016) FLASH_ADDR_SYS_RES_16
	0,						// (40017) FLASH_ADDR_SYS_RES_17
	0,						// (40018) FLASH_ADDR_SYS_RES_18
	90,						// (40019) FLASH_ADDR_SYS_ADC_BUFF_SIZE
	100,					// (40020) FLASH_ADDR_SYS_SENSING_PERIOD
	0,						// (40021) FLASH_ADDR_SYS_RES_21
	0,						// (40022) FLASH_ADDR_SYS_RES_22
	0,						// (40023) FLASH_ADDR_SYS_RES_23
	0,						// (40024) FLASH_ADDR_SYS_RES_24
	0,						// (40025) FLASH_ADDR_SYS_RES_25
	0,						// (40026) FLASH_ADDR_SYS_RES_26
	0,						// (40027) FLASH_ADDR_SYS_AMP_CAL_RUN_FLAG
	0,						// (40028) FLASH_ADDR_SYS_RES_28
	0,						// (40029) FLASH_ADDR_SYS_RES_29
	0,						// (40030) FLASH_ADDR_SYS_RES_30
	0,						// (40031) FLASH_ADDR_SYS_RES_31
	0,						// (40032) FLASH_ADDR_SYS_RES_32
	0,						// (40033) FLASH_ADDR_SYS_DAC_RESET_CNT
	0,						// (40034) FLASH_ADDR_SYS_RES_34
	0,						// (40035) FLASH_ADDR_SYS_RES_35
	0,						// (40036) FLASH_ADDR_SYS_RES_36
	0,						// (40037) FLASH_ADDR_SYS_RES_37
	0,						// (40038) FLASH_ADDR_SYS_CTG_SEN_TYPE_0
	0,						// (40039) FLASH_ADDR_SYS_CTG_SEN_TYPE_1
	0,						// (40040) FLASH_ADDR_SYS_CTG_CONF_NO_0
	0,						// (40041) FLASH_ADDR_SYS_CTG_CONF_NO_1
	0,						// (40042) FLASH_ADDR_SYS_CVT_CONF_NO
	0,						// (40043) FLASH_ADDR_SYS_RES_43
	0,						// (40044) FLASH_ADDR_SYS_RES_44
	0,						// (40045) FLASH_ADDR_SYS_RES_45
	0,						// (40046) FLASH_ADDR_SYS_RES_46
	0,						// (40047) FLASH_ADDR_SYS_RES_47
	0,						// (40048) FLASH_ADDR_SYS_RES_48
	0,						// (40049) FLASH_ADDR_SYS_RES_49
	0,						// (40050) FLASH_ADDR_SYS_RES_50
	0,						// (40051) FLASH_ADDR_SYS_RES_51
	0,						// (40052) FLASH_ADDR_SYS_RES_52
	0,						// (40053) FLASH_ADDR_SYS_RES_53
	0,						// (40054) FLASH_ADDR_SYS_RES_54
	0,						// (40055) FLASH_ADDR_SYS_RES_55
	0,						// (40056) FLASH_ADDR_SYS_RES_56
	0,						// (40057) FLASH_ADDR_SYS_RES_57
	0,						// (40058) FLASH_ADDR_SYS_RES_58
	0,						// (40059) FLASH_ADDR_SYS_RES_59
	0,						// (40060) FLASH_ADDR_SYS_RES_60
	0,						// (40061) FLASH_ADDR_SYS_RES_61
	0,						// (40062) FLASH_ADDR_SYS_RES_62
	0,						// (40063) FLASH_ADDR_SYS_RES_63
	0,						// (40064) FLASH_ADDR_SYS_RES_64
	0,						// (40065) FLASH_ADDR_CH0_PREFIX
	0,						// (40066) FLASH_ADDR_CH0_PTM_mOFFS_TIA_0
	LPTIA_SWMODE_OFF,		// (40067) FLASH_ADDR_CH0_PTM_mGAIN_TIA_1
	400,					// (40068) FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_OFFS
	1800,					// (40069) FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_GAIN
	400,					// (40070) FLASH_ADDR_CH0_AMP_CAL_TARGET_mV_BIAS
	30,						// (40071) FLASH_ADDR_CH0_STABLE_HOLD_TIME_SEC
	52429,					// (40072) FLASH_ADDR_CH0_mZERO_L
	16076,					// (40073) FLASH_ADDR_CH0_mZERO_H
	0,						// (40074) FLASH_ADDR_CH0_mZERO_TEMP_L
	16932,					// (40075) FLASH_ADDR_CH0_mZERO_TEMP_H
	26214,					// (40076) FLASH_ADDR_CH0_mSPAN_L
	16358,					// (40077) FLASH_ADDR_CH0_mSPAN_H
	0,						// (40078) FLASH_ADDR_CH0_mSPAN_TEMP_L
	16932,					// (40079) FLASH_ADDR_CH0_mSPAN_TEMP_H
	50,						// (40080) FLASH_ADDR_CH0_SPAN_GAS
	52429,					// (40081) FLASH_ADDR_CH0_bZERO_L
	16076,					// (40082) FLASH_ADDR_CH0_bZERO_H
	0,						// (40083) FLASH_ADDR_CH0_bZERO_TEMP_L
	16932,					// (40084) FLASH_ADDR_CH0_bZERO_TEMP_H
	26214,					// (40085) FLASH_ADDR_CH0_bSPAN_L
	16358,					// (40086) FLASH_ADDR_CH0_bSPAN_H
	0,						// (40087) FLASH_ADDR_CH0_bSPAN_TEMP_L
	16932,					// (40088) FLASH_ADDR_CH0_bSPAN_TEMP_H
	0,						// (40089) FLASH_ADDR_CH0_RES_25
	0,						// (40090) FLASH_ADDR_CH0_GAS_NAME_0
	0,						// (40091) FLASH_ADDR_CH0_GAS_NAME_1
	0,						// (40092) FLASH_ADDR_CH0_GAS_NAME_2
	0,						// (40093) FLASH_ADDR_CH0_GAS_NAME_3
	0,						// (40094) FLASH_ADDR_CH0_GAS_NAME_4
	0,						// (40095) FLASH_ADDR_CH0_GAS_NAME_5
	0,						// (40096) FLASH_ADDR_CH0_RES_32
	false,					// (40097) FLASH_ADDR_CH0_bZERO_USE
	30,						// (40098) FLASH_ADDR_CH0_bZERO_PERCENTAGE_x10
	120,					// (40099) FLASH_ADDR_CH0_bZERO_SEC
	0,						// (40100) FLASH_ADDR_CH0_bZERO_COUNT
	1,						// (40101) FLASH_ADDR_CH0_METHOD
	0,						// (40102) FLASH_ADDR_CH0_SEN_NO
	0,						// (40103) FLASH_ADDR_CH0_GAS_NO
	0,						// (40104) FLASH_ADDR_CH0_DP
	100,					// (40105) FLASH_ADDR_CH0_HIGH_SCALE
	1,						// (40106) FLASH_ADDR_CH0_UNIT
	0,						// (40107) FLASH_ADDR_CH0_MAINTENANCE_GAS
	100,					// (40108) FLASH_ADDR_CH0_CROSS_SENS_SCALE_x100
	30,						// (40109) FLASH_ADDR_CH0_ZERO_SKIP_PERCENTAGE_x10
	30,						// (40110) FLASH_ADDR_CH0_SPAN_SKIP_PERCENTAGE_x10
	50,						// (40111) FLASH_ADDR_CH0_OUT_DELAY_PERCENTAGE_x10
	5,						// (40112) FLASH_ADDR_CH0_OUT_DELAY_SEC
	false,					// (40113) FLASH_ADDR_CH0_ALARM_UNDER_CHECK
	false,					// (40114) FLASH_ADDR_CH0_ALARM_1ST_LATCH
	true,					// (40115) FLASH_ADDR_CH0_ALARM_1ST_INCREASE_TYPE
	20,						// (40116) FLASH_ADDR_CH0_ALARM_1ST_LEVEL
	0,						// (40117) FLASH_ADDR_CH0_ALARM_1ST_DEAD_BAND
	0,						// (40118) FLASH_ADDR_CH0_ALARM_1ST_DELAY_SEC
	false,					// (40119) FLASH_ADDR_CH0_ALARM_2ND_LATCH
	true,					// (40120) FLASH_ADDR_CH0_ALARM_2ND_INCREASE_TYPE
	40,						// (40121) FLASH_ADDR_CH0_ALARM_2ND_LEVEL
	0,						// (40122) FLASH_ADDR_CH0_ALARM_2ND_DEAD_BAND
	0,						// (40123) FLASH_ADDR_CH0_ALARM_2ND_DELAY_SEC
	0,						// (40124) FLASH_ADDR_CH0_RES_60
	0,						// (40125) FLASH_ADDR_CH0_RES_61
	0,						// (40126) FLASH_ADDR_CH0_RES_62
	0,						// (40127) FLASH_ADDR_CH0_RES_63
	0,						// (40128) FLASH_ADDR_CH0_RES_64
	0,						// (40129) FLASH_ADDR_CH1_PREFIX
	0,						// (40130) FLASH_ADDR_CH1_PTM_mOFFS_TIA_0
	LPTIA_SWMODE_OFF,		// (40131) FLASH_ADDR_CH1_PTM_mGAIN_TIA_1
	400,					// (40132) FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_OFFS
	1800,					// (40133) FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_GAIN
	400,					// (40134) FLASH_ADDR_CH1_AMP_CAL_TARGET_mV_BIAS
	30,						// (40135) FLASH_ADDR_CH1_STABLE_HOLD_TIME_SEC
	52429,					// (40136) FLASH_ADDR_CH1_mZERO_L
	16076,					// (40137) FLASH_ADDR_CH1_mZERO_H
	0,						// (40138) FLASH_ADDR_CH1_mZERO_TEMP_L
	16932,					// (40139) FLASH_ADDR_CH1_mZERO_TEMP_H
	26214,					// (40140) FLASH_ADDR_CH1_mSPAN_L
	16358,					// (40141) FLASH_ADDR_CH1_mSPAN_H
	0,						// (40142) FLASH_ADDR_CH1_mSPAN_TEMP_L
	16932,					// (40143) FLASH_ADDR_CH1_mSPAN_TEMP_H
	50,						// (40144) FLASH_ADDR_CH1_SPAN_GAS
	52429,					// (40145) FLASH_ADDR_CH1_bZERO_L
	16076,					// (40146) FLASH_ADDR_CH1_bZERO_H
	0,						// (40147) FLASH_ADDR_CH1_bZERO_TEMP_L
	16932,					// (40148) FLASH_ADDR_CH1_bZERO_TEMP_H
	26214,					// (40149) FLASH_ADDR_CH1_bSPAN_L
	16358,					// (40150) FLASH_ADDR_CH1_bSPAN_H
	0,						// (40151) FLASH_ADDR_CH1_bSPAN_TEMP_L
	16932,					// (40152) FLASH_ADDR_CH1_bSPAN_TEMP_H
	0,						// (40153) FLASH_ADDR_CH1_RES_25
	0,						// (40154) FLASH_ADDR_CH1_GAS_NAME_0
	0,						// (40155) FLASH_ADDR_CH1_GAS_NAME_1
	0,						// (40156) FLASH_ADDR_CH1_GAS_NAME_2
	0,						// (40157) FLASH_ADDR_CH1_GAS_NAME_3
	0,						// (40158) FLASH_ADDR_CH1_GAS_NAME_4
	0,						// (40159) FLASH_ADDR_CH1_GAS_NAME_5
	0,						// (40160) FLASH_ADDR_CH1_RES_32
	false,					// (40161) FLASH_ADDR_CH1_bZERO_USE
	30,						// (40162) FLASH_ADDR_CH1_bZERO_PERCENTAGE_x10
	120,					// (40163) FLASH_ADDR_CH1_bZERO_SEC
	0,						// (40164) FLASH_ADDR_CH1_bZERO_COUNT
	1,						// (40165) FLASH_ADDR_CH1_METHOD
	0,						// (40166) FLASH_ADDR_CH1_SEN_NO
	0,						// (40167) FLASH_ADDR_CH1_GAS_NO
	0,						// (40168) FLASH_ADDR_CH1_DP
	100,					// (40169) FLASH_ADDR_CH1_HIGH_SCALE
	1,						// (40170) FLASH_ADDR_CH1_UNIT
	0,						// (40171) FLASH_ADDR_CH1_MAINTENANCE_GAS
	100,					// (40172) FLASH_ADDR_CH1_CROSS_SENS_SCALE_x100
	30,						// (40173) FLASH_ADDR_CH1_ZERO_SKIP_PERCENTAGE_x10
	30,						// (40174) FLASH_ADDR_CH1_SPAN_SKIP_PERCENTAGE_x10
	50,						// (40175) FLASH_ADDR_CH1_OUT_DELAY_PERCENTAGE_x10
	5,						// (40176) FLASH_ADDR_CH1_OUT_DELAY_SEC
	false,					// (40177) FLASH_ADDR_CH1_ALARM_UNDER_CHECK
	false,					// (40178) FLASH_ADDR_CH1_ALARM_1ST_LATCH
	true,					// (40179) FLASH_ADDR_CH1_ALARM_1ST_INCREASE_TYPE
	20,						// (40180) FLASH_ADDR_CH1_ALARM_1ST_LEVEL
	0,						// (40181) FLASH_ADDR_CH1_ALARM_1ST_DEAD_BAND
	0,						// (40182) FLASH_ADDR_CH1_ALARM_1ST_DELAY_SEC
	false,					// (40183) FLASH_ADDR_CH1_ALARM_2ND_LATCH
	true,					// (40184) FLASH_ADDR_CH1_ALARM_2ND_INCREASE_TYPE
	40,						// (40185) FLASH_ADDR_CH1_ALARM_2ND_LEVEL
	0,						// (40186) FLASH_ADDR_CH1_ALARM_2ND_DEAD_BAND
	0,						// (40187) FLASH_ADDR_CH1_ALARM_2ND_DELAY_SEC
	0,						// (40188) FLASH_ADDR_CH1_RES_60
	0,						// (40189) FLASH_ADDR_CH1_RES_61
	0,						// (40190) FLASH_ADDR_CH1_RES_62
	0,						// (40191) FLASH_ADDR_CH1_RES_63
	0						// (40192) FLASH_ADDR_CH1_RES_64
};
U16		FLASH_ErrCntMin;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLASH_Init( void )
{
	U16		i;
	
	DmaBase(); //Init data base pointer of DMA controller
	NVIC_EnableIRQ( DMA0_CH15_DONE_IRQn ); //flash dma interrupt
	DmaPeripheralStructSetup( Flash_C, DMA_DSTINC_NO | DMA_SRCINC_HWORD | DMA_SIZE_HWORD ); // Setup Flash DMA structure
	
	Flash_Handle.IsWriting = false;
	Flash_Handle.pDefault = (U16 *)SYSTEM_FLASH_DEFAULT;
	Flash_Handle.Update.Byte = 0;
	
	FLASH_Load();
	
	if( Flash_Handle.Sram[FLASH_ADDR_SYS_PREFIX] != FLASH_SYS_PREFIX ){
		for( i=FLASH_ADDR_SYS_PREFIX; i<FLASH_ADDR_MAX; i++ ){
			Flash_Handle.Sram[i] = Flash_Handle.pDefault[i];
		}
		Flash_Handle.Sram[FLASH_ADDR_SYS_CRC] = FLASH_Make_CRC();
		FLASH_Write_DMA( Flash_Handle.Sram, FLASH_ADDR_SYS_CRC, FLASH_ADDR_MAX );
	}
	
	Flash_Handle.CRC_Check = true;
	
	FLASH_ErrCntMin = FLASH_ERR_CNT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void FLASH_Task( void )
{
	if( TaskSyncTimer.Evt.Bit._40_MSEC_1 == true ){
		TaskSyncTimer.Evt.Bit._40_MSEC_1 = false;
		
		if( Flash_Handle.Update.Item.IsEnabled == true ){
			if( Flash_Handle.Update.Item.Cnt ){
				Flash_Handle.Update.Item.Cnt--;
			}
			else {
				if( Flash_Handle.IsWriting == false ){
					Flash_Handle.IsWriting = true;
					
					Flash_Handle.Update.Item.IsEnabled = false;
					FLASH_Write_DMA( Flash_Handle.Sram, FLASH_ADDR_SYS_CRC, FLASH_ADDR_MAX );
				}
			}
		}
	}
	FLASH_CRC_Check();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void FLASH_Update( U16 *pData, U16 Addr, U16 Size )
{
	U16		i;
	
	for( i=0; i<Size; i++ ){
		Flash_Handle.Sram[Addr + i] = pData[i];
	}
	Flash_Handle.Sram[FLASH_ADDR_SYS_CRC] = FLASH_Make_CRC();
	
	Flash_Handle.Update.Item.Cnt = 4; // 40 msec * 5(4 3 2 1 0) = 200 msec
	Flash_Handle.Update.Item.IsEnabled = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FLASH_Write_DMA( U16 *pData, U16 Addr, U16 Size )
{
	U16			err_cnt = FLASH_ERR_CNT;
	uint32_t	DataAddr = FLASH_BASE + FLASH_DATA_ADDR( Addr );
	
	while( pADI_FLCC0->STAT & BITM_FLCC_STAT_CMDBUSY ){
		if( --err_cnt == 0 ){
			SystemError.Bit.Flash = true;
			return;
		}
	}
	delay_10us( 100 ); // Even with a Busy check, it can't be written quickly.....

	pADI_FLCC0->KEY = ENUM_FLCC_KEY_USERKEY;
	pADI_FLCC0->PAGE_ADDR0 = FLASH_BASE;
	pADI_FLCC0->CMD = ENUM_FLCC_CMD_ERASEPAGE;
	
	while( pADI_FLCC0->STAT & BITM_FLCC_STAT_CMDBUSY ){
		if( --err_cnt == 0 ){
			SystemError.Bit.Flash = true;
			return;
		}
	}
	
	if( FLASH_ErrCntMin > err_cnt ){
		FLASH_ErrCntMin = err_cnt;
	}
	
	SystemError.Bit.Flash = false;
	
	DmaStructPtrOutSetup( Flash_C, Size, (unsigned char *)pData );	// Setup source/destination pointer fields
	DmaSet( 0, Flash_B, 0, 0 );										// Enable DMA structure for flash
	DmaGo( Flash_C, Size, DMA_BASIC );								// Enable Flash DMA structure
	
	pADI_FLCC0->KH_ADDR	= DataAddr;
	pADI_FLCC0->KEY		= ENUM_FLCC_KEY_USERKEY;
	pADI_FLCC0->UCFG	|= BITM_FLCC_UCFG_KHDMAEN;					// Enable Flash DMA mode, this starts the DMA transfer
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static U16 FLASH_Read( U16 Addr )
{
	uint32_t	*pAddr = (uint32_t *)( FLASH_BASE + FLASH_DATA_ADDR( Addr ) );
	
	return (U16)(*pAddr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FLASH_Load( void )
{
	U16		i;
	
	for( i=FLASH_ADDR_SYS_CRC; i<FLASH_ADDR_MAX; i++ ){
		Flash_Handle.Sram[i] = FLASH_Read( i );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static U16 FLASH_Make_CRC( void )
{
	return CRC16( (U8*)( &Flash_Handle.Sram[FLASH_ADDR_SYS_PREFIX] ), ( FLASH_ADDR_MAX-FLASH_ADDR_SYS_PREFIX ) * 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void FLASH_CRC_Check( void )
{	
	if( Flash_Handle.CRC_Check == true ){
		Flash_Handle.CRC_Check = false;
		
		if( FLASH_Read( FLASH_ADDR_SYS_CRC ) != FLASH_Make_CRC() ){
			SystemError.Bit.Flash_CRC = true;
		}
		else {
			SystemError.Bit.Flash_CRC = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DMA_FLASH0_Int_Handler( void )
{
	pADI_FLCC0->KEY = ENUM_FLCC_KEY_USERKEY;
	pADI_FLCC0->UCFG &= (~BITM_FLCC_UCFG_KHDMAEN); // Disable Flash DMA mode
	
	Flash_Handle.IsWriting = false;
	Flash_Handle.CRC_Check = true;
}


