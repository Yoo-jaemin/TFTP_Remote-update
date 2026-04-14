#define __MODBUS_MAP_C__
    #include "modbus_map.h"
#undef  __MODBUS_MAP_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "option.h"
#include "version.h"
#include "measuring.h"
/* End Include */

/* Start Define */          
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Mapping( void )
{
	U16		i, iScope[2];
	uuword	Scope[2];
	
	// Information
	ModbusMap._3x[MODBUS_30001] = 0;
	ModbusMap._3x[MODBUS_30002] = (U16)(HardwareVersion * 10);
	ModbusMap._3x[MODBUS_30003] = (U16)(FirmwareVersion * 100);
	ModbusMap._3x[MODBUS_30004] = 0;
	ModbusMap._3x[MODBUS_30005] = 0;
	ModbusMap._3x[MODBUS_30006] = MeasHandle.pSen->ADC_BuffSize * 2;
	ModbusMap._3x[MODBUS_30007] = 0;
	ModbusMap._3x[MODBUS_30008] = SystemOption.Item.Model;
	
	switch( SystemOption.Item.Model ){
		case MODEL_PROTO:
		case MODEL_5000N:
		case MODEL_5100FN:
		case MODEL_5200N:
		case MODEL_5200FN:		ModbusMap._3x[MODBUS_30009] = 1; break;
		case MODEL_5200N_DUAL:
		case MODEL_5200FN_DUAL:	ModbusMap._3x[MODBUS_30009] = 2; break;
	}
	
	// Status & Calculated Data
	ModbusMap._3x[MODBUS_30010]++;
	ModbusMap._3x[MODBUS_30011] = 0;
	ModbusMap._3x[MODBUS_30012] = 0;
	ModbusMap._3x[MODBUS_30013] = 0;
	ModbusMap._3x[MODBUS_30014] = 0;
	ModbusMap._3x[MODBUS_30015] = 0;
	ModbusMap._3x[MODBUS_30016] = 0;
	ModbusMap._3x[MODBUS_30017] = 0;
	ModbusMap._3x[MODBUS_30018] = MeasHandle.Data.SenHumi.w.l;
	ModbusMap._3x[MODBUS_30019] = MeasHandle.Data.SenHumi.w.h;
	ModbusMap._3x[MODBUS_30020] = MeasHandle.Data.SenTemp.w.l;
	ModbusMap._3x[MODBUS_30021] = MeasHandle.Data.SenTemp.w.h;
	ModbusMap._3x[MODBUS_30022] = MeasHandle.Data.McuTemp.w.l;
	ModbusMap._3x[MODBUS_30023] = MeasHandle.Data.McuTemp.w.h;
	ModbusMap._3x[MODBUS_30024] = MeasHandle.Data.AVDD.w.l;
	ModbusMap._3x[MODBUS_30025] = MeasHandle.Data.AVDD.w.h;
	ModbusMap._3x[MODBUS_30026] = MeasHandle.Data.DVDD.w.l;
	ModbusMap._3x[MODBUS_30027] = MeasHandle.Data.DVDD.w.h;
	ModbusMap._3x[MODBUS_30028] = MeasHandle.Data.VCB.w.l;
	ModbusMap._3x[MODBUS_30029] = MeasHandle.Data.VCB.w.h;
	
	// Sensing Data
	ModbusMap._3x[MODBUS_30030] = MeasHandle.Data.RawDvSum.Item.LPTIA0_LPF;
	ModbusMap._3x[MODBUS_30031] = MeasHandle.Data.RawDvSum.Item.VZERO0;
	ModbusMap._3x[MODBUS_30032] = MeasHandle.Data.RawDvSum.Item.VBIAS0;
	ModbusMap._3x[MODBUS_30033] = MeasHandle.Data.PeakDv.Item.LPTIA0_LPF;
	ModbusMap._3x[MODBUS_30034] = MeasHandle.Data.PeakDv.Item.VZERO0;
	ModbusMap._3x[MODBUS_30035] = MeasHandle.Data.PeakDv.Item.VBIAS0;
	ModbusMap._3x[MODBUS_30036] = MeasHandle.Data.LPTIA0_LPF.w.l;
	ModbusMap._3x[MODBUS_30037] = MeasHandle.Data.LPTIA0_LPF.w.h;
	ModbusMap._3x[MODBUS_30038] = MeasHandle.Data.VZERO0.w.l;
	ModbusMap._3x[MODBUS_30039] = MeasHandle.Data.VZERO0.w.h;
	ModbusMap._3x[MODBUS_30040] = MeasHandle.Data.VBIAS0.w.l;
	ModbusMap._3x[MODBUS_30041] = MeasHandle.Data.VBIAS0.w.h;
	ModbusMap._3x[MODBUS_30042] = MeasHandle.Data.ICB0.w.l;
	ModbusMap._3x[MODBUS_30043] = MeasHandle.Data.ICB0.w.h;
	ModbusMap._3x[MODBUS_30044] = 0;
	ModbusMap._3x[MODBUS_30045] = 0;
	ModbusMap._3x[MODBUS_30046] = 0;
	ModbusMap._3x[MODBUS_30047] = 0;
	ModbusMap._3x[MODBUS_30048] = 0;
	ModbusMap._3x[MODBUS_30049] = 0;
	ModbusMap._3x[MODBUS_30050] = 0;
	ModbusMap._3x[MODBUS_30051] = 0;
	ModbusMap._3x[MODBUS_30052] = 0;
	ModbusMap._3x[MODBUS_30053] = 0;
	ModbusMap._3x[MODBUS_30054] = 0;
	ModbusMap._3x[MODBUS_30055] = MeasHandle.Data.RawDvSum.Item.LPTIA1_LPF;
	ModbusMap._3x[MODBUS_30056] = MeasHandle.Data.RawDvSum.Item.VZERO1;
	ModbusMap._3x[MODBUS_30057] = MeasHandle.Data.RawDvSum.Item.VBIAS1;
	ModbusMap._3x[MODBUS_30058] = MeasHandle.Data.PeakDv.Item.LPTIA1_LPF;
	ModbusMap._3x[MODBUS_30059] = MeasHandle.Data.PeakDv.Item.VZERO1;
	ModbusMap._3x[MODBUS_30060] = MeasHandle.Data.PeakDv.Item.VBIAS1;
	ModbusMap._3x[MODBUS_30061] = MeasHandle.Data.LPTIA1_LPF.w.l;
	ModbusMap._3x[MODBUS_30062] = MeasHandle.Data.LPTIA1_LPF.w.h;
	ModbusMap._3x[MODBUS_30063] = MeasHandle.Data.VZERO1.w.l;
	ModbusMap._3x[MODBUS_30064] = MeasHandle.Data.VZERO1.w.h;
	ModbusMap._3x[MODBUS_30065] = MeasHandle.Data.VBIAS1.w.l;
	ModbusMap._3x[MODBUS_30066] = MeasHandle.Data.VBIAS1.w.h;
	ModbusMap._3x[MODBUS_30067] = MeasHandle.Data.ICB1.w.l;
	ModbusMap._3x[MODBUS_30068] = MeasHandle.Data.ICB1.w.h;
	ModbusMap._3x[MODBUS_30069] = 0;
	ModbusMap._3x[MODBUS_30070] = 0;
	ModbusMap._3x[MODBUS_30071] = 0;
	ModbusMap._3x[MODBUS_30072] = 0;
	ModbusMap._3x[MODBUS_30073] = 0;
	ModbusMap._3x[MODBUS_30074] = 0;
	ModbusMap._3x[MODBUS_30075] = 0;
	ModbusMap._3x[MODBUS_30076] = 0;
	ModbusMap._3x[MODBUS_30077] = 0;
	ModbusMap._3x[MODBUS_30078] = 0;
	ModbusMap._3x[MODBUS_30079] = 0;
	
	// Below are scope data
	iScope[0] = MODBUS_30110;
	iScope[1] = iScope[0] + MeasHandle.pSen->ADC_BuffSize + 1;
	
	ModbusMap._3x[iScope[0]] = ModbusMap._3x[MODBUS_30010];
	ModbusMap._3x[iScope[1]] = ModbusMap._3x[MODBUS_30010];
	
	iScope[0]++;
	iScope[1]++;
	
	for( i=0; i<MeasHandle.pSen->ADC_BuffSize; i++ ){
		Scope[0].b.l = (U8)(MeasHandle.Data.LPTIA0_LPF.f / 0.03125);
		Scope[0].b.h = Scope[0].b.l;
		Scope[1].b.l = (U8)(MeasHandle.Data.LPTIA1_LPF.f / 0.03125);
		Scope[1].b.h = Scope[1].b.l;
		ModbusMap._3x[iScope[0]+i] = Scope[0].w;
		ModbusMap._3x[iScope[1]+i] = Scope[1].w;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Map_Update_Check( void )
{
#if 0
	if( ModbusMap._3x[MODBUS_30009] != (ModbusHandle.Slave.pReg_4x[FLASH_ADDR_SYS_ADC_BUFF_SIZE] * 2) ){
		ModbusMap._3x[MODBUS_30009]  = (ModbusHandle.Slave.pReg_4x[FLASH_ADDR_SYS_ADC_BUFF_SIZE] * 2);
	}
#endif
}


