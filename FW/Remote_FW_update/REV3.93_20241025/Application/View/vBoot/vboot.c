#define __VBOOT_C__
    #include "vboot.h"
#undef  __VBOOT_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vlibrary.h"
#include "option.h"
#include "version.h"
#include "ma_output.h"
#include "micro_sd.h"
#include "measuring.h"
#include "vmeasuring.h"
/* End Include */

/* Start Define */                              
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */  
static void vInit( void );
static void vDelay( U16 vDly );
/* End Function */

/* Start Variable */
#if 0
const uint32_t aBMPHeader[14]=         
{0x2C364D42, 0x00000001, 0x00360000, 0x00280000, 0x00F00000, 0x00A00000, 0x00010000, 
 0x00000010, 0xF5400000, 0x00000006, 0x00000000, 0x00000000, 0x00000000, 0x0000};
#endif

static vMessageBoxType			*pMsgBox = &vMsgBox;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vSysInit( System_InitType seq, bool err )
{
	float		progress = (float)(seq - SYSTEM_INIT_FSMC) / (SYSTEM_INIT_OK - SYSTEM_INIT_GDI) * 100.f;
	char		str[3][18];
	const char	*Init_Message[] =
	{
		"FAIL: DBG USART",
		"FAIL: OPT LOAD",
		"FAIL: I2C",
		"FAIL: EEPROM",
		"FAIL: PWR CTRL",
		"FAIL: mA OUTPUT", 
		"FAIL: RELAY",
		"FAIL: FSMC",
		"FAIL: GDI",
		"FAIL: BOR",
		"FAIL: NAND FLASH",
		"FAIL: VER LOAD",
		"FAIL: USART",
		"FAIL: RS485",
		"FAIL: POE",
		"FAIL: ADC",
		"FAIL: RTC",
		"FAIL: TIM",
		"FAIL: SDIO",
		"FAIL: KEY",
		"FAIL: LED", 
		"FAIL: PSEN-DPM",
		"FAIL: FLOW",
		"FAIL: BUZZER",
		"FAIL: SENSING",
		"FAIL: TASK"
	};
#ifdef _DEBUG_BUILD
	const char	*Init_Status[] =
	{
		"DBG USART ",
		"OPT LOAD  ",
		"I2C       ",
		"EEPROM    ",
		"PWR CTRL  ",
		"mA OUTPUT ", 
		"RELAY     ",
		"FSMC      ",
		"GDI       ",
		"BOR       ",
		"NAND FLASH",
		"VER LOAD  ",
		"USART     ",
		"RS485     ",
		"POE       ",
		"ADC       ",
		"RTC       ",
		"TIM       ",
		"SDIO      ",
		"KEY       ",
		"LED       ", 
		"PSEN-DPM  ",
		"FLOW      ",
		"BUZZER    ",
		"SENSING   ",
		"TASK      "
	};
#endif
	if( seq <= SYSTEM_INIT_GDI ){ vInit(); }
	
	pMsgBox->Title.pStr = "  INITIALIZING..";
	
	if( err == true ){
		strcpy ( &str[0][0], Init_Message[seq] );
		pMsgBox->Msg[0].pStr = &str[0][0];
		pMsgBox->Msg[0].tClr = RGB.YELLOW;
		pMsgBox->MsgLine = 1;
		pMsgBox->ClearMsg = true;
	}
	else {
		pMsgBox->MsgLine = 0;
		vProgressBar( 46, 92, GDI_GetXSize()-46*2, 20, progress );
#ifdef _DEBUG_BUILD
		GDI_StringColor( 46, 70, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, (char *)Init_Status[seq] );
#endif
	}
	pMsgBox->OutLine = 6;
	vMessageBox( pMsgBox );
	vDelay( 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vSysConf( System_ConfType seq )
{
	char		str[4][22];
	uint16_t	rgb[4];
	
	pMsgBox->Title.pStr = "CONFIGURATION";
	pMsgBox->Width = 216;
	pMsgBox->MsgLine = 0;
	pMsgBox->OutLine = 6;
	if( seq == SYSTEM_CONFIG_CAVITY ){ pMsgBox->ClearMsg = true; }
	vMessageBox( pMsgBox );
	GDI_SetFont( &Font07x12 );
	GDI_StringColor( 40,  70, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "CAVITY-IR" );
	GDI_StringColor( 40,  86, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "CARTRIDGE" );
	GDI_StringColor( 40, 102, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "MAIN" );
	GDI_StringColor( 40, 118, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "OPTION No" );
	
	if( CavityHandle.IsPresent == true ){
		sprintf( &str[0][0], "REV%4.2f", CavityHandle.Inf[CAVITY_INF_FW_VER] * 0.01f );
		rgb[0] = RGB.LIGHTSKYBLUE;
	}
	else {
		strcpy( &str[0][0], "DISABLE" );
		rgb[0] = RGB.DARKGRAY;
	}
	if( CartridgeHandle.IsPresent == true ){
		sprintf( &str[1][0], "REV%4.2f", CartridgeHandle.Inf[CARTRIDGE_INF_FW_VER] * 0.01f );
		rgb[1] = RGB.LIGHTSKYBLUE;
	}
	else {
		strcpy( &str[1][0], "DISABLE" );
		rgb[1] = RGB.DARKGRAY;
	}
	sprintf( &str[2][0], "REV%4.2f", FirmwareVersion );
	sprintf( &str[3][0], "TYPE-%02u", SystemOption.Item.TypeNo );
	
	switch( seq ){
		case SYSTEM_CONFIG_CAVITY:
			rgb[0] = RGB.YELLOW;
			rgb[1] = RGB.WHITE;
			rgb[2] = RGB.WHITE;
			rgb[3] = RGB.WHITE;
			strcpy( &str[0][0], "LOADING" );
			strcpy( &str[1][0], " READY " );
			strcpy( &str[2][0], " READY " );
			strcpy( &str[3][0], " READY " );
			break;
			
		case SYSTEM_CONFIG_CARTRIDGE:
			rgb[1] = RGB.YELLOW;
			rgb[2] = RGB.WHITE;
			rgb[3] = RGB.WHITE;
			strcpy( &str[1][0], "LOADING" );
			strcpy( &str[2][0], " READY " );
			strcpy( &str[3][0], " READY " );
			break;
			
		case SYSTEM_CONFIG_FW_VER:
			rgb[2] = RGB.YELLOW;
			rgb[3] = RGB.WHITE;
			strcpy( &str[2][0], "LOADING" );
			strcpy( &str[3][0], " READY " );
			break;
			
		case SYSTEM_CONFIG_OPTION:
			rgb[2] = RGB.LIGHTSKYBLUE;
			rgb[3] = RGB.YELLOW;
			strcpy( &str[3][0], "LOADING" );
			break;
			
		case SYSTEM_CONFIG_END:
			rgb[2] = RGB.LIGHTSKYBLUE;
			rgb[3] = RGB.LIGHTSKYBLUE;
			break;
	}
	GDI_StringColor( 119,  70, GDI_STR_ALIGN_R, rgb[0], "[" );
	GDI_StringColor( 119,  86, GDI_STR_ALIGN_R, rgb[1], "[" );
	GDI_StringColor( 119, 102, GDI_STR_ALIGN_R, rgb[2], "[" );
	GDI_StringColor( 119, 118, GDI_STR_ALIGN_R, rgb[3], "[" );
	GDI_StringColor( 170,  70, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
	GDI_StringColor( 170,  86, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
	GDI_StringColor( 170, 102, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
	GDI_StringColor( 170, 118, GDI_STR_ALIGN_R, rgb[3], &str[3][0] );
	GDI_StringColor( 180,  70, GDI_STR_ALIGN_R, rgb[0], "]" );
	GDI_StringColor( 180,  86, GDI_STR_ALIGN_R, rgb[1], "]" );
	GDI_StringColor( 180, 102, GDI_STR_ALIGN_R, rgb[2], "]" );
	GDI_StringColor( 180, 118, GDI_STR_ALIGN_R, rgb[3], "]" );
	vDelay( 10 );
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vInit( void )
{
	vMessageBox_Parameter_Init( pMsgBox );
	GDI_Clear( RGB.BLACK );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vDelay( U16 vDly )
{
	while( vDly-- ){
		HAL_Delay( 100 );
	}
}



