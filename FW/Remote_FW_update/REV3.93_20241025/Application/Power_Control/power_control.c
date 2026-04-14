#define __POWER_CONTROL_C__
    #include "power_control.h"
#undef  __POWER_CONTROL_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "option.h"
#include "task_sync_timer.h"
#include "vpower_off.h"
#include "lcd.h"
#include "measuring.h"
/* End Include */

/* Start Define */
#define nSHDN_SensPower_Pin			( GPIO_PIN_4 )
#define nSHDN_SensPower_GPIO_Port	( GPIOF )
#define nSHDN_PumpPower_Pin			( GPIO_PIN_10 )
#define nSHDN_PumpPower_GPIO_Port	( GPIOG )

#define nLED_Power_Pin				( GPIO_PIN_3 )
#define nLED_Power_GPIO_Port		( GPIOG )
#define nLED_FullyCharged_Pin		( GPIO_PIN_7 )
#define nLED_FullyCharged_GPIO_Port	( GPIOG )

#define PwrKey_Pin					( GPIO_PIN_6 )
#define PwrKey_GPIO_Port			( GPIOA )

#define SensPower_On()				HAL_GPIO_WritePin( nSHDN_SensPower_GPIO_Port	, nSHDN_SensPower_Pin	, GPIO_PIN_SET		)
#define PumpPower_On()				HAL_GPIO_WritePin( nSHDN_PumpPower_GPIO_Port	, nSHDN_PumpPower_Pin	, GPIO_PIN_SET		)
#define Power_LED_On()				HAL_GPIO_WritePin( nLED_Power_GPIO_Port			, nLED_Power_Pin		, GPIO_PIN_RESET	)
#define Fully_Charged_LED_On()		HAL_GPIO_WritePin( nLED_FullyCharged_GPIO_Port	, nLED_FullyCharged_Pin	, GPIO_PIN_RESET	)

#define SensPower_Off()				HAL_GPIO_WritePin( nSHDN_SensPower_GPIO_Port	, nSHDN_SensPower_Pin	, GPIO_PIN_RESET	)
#define PumpPower_Off()				HAL_GPIO_WritePin( nSHDN_PumpPower_GPIO_Port	, nSHDN_PumpPower_Pin	, GPIO_PIN_RESET	)
#define Power_LED_Off()				HAL_GPIO_WritePin( nLED_Power_GPIO_Port			, nLED_Power_Pin		, GPIO_PIN_SET		)
#define Fully_Charged_LED_Off()		HAL_GPIO_WritePin( nLED_FullyCharged_GPIO_Port	, nLED_FullyCharged_Pin	, GPIO_PIN_SET		)
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
    U8	by;
    struct{
        bool			VBUS_Present: 1;
        GPIO_PinState	PwrKey		: 1;
        U8				Reserved	: 6;
    }bi;
}Pwr_State;
/* End Struct */

/* Start Function */
static void GPIO_Init( void );
static void PowerKey_Scan( void );
static void Battery_Level_Decision( void );
static void Fully_Charge_Indicator( void );
/* End Function */

