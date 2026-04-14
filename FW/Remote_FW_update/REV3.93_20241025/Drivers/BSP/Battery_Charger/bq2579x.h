#ifndef   __BQ2579X_H__
#define   __BQ2579X_H__


#ifdef __BQ2579X_C__
	#define BQ2579X_EXT
#else
	#define BQ2579X_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
enum {
	BQ2579x_REG00_Minimal_System_Voltage,
	BQ2579x_REG01_Charge_Voltage_Limit_H,
	BQ2579x_REG02_Charge_Voltage_Limit_L,
	BQ2579x_REG03_Charge_Current_Limit_H,
	BQ2579x_REG04_Charge_Current_Limit_L,
	BQ2579x_REG05_Input_Voltage_Limit,
	BQ2579x_REG06_Input_Current_Limit_H,
	BQ2579x_REG07_Input_Current_Limit_L,
	BQ2579x_REG08_Precharge_Control,
	BQ2579x_REG09_Termination_Control,
	BQ2579x_REG0A_Re_charge_Control,
	BQ2579x_REG0B_VOTG_regulation_H,
	BQ2579x_REG0C_VOTG_regulation_L,
	BQ2579x_REG0D_IOTG_regulation,
	BQ2579x_REG0E_Timer_Control,
	BQ2579x_REG0F_Charger_Control_0,
	BQ2579x_REG10_Charger_Control_1,
	BQ2579x_REG11_Charger_Control_2,
	BQ2579x_REG12_Charger_Control_3,
	BQ2579x_REG13_Charger_Control_4,
	BQ2579x_REG14_Charger_Control_5,
	BQ2579x_REG15_Reserved,
	BQ2579x_REG16_Temperature_Control,
	BQ2579x_REG17_NTC_Control_0,
	BQ2579x_REG18_NTC_Control_1,
	BQ2579x_REG19_ICO_Current_Limit_H,
	BQ2579x_REG1A_ICO_Current_Limit_L,
	BQ2579x_REG1B_Charger_Status_0,
	BQ2579x_REG1C_Charger_Status_1,
	BQ2579x_REG1D_Charger_Status_2,
	BQ2579x_REG1E_Charger_Status_3,
	BQ2579x_REG1F_Charger_Status_4,
	BQ2579x_REG20_FAULT_Status_0,
	BQ2579x_REG21_FAULT_Status_1,
	BQ2579x_REG22_Charger_Flag_0,
	BQ2579x_REG23_Charger_Flag_1,
	BQ2579x_REG24_Charger_Flag_2,
	BQ2579x_REG25_Charger_Flag_3,
	BQ2579x_REG26_FAULT_Flag_0,
	BQ2579x_REG27_FAULT_Flag_1,
	BQ2579x_REG28_Charger_Mask_0,
	BQ2579x_REG29_Charger_Mask_1,
	BQ2579x_REG2A_Charger_Mask_2,
	BQ2579x_REG2B_Charger_Mask_3,
	BQ2579x_REG2C_FAULT_Mask_0,
	BQ2579x_REG2D_FAULT_Mask_1,
	BQ2579x_REG2E_ADC_Control,
	BQ2579x_REG2F_ADC_Function_Disable_0,
	BQ2579x_REG30_ADC_Function_Disable_1,
	BQ2579x_REG31_IBUS_ADC_H,
	BQ2579x_REG32_IBUS_ADC_L,
	BQ2579x_REG33_IBAT_ADC_H,
	BQ2579x_REG34_IBAT_ADC_L,
	BQ2579x_REG35_VBUS_ADC_H,
	BQ2579x_REG36_VBUS_ADC_L,
	BQ2579x_REG37_VAC1_ADC_H,
	BQ2579x_REG38_VAC1_ADC_L,
	BQ2579x_REG39_VAC2_ADC_H,
	BQ2579x_REG3A_VAC2_ADC_L,
	BQ2579x_REG3B_VBAT_ADC_H,
	BQ2579x_REG3C_VBAT_ADC_L,
	BQ2579x_REG3D_VSYS_ADC_H,
	BQ2579x_REG3E_VSYS_ADC_L,
	BQ2579x_REG3F_TS_ADC_H,
	BQ2579x_REG40_TS_ADC_L,
	BQ2579x_REG41_TDIE_ADC_H,
	BQ2579x_REG42_TDIE_ADC_L,
	BQ2579x_REG43_DP_ADC_H,
	BQ2579x_REG44_DP_ADC_L,
	BQ2579x_REG45_DM_ADC_H,
	BQ2579x_REG46_DM_ADC_L,
	BQ2579x_REG47_DPDM_Driver,
	BQ2579x_REG48_Part_Information,
	BQ2579x_REG_MAX
};

