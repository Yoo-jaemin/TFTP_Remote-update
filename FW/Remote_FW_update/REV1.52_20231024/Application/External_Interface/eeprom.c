#define __EEPROM_C__
    #include "eeprom.h"
    #include "eeprom_limit_table.h"
#undef  __EEPROM_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "at24c32.h" 
#include "crc16.h"   
#include "led.h"   
#include "ir.h"    
#include "modbus.h"
#include "option.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static U16 EEPROM_Read( U16 addr );
static void EEPROM_Write( U16 addr, U16 data );
static void EEPROM_Load( void );
static void EEPROM_Apply( U16 addr );
static void EEPROM_CRC_Write( void );
static void EEPROM_CRC_Check( void );
/* End Function */
                                     
/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EEPROM_Init( void )
{
	U16		i;
	
	EepHandle.UpdateHp = 0;
	EepHandle.UpdateTp = 0;
	EepHandle.SysDefault = EEP_SYS_DEFAULT;
	
	EEPROM_Load();
	
	if( EepHandle.Sram[EEP_PREFIX] != EEPROM_PREFIX ){
		for( i=EEP_PREFIX; i<EEP_MAX; i++ ){
			EEPROM_Write( i, EepHandle.SysDefault[i] );
		}
		EEPROM_CRC_Write();
	}
	
	EEPROM_CRC_Check();
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
			EEPROM_Write( EepHandle.UpdateStartAddress[Tp] + i, EepHandle.UpdateData[Tp][i] );
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
	
	for( i=0; i<Size; i++ ){ EepHandle.UpdateData[Hp][i] = *(pData+i); }
	EepHandle.UpdateStartAddress[Hp] = Address;
	EepHandle.UpdateSize[Hp] = Size;
	EepHandle.UpdateHp++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static U16 EEPROM_Read( U16 addr )
{
	return AT24C32_ReadWord( addr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_Write( U16 addr, U16 data )
{
	if( addr < EEPROM_SIZE ){
		
		if( EepHandle.Sram[addr] != data ){
			EepHandle.Sram[addr]  = data;
			AT24C32_WriteWord( addr, EepHandle.Sram[addr] );
			EEPROM_Apply( addr );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_Load( void )
{
	U16		i;
	
	for( i=EEP_CRC; i<EEP_MAX; i++ ){
		EepHandle.Sram[i] = EEPROM_Read( i );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_Apply( U16 addr )
{
	if( addr < EEPROM_SIZE ){
		
		switch( addr ){
			case EEP_SYS_LOG_MODE				: IrProcess.LogMode				= EepHandle.Sram[addr];				break;
			case EEP_SYS_MODBUS_ADDRESS			: Modbus_Addr					= EepHandle.Sram[addr];				break;
			case EEP_SYS_AMP_CAL_RUN_FLAG		: IrAmpCal.RunFlag				= EepHandle.Sram[addr];				break;
			case EEP_IR1_AMP_CAL_PTM_MANUAL_OFFS: IrAmpCal.ManualOffs[IR_WL_1]	= EepHandle.Sram[addr];				break;
			case EEP_IR1_AMP_CAL_PTM_MANUAL_GAIN: IrAmpCal.ManualGain[IR_WL_1]	= EepHandle.Sram[addr];				break;
			case EEP_IR1_AMP_CAL_TARGET_mV_OFFS	: IrAmpCal.TargetOffs[IR_WL_1]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR1_AMP_CAL_TARGET_mV_GAIN	: IrAmpCal.TargetGain[IR_WL_1]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR2_AMP_CAL_PTM_MANUAL_OFFS: IrAmpCal.ManualOffs[IR_WL_2]	= EepHandle.Sram[addr];				break;
			case EEP_IR2_AMP_CAL_PTM_MANUAL_GAIN: IrAmpCal.ManualGain[IR_WL_2]	= EepHandle.Sram[addr];				break;
			case EEP_IR2_AMP_CAL_TARGET_mV_OFFS	: IrAmpCal.TargetOffs[IR_WL_2]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR2_AMP_CAL_TARGET_mV_GAIN	: IrAmpCal.TargetGain[IR_WL_2]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR3_AMP_CAL_PTM_MANUAL_OFFS: IrAmpCal.ManualOffs[IR_WL_3]	= EepHandle.Sram[addr];				break;
			case EEP_IR3_AMP_CAL_PTM_MANUAL_GAIN: IrAmpCal.ManualGain[IR_WL_3]	= EepHandle.Sram[addr];				break;
			case EEP_IR3_AMP_CAL_TARGET_mV_OFFS	: IrAmpCal.TargetOffs[IR_WL_3]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR3_AMP_CAL_TARGET_mV_GAIN	: IrAmpCal.TargetGain[IR_WL_3]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR4_AMP_CAL_PTM_MANUAL_OFFS: IrAmpCal.ManualOffs[IR_WL_4]	= EepHandle.Sram[addr];				break;
			case EEP_IR4_AMP_CAL_PTM_MANUAL_GAIN: IrAmpCal.ManualGain[IR_WL_4]	= EepHandle.Sram[addr];				break;
			case EEP_IR4_AMP_CAL_TARGET_mV_OFFS	: IrAmpCal.TargetOffs[IR_WL_4]	= EepHandle.Sram[addr] * 0.001f;	break;
			case EEP_IR4_AMP_CAL_TARGET_mV_GAIN	: IrAmpCal.TargetGain[IR_WL_4]	= EepHandle.Sram[addr] * 0.001f;	break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_CRC_Write( void )
{
	U16		wCRC;
	
	wCRC = CRC16( (U8*)( &EepHandle.Sram[EEP_PREFIX] ), ( EEP_MAX-EEP_PREFIX ) * 2 );
	EEPROM_Write( EEP_CRC, wCRC );
	
	EEPROM_CRC_Check();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void EEPROM_CRC_Check( void )
{
	U16		wCRC;
	
	wCRC = CRC16( (U8*)( &EepHandle.Sram[EEP_PREFIX] ), ( EEP_MAX-EEP_PREFIX ) * 2 );
	
	if( EEPROM_Read( EEP_CRC ) != wCRC ){
		SystemError.Bit.Eeprom_CRC = 1;
	}
	else {
		SystemError.Bit.Eeprom_CRC = 0;
	}
}