/* Start Variable */
static BQ2579x_CHRG_STAT	PrvChargeStatus = BQ2579x_CHRG_STAT_Not_Charging;
const float	vBatRef[100] = {
	13.000, //   0
	13.198, //   1
	13.238, //   2
	13.328, //   3
	13.351, //   4
	13.417, //   5
	13.483, //   6
	13.513, //   7
	13.579, //   8
	13.643, //   9
	13.689, //  10
	13.749, //  11
	13.814, //  12
	13.830, //  13
	13.895, //  14
	13.916, //  15
	13.948, //  16
	13.985, //  17
	14.019, //  18
	14.052, //  19
	14.054, //  20
	14.093, //  21
	14.150, //  22
	14.179, //  23
	14.195, //  24
	14.225, //  25
	14.230, //  26
	14.265, //  27
	14.280, //  28
	14.327, //  29
	14.355, //  30
	14.378, //  31
	14.385, //  32
	14.402, //  33
	14.439, //  34
	14.459, //  35
	14.483, //  36
	14.514, //  37
	14.524, //  38
	14.563, //  39
	14.609, //  40
	14.628, //  41
	14.660, //  42
	14.684, //  43
	14.728, //  44
	14.748, //  45
	14.781, //  46
	14.814, //  47
	14.851, //  48
	14.904, //  49
	14.930, //  50
	14.986, //  51
	15.038, //  52
	15.056, //  53
	15.096, //  54
	15.147, //  55
	15.187, //  56
	15.237, //  57
	15.254, //  58
	15.319, //  59
	15.347, //  60
	15.410, //  61
	15.431, //  62
	15.462, //  63
	15.493, //  64
	15.528, //  65
	15.551, //  66
	15.562, //  67
	15.579, //  68
	15.607, //  69
	15.633, //  70
	15.664, //  71
	15.713, //  72
	15.737, //  73
	15.801, //  74
	15.830, //  75
	15.863, //  76
	15.901, //  77
	15.949, //  78
	16.019, //  79
	16.047, //  80
	16.125, //  81
	16.146, //  82
	16.167, //  83
	16.190, //  84
	16.205, //  85
	16.221, //  86
	16.236, //  87
	16.245, //  88
	16.258, //  89
	16.261, //  90
	16.270, //  91
	16.284, //  92
	16.299, //  93
	16.310, //  94
	16.324, //  95
	16.352, //  96
	16.358, //  97
	16.370, //  98
	16.641  //  99
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Power_Control_Init( void )
{
	bool	err = false;
	
	U8			pCnt = 0;
	U8			rCnt = 0;
	
	Pwr_State	cur = {0};
	Pwr_State	cop = {0};
	U8			prv = 0xFF;
	
	PwrCtrl.KeyScan.Item.Tmr = 0;
	PwrCtrl.KeyScan.Item.Cur = GPIO_PIN_SET;
	PwrCtrl.KeyScan.Item.Prv = GPIO_PIN_SET;
	PwrCtrl.KeyScan.Item.Cop = GPIO_PIN_SET;
	PwrCtrl.KeyScan.Item.IsPressed = false;
	
	PwrCtrl.BatScanTmr = 25;
	PwrCtrl.PatchReloadTmr = 25;
	
	if( SystemOption.Item.IsBatMode == true ){
		
		GPIO_Init();
		err |= TPS25750_Init( &PwrCtrl.PD );
		err |=  BQ2579x_Init( &PwrCtrl.BQ );
		
		if( err == false ){
			
			while( 1 )
			{
				if( TaskSyncTimer.Evt.Bit._08_MSEC_1 == true ){
					TaskSyncTimer.Evt.Bit._08_MSEC_1 = false;
					
					if( BQ2579x_Read_Status( &PwrCtrl.BQ ) == false ){
						
						Fully_Charge_Indicator();
						
						cur.bi.VBUS_Present = PwrCtrl.BQ.Status.Item.VBUS_Present;
						cur.bi.PwrKey = HAL_GPIO_ReadPin( PwrKey_GPIO_Port, PwrKey_Pin );
						cop.by = cur.by ^ prv;
						
						if( cop.by ){
							pCnt = 0;
							rCnt = 0;
						}
						else {
							if( cur.bi.PwrKey == GPIO_PIN_RESET ){ // Key Pressed
								if( pCnt < 125 ){
									pCnt++;
								}
								else {
									SensPower_On();
									PumpPower_On();
									Power_LED_On();
									if( cur.bi.VBUS_Present == false ){
										Fully_Charged_LED_Off();
									}
									break;
								}
							}
							else {
								if( rCnt < 10 ){
									rCnt++;
								}
								else {
									if( cur.bi.VBUS_Present == false ){
										BQ2579x_Enter_Ship_Mode( &PwrCtrl.BQ );
										break;
									}
								}
							}
						}
						prv = cur.by;
					}
				}
			}
		}
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Power_Control_Task( void )
{
	bool	VBUS_Present;
	
	if( SystemOption.Item.IsBatMode == true ){
		
		PowerKey_Scan();
		Battery_Level_Decision();
		
		PwrCtrl.PatchReloadTmr--;
		if( PwrCtrl.PatchReloadTmr == 0 ){
			PwrCtrl.PatchReloadTmr = 25; // 40msec * 25 = 1sec
			
			SystemError.Peripheral.Bit.USB_PD_Controller = TPS25750_Patch_Bundle_Reload();
		}
		
		VBUS_Present = PwrCtrl.PD.Status.Item.PlugPresent;
		SystemError.Peripheral.Bit.USB_PD_Controller = TPS25750_Read_Status( &PwrCtrl.PD );
		
		if( SystemError.Peripheral.Bit.USB_PD_Controller == false ){
			if( VBUS_Present != PwrCtrl.PD.Status.Item.PlugPresent ){
				if( PwrCtrl.PD.Status.Item.PlugPresent == true ){
					//SystemError.Peripheral.Bit.USB_PD_Controller = TPS25750_Init( &PwrCtrl.PD );
				}
			}
		}
		
		VBUS_Present = PwrCtrl.BQ.Status.Item.VBUS_Present;
		SystemError.Peripheral.Bit.BatCharger = BQ2579x_Read_Status( &PwrCtrl.BQ );
		
		if( SystemError.Peripheral.Bit.BatCharger == false ){
			
			Fully_Charge_Indicator();
			
			if( VBUS_Present != PwrCtrl.BQ.Status.Item.VBUS_Present ){
				if( PwrCtrl.BQ.Status.Item.VBUS_Present == true ){
					//SystemError.Peripheral.Bit.BatCharger = BQ2579x_PostInit( &PwrCtrl.BQ );
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Battery_Level_Decision( void )
{
	U16		i;
	
	switch( PwrCtrl.BatScanTmr ){
		case 0:
			if( PwrCtrl.BQ.Status.Item.Charge_Status == BQ2579x_CHRG_STAT_Charge_Termination_Done ){
				PwrCtrl.BatLevel = 100;
			}
			else {
				if( PwrCtrl.BQ.Status.Item.VBUS_Present == true ){
					if( PwrCtrl.BatLevel < 99 ){
						if( MeasHandle.pCur->VBAT > vBatRef[PwrCtrl.BatLevel] ){
							PwrCtrl.BatLevel++;
						}
					}
				}
				else {
					if( PwrCtrl.BatLevel ){
						if( MeasHandle.pCur->VBAT < vBatRef[PwrCtrl.BatLevel-1] ){
							PwrCtrl.BatLevel--;
						}
					}
					else {
						vPowerOff_Low_Battery();
						HAL_Delay( 2000 );
						NVIC_SystemReset();
					}
				}
				if( (PwrCtrl.BatLevel == 100) &&
					(PwrCtrl.BQ.Status.Item.Charge_Status != BQ2579x_CHRG_STAT_Not_Charging) )
				{
					PwrCtrl.BatLevel = 99;
				}
			}
			break;
			
		case 1:
			for( i=0; i<99; i++ ){
				if( MeasHandle.pCur->VBAT < vBatRef[i] ){ break; }
			}
			PwrCtrl.BatLevel = i;
			PwrCtrl.BatScanTmr--;
			break;
			
		default:
			PwrCtrl.BatScanTmr--;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
    GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
	SensPower_Off();
	PumpPower_Off();
	Power_LED_Off();
	Fully_Charged_LED_Off();
    
    GPIO_InitStructure.Mode			= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull			= GPIO_NOPULL;
    GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_LOW;
	
    GPIO_InitStructure.Pin = nSHDN_SensPower_Pin;
    HAL_GPIO_Init( nSHDN_SensPower_GPIO_Port, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = nSHDN_PumpPower_Pin;
    HAL_GPIO_Init( nSHDN_PumpPower_GPIO_Port, &GPIO_InitStructure );
	
    GPIO_InitStructure.Pin = nLED_Power_Pin;
    HAL_GPIO_Init( nLED_Power_GPIO_Port, &GPIO_InitStructure );
	
	GPIO_InitStructure.Mode			= GPIO_MODE_OUTPUT_OD;
	
    GPIO_InitStructure.Pin = nLED_FullyCharged_Pin;
    HAL_GPIO_Init( nLED_FullyCharged_GPIO_Port, &GPIO_InitStructure );
	
    GPIO_InitStructure.Mode			= GPIO_MODE_INPUT;
	
    GPIO_InitStructure.Pin = PwrKey_Pin;
    HAL_GPIO_Init( PwrKey_GPIO_Port, &GPIO_InitStructure );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void PowerKey_Scan( void )
{
    bool	Cur = HAL_GPIO_ReadPin( PwrKey_GPIO_Port, PwrKey_Pin );
	
	PwrCtrl.KeyScan.Item.Cop = Cur ^ PwrCtrl.KeyScan.Item.Prv;
	
	if( PwrCtrl.KeyScan.Item.Cop ){
		if( Cur == GPIO_PIN_RESET ){
			PwrCtrl.KeyScan.Item.Tmr = 0;
			PwrCtrl.KeyScan.Item.CountDown = 3;
		}
		else {
			if( PwrCtrl.KeyScan.Item.Tmr == 77 ){
				NVIC_SystemReset();
			}
			else {
				PwrCtrl.KeyScan.Item.IsPressed = false;
				vPowerClr();
			}
		}
	}
	else {
		if( Cur == GPIO_PIN_RESET ){
			if( PwrCtrl.KeyScan.Item.Tmr < 77 ){ PwrCtrl.KeyScan.Item.Tmr++; }
			switch( PwrCtrl.KeyScan.Item.Tmr ){
				case   1:
					PwrCtrl.KeyScan.Item.IsPressed = true;
					vPowerOff( &PwrCtrl );
					break;
					
				case  26:
					PwrCtrl.KeyScan.Item.CountDown = 2;
					vPowerOff( &PwrCtrl );
					break;
					
				case  51:
					PwrCtrl.KeyScan.Item.CountDown = 1;
					vPowerOff( &PwrCtrl );
					break;
					
				case  76:
					SensPower_Off();
					PumpPower_Off();
					Power_LED_Off();
					LcdDrive.BL_Off();
					break;
			}
		}
	}
	
	PwrCtrl.KeyScan.Item.Prv = Cur;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Fully_Charge_Indicator( void )
{
	if( PrvChargeStatus != PwrCtrl.BQ.Status.Item.Charge_Status ){
		PrvChargeStatus  = PwrCtrl.BQ.Status.Item.Charge_Status;
		
		if( PwrCtrl.BQ.Status.Item.Charge_Status == BQ2579x_CHRG_STAT_Charge_Termination_Done ){
			Fully_Charged_LED_On();
		}
		else {
			Fully_Charged_LED_Off();
		}
		
		switch( PrvChargeStatus ){
			case BQ2579x_CHRG_STAT_Not_Charging						: dprintf( ", NOT CHARGING" );					break;
			case BQ2579x_CHRG_STAT_Trickle_Charge					: dprintf( ", TRICKLE CHARGE" );				break;
			case BQ2579x_CHRG_STAT_Pre_Charge						: dprintf( ", PRE CHARGE" );					break;
			case BQ2579x_CHRG_STAT_Fast_Charge						: dprintf( ", FAST CHARGE" );					break;
			case BQ2579x_CHRG_STAT_Taper_Charge						: dprintf( ", TAPER CHARGE" );					break;
			case BQ2579x_CHRG_STAT_Reserved							: dprintf( ", RESERVED" );						break;
			case BQ2579x_CHRG_STAT_Top_Off_Timer_Active_Charging	: dprintf( ", TOP OFF TIMER ACTIVE CHARGING" );	break;
			case BQ2579x_CHRG_STAT_Charge_Termination_Done			: dprintf( ", CHARGE TERMINATION DONE" );		break;
		}
	}
}