typedef enum{
	BQ2579x_FAST_CHG_TIME_05_HRS,
	BQ2579x_FAST_CHG_TIME_08_HRS,
	BQ2579x_FAST_CHG_TIME_12_HRS,
	BQ2579x_FAST_CHG_TIME_24_HRS
}BQ2579x_Fast_Charge_Time;

typedef enum{
	BQ2579x_TOP_OFF_TIME_DISABLE,
	BQ2579x_TOP_OFF_TIME_15_MINS,
	BQ2579x_TOP_OFF_TIME_30_MINS,
	BQ2579x_TOP_OFF_TIME_40_MINS
}BQ2579x_Top_Off_Time;

typedef enum{
	BQ2579x_CHRG_STAT_Not_Charging,
	BQ2579x_CHRG_STAT_Trickle_Charge,
	BQ2579x_CHRG_STAT_Pre_Charge,
	BQ2579x_CHRG_STAT_Fast_Charge,
	BQ2579x_CHRG_STAT_Taper_Charge,
	BQ2579x_CHRG_STAT_Reserved,
	BQ2579x_CHRG_STAT_Top_Off_Timer_Active_Charging,
	BQ2579x_CHRG_STAT_Charge_Termination_Done
}BQ2579x_CHRG_STAT;

typedef enum{
	BQ2579x_15_0_x_VREG,
	BQ2579x_62_2_x_VREG,
	BQ2579x_66_7_x_VREG,
	BQ2579x_71_4_x_VREG
}BQ2579x_VBAT_LOWV;

typedef enum{
	BQ2579x_TRECHG_64_MSEC,
	BQ2579x_TRECHG_256_MSEC,
	BQ2579x_TRECHG_1024_MSEC,
	BQ2579x_TRECHG_2048_MSEC
}BQ2579x_TRECHG;

typedef enum{
	BQ2579x_1_CELL,
	BQ2579x_2_CELL,
	BQ2579x_3_CELL,
	BQ2579x_4_CELL
}BQ2579x_CELL;

typedef enum{
	BQ2579x_WDG_DISABLE,
	BQ2579x_WDG_0_5_SEC,
	BQ2579x_WDG_001_SEC,
	BQ2579x_WDG_002_SEC,
	BQ2579x_WDG_020_SEC,
	BQ2579x_WDG_040_SEC,
	BQ2579x_WDG_080_SEC,
	BQ2579x_WDG_160_SEC
}BQ2579x_WDG_TMR;

typedef enum{
	BQ2579x_SDRV_CTRL_IDLE,
	BQ2579x_SDRV_CTRL_SHUTDOWN,
	BQ2579x_SDRV_CTRL_SHIP,
	BQ2579x_SDRV_CTRL_SYS_PWR_RST
}BQ2579x_SDRV_CTRL;

typedef enum{
	BQ2579x_VAC_OVP_26_V,
	BQ2579x_VAC_OVP_18_V,
	BQ2579x_VAC_OVP_12_V,
	BQ2579x_VAC_OVP_07_V
}BQ2579x_VAC_OVP;

typedef enum{
	BQ2579x_IBAT_REG_3_A,
	BQ2579x_IBAT_REG_4_A,
	BQ2579x_IBAT_REG_5_A,
	BQ2579x_IBAT_REG_DISABLE
}BQ2579x_IBAT_REG;

typedef enum{
	BQ2579x_TH_SHT_150,
	BQ2579x_TH_SHT_130,
	BQ2579x_TH_SHT_120,
	BQ2579x_TH_SHT_85
}BQ2579x_TH_SHT;

typedef enum{
	BQ2579x_TH_REG_60,
	BQ2579x_TH_REG_80,
	BQ2579x_TH_REG_100,
	BQ2579x_TH_REG_120
}BQ2579x_TH_REG;

typedef enum{
	BQ2579x_ADC_RES_15_BIT,
	BQ2579x_ADC_RES_14_BIT,
	BQ2579x_ADC_RES_13_BIT,
	BQ2579x_ADC_RES_12_BIT
}BQ2579x_ADC_RES;
/* End Enum */

/* Start Struct */
typedef union {
    U8	Byte;
    struct{
        bool						EN_2X				: 1; // 0
        BQ2579x_Fast_Charge_Time	FstChargeTime		: 2; // 1~2
        bool						EN_FstCharge		: 1; // 3
        bool						EN_PreCharge		: 1; // 4
        bool						EN_TriCharge		: 1; // 5
        BQ2579x_Top_Off_Time		TopOffTime			: 2; // 6~7
    }Item;
}BQ2579x_Timer_Ctrl;

