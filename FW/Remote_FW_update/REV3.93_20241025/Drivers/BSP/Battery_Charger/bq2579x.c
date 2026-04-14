#define __BQ2579X_C__
    #include "bq2579x.h"
#undef  __BQ2579X_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c2.h"
/* End Include */

/* Start Define */
#define BQ2579X_DEVICE_ADDRESS			( 0x6B )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
    U8	Byte[18];
	struct{
        U8					vSysMin			:  6; // 0- 0~ 5
        U8					Reserved_0		:  2; // 0- 6~ 7
        U8					Dummy_0			:  8; // 0- 8~15
					                        
		U16					vReg			: 11; // 1- 0~10
		U8					Reserved_1		:  5; // 1-11~15
					                        
		U16					iCharge			:  9; // 2- 0~ 8
		U8					Reserved_2		:  7; // 2- 9~15
					                        
		U8					vInLmt			:  8; // 3- 0~ 7
        U8					Dummy_1			:  8; // 3- 8~15
					                        
		U16					iInLmt			:  9; // 4- 0~ 8
		U8					Reserved_3		:  7; // 4- 9~15
		                                    
		U8					iPrecharge		:  6; // 5- 0~ 5
		BQ2579x_VBAT_LOWV	vBatLow			:  2; // 5- 6~ 7
        U8					Dummy_2			:  8; // 5- 8~15
		                                    
		U8					iTermination	:  5; // 6- 0~ 4
		bool				Reserved_4		:  1; // 6- 5
		bool				RegRst			:  1; // 6- 6
		bool				Reserved_5		:  1; // 6- 7
        U8					Dummy_3			:  8; // 6- 8~15
		                                    
		U8					vReCharge		:  4; // 7- 0~ 4
		BQ2579x_TRECHG		tReCharge		:  2; // 7- 4~ 5
		BQ2579x_CELL		Cell			:  2; // 7- 6~ 7
        U8					Dummy_4			:  8; // 7- 8~15
					                        
		U16					iLmtICO			:  9; // 8- 0~ 8
		U8					Reserved_6		:  7; // 8- 9~15
    }Item;
}BQ2579x_Charge_Profile_Parsing;

typedef union {
    U8	Byte;
	struct{
		U8					iPrecharge		:  6; // 5- 0~ 5
		BQ2579x_VBAT_LOWV	vBatLow			:  2; // 5- 6~ 7
    }Item;
}BQ2579x_Precharge_t;

typedef union {
    U8	Byte;
	struct{
		U8					iTermination	:  5; // 6- 0~ 4
		bool				Reserved_4		:  1; // 6- 5
		bool				RegRst			:  1; // 6- 6
		bool				Reserved_5		:  1; // 6- 7
    }Item;
}BQ2579x_Termination_t;

typedef union {
    U8	Byte;
	struct{
		U8					vReCharge		:  4; // 7- 0~ 4
		BQ2579x_TRECHG		tReCharge		:  2; // 7- 4~ 5
		BQ2579x_CELL		Cell			:  2; // 7- 6~ 7
    }Item;
}BQ2579x_Re_Charge_t;
/* End Struct */

/* Start Function */
static bool BQ2579x_Reg_Write( uint16_t MemAddr, uint8_t *pData, uint16_t Size );
static bool BQ2579x_Reg_Read( uint16_t MemAddr, uint8_t *pData, uint16_t Size );
/* End Function */