typedef union {
    U8	Byte[6];
    struct{
        bool				Reserved_0					: 1; // 0-0
        bool				EN_Termination				: 1; // 0-1
        bool				EN_HIZ_Mode					: 1; // 0-2
        bool				Force_ICO					: 1; // 0-3
        bool				EN_ICO						: 1; // 0-4
        bool				EN_Charge					: 1; // 0-5
        bool				Force_iBAT_Discharging		: 1; // 0-6
        bool				EN_Auto_iBAT_Discharging	: 1; // 0-7
		
        BQ2579x_WDG_TMR		Watchdog_Tmr				: 3; // 1-0~2
		bool				Watchdog_Rst				: 1; // 1-3
        BQ2579x_VAC_OVP		VAC_OVP_Thresholds			: 2; // 1-4~5
        U8					Reserved_1					: 2; // 1-6~7
		
        bool				SDRV_Not_Add_10s_Delay		: 1; // 2-0
        BQ2579x_SDRV_CTRL	SDRV_Control				: 2; // 2-1~2
        bool				EN_HVDCP					: 1; // 2-3
        bool				EN_9V_HVDCP					: 1; // 2-4
        bool				EN_12V_HVDCP				: 1; // 2-5
        bool				Auto_DP_DM_Detection		: 1; // 2-6
        bool				Force_DP_DM_Detection		: 1; // 2-7
		
        bool				DIS_OOA_FWD					: 1; // 3-0
        bool				DIS_OOA_OTG					: 1; // 3-1
        bool				DIS_LDO						: 1; // 3-2
        bool				WKUP_DLY_15_MSEC			: 1; // 3-3
        bool				DIS_PFM_FWD					: 1; // 3-4
        bool				DIS_PFM_OTG					: 1; // 3-5
        bool				EN_OTG						: 1; // 3-6
        bool				DIS_ACDRV					: 1; // 3-7
		
        bool				EN_IBUS_OCP_FWD				: 1; // 4-0
        bool				Force_VINDPM_Detection		: 1; // 4-1
        bool				DIS_VOTG_UVP				: 1; // 4-2
        bool				DIS_VSYS_Short				: 1; // 4-3
        bool				DIS_STAT_Pin				: 1; // 4-4
        bool				PWM_FREQ_750_KHz			: 1; // 4-5
        bool				EN_ACDRV1					: 1; // 4-6
        bool				EN_ACDRV2					: 1; // 4-7
		
        bool				EN_BAT_OCP					: 1; // 5-0
        bool				EN_EXTILIM					: 1; // 5-1
        bool				EN_IINDPM					: 1; // 5-2
        BQ2579x_IBAT_REG	IBAT_REG					: 2; // 5-3~4
        bool				EN_IBAT_Sensing				: 1; // 5-5
        bool				Reserved_2					: 1; // 5-6
        bool				SFET_Present				: 1; // 5-7
    }Item;
}BQ2579x_Charger_Ctrl;

typedef union {
    U8	Byte;
    struct{
        bool				Reserved_0					: 1; // 0-0
        bool				VAC2_Pull_Down_EN			: 1; // 0-1
        bool				VAC1_Pull_Down_EN			: 1; // 0-2
        bool				VBUS_Pull_Down_EN			: 1; // 0-3
		BQ2579x_TH_SHT		Thermal_Shutdown			: 2; // 5-4~5
		BQ2579x_TH_REG		Thermal_Regulation			: 2; // 5-6~7
    }Item;
}BQ2579x_Temp_Ctrl;

typedef union {
    U8	Byte;
    struct{
        U8					Reserved			: 2; // 0-0~1
        bool				Avg_Init_New_Conv	: 1; // 0-2
        bool				Running_Avg			: 1; // 0-3
        BQ2579x_ADC_RES		Resolution			: 2; // 0-4~5
        bool				One_Shot_Mode		: 1; // 0-6
        bool				Enable				: 1; // 0-7
    }Item;
}BQ2579x_ADCs_Ctrl;

typedef union {
    U8	Byte[5];
    struct{
        bool				VBUS_Present				: 1; // 0-0
        bool				VAC1_Present				: 1; // 0-1
        bool				VAC2_Present				: 1; // 0-2
        bool				Power_Good					: 1; // 0-3
        bool				Weak_Adaptor_Detected		: 1; // 0-4
        bool				WD_Tmr_Expired				: 1; // 0-5
        bool				VDPM_VOTG_Regulation		: 1; // 0-6
        bool				IDPM_IOTG_Regulation		: 1; // 0-7
		
        bool				BC12_NonStd_Det_Complete	: 1; // 1-0
        U8					VBUS_Status					: 4; // 1-1~4
        BQ2579x_CHRG_STAT	Charge_Status				: 3; // 1-5~7
		
        bool				VBAT_Present				: 1; // 2-0
        bool				DPDM_Det_Ongoing			: 1; // 2-1
        bool				Thermal_Regulation			: 1; // 2-2
        U8					Reserved_0					: 3; // 2-3~5
        U8					Input_Current_Optimizer		: 2; // 2-6~7
		
        bool				Reserved_1					: 1; // 3-0
        bool				PreCharge_Safe_Tmr_Expired	: 1; // 3-1
        bool				TriCharge_Safe_Tmr_Expired	: 1; // 3-2
        bool				FasCharge_Safe_Tmr_Expired	: 1; // 3-3
        bool				VSYSMIN_Regulation			: 1; // 3-4
        bool				ADC_Conversion_Complete		: 1; // 3-5
        bool				ACFET1_RBFET1_Placed		: 1; // 3-6
        bool				ACFET2_RBFET2_Placed		: 1; // 3-7
		
        bool				TS_Hot						: 1; // 4-0
        bool				TS_Warm						: 1; // 4-1
        bool				TS_Cool						: 1; // 4-2
        bool				TS_Cold						: 1; // 4-3
        bool				VBAT_Low_To_Enable_OTG		: 1; // 4-4
        U8					Reserved_2					: 3; // 4-5~7
    }Item;
}BQ2579x_Status;

typedef union {
    U8	Byte[2];
    struct{
        bool	VAC1_OVP			: 1; // 0-0
        bool	VAC2_OVP			: 1; // 0-1
        bool	CONV_OCP			: 1; // 0-2
        bool	IBAT_OCP			: 1; // 0-3
        bool	IBUS_OCP			: 1; // 0-4
        bool	VBAT_OVP			: 1; // 0-5
        bool	VBUS_OVP			: 1; // 0-6
        bool	IBAT_Rergulation	: 1; // 0-7
		
        U8		Reserved_0			: 2; // 1-0~1
        bool	Thermal_Shutdown	: 1; // 1-2
        bool	Reserved_1			: 1; // 1-3
        bool	OTG_UVP				: 1; // 1-4
        bool	OTG_OVP				: 1; // 1-5
        bool	VSYS_OVP			: 1; // 1-6
        bool	VSYS_Short			: 1; // 1-7
    }Item;
}BQ2579x_Faults;

typedef struct {
	usword					IBUS;
	usword					IBAT;
	usword					VBUS;
	usword					VAC1;
	usword					VAC2;
	usword					VBAT;
	usword					VSYS;
	usword					TS;
	usword					TDIE;
	usword					DP;
	usword					DM;
}BQ2579x_ADC_Data;

typedef struct {
	U16						vSysMin;		// Minimal System Voltage
	U16						vReg;			// Charge Voltage Limit
	U16						iCharge;		// Charge Current Limit
	U16						vInLmt;			// Input Voltage Limit
	U16						iInLmt;			// Input Current Limit
	U16						iPrecharge;		// Precharge current limit
	U16						iTermination;	// Termination current
	U16						vRecharge;		// Recharge Threshold Offset
	U16						iLmtICO;
	BQ2579x_VBAT_LOWV		vBatLow;		// Precharge to Fast charge Thresholds
	BQ2579x_TRECHG			tReCharge;		// Recharge deglich time
	BQ2579x_CELL			Cell;
	BQ2579x_Timer_Ctrl		Tmr;
	BQ2579x_Charger_Ctrl	Ctrl;
	BQ2579x_Temp_Ctrl		Temp;
	BQ2579x_ADCs_Ctrl		ADCs;
}BQ2579x_InitType;

typedef struct {
	BQ2579x_InitType		Init;
	BQ2579x_Status			Status;
	BQ2579x_Faults			Faults;
	BQ2579x_ADC_Data		ADCs;
}BQ2579x_HandleType;
/* End Struct */

/* Start Function */
BQ2579X_EXT bool BQ2579x_Init( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_PostInit( BQ2579x_HandleType *pbq );
BQ2579X_EXT void BQ2579x_Enter_Ship_Mode( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Watchdog_Reset( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Read_Charge_Profile( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Read_Charger_Ctrl( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Read_Temperature_Ctrl( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Read_ADC_Ctrl( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Read_Status( BQ2579x_HandleType *pbq );
BQ2579X_EXT bool BQ2579x_Read_ADC( BQ2579x_HandleType *pbq );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __BQ2579X_H__