/* Start Variable */
const U8 BQ2579x_Ctrl_Reg_Default[6] = { 0xA2, 0x05, 0x40, 0x00, 0x01, 0x16 };
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Init( BQ2579x_HandleType *pbq )
{
	U8						vSysMin;
	U8						iCharge[2];
	U8						iInLmt[2];
	BQ2579x_Precharge_t		Precharge = {0};
	BQ2579x_Termination_t	Termination = {0};
	BQ2579x_Re_Charge_t		Recharge = {0};
	uuword					uw;
	bool					err = false;
	
	pbq->Init.vSysMin		= 13000;	// mV
	pbq->Init.iCharge		= 1370;		// mA
	pbq->Init.iInLmt		= 1200;		// mA
	pbq->Init.iPrecharge	= 120;		// mA
	pbq->Init.iTermination	= 200;		// mA
	pbq->Init.vRecharge		= 200;		// mV
	pbq->Init.vBatLow		= BQ2579x_71_4_x_VREG; // 16.8[V] * 71.4[%] = 11.9952[V]
	pbq->Init.Cell			= BQ2579x_4_CELL;
	pbq->Init.tReCharge		= BQ2579x_TRECHG_1024_MSEC;
	
	pbq->Init.Ctrl.Byte[0]	= 0;
	pbq->Init.Ctrl.Byte[1]	= 0;
	pbq->Init.Ctrl.Byte[2]	= 0;
	pbq->Init.Ctrl.Byte[3]	= 0;
	pbq->Init.Ctrl.Byte[4]	= 0;
	pbq->Init.Ctrl.Byte[5]	= 0;
	pbq->Init.Temp.Byte		= 0;
	pbq->Init.ADCs.Byte		= 0;
	
	// Charge Profile Parsing
	vSysMin = (pbq->Init.vSysMin - 2500) / 250;
	uw.w = pbq->Init.iCharge / 10;
	iCharge[0] = uw.b.h;
	iCharge[1] = uw.b.l;
	uw.w = pbq->Init.iInLmt / 10;
	iInLmt[0] = uw.b.h;
	iInLmt[1] = uw.b.l;
	Precharge.Item.iPrecharge = pbq->Init.iPrecharge / 40;
	Precharge.Item.vBatLow = pbq->Init.vBatLow;
	Termination.Item.iTermination = pbq->Init.iTermination / 40;
	Termination.Item.RegRst = false;
	Recharge.Item.vReCharge = (pbq->Init.vRecharge - 50) / 50;
	Recharge.Item.tReCharge = pbq->Init.tReCharge;
	Recharge.Item.Cell = pbq->Init.Cell;
#if 0
	pbq->Init.Tmr.Item.EN_2X						= true;
	pbq->Init.Tmr.Item.FstChargeTime				= BQ2579x_FAST_CHG_TIME_24_HRS;
	pbq->Init.Tmr.Item.EN_FstCharge					= true;
	pbq->Init.Tmr.Item.EN_PreCharge					= true;
	pbq->Init.Tmr.Item.EN_TriCharge					= false;
	pbq->Init.Tmr.Item.TopOffTime					= BQ2579x_TOP_OFF_TIME_DISABLE;
#endif
	pbq->Init.Ctrl.Item.EN_Termination				= true;
    pbq->Init.Ctrl.Item.EN_Charge					= true;
    pbq->Init.Ctrl.Item.EN_Auto_iBAT_Discharging	= true;
    pbq->Init.Ctrl.Item.Watchdog_Tmr				= BQ2579x_WDG_DISABLE;
	pbq->Init.Ctrl.Item.Watchdog_Rst				= true;
    pbq->Init.Ctrl.Item.VAC_OVP_Thresholds			= BQ2579x_VAC_OVP_26_V;
	pbq->Init.Ctrl.Item.SDRV_Not_Add_10s_Delay		= true;
    pbq->Init.Ctrl.Item.SDRV_Control				= BQ2579x_SDRV_CTRL_IDLE;
    pbq->Init.Ctrl.Item.Auto_DP_DM_Detection		= true;
	//pbq->Init.Ctrl.Item.DIS_PFM_FWD					= true;
    pbq->Init.Ctrl.Item.EN_IBUS_OCP_FWD				= true;
    pbq->Init.Ctrl.Item.EN_EXTILIM					= true;
    pbq->Init.Ctrl.Item.EN_IINDPM					= true;
    pbq->Init.Ctrl.Item.IBAT_REG					= BQ2579x_IBAT_REG_5_A;
	pbq->Init.Ctrl.Item.SFET_Present				= true;
	
	pbq->Init.Temp.Item.Thermal_Regulation			= BQ2579x_TH_REG_60;
	
	pbq->Init.ADCs.Item.Resolution					= BQ2579x_ADC_RES_12_BIT;
	pbq->Init.ADCs.Item.Enable						= true;
	
	err |= BQ2579x_Reg_Write( BQ2579x_REG00_Minimal_System_Voltage	, &vSysMin				, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG03_Charge_Current_Limit_H	,  iCharge				, 2 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG06_Input_Current_Limit_H	,  iInLmt				, 2 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG08_Precharge_Control		, &Precharge.Byte		, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG09_Termination_Control		, &Termination.Byte		, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG0A_Re_charge_Control		, &Recharge.Byte		, 1 );
#if 0
	err |= BQ2579x_Reg_Write( BQ2579x_REG0E_Timer_Control			, &pbq->Init.Tmr.Byte	, 1 );
#endif
	err |= BQ2579x_Reg_Write( BQ2579x_REG0F_Charger_Control_0		,  pbq->Init.Ctrl.Byte	, 6 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG16_Temperature_Control		, &pbq->Init.Temp.Byte	, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG2E_ADC_Control				, &pbq->Init.ADCs.Byte	, 1 );
	
	err |= BQ2579x_Read_Charge_Profile( pbq );
	err |= BQ2579x_Read_Charger_Ctrl( pbq );
	err |= BQ2579x_Read_Temperature_Ctrl( pbq );
	err |= BQ2579x_Read_ADC_Ctrl( pbq );
	
	err |= BQ2579x_Read_Status( pbq );
	err |= BQ2579x_Read_ADC( pbq );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_PostInit( BQ2579x_HandleType *pbq )
{
	U8						vSysMin;
	U8						iCharge[2];
	U8						iInLmt[2];
	BQ2579x_Precharge_t		Precharge = {0};
	BQ2579x_Termination_t	Termination = {0};
	BQ2579x_Re_Charge_t		Recharge = {0};
	uuword					uw;
	bool					err = false;
	
	// Charge Profile Parsing
	vSysMin = (pbq->Init.vSysMin - 2500) / 250;
	uw.w = pbq->Init.iCharge / 10;
	iCharge[0] = uw.b.h;
	iCharge[1] = uw.b.l;
	uw.w = pbq->Init.iInLmt / 10;
	iInLmt[0] = uw.b.h;
	iInLmt[1] = uw.b.l;
	Precharge.Item.iPrecharge = pbq->Init.iPrecharge / 40;
	Precharge.Item.vBatLow = pbq->Init.vBatLow;
	Termination.Item.iTermination = pbq->Init.iTermination / 40;
	Termination.Item.RegRst = false;
	Recharge.Item.vReCharge = (pbq->Init.vRecharge - 50) / 50;
	Recharge.Item.tReCharge = pbq->Init.tReCharge;
	Recharge.Item.Cell = pbq->Init.Cell;
	
	err |= BQ2579x_Reg_Write( BQ2579x_REG00_Minimal_System_Voltage	, &vSysMin				, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG03_Charge_Current_Limit_H	,  iCharge				, 2 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG06_Input_Current_Limit_H	,  iInLmt				, 2 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG08_Precharge_Control		, &Precharge.Byte		, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG09_Termination_Control		, &Termination.Byte		, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG0A_Re_charge_Control		, &Recharge.Byte		, 1 );
#if 0
	err |= BQ2579x_Reg_Write( BQ2579x_REG0E_Timer_Control			, &pbq->Init.Tmr.Byte	, 1 );
#endif
	err |= BQ2579x_Reg_Write( BQ2579x_REG0F_Charger_Control_0		,  pbq->Init.Ctrl.Byte	, 6 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG16_Temperature_Control		, &pbq->Init.Temp.Byte	, 1 );
	err |= BQ2579x_Reg_Write( BQ2579x_REG2E_ADC_Control				, &pbq->Init.ADCs.Byte	, 1 );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BQ2579x_Enter_Ship_Mode( BQ2579x_HandleType *pbq )
{
	pbq->Init.Ctrl.Item.SDRV_Control = BQ2579x_SDRV_CTRL_SHIP;
	
	BQ2579x_Reg_Write( BQ2579x_REG11_Charger_Control_2, &pbq->Init.Ctrl.Byte[2], 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Watchdog_Reset( BQ2579x_HandleType *pbq )
{
	pbq->Init.Ctrl.Item.Watchdog_Rst = true;
	
	return BQ2579x_Reg_Write( BQ2579x_REG10_Charger_Control_1, &pbq->Init.Ctrl.Byte[1], 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Read_Charge_Profile( BQ2579x_HandleType *pbq )
{
	U8									bData[13];
	BQ2579x_Charge_Profile_Parsing		p;
	bool								err = false;
	
	err |= BQ2579x_Reg_Read( BQ2579x_REG00_Minimal_System_Voltage, bData, 11 );
	err |= BQ2579x_Reg_Read( BQ2579x_REG19_ICO_Current_Limit_H, bData+11,  2 );
	
	if( err == false ){
		p.Byte[ 0] = bData[0];
		p.Byte[ 1] = 0;
		p.Byte[ 2] = bData[2];
		p.Byte[ 3] = bData[1];
		p.Byte[ 4] = bData[4];
		p.Byte[ 5] = bData[3];
		p.Byte[ 6] = bData[5];
		p.Byte[ 7] = 0;
		p.Byte[ 8] = bData[7];
		p.Byte[ 9] = bData[6];
		p.Byte[10] = bData[8];
		p.Byte[11] = 0;
		p.Byte[12] = bData[9];
		p.Byte[13] = 0;
		p.Byte[14] = bData[10];
		p.Byte[15] = 0;
		p.Byte[16] = bData[12];
		p.Byte[17] = bData[11];
		
		pbq->Init.vSysMin		= p.Item.vSysMin * 250 + 2500;
		pbq->Init.vReg			= p.Item.vReg * 10;
		pbq->Init.iCharge		= p.Item.iCharge * 10;
		pbq->Init.vInLmt		= p.Item.vInLmt * 100;
		pbq->Init.iInLmt		= p.Item.iInLmt * 10;
		pbq->Init.iPrecharge	= p.Item.iPrecharge * 40;
		pbq->Init.iTermination	= p.Item.iTermination * 40;
		pbq->Init.vRecharge		= p.Item.vReCharge * 50 + 50;
		pbq->Init.iLmtICO		= p.Item.iLmtICO * 10;
		pbq->Init.vBatLow		= p.Item.vBatLow;
		pbq->Init.tReCharge		= p.Item.tReCharge;
		pbq->Init.Cell			= p.Item.Cell;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Read_Charger_Ctrl( BQ2579x_HandleType *pbq )
{
	BQ2579x_Charger_Ctrl	c;
	bool					err = BQ2579x_Reg_Read( BQ2579x_REG0F_Charger_Control_0, c.Byte, 6 );
	
	if( err == false ){
		pbq->Init.Ctrl = c;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Read_Temperature_Ctrl( BQ2579x_HandleType *pbq )
{
	BQ2579x_Temp_Ctrl	t;
	bool				err = BQ2579x_Reg_Read( BQ2579x_REG16_Temperature_Control, &t.Byte, 1 );
	
	if( err == false ){
		pbq->Init.Temp = t;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Read_ADC_Ctrl( BQ2579x_HandleType *pbq )
{
	BQ2579x_ADCs_Ctrl	a;
	bool				err = BQ2579x_Reg_Read( BQ2579x_REG2E_ADC_Control, &a.Byte, 1 );
	
	if( err == false ){
		pbq->Init.ADCs = a;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Read_Status( BQ2579x_HandleType *pbq )
{
	U8				b[7];
	bool			err = BQ2579x_Reg_Read( BQ2579x_REG1B_Charger_Status_0, b, 7 );
	
	if( err == false ){
		pbq->Status.Byte[0] = b[0];
		pbq->Status.Byte[1] = b[1];
		pbq->Status.Byte[2] = b[2];
		pbq->Status.Byte[3] = b[3];
		pbq->Status.Byte[4] = b[4];
		pbq->Faults.Byte[0] = b[5];
		pbq->Faults.Byte[1] = b[6];
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BQ2579x_Read_ADC( BQ2579x_HandleType *pbq )
{
	U8		bData[22];
	bool	err = BQ2579x_Reg_Read( BQ2579x_REG31_IBUS_ADC_H, bData, 22 );
	
	if( err == false ){
		pbq->ADCs.IBUS.b.h	= bData[ 0];
		pbq->ADCs.IBUS.b.l	= bData[ 1];
		pbq->ADCs.IBAT.b.h	= bData[ 2];
		pbq->ADCs.IBAT.b.l	= bData[ 3];
		pbq->ADCs.VBUS.b.h	= bData[ 4];
		pbq->ADCs.VBUS.b.l	= bData[ 5];
		pbq->ADCs.VAC1.b.h	= bData[ 6];
		pbq->ADCs.VAC1.b.l	= bData[ 7];
		pbq->ADCs.VAC2.b.h	= bData[ 8];
		pbq->ADCs.VAC2.b.l	= bData[ 9];
		pbq->ADCs.VBAT.b.h	= bData[10];
		pbq->ADCs.VBAT.b.l	= bData[11];
		pbq->ADCs.VSYS.b.h	= bData[12];
		pbq->ADCs.VSYS.b.l	= bData[13];
		pbq->ADCs.TS.b.h	= bData[14];
		pbq->ADCs.TS.b.l	= bData[15];
		pbq->ADCs.TDIE.b.h	= bData[16];
		pbq->ADCs.TDIE.b.l	= bData[17];
		pbq->ADCs.DP.b.h	= bData[18];
		pbq->ADCs.DP.b.l	= bData[19];
		pbq->ADCs.DM.b.h	= bData[20];
		pbq->ADCs.DM.b.l	= bData[21];
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool BQ2579x_Reg_Write( uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	return I2C2_Mem_8Bit_Write( (BQ2579X_DEVICE_ADDRESS << 1), MemAddr, pData, Size );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool BQ2579x_Reg_Read( uint16_t MemAddr, uint8_t *pData, uint16_t Size )
{
	return I2C2_Mem_8Bit_Read( (BQ2579X_DEVICE_ADDRESS << 1), MemAddr, pData, Size );
}


