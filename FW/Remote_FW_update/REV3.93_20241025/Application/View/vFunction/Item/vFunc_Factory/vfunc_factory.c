#define __VFUNC_FACTORY_C__
    #include "vfunc_factory.h"
#undef  __VFUNC_FACTORY_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vmeasuring.h"
#include "version.h"
#include "eeprom.h"
#include "pump.h"
#include "power_control.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void vFunc_Factory_Measuring( Func_HandleType *pFunc, Factory_SetType *pSet );
static void vFunc_Factory_Cal( Func_HandleType *pFunc, Factory_SetType *pSet );
static void vFunc_Factory_SysInit( Func_HandleType *pFunc, Factory_SetType *pSet );
static void vFunc_Factory_SysInfo( Func_HandleType *pFunc, Factory_SetType *pSet );
/* End Function */

/* Start Variable */
#if 0
static vPoint	vp[30];
static vLine	vl[30];
static vArea	va[30];
#endif

static char		*pUnit[MEASURING_UNIT_MAX] =
{
	" PPB",
	" PPM",
	"%LEL",
	"%VOL"
};
static char		*pCtgSenName[CT_SEN_MAX+1] =
{
	"UNDEFINED     ",
	"GAAC-Cl2-3    ",
	"GAAC-H2-2000  ",
	"GAAC-HCl-15   ",
	"GAAC-HF-9     ",
	"GAAC-O3-1     ",
	"GSBM-ETO-20   ",
	"GSBM-H2-20000 ",
	"GSBM-H2O2-100 ",
	"GSBM-H2S-50   ",
	"GSBM-HCN-100  ",
	"GSBM-NH3-200  ",
	"GSBM-NO-100   ",
	"GSBM-NO2-20   ",
	"GSBM-PH3-5    ",
	"GSBM-SiH4-50  ",
	"GSBM-SO2-20   ",
	"GSBM-C2H4-1500",
	"GSBM-CH2O-10  ",
	"GSBM-CO-200   ",
	"GSBM-O2-30    ",
	"GSDC-AsH3-1   ",
	"GSDC-COCl2-1  ",
	"GSNM-100AM    ",
	"GAAC-100A     ",
	"GAAC-100SC    ",
	"UNDEFINED     "
};
static char		*pCtgCfgName[] =
{
	"                       ",
	"                       ",
	"                       ",
	"[ 000. UNDEFINED      ]",
	"[ 001. GAAC-Cl2-3     ]",
	"[ 002. GAAC-H2-2000   ]",
	"[ 003. GAAC-HCl-15    ]",
	"[ 004. GAAC-HF-9      ]",
	"[ 005. GAAC-O3-1      ]",
	"[ 006. GSBM-ETO-20    ]",
	"[ 007. GSBM-H2-20000  ]",
	"[ 008. GSBM-H2O2-100  ]",
	"[ 009. GSBM-H2S-50    ]",
	"[ 010. GSBM-HCN-100   ]",
	"[ 011. GSBM-NH3-200   ]",
	"[ 012. GSBM-NO-100    ]",
	"[ 013. GSBM-NO2-20    ]",
	"[ 014. GSBM-PH3-5     ]",
	"[ 015. GSBM-SiH4-50   ]",
	"[ 016. GSBM-SO2-20    ]",
	"[ 017. GSBM-C2H4-1500 ]",
	"[ 018. GSBM-CH2O-10   ]",
	"[ 019. GSBM-CO-200    ]",
	"[ 020. GSBM-O2-30     ]",
	"[ 021. GSDC-AsH3-1    ]",
	"[ 022. GSDC-COCl2-1   ]",
	"[ 023. GSNM-100AM     ]",
	"[ 024. GAAC-100A      ]",
	"[ 025. GAAC-100SC     ]",
	"                       ",
	"                       ",
	"                       ",
};
static char		*pCvtSenName[CAVITY_SEN_TYPE_MAX+1] =
{
	"QUAD-025      ",
	"QUAD-027      ",
	"DUAL-CH       ",
	"DUAL-DH       ",
	"DUAL-IH       ",
	"DUAL-018      ",
	"DUAL-021      ",
	"DUAL-054      ",
	"DUAL-040      ",
	"DUAL-044      ",
	"QUAD-033      ",
	"QUAD-040      ",
	"QUAD-042      ",
	"QUAD-044      ",
	"DUAL-127      ",
	"UNDEFINED     "
};

static char		*pCtgRGAIN[LPTIA_RGAIN_MAX] =
{
	"OPEN",
	" 200",
	"  1K",
	"  2K",
	"  3K",
	"  4K",
	"  6K",
	"  8K",
	" 10K",
	" 12K",
	" 16K",
	" 20K",
	" 24K",
	" 30K",
	" 32K",
	" 40K",
	" 48K",
	" 64K",
	" 85K",
	" 96K",
	"100K",
	"120K",
	"128K",
	"160K",
	"196K",
	"256K",
	"512K"
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vFunc_Factory( Func_HandleType *pFunc, Factory_SetType *pSet )
{	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
		case FUNC_LEVEL_2:
			pFunc->pMsgBox->Title.pStr = "FACTORY MENU";
			pFunc->pMsgBox->Msg[0].pStr = "MEASURING";
			pFunc->pMsgBox->Msg[1].pStr = "CALIBRATION";
			pFunc->pMsgBox->Msg[2].pStr = "SYSTEM INIT";
			pFunc->pMsgBox->Msg[3].pStr = "SYSTEM INFO";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv2].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 4;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		default:
			switch( pFunc->Item.Lv2 ){
				case 0: vFunc_Factory_Measuring	( pFunc, pSet ); break;
				case 1: vFunc_Factory_Cal		( pFunc, pSet ); break;
				case 2: vFunc_Factory_SysInit	( pFunc, pSet ); break;
				case 3: vFunc_Factory_SysInfo	( pFunc, pSet ); break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vFunc_Factory_Measuring( Func_HandleType *pFunc, Factory_SetType *pSet )
{
	char		str[32][41];
	uint16_t	rgb[32];
	U8			Digit[4];
	U8			is;
	U8			id;
	U8			iCvt;
	U8			i;
	U8			iType;
	U8			h, ho;
	U8			v;
	char		*pIsEnabled[2] = { "OFF", " ON" };
	U8			*pWave;
	U16			WaveSize;
	uint16_t	rgb_error[2];
	U16			RawDvSumLmt_VZB, RawDvSumLmt_TIA, PeakDvLmt_VZB, PeakDvLmt_TIA;
	U16			TimeDvLmt, RawDvMaxLmt, RawDvMaxSumLmt, AvDvSumLmt;
	char		*pSenName;
	U8			iStr0, iStr1, iStr2;
	float		CalRsp;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			pFunc->pMsgBox->Title.pStr = "MEASURING MENU";
			pFunc->pMsgBox->Msg[0].pStr = "CONFIG";
			pFunc->pMsgBox->Msg[1].pStr = "OPTION";
			pFunc->pMsgBox->Msg[2].pStr = "FILTER";
			pFunc->pMsgBox->Msg[3].pStr = "MAINTENANCE";
			pFunc->pMsgBox->Msg[4].pStr = "VIEW STATUS";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv3].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 5;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->Item.Lv3 ){
				case 0: // Config
					pFunc->pMsgBox->Title.pStr = "CONFIG MENU";
					pFunc->pMsgBox->Msg[0].pStr = "CARTRIDGE";
					pFunc->pMsgBox->Msg[1].pStr = "SYSTEM";
					pFunc->pMsgBox->Msg[pFunc->Item.Lv4].tClr = RGB.YELLOW;
					if( pSet->pMeas->pSen->pCtg->IsPresent == false ){
						pFunc->pMsgBox->Msg[0].tClr = RGB.DARKGRAY;
					}
					pFunc->pMsgBox->MsgLine = 2;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 1: // Option
					pFunc->pMsgBox->Title.pStr = "MEASURING OPTION";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  22,  59, 176 );
					GDI_DrawVLine( 125,  45,  91 );
					GDI_DrawVLine( 163,  45,  91 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  22,  45, GDI_STR_ALIGN_L, RGB.PINK		, "ITEM" );
					GDI_StringColor( 159,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "CH1" );
					GDI_StringColor( 198,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "CH2" );
					GDI_StringColor(  22,  65, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "ZERO SKIP[ % ]" );
					GDI_StringColor(  22,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "SPAN SKIP[ % ]" );
					GDI_StringColor(  22,  95, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "OUT DELAY[ % ]" );
					GDI_StringColor(  22, 110, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "OUT DELAY[SEC]" );
					GDI_StringColor(  22, 125, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "UNDER CHECK" );
					sprintf( &str[0][0], "%4.1f", pSet->sGas[0].ZeroSkipPer * 0.1f );
					sprintf( &str[1][0], "%4.1f", pSet->sGas[0].SpanSkipPer * 0.1f );
					sprintf( &str[2][0], "%4.1f", pSet->sGas[0].OutDelayPer * 0.1f );
					if( pSet->sGas[0].OutDelaySec ){
						sprintf( &str[3][0], "%4u", pSet->sGas[0].OutDelaySec );
					}
					else {
						sprintf( &str[3][0], " OFF" );
					}
					sprintf( &str[4][0], "%s", pIsEnabled[pSet->sAlr[0].IsUnderCheckEnabled] );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[5][0], "%4.1f", pSet->sGas[1].ZeroSkipPer * 0.1f );
						sprintf( &str[6][0], "%4.1f", pSet->sGas[1].SpanSkipPer * 0.1f );
						sprintf( &str[7][0], "%4.1f", pSet->sGas[1].OutDelayPer * 0.1f );
						if( pSet->sGas[1].OutDelaySec ){
							sprintf( &str[8][0], "%4u", pSet->sGas[1].OutDelaySec );
						}
						else {
							sprintf( &str[8][0], " OFF" );
						}
						sprintf( &str[9][0], "%s", pIsEnabled[pSet->sAlr[1].IsUnderCheckEnabled] );
					}
					else {
						strcpy( &str[5][0], "DIS" );
						strcpy( &str[6][0], "DIS" );
						strcpy( &str[7][0], "DIS" );
						strcpy( &str[8][0], "DIS" );
						strcpy( &str[9][0], "DIS" );
					}
					if( pFunc->IsApproved == true )	{ strcpy( &str[10][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[10][0], "[  CANCEL  ]" ); }
					for( i=0; i<11; i++ ){
						if( (i > 4) && (i < 10) ){
							if( pSet->pMeas->Conf.OutMax > 1 ){ rgb[i] = RGB.WHITE; }
							else { rgb[i] = RGB.DARKGRAY; }
						}
						else {
							rgb[i] = RGB.WHITE;
						}
					}
					switch( pFunc->Item.Lv4 ){
						case  0: if( pFunc->ViewBlk == true ){ str[0][0] = ' '; str[0][1] = ' '; } rgb[0] = RGB.YELLOW; break;
						case  1: if( pFunc->ViewBlk == true ){ str[0][3] = ' ';                  } rgb[0] = RGB.YELLOW; break;
						case  2: if( pFunc->ViewBlk == true ){ str[1][0] = ' '; str[1][1] = ' '; } rgb[1] = RGB.YELLOW; break;
						case  3: if( pFunc->ViewBlk == true ){ str[1][3] = ' ';                  } rgb[1] = RGB.YELLOW; break;
						case  4: if( pFunc->ViewBlk == true ){ str[2][0] = ' '; str[2][1] = ' '; } rgb[2] = RGB.YELLOW; break;
						case  5: if( pFunc->ViewBlk == true ){ str[2][3] = ' ';                  } rgb[2] = RGB.YELLOW; break;
						case  6: rgb[ 3] = RGB.YELLOW; break;
						case  7: rgb[ 4] = RGB.YELLOW; break;
						case  8: if( pFunc->ViewBlk == true ){ str[5][0] = ' '; str[5][1] = ' '; } rgb[5] = RGB.YELLOW; break;
						case  9: if( pFunc->ViewBlk == true ){ str[5][3] = ' ';                  } rgb[5] = RGB.YELLOW; break;
						case 10: if( pFunc->ViewBlk == true ){ str[6][0] = ' '; str[6][1] = ' '; } rgb[6] = RGB.YELLOW; break;
						case 11: if( pFunc->ViewBlk == true ){ str[6][3] = ' ';                  } rgb[6] = RGB.YELLOW; break;
						case 12: if( pFunc->ViewBlk == true ){ str[7][0] = ' '; str[7][1] = ' '; } rgb[7] = RGB.YELLOW; break;
						case 13: if( pFunc->ViewBlk == true ){ str[7][3] = ' ';                  } rgb[7] = RGB.YELLOW; break;
						case 14: rgb[ 8] = RGB.YELLOW; break;
						case 15: rgb[ 9] = RGB.YELLOW; break;
						case 16: rgb[10] = RGB.YELLOW; break;
					}
					GDI_StringColor( 159,  65, GDI_STR_ALIGN_R, rgb[ 0], &str[ 0][0] );
					GDI_StringColor( 159,  80, GDI_STR_ALIGN_R, rgb[ 1], &str[ 1][0] );
					GDI_StringColor( 159,  95, GDI_STR_ALIGN_R, rgb[ 2], &str[ 2][0] );
					GDI_StringColor( 159, 110, GDI_STR_ALIGN_R, rgb[ 3], &str[ 3][0] );
					GDI_StringColor( 159, 125, GDI_STR_ALIGN_R, rgb[ 4], &str[ 4][0] );
					GDI_StringColor( 198,  65, GDI_STR_ALIGN_R, rgb[ 5], &str[ 5][0] );
					GDI_StringColor( 198,  80, GDI_STR_ALIGN_R, rgb[ 6], &str[ 6][0] );
					GDI_StringColor( 198,  95, GDI_STR_ALIGN_R, rgb[ 7], &str[ 7][0] );
					GDI_StringColor( 198, 110, GDI_STR_ALIGN_R, rgb[ 8], &str[ 8][0] );
					GDI_StringColor( 198, 125, GDI_STR_ALIGN_R, rgb[ 9], &str[ 9][0] );
					GDI_StringColor( 110, 150, GDI_STR_ALIGN_C, rgb[10], &str[10][0] );
					break;
					
				case 2: // Filter
					pFunc->pMsgBox->Title.pStr = "MEASURING FILTER";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  22, 72, 176 );
					GDI_DrawVLine( 105, 55,  66 );
					GDI_DrawVLine( 157, 55,  66 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  22,  55, GDI_STR_ALIGN_L, RGB.PINK		, "ITEM" );
					GDI_StringColor( 149,  55, GDI_STR_ALIGN_R, RGB.PINK		, "SET" );
					GDI_StringColor( 197,  55, GDI_STR_ALIGN_R, RGB.PINK		, "UNIT" );
					GDI_StringColor(  22,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "NOT STABLE" );
					GDI_StringColor(  22,  95, GDI_STR_ALIGN_L, RGB.DARKGRAY	, "RESERVED" );
					GDI_StringColor(  22, 110, GDI_STR_ALIGN_L, RGB.DARKGRAY	, "RESERVED" );
					sprintf( &str[0][0], "%5u", pSet->NotStableDecTime / 10 );
					GDI_StringColor( 149,  95, GDI_STR_ALIGN_R, RGB.DARKGRAY	, "0" );
					GDI_StringColor( 149, 110, GDI_STR_ALIGN_R, RGB.DARKGRAY	, "0" );
					GDI_StringColor( 197,  80, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "SEC" );
					GDI_StringColor( 197,  95, GDI_STR_ALIGN_R, RGB.DARKGRAY	, "---" );
					GDI_StringColor( 197, 110, GDI_STR_ALIGN_R, RGB.DARKGRAY	, "---" );
					if( pFunc->IsApproved == true )	{ strcpy( &str[1][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[1][0], "[  CANCEL  ]" ); }
					for( i=0; i<2; i++ ){ rgb[i] = RGB.WHITE; }
					rgb[pFunc->Item.Lv4] = RGB.YELLOW;
					GDI_StringColor( 149,  80, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
					GDI_StringColor( 110, 140, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
					break;
					
				case 3: // Maintenance
					pFunc->pMsgBox->Title.pStr = "MAINTENANCE SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  28,  72, 164 );
					GDI_DrawVLine( 103,  55,  66 );
					GDI_DrawVLine( 154,  55,  66 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  28,  55, GDI_STR_ALIGN_L, RGB.PINK		, "ITEM" );
					GDI_StringColor( 148,  55, GDI_STR_ALIGN_R, RGB.PINK		, "VALUE" );
					GDI_StringColor( 191,  55, GDI_STR_ALIGN_R, RGB.PINK		, "UNIT" );
					GDI_StringColor(  28,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "CH-1 LEVEL" );
					GDI_StringColor(  28,  95, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "CH-2 LEVEL" );
					GDI_StringColor(  28, 110, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "QUIT TIMER" );
					for( i=0; i<4; i++ ){
						if( i == 1 ){
							if( pSet->pMeas->Conf.OutMax > 1 ){ rgb[i] = RGB.WHITE; }
							else { rgb[i] = RGB.DARKGRAY; }
						}
						else {
							rgb[i] = RGB.WHITE;
						}
					}
					sprintf( &str[0][0], "%s", vStrGasConc( pSet->sGas[0].Dp, pSet->sGas[0].MaintenanceGas ) );
					GDI_StringColor( 191, 80, GDI_STR_ALIGN_R, RGB.SPRINGGREEN , pUnit[pSet->sGas[0].Unit] );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[1][0], "%s", vStrGasConc( pSet->sGas[1].Dp, pSet->sGas[1].MaintenanceGas ) );
						GDI_StringColor( 191, 95, GDI_STR_ALIGN_R, RGB.SPRINGGREEN , pUnit[pSet->sGas[1].Unit] );
					}
					else {
						strcpy( &str[1][0], "DIS" );
						GDI_StringColor( 191, 95, GDI_STR_ALIGN_R, RGB.DARKGRAY, "DIS" );
					}
					if( pSet->sMode.Item.Maintenance_QuitMin ){
						sprintf( &str[2][0], "%3u", pSet->sMode.Item.Maintenance_QuitMin );
					}
					else {
						sprintf( &str[2][0], "OFF" );
					}
					GDI_StringColor( 191, 110, GDI_STR_ALIGN_R, RGB.SPRINGGREEN , "MIN" );
					if( pFunc->IsApproved == true )	{ strcpy( &str[3][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[3][0], "[  CANCEL  ]" ); }
					if( pFunc->Item.Lv4 < 8 ){
						is = pFunc->Item.Lv4 / 4;
						id = pFunc->Item.Lv4 % 4;
						switch( pSet->sGas[is].Dp ){
							case 0: Digit[0] = 1, Digit[1] = 2, Digit[2] = 3, Digit[3] = 4; break;
							case 1: Digit[0] = 0, Digit[1] = 1, Digit[2] = 2, Digit[3] = 4; break;
							case 2: Digit[0] = 0, Digit[1] = 1, Digit[2] = 3, Digit[3] = 4; break;
							case 3: Digit[0] = 0, Digit[1] = 2, Digit[2] = 3, Digit[3] = 4; break;
						}
						for( i=id; i<4; i++ ){
							if( i == id ){
								if( pFunc->ViewBlk == true ){
									if( str[is][Digit[i]] != ' ' ){ str[is][Digit[i]] = ' '; }
								}
								else {
									if( str[is][Digit[i]] == ' ' ){ str[is][Digit[i]] = '0'; }
								}
							}
							else {
								if( str[is][Digit[i]] == ' ' ){ str[is][Digit[i]] = '0'; }
							}
						}
						rgb[is] = RGB.YELLOW;
					}
					else {
						rgb[pFunc->Item.Lv4-6] = RGB.YELLOW;
					}
					GDI_StringColor( 148,  80, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
					GDI_StringColor( 148,  95, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
					GDI_StringColor( 148, 110, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
					GDI_StringColor( 110, 140, GDI_STR_ALIGN_C, rgb[3], &str[3][0] );
					break;
					
				case 4: // View Status
					switch( pFunc->Item.Lv4 ){
						case 0:
							pFunc->pMsgBox->Title.pStr = "MEASURING STATUS";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetFont( &Font05x08 );
							sprintf( &str[0][0], "SUP: %5.2f", pSet->pMeas->pCur->VSUP );
							sprintf( &str[1][0], "F/R: %4.0f D/T: %5.2f", pSet->pMeas->pCur->FlowRate, PumpDrive.OutDuty );
							sprintf( &str[2][0], "%08X", pSet->pMeas->Filter.Res.Word );
							sprintf( &str[3][0], "CTG:%5.1f %5.1f %4.2f %4.2f",
								pSet->pMeas->pCur->CtgSenTemp,
								pSet->pMeas->pCur->CtgSenHumi,
								pSet->pMeas->pCur->CtgDVDD,
								pSet->pMeas->pCur->CtgAVDD
							);
							sprintf( &str[4][0], "CVT:%5.1f %3u",
								pSet->pMeas->pCur->CvtSenTemp,
								(U16)pSet->pMeas->pCur->CvtSrcPwr
							);
							rgb[0] = RGB.SPRINGGREEN;
							rgb[1] = RGB.LIGHTSKYBLUE;
							if( pSet->pMeas->Filter.Res.Word				){ rgb[2] = RGB.ORANGE;		}
							else											 { rgb[2] = RGB.ROYALBLUE;	}
							if( pSet->pMeas->pSen->pCtg->IsPresent == true	){ rgb[3] = RGB.ORANGE;		}
							else											 { rgb[3] = RGB.DARKGRAY;	}
							if( pSet->pMeas->pSen->pCvt->IsPresent == true	){ rgb[4] = RGB.PINK;		}
							else											 { rgb[4] = RGB.DARKGRAY;	}
							GDI_StringColor(  10, 34, GDI_STR_ALIGN_L, rgb[0], &str[0][0] );
							GDI_StringColor( 165, 34, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
							GDI_StringColor( 210, 34, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
							GDI_StringColor(  10, 42, GDI_STR_ALIGN_L, rgb[3], &str[3][0] );
							GDI_StringColor( 210, 42, GDI_STR_ALIGN_R, rgb[4], &str[4][0] );
							for( i=0, v=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								iStr0 = 5 + i * 3;
								iStr1 = iStr0 + 1;
								iStr2 = iStr0 + 2;
								CalRsp =
								Measuring_Gas_Response_Calculation(
									i,
									pSet->pMeas->Gas[i].Inf.SpanGas,
									pSet->pMeas->Gas[i].Inf.bSpan.f
								);
								if( i < CARTRIDGE_SEN_CH_MAX ){
									sprintf( &str[iStr0][0], "S%u %5.3f%6.1f%6u%6u %13.1f",
										i+1,
										pSet->pMeas->Gas[i].Inf.bZero.f,
										pSet->pMeas->Gas[i].Inf.bZeroTemp.f,
										pSet->pMeas->pCur->CtgRawDvSum.VZERO[i],
										pSet->pMeas->pCur->CtgPeakDv.VZERO[i],
										pSet->pMeas->pCur->nRat[i]
									);
									if( pSet->pMeas->Gas[i].Inf.SenNo < CT_SEN_NAP_100AM ){
										sprintf( &str[iStr1][0], "   %5.3f%6.0f%6u%6u%6u%8.2f",
											pSet->pMeas->Gas[i].Inf.bSpan.f,
											CalRsp,
											pSet->pMeas->pCur->CtgRawDvSum.VBIAS[i],
											pSet->pMeas->pCur->CtgPeakDv.VBIAS[i],
											pSet->pMeas->Gas[i].Inf.bZeroTmr,
											pSet->pMeas->pCur->nPer[i]
										);
									}
									else {
										sprintf( &str[iStr1][0], "   %5.3f%6.1f%6u%6u%6u%8.2f",
											pSet->pMeas->Gas[i].Inf.bSpan.f,
											CalRsp,
											pSet->pMeas->pCur->CtgRawDvSum.VBIAS[i],
											pSet->pMeas->pCur->CtgPeakDv.VBIAS[i],
											pSet->pMeas->Gas[i].Inf.bZeroTmr,
											pSet->pMeas->pCur->nPer[i]
										);
									}
									sprintf( &str[iStr2][0], "   %5.3f %5.3f%6u%6u%6.3f%8.2f",
										pSet->pMeas->pCur->CtgVZERO[i],
										pSet->pMeas->pCur->CtgICB[i],
										pSet->pMeas->pCur->CtgRawDvSum.LPTIA_LPF[i],
										pSet->pMeas->pCur->CtgPeakDv.LPTIA_LPF[i],
										pSet->pMeas->pCur->nDet[i],
										pSet->pMeas->Gas[i].Per
									);
								}
								else {
									sprintf( &str[iStr0][0], "S%u %5.3f%6.1f%3u%3u %5.3f%6.1f%8.2f",
										i+1,
										pSet->pMeas->Gas[i].Inf.bZero.f,
										pSet->pMeas->Gas[i].Inf.bZeroTemp.f,
										pSet->pMeas->Dev.CvtHiTime[i-2],
										pSet->pMeas->Dev.CvtLoTime[i-2],
										pSet->pMeas->pCur->CvtHiPeak[i-2],
										pSet->pMeas->pCur->nRat[i],
										pSet->pMeas->pCur->nPer[i]
									);
									sprintf( &str[iStr1][0], "   %5.3f%6.1f%6u %5.3f%6u%8.2f",
										pSet->pMeas->Gas[i].Inf.bSpan.f,
										CalRsp,
										pSet->pMeas->Dev.CvtRawDvMax[i-2],
										pSet->pMeas->pCur->CvtLoPeak[i-2],
										pSet->pMeas->Gas[i].Inf.bZeroTmr,
										pSet->pMeas->pCur->iPer[i]
									);
									sprintf( &str[iStr2][0], "   %5.3f%6.1f%6u %5.3f%6u%8.2f",
										pSet->pMeas->Gas[i].Inf.bZero.f * pSet->pMeas->Conf.iSpanScale[i],
										pSet->pMeas->Conf.iSpanScale[i] * 100.f - 100.f,
										pSet->pMeas->Dev.CvtRawDvMaxSum[i-2],
										pSet->pMeas->pCur->nDet[i],
										pSet->pMeas->Gas[i].Inf.bZeroCnt,
										pSet->pMeas->Gas[i].Per
									);
								}
								if( pSet->pMeas->Gas[i].IsPresent == true ){
									rgb[iStr0] = RGB.ROYALBLUE;
									rgb[iStr1] = RGB.LIGHTSKYBLUE;
									rgb[iStr2] = RGB.SPRINGGREEN;
								}
								else {
									rgb[iStr0] = RGB.DARKGRAY;
									rgb[iStr1] = RGB.DARKGRAY;
									rgb[iStr2] = RGB.DARKGRAY;
								}
							}
							GDI_StringColor( 110,  52, GDI_STR_ALIGN_C, rgb[ 5+pSet->vScroll], &str[ 5+pSet->vScroll][0] );
							GDI_StringColor( 110,  60, GDI_STR_ALIGN_C, rgb[ 6+pSet->vScroll], &str[ 6+pSet->vScroll][0] );
							GDI_StringColor( 110,  68, GDI_STR_ALIGN_C, rgb[ 7+pSet->vScroll], &str[ 7+pSet->vScroll][0] );
							GDI_StringColor( 110,  76, GDI_STR_ALIGN_C, rgb[ 8+pSet->vScroll], &str[ 8+pSet->vScroll][0] );
							GDI_StringColor( 110,  84, GDI_STR_ALIGN_C, rgb[ 9+pSet->vScroll], &str[ 9+pSet->vScroll][0] );
							GDI_StringColor( 110,  92, GDI_STR_ALIGN_C, rgb[10+pSet->vScroll], &str[10+pSet->vScroll][0] );
							GDI_StringColor( 110, 100, GDI_STR_ALIGN_C, rgb[11+pSet->vScroll], &str[11+pSet->vScroll][0] );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, rgb[12+pSet->vScroll], &str[12+pSet->vScroll][0] );
							GDI_StringColor( 110, 116, GDI_STR_ALIGN_C, rgb[13+pSet->vScroll], &str[13+pSet->vScroll][0] );
							GDI_StringColor( 110, 124, GDI_STR_ALIGN_C, rgb[14+pSet->vScroll], &str[14+pSet->vScroll][0] );
							GDI_StringColor( 110, 132, GDI_STR_ALIGN_C, rgb[15+pSet->vScroll], &str[15+pSet->vScroll][0] );
							GDI_StringColor( 110, 140, GDI_STR_ALIGN_C, rgb[16+pSet->vScroll], &str[16+pSet->vScroll][0] );
							GDI_StringColor( 110, 148, GDI_STR_ALIGN_C, rgb[17+pSet->vScroll], &str[17+pSet->vScroll][0] );
							GDI_StringColor( 110, 156, GDI_STR_ALIGN_C, rgb[18+pSet->vScroll], &str[18+pSet->vScroll][0] );
							GDI_StringColor( 110, 164, GDI_STR_ALIGN_C, rgb[19+pSet->vScroll], &str[19+pSet->vScroll][0] );
							break;
							
						case 1:
							pFunc->pMsgBox->Title.pStr = "PEAK MONITOR";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 24, 75, 172 );
							GDI_SetFont( &Font05x08 );
							GDI_StringColor( 110, 64, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH    nMIN    nMAX    iMIN    iMAX" );
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( (pSet->pMeas->Gas[i].IsPresent == true) &&
									(pSet->pMeas->Gas[i].Inf.Func != MEASURING_GAS_FUNC_IR_REF) )
								{
									sprintf( &str[0][0], "S%u%8.2f%8.2f%8.2f%8.2f",
										1+i,
										pSet->pMeas->Gas[i].nPerMin,
										pSet->pMeas->Gas[i].nPerMax,
										pSet->pMeas->Gas[i].iPerMin,
										pSet->pMeas->Gas[i].iPerMax
									);
									if( (pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_CT_OUT) ||
										(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_CT_OUT_WITH_REF) ||
										(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_OUT) ||
										(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF) )
									{
										rgb[0] = RGB.ROYALBLUE;
									}
									else {
										rgb[0] = RGB.LIGHTSKYBLUE;
									}
								}
								else {
									sprintf( &str[0][0], "S%u    0.00    0.00    0.00    0.00", 1+i );
									rgb[0] = RGB.DARKGRAY;
								}
								GDI_StringColor( 110, 82+i*10, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
							}
							break;
							
						case 2:
							pFunc->pMsgBox->Title.pStr = "FILTERING STATUS";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 19, 53, 182 );
							GDI_SetFont( &Font05x08 );
							GDI_StringColor( 110, 42, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH    > REF  FT-1  FT-2  FT-3  TIMER" );
							RawDvSumLmt_VZB	= 300;
							PeakDvLmt_VZB = 30;
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( i < CARTRIDGE_SEN_CH_MAX ){
									switch( pSet->pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_MODEL] ){
										case CARTRIDGE_MODEL_PROTO:
										case CARTRIDGE_MODEL_5100FN:
											RawDvSumLmt_VZB	= 300;
											PeakDvLmt_VZB = 30;
											switch( pSet->pMeas->Gas[i].Inf.SenNo ){
												case CT_SEN_CO_CF_200	:
												case CT_SEN_EC_DC_HF_9	: RawDvSumLmt_TIA	= 4000; PeakDvLmt_TIA = 2000; break;
												case CT_SEN_EC_DC_O3_1	:
												case CT_SEN_EC_DC_HCL_15: RawDvSumLmt_TIA	= 4000; PeakDvLmt_TIA = 3000; break;
												case CT_SEN_NO_CF_100	:
												case CT_SEN_SIH4_CD_50	: RawDvSumLmt_TIA	= 5000; PeakDvLmt_TIA = 3000; break;
												case CT_SEN_C2H4_C_1500	: RawDvSumLmt_TIA	= 5000; PeakDvLmt_TIA = 2000; break;
												case CT_SEN_NH3_CR_200	: RawDvSumLmt_TIA	= 4000; PeakDvLmt_TIA = 4000; break;
												case CT_SEN_O2_MP_100	: RawDvSumLmt_TIA	=  500; PeakDvLmt_TIA =  500; break;
												case CT_SEN_NAP_100AM	: RawDvSumLmt_TIA	= 1000; PeakDvLmt_TIA = 1000; break;
												default					: RawDvSumLmt_TIA	= 5000; PeakDvLmt_TIA = 4000; break;
											}
											break;
											
										case CARTRIDGE_MODEL_5000N:
											RawDvSumLmt_VZB	= 300;
											PeakDvLmt_VZB = 30;
											switch( pSet->pMeas->Gas[i].Inf.SenNo ){
												case CT_SEN_CO_CF_200	: RawDvSumLmt_TIA	= 1500; PeakDvLmt_TIA = 1500; break;
												case CT_SEN_C2H4_C_1500	:
												case CT_SEN_NAP_100AM	:
												case CT_SEN_EC_DC_O3_1	:
												case CT_SEN_SIH4_CD_50	: RawDvSumLmt_TIA	= 1000; PeakDvLmt_TIA = 1000; break;
												case CT_SEN_EC_DC_HF_9	:
												case CT_SEN_NH3_CR_200	:
												case CT_SEN_EC_DC_HCL_15: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
												case CT_SEN_NO_CF_100	: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 1000; break;
												case CT_SEN_O2_MP_100	: RawDvSumLmt_TIA	=  500; PeakDvLmt_TIA =  500; break;
												default					: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
											}
											break;
											
										case CARTRIDGE_MODEL_5200N:
										case CARTRIDGE_MODEL_5200N_DUAL:
											RawDvSumLmt_VZB	= 300;
											PeakDvLmt_VZB = 30;
											switch( pSet->pMeas->Gas[i].Inf.SenNo ){
												case CT_SEN_CO_CF_200	: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
												default					: RawDvSumLmt_TIA	= 2000; PeakDvLmt_TIA = 2000; break;
											}
											break;
											
										case CARTRIDGE_MODEL_5200FN:
										case CARTRIDGE_MODEL_5200FN_DUAL:
											RawDvSumLmt_VZB	= 400;
											PeakDvLmt_VZB = 50;
											switch( pSet->pMeas->Gas[i].Inf.SenNo ){
												case CT_SEN_CO_CF_200	: RawDvSumLmt_TIA	= 3000; PeakDvLmt_TIA = 2000; break;
												default					: RawDvSumLmt_TIA	= 3000; PeakDvLmt_TIA = 2000; break;
											}
											break;
									}
									if( pSet->pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_BUF_SIZE] == 200 ){
										RawDvSumLmt_VZB = (U16)((float)RawDvSumLmt_VZB * 1.111f);
										RawDvSumLmt_TIA = (U16)((float)RawDvSumLmt_TIA * 1.111f);
										PeakDvLmt_VZB   = (U16)((float)PeakDvLmt_VZB   * 1.111f);
										PeakDvLmt_TIA   = (U16)((float)PeakDvLmt_TIA   * 1.111f);
									}
									sprintf( &str[0][0], "S%u %3u/%4u %5u %5u %5u %6.1f",
										i+1,
										RawDvSumLmt_VZB,
										RawDvSumLmt_TIA,
										pSet->pMeas->pCur->CtgRawDvSum.VZERO[i],
										pSet->pMeas->pCur->CtgRawDvSum.VBIAS[i],
										pSet->pMeas->pCur->CtgRawDvSum.LPTIA_LPF[i],
										pSet->pMeas->Filter.NotStableTmr[i] / 10.f
									);
									sprintf( &str[1][0], "    %2u/%4u %5u %5u %5u %6.1f",
										PeakDvLmt_VZB,
										PeakDvLmt_TIA,
										pSet->pMeas->pCur->CtgPeakDv.VZERO[i],
										pSet->pMeas->pCur->CtgPeakDv.VBIAS[i],
										pSet->pMeas->pCur->CtgPeakDv.LPTIA_LPF[i],
										pSet->pMeas->Filter.NotStableTmrMax[i] / 10.f
									);
								}
								else {
									iCvt = i - CARTRIDGE_SEN_CH_MAX;
									switch( pSet->pMeas->pSen->pCvt->Inf[CAVITY_INF_DET_TYPE] ){
										case 970:
											TimeDvLmt = 3; // 300 usec
											RawDvMaxLmt = 38; // 2.968750 mV
											RawDvMaxSumLmt = (U16)( 777.f - (3.8f - pSet->pMeas->Gas[i].Inf.bZero.f) * 130.f);
											AvDvSumLmt = 19;
											break;
											
										case 972:
											TimeDvLmt = 5; // 500 usec
											RawDvMaxLmt = 53; // 4.140625 mV
											RawDvMaxSumLmt = (U16)(1100.f - (3.8f - pSet->pMeas->Gas[i].Inf.bZero.f) * 130.f);
											AvDvSumLmt = 500;
											break;
											
										default:
											TimeDvLmt = 3; // 300 usec
											RawDvMaxLmt = 38; // 2.968750 mV
											RawDvMaxSumLmt = (U16)( 777.f - (3.8f - pSet->pMeas->Gas[i].Inf.bZero.f) * 130.f);
											AvDvSumLmt = 500;
											break;
									}
									sprintf( &str[0][0], "S%u     %1u/%2u %5u %5u %5u %6.1f",
										i+1,
										TimeDvLmt,
										RawDvMaxLmt,
										pSet->pMeas->Dev.CvtHiTime[iCvt],
										pSet->pMeas->Dev.CvtLoTime[iCvt],
										pSet->pMeas->Dev.CvtRawDvMax[iCvt],
										pSet->pMeas->Filter.NotStableTmr[i] / 10.f
									);
									sprintf( &str[1][0], "   %4u/%3u       %5u %5u %6.1f",
										AvDvSumLmt,
										RawDvMaxSumLmt,
										pSet->pMeas->Dev.CvtOcAvrgSum[iCvt],
										pSet->pMeas->Dev.CvtRawDvMaxSum[iCvt],
										pSet->pMeas->Filter.NotStableTmrMax[i] / 10.f
									);
								}
								if( pSet->pMeas->Gas[i].IsPresent == true ){
									if( (pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_CT_OUT) ||
										(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_CT_OUT_WITH_REF) ||
										(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_OUT) ||
										(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_OUT_WITH_REF) )
									{
										rgb[0] = RGB.ROYALBLUE;
										rgb[1] = RGB.ROYALBLUE;
									}
									else {
										rgb[0] = RGB.LIGHTSKYBLUE;
										rgb[1] = RGB.LIGHTSKYBLUE;
									}
								}
								else {
									rgb[0] = RGB.DARKGRAY;
									rgb[1] = RGB.DARKGRAY;
								}
								GDI_StringColor( 110, 59+i*18, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
								GDI_StringColor( 110, 67+i*18, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
							}
							break;
							
						case 3:
							GDI_SetFont( &Font07x12 );
							i = pSet->iWave;
							if( (pSet->pMeas->pSen->pCvt->IsPresent == true) &&
								(pSet->pMeas->pSen->pCvt->Inf[CAVITY_INF_SRC_FREQ] == 8) )
							{
								ho = 10;
								WaveSize = 200;
							}
							else {
								ho = 20;
								WaveSize = 180;
							}
							if( pSet->pMeas->Gas[i].IsPresent == true ){
								sprintf( &str[0][0], "S%u: %s", i+1, (char *)pSet->pMeas->Gas[i].Inf.GasName );
							}
							else {
								sprintf( &str[0][0], "S%u: DISABLED  ", i+1 );
							}
							GDI_StringColor( ho+1, 2, GDI_STR_ALIGN_L, RGB.YELLOW, &str[0][0] );
							rgb[0] = RGB.RED;
							rgb[1] = RGB.ORANGE;
							rgb[2] = RGB.YELLOW;
							rgb[3] = RGB.GREEN;
							rgb[4] = RGB.BLUE;
							rgb[5] = RGB.ROYALBLUE;
							if( i < CARTRIDGE_SEN_CH_MAX ){
								pWave = &pSet->pMeas->pSen->pCtg->Wave[i][0];
							}
							else {
								pWave = &pSet->pMeas->pSen->pCvt->Wave[i-CARTRIDGE_SEN_CH_MAX][0];
							}
							for( h=0; h<WaveSize; h++ ){
								for( v=0; v<160; v++ ){
									if( v == pWave[h] ){
										rgb[6] = rgb[i];
									}
									else if( v && ((v % 32) == 0) ){
										rgb[6] = RGB.DARKGRAY;
									}
									else {
										rgb[6] = 0x0003;
									}
									GDI_DrawPixel( ho+h, 173-v, rgb[6] );
								}
							}
							break;
							
						case 4:
							pFunc->pMsgBox->Title.pStr = "SYSTEM ERROR";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_SetFont( &Font05x08 );
							rgb_error[0] = RGB.SPRINGGREEN;
							rgb_error[1] = RGB.ORANGE;
							strcpy( &str[ 0][0], "[ SYSTEM POWER ]" ); strcpy( &str[16][0], "[ IR EXC SIG-R ]" );
							strcpy( &str[ 1][0], "[ PARAMETER    ]" ); strcpy( &str[17][0], "[ IR NOT STB-1 ]" );
							strcpy( &str[ 2][0], "[ CTG TEMP     ]" ); strcpy( &str[18][0], "[ IR NOT STB-2 ]" );
							strcpy( &str[ 3][0], "[ CVT TEMP     ]" ); strcpy( &str[19][0], "[ IR NOT STB-R ]" );
							strcpy( &str[ 4][0], "[ EEPROM CRC   ]" ); strcpy( &str[20][0], "[ MEAS PENDING ]" );
							strcpy( &str[ 5][0], "[ CTG COM      ]" ); strcpy( &str[21][0], "[ OPT PATH-1   ]" );
							strcpy( &str[ 6][0], "[ CVT COM      ]" ); strcpy( &str[22][0], "[ OPT PATH-2   ]" );
							strcpy( &str[ 7][0], "[ FLOW UNDER   ]" ); strcpy( &str[23][0], "[ OPT PATH-R   ]" );
							strcpy( &str[ 8][0], "[ FLOW OVER    ]" ); strcpy( &str[24][0], "[ CTG DAC      ]" );
							strcpy( &str[ 9][0], "[ SOURCE POWER ]" ); strcpy( &str[25][0], "[ CT EXC SIG-1 ]" );
							strcpy( &str[10][0], "[ IR OFS N-STB ]" ); strcpy( &str[26][0], "[ CT EXC SIG-2 ]" );
							strcpy( &str[11][0], "[ IR OFS DRF-1 ]" ); strcpy( &str[27][0], "[ CT NOT STB-1 ]" );
							strcpy( &str[12][0], "[ IR OFS DRF-2 ]" ); strcpy( &str[28][0], "[ CT NOT STB-2 ]" );
							strcpy( &str[13][0], "[ IR OFS DRF-R ]" ); strcpy( &str[29][0], "[ CT EXC CUR-1 ]" );
							strcpy( &str[14][0], "[ IR EXC SIG-1 ]" ); strcpy( &str[30][0], "[ CT EXC CUR-2 ]" );
							strcpy( &str[15][0], "[ IR EXC SIG-2 ]" ); strcpy( &str[31][0], "[ CT GAS DEV   ]" );
							rgb[ 0] = rgb_error[SystemError.Main.Bit.Power						];
							rgb[ 1] = rgb_error[SystemError.Main.Bit.Measurement_Parameter		];
							rgb[ 2] = rgb_error[SystemError.Main.Bit.CtgTemp					];
							rgb[ 3] = rgb_error[SystemError.Main.Bit.CvtTemp					];
							rgb[ 4] = rgb_error[SystemError.Main.Bit.Eeprom_CRC					];
							rgb[ 5] = rgb_error[SystemError.Main.Bit.CtgCom						];
							rgb[ 6] = rgb_error[SystemError.Main.Bit.CvtCom						];
							rgb[ 7] = rgb_error[SystemError.Main.Bit.FlowRate_Udr				];
							rgb[ 8] = rgb_error[SystemError.Main.Bit.FlowRate_Ovr				];
							rgb[ 9] = rgb_error[SystemError.Main.Bit.IR_SourcePower				];
							rgb[10] = rgb_error[SystemError.Main.Bit.IR_OffsNotStable			];
							rgb[11] = rgb_error[SystemError.Main.Bit.IR_OffsDrift_CH1			];
							rgb[12] = rgb_error[SystemError.Main.Bit.IR_OffsDrift_CH2			];
							rgb[13] = rgb_error[SystemError.Main.Bit.IR_OffsDrift_REF			];
							rgb[14] = rgb_error[SystemError.Main.Bit.IR_ExcSignal_CH1			];
							rgb[15] = rgb_error[SystemError.Main.Bit.IR_ExcSignal_CH2			];
							rgb[16] = rgb_error[SystemError.Main.Bit.IR_ExcSignal_REF			];
							rgb[17] = rgb_error[SystemError.Main.Bit.IR_NotStable_CH1			];
							rgb[18] = rgb_error[SystemError.Main.Bit.IR_NotStable_CH2			];
							rgb[19] = rgb_error[SystemError.Main.Bit.IR_NotStable_REF			];
							rgb[20] = rgb_error[SystemError.Main.Bit.Measurement_Pending		];
							rgb[21] = rgb_error[SystemError.Main.Bit.IR_OptPathContaminated_CH1	];
							rgb[22] = rgb_error[SystemError.Main.Bit.IR_OptPathContaminated_CH2	];
							rgb[23] = rgb_error[SystemError.Main.Bit.IR_OptPathContaminated_REF	];
							rgb[24] = rgb_error[SystemError.Main.Bit.CtgDac						];
							rgb[25] = rgb_error[SystemError.Main.Bit.CT_ExcSignal_CH1			];
							rgb[26] = rgb_error[SystemError.Main.Bit.CT_ExcSignal_CH2			];
							rgb[27] = rgb_error[SystemError.Main.Bit.CT_NotStable_CH1			];
							rgb[28] = rgb_error[SystemError.Main.Bit.CT_NotStable_CH2			];
							rgb[29] = rgb_error[SystemError.Main.Bit.CT_ExcCurrent_CH1			];
							rgb[30] = rgb_error[SystemError.Main.Bit.CT_ExcCurrent_CH2			];
							rgb[31] = rgb_error[SystemError.Main.Bit.CT_Gas_Deviation			];
							for( i=0; i<32; i++ ){
								if( i < 16 ){
									GDI_StringColor(  22, 39+(i&15)*8, GDI_STR_ALIGN_L, rgb[i], &str[i][0] );
								}
								else {
									GDI_StringColor( 198, 39+(i&15)*8, GDI_STR_ALIGN_R, rgb[i], &str[i][0] );
								}
							}
							break;
							
						case 5:
							pFunc->pMsgBox->Title.pStr = "PERIPHERAL ERROR";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_SetFont( &Font05x08 );
							rgb_error[0] = RGB.SPRINGGREEN;
							rgb_error[1] = RGB.ORANGE;
							strcpy( &str[ 0][0], "[ EEPROM MAIN  ]" ); strcpy( &str[16][0], "[ DRIVER CVT   ]" );
							strcpy( &str[ 1][0], "[ mA DAC CH-1  ]" ); strcpy( &str[17][0], "[ INIT CVT     ]" );
							strcpy( &str[ 2][0], "[ mA DAC CH-2  ]" ); strcpy( &str[18][0], "[ RESERVED     ]" );
							strcpy( &str[ 3][0], "[ PSEN PTM     ]" ); strcpy( &str[19][0], "[ RESERVED     ]" );
							strcpy( &str[ 4][0], "[ BUZZER TMR   ]" ); strcpy( &str[20][0], "[ RESERVED     ]" );
							strcpy( &str[ 5][0], "[ PUMP TMR     ]" ); strcpy( &str[21][0], "[ RESERVED     ]" );
							strcpy( &str[ 6][0], "[ WATCHDOG     ]" ); strcpy( &str[22][0], "[ RESERVED     ]" );
							strcpy( &str[ 7][0], "[ INTERNAL ADC ]" ); strcpy( &str[23][0], "[ RESERVED     ]" );
							strcpy( &str[ 8][0], "[ FLASH CTG    ]" ); strcpy( &str[24][0], "[ RESERVED     ]" );
							strcpy( &str[ 9][0], "[ EEPROM CVT   ]" ); strcpy( &str[25][0], "[ RESERVED     ]" );
							strcpy( &str[10][0], "[ HS300x CTG   ]" ); strcpy( &str[26][0], "[ RESERVED     ]" );
							strcpy( &str[11][0], "[ PTM CVT      ]" ); strcpy( &str[27][0], "[ RESERVED     ]" );
							strcpy( &str[12][0], "[ iADC CTG     ]" ); strcpy( &str[28][0], "[ RESERVED     ]" );
							strcpy( &str[13][0], "[ EXT.ADC CVT  ]" ); strcpy( &str[29][0], "[ RESERVED     ]" );
							strcpy( &str[14][0], "[ BAT CHARGER  ]" ); strcpy( &str[30][0], "[ RESERVED     ]" );
							strcpy( &str[15][0], "[ USB PD CTRL  ]" ); strcpy( &str[31][0], "[ RESERVED     ]" );
							rgb[ 0] = rgb_error[SystemError.Peripheral.Bit.Eeprom_System		];
							rgb[ 1] = rgb_error[SystemError.Peripheral.Bit.mA_Output_DAC_CH1	];
							rgb[ 2] = rgb_error[SystemError.Peripheral.Bit.mA_Output_DAC_CH2	];
							rgb[ 3] = rgb_error[SystemError.Peripheral.Bit.Psen_Potentiometer	];
							rgb[ 4] = rgb_error[SystemError.Peripheral.Bit.Buzzer_Timer			];
							rgb[ 5] = rgb_error[SystemError.Peripheral.Bit.Pump_Timer			];
							rgb[ 6] = rgb_error[SystemError.Peripheral.Bit.WatchDog				];
							rgb[ 7] = rgb_error[SystemError.Peripheral.Bit.iADC					];
							rgb[ 8] = rgb_error[SystemError.Peripheral.Bit.Flash_Cartridge		];
							rgb[ 9] = rgb_error[SystemError.Peripheral.Bit.Eeprom_Cavity		];
							rgb[10] = rgb_error[SystemError.Peripheral.Bit.HS300x_Cartridge		];
							rgb[11] = rgb_error[SystemError.Peripheral.Bit.Ptm_Cavity			];
							rgb[12] = rgb_error[SystemError.Peripheral.Bit.iADC_Cartridge		];
							rgb[13] = rgb_error[SystemError.Peripheral.Bit.eADC_Cavity			];
							rgb[14] = rgb_error[SystemError.Peripheral.Bit.BatCharger			];
							rgb[15] = rgb_error[SystemError.Peripheral.Bit.USB_PD_Controller	];
							rgb[16] = rgb_error[SystemError.Peripheral.Bit.Driver_Cavity		];
							rgb[17] = rgb_error[SystemError.Peripheral.Bit.Init_Cavity			];
							rgb[18] = RGB.DARKGRAY;
							rgb[19] = RGB.DARKGRAY;
							rgb[20] = RGB.DARKGRAY;
							rgb[21] = RGB.DARKGRAY;
							rgb[22] = RGB.DARKGRAY;
							rgb[23] = RGB.DARKGRAY;
							rgb[24] = RGB.DARKGRAY;
							rgb[25] = RGB.DARKGRAY;
							rgb[26] = RGB.DARKGRAY;
							rgb[27] = RGB.DARKGRAY;
							rgb[28] = RGB.DARKGRAY;
							rgb[29] = RGB.DARKGRAY;
							rgb[30] = RGB.DARKGRAY;
							rgb[31] = RGB.DARKGRAY;
							for( i=0; i<32; i++ ){
								if( i < 16 ){
									GDI_StringColor(  22, 39+(i&15)*8, GDI_STR_ALIGN_L, rgb[i], &str[i][0] );
								}
								else {
									GDI_StringColor( 198, 39+(i&15)*8, GDI_STR_ALIGN_R, rgb[i], &str[i][0] );
								}
							}
							break;
					}
					break;
			}
			break;
			
		case FUNC_LEVEL_5:
			switch( pFunc->Item.Lv3 ){
				case 0: // Config
					switch( pFunc->Item.Lv4 ){
						case 0: // Cartridge
							pFunc->pMsgBox->Title.pStr = "CARTRIDGE CONFIG";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetFont( &Font07x12 );
							for( i=1, rgb[0] = RGB.SPRINGGREEN; i<9; i++ ){ rgb[i] = RGB.WHITE; }
							switch( pFunc->Item.Lv5 ){
								case 0:
									rgb[4] = RGB.YELLOW;
									i = 0;
									break;
									
								case 1:
									rgb[4] = RGB.YELLOW;
									i = 1;
									break;
									
								case 2:
									rgb[4] = RGB.LIGHTSKYBLUE;
									rgb[8] = RGB.YELLOW;
									if( pSet->pMeas->Conf.SenComb != MEASURING_SEN_COMB_CTG_DUAL ){
										i = 0;
									}
									else {
										i = 1;
									}
									break;
							}
							sprintf( &str[0][0], "SENSOR-%u SELECTION", i+1 );
							sprintf( &str[1][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]  ] );
							sprintf( &str[2][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]+1] );
							sprintf( &str[3][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]+2] );
							sprintf( &str[4][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]+3] );
							sprintf( &str[5][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]+4] );
							sprintf( &str[6][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]+5] );
							sprintf( &str[7][0], "%s", pCtgCfgName[pSet->sCtgSenType[i]+6] );
							if( pFunc->IsApproved == true ){
								strcpy( &str[8][0], "[   SAVE   ]" );
							}
							else {
								strcpy( &str[8][0], "[  CANCEL  ]" );
							}
							GDI_StringColor( 110,  42, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
							GDI_StringColor( 110,  61, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
							GDI_StringColor( 110,  73, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
							GDI_StringColor( 110,  85, GDI_STR_ALIGN_C, rgb[3], &str[3][0] );
							GDI_StringColor( 110,  97, GDI_STR_ALIGN_C, rgb[4], &str[4][0] );
							GDI_StringColor( 110, 109, GDI_STR_ALIGN_C, rgb[5], &str[5][0] );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, rgb[6], &str[6][0] );
							GDI_StringColor( 110, 133, GDI_STR_ALIGN_C, rgb[7], &str[7][0] );
							GDI_StringColor( 110, 152, GDI_STR_ALIGN_C, rgb[8], &str[8][0] );
							break;
							
						case 1: // System
							pFunc->pMsgBox->Title.pStr = "SYSTEM CONFIG";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.LIGHTSKYBLUE );
							GDI_DrawRect( 24, 42, 171, 50 );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 25, 115, 169 );
							GDI_SetFont( &Font07x12 );
							if( pSet->pMeas->pSen->pCtg->IsPresent == true ){
								for( i=0; i<CARTRIDGE_SEN_CH_MAX; i++ ){
									if( i < pSet->pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] ){
										iType = SENSOR_EEP_SYS_CTG_SEN_TYPE_0 + i;
										if( MeasHandle.pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_SYS][iType] < CT_SEN_MAX ){
											pSenName = pCtgSenName[pSet->pMeas->pSen->pCtg->Eep[CARTRIDGE_EEP_BLK_SYS][iType]];
										}
										else {
											pSenName = pCtgSenName[CT_SEN_MAX];
										}
										sprintf( &str[i][0], "S%u %s: %3u", i+1, pSenName, pSet->sConf.SetNo.Ctg[i] );
										if( (i == 1) && (pSet->pMeas->pSen->pCvt->IsPresent == true) ){
											rgb[i] = RGB.DARKGRAY;
										}
										else {
											if( pFunc->Item.Lv5 == i ){ rgb[i] = RGB.YELLOW; }
											else { rgb[i] = RGB.WHITE; }
										}
									}
									else {
										sprintf( &str[i][0], "S%u DISABLED      : ---", i+1 );
										rgb[i] = RGB.DARKGRAY;
									}
								}
							}
							else {
								strcpy( &str[0][0], "S1 DISABLED      : ---" );
								strcpy( &str[1][0], "S2 DISABLED      : ---" );
								rgb[0] = RGB.DARKGRAY;
								rgb[1] = RGB.DARKGRAY;
							}
							if( pSet->pMeas->pSen->pCvt->IsPresent == true ){
								if( pSet->pMeas->pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE] < CAVITY_SEN_TYPE_MAX ){
									pSenName = pCvtSenName[pSet->pMeas->pSen->pCvt->Inf[CAVITY_INF_SEN_TYPE]];
								}
								else {
									pSenName = pCvtSenName[CAVITY_SEN_TYPE_MAX];
								}
								sprintf( &str[2][0], "IR %s: %3u", pSenName, pSet->sConf.SetNo.Cvt );
								if( pFunc->Item.Lv5 == 2 ){ rgb[2] = RGB.YELLOW; }
								else { rgb[2] = RGB.WHITE; }
							}
							else {
								strcpy( &str[2][0], "IR DISABLED      : ---" );
								rgb[2] = RGB.DARKGRAY;
							}
							
							GDI_StringColor( 110, 101, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH GAS NAME     H/S UNIT" );
							sprintf( &str[3][0], "1. %s %s %s",
								(char *)pSet->sGas[pSet->sConf.iOut[0]].GasName,
								vStrGasConc( pSet->sGas[pSet->sConf.iOut[0]].Dp, pSet->sGas[pSet->sConf.iOut[0]].HighScale ),
								pUnit[pSet->sGas[pSet->sConf.iOut[0]].Unit]
							);
							rgb[3] = RGB.LIGHTSKYBLUE;
							if( pSet->sConf.OutMax > 1 ){
								sprintf( &str[4][0], "2. %s %s %s",
									(char *)pSet->sGas[pSet->sConf.iOut[1]].GasName,
									vStrGasConc( pSet->sGas[pSet->sConf.iOut[1]].Dp, pSet->sGas[pSet->sConf.iOut[1]].HighScale ),
									pUnit[pSet->sGas[pSet->sConf.iOut[1]].Unit]
								);
								rgb[4] = RGB.LIGHTSKYBLUE;
							}
							else {
								strcpy( &str[4][0], "2. DISABLED       0 ----" );
								rgb[4] = RGB.DARKGRAY;
							}
							if( pFunc->IsApproved == true )	{ strcpy( &str[5][0], "[   SAVE   ]" ); }
							else							{ strcpy( &str[5][0], "[  CANCEL  ]" ); }
							if( pFunc->Item.Lv5 == 3 ){ rgb[5] = RGB.YELLOW; }
							else { rgb[5] = RGB.WHITE; }
							GDI_StringColor( 110,  49, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
							GDI_StringColor( 110,  62, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
							GDI_StringColor( 110,  75, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
							GDI_StringColor( 110, 120, GDI_STR_ALIGN_C, rgb[3], &str[3][0] );
							GDI_StringColor( 110, 132, GDI_STR_ALIGN_C, rgb[4], &str[4][0] );
							GDI_StringColor( 110, 152, GDI_STR_ALIGN_C, rgb[5], &str[5][0] );
							break;
					}
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vFunc_Factory_Cal( Func_HandleType *pFunc, Factory_SetType *pSet )
{
	char				str[25][30];
	uint16_t			rgb[25];
	U8					Digit[4];
	U8					i;
	Measuring_GasType	*pGas;
	char				*pUse[2] = { "OFF", " ON" };
	bool				IsSuccessful = true;
	float				tOffs;
	float				tGain;
	float				ztos;
	float				SpanScale;
	float				tFull;
	float				tVolt;
	float				tRate;
	float				cRate;
	float				CalRsp;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			pFunc->pMsgBox->Title.pStr = "CALIBRATION MENU";
			pFunc->pMsgBox->Msg[0].pStr = "GAIN";
			pFunc->pMsgBox->Msg[1].pStr = "ZERO";
			pFunc->pMsgBox->Msg[2].pStr = "SPAN";
			pFunc->pMsgBox->Msg[3].pStr = "BASE ZERO";
			pFunc->pMsgBox->Msg[4].pStr = "VIEW INFO";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv3].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 5;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->Item.Lv3 ){
				case 0: // Gain
					pFunc->pMsgBox->Title.pStr = "GAIN MENU";
					pFunc->pMsgBox->Msg[0].pStr = "CARTRIDGE";
					pFunc->pMsgBox->Msg[1].pStr = "CAVITY-IR";
					pFunc->pMsgBox->Msg[2].pStr = "TARGET VOLTAGE";
					if( pSet->pMeas->pSen->pCtg->IsPresent == false ){ pFunc->pMsgBox->Msg[0].tClr = RGB.DARKGRAY; }
					if( pSet->pMeas->pSen->pCvt->IsPresent == false ){ pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY; }
					pFunc->pMsgBox->Msg[pFunc->Item.Lv4].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 3;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 1: // Zero
					pFunc->pMsgBox->Title.pStr = "ZERO CAL";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 26, 61, 168 );
					GDI_SetFont( &Font07x12 );                                                            
					GDI_StringColor( 110, 47, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH GAS NAME    CONC UNIT" );
					for( i=0; i<7; i++ ){ rgb[i] = RGB.WHITE; }
					for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
						pGas = &pSet->pMeas->Gas[i];
						if( pGas->IsPresent == true ){
							if( pGas->Inf.Func == MEASURING_GAS_FUNC_IR_REF ){
								sprintf( &str[i][0], "S%u %s     0 ----", 1+i, (char *)pGas->Inf.GasName );
								rgb[i] = RGB.DARKGRAY;
							}
							else {
								sprintf( &str[i][0], "S%u %s %s %s",
									1+i,
									(char *)pGas->Inf.GasName,
									vStrGasConc( pGas->Inf.Dp, pGas->Act ),
									pUnit[pGas->Inf.Unit]
								);
							}
							if( pFunc->Item.Lv4 < 6 ){
								rgb[pFunc->Item.Lv4] = RGB.YELLOW;
							}
							else {
								if( (pSet->Cal.Select[i] == true) && (pSet->Cal.Status == FACTORY_CAL_STATUS_FINISHED) ){
									switch( pSet->Cal.Result[i] ){
										case FACTORY_CAL_RESULT_SUCCESS:
											rgb[i] = RGB.LIGHTSKYBLUE;
											break;
											
										case FACTORY_CAL_RESULT_TIMEOUT:
											sprintf( &str[i][0], "S%u TIMEOUT    %s %s",
												1+i,
												vStrGasConc( pGas->Inf.Dp, pGas->Act ),
												pUnit[pGas->Inf.Unit]
											);
											rgb[i] = RGB.ORANGE;
											break;
											
										case FACTORY_CAL_RESULT_OUTPUT_REVERSE:
											sprintf( &str[i][0], "S%u REVERSE    %s %s",
												1+i,
												vStrGasConc( pGas->Inf.Dp, pGas->Act ),
												pUnit[pGas->Inf.Unit]
											);
											rgb[i] = RGB.ORANGE;
											break;
									}
								}
							}
						}
						else {
							sprintf( &str[i][0], "S%u DISABLED       0 ----", 1+i );
							rgb[i] = RGB.DARKGRAY;
						}
					}
					switch( pSet->Cal.Status ){
						case FACTORY_CAL_STATUS_READY:
							strcpy( &str[6][0], "READY        " );
							break;
							
						case FACTORY_CAL_STATUS_INJECT_GAS:
							strcpy( &str[6][0], "INJECT GAS..." );
							break;
							
						case FACTORY_CAL_STATUS_DECISION:
							strcpy( &str[6][0], "DECISION...  " );
							break;
							
						case FACTORY_CAL_STATUS_FINISHED:
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( (pSet->Cal.Select[i] == true) && (pSet->Cal.Result[i] != FACTORY_CAL_RESULT_SUCCESS) ){
									IsSuccessful = false;
									break;
								}
							}
							if( IsSuccessful == true )	{ strcpy( &str[6][0], "SUCCESS      " ); }
							else						{ strcpy( &str[6][0], "FAILURE      " ); }
							break;
					}
					if( (pFunc->Item.Lv4 == 6) && (pSet->Cal.Status != FACTORY_CAL_STATUS_DECISION) ){
						rgb[6] = RGB.YELLOW;
					}
					sprintf( &str[7][0], "%3u  SEC", pSet->Cal.InjectionTmr );
					rgb[7] = RGB.GREEN;
					for( i=0; i<6; i++ ){
						if( pSet->Cal.Select[i] == true ){ GDI_SetBackColor( RGB.SELECTED ); }
						else { GDI_SetBackColor( RGB.BLACK ); }
						GDI_StringColor( 110, 67+i*13, GDI_STR_ALIGN_C, rgb[i], &str[i][0] );
					}
					GDI_SetBackColor( RGB.BLACK );
					GDI_StringColor(  26, 150, GDI_STR_ALIGN_L, rgb[6], &str[6][0] );
					GDI_StringColor( 194, 150, GDI_STR_ALIGN_R, rgb[7], &str[7][0] );
					break;
					
				case 2: // Span
					pFunc->pMsgBox->Title.pStr = "SELECT CHANNEL";
					for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
						if( pSet->pMeas->Gas[i].IsPresent == true ){
							sprintf( &str[i][0], "%s[S%u]", (char *)pSet->pMeas->Gas[i].Inf.GasName, i+1 );
							if( (pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_CT_INT) ||
								(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_CT_REF) ||
								(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_INT) ||
								(pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_REF) )
							{
								pFunc->pMsgBox->Msg[i].tClr = RGB.DARKGRAY;
							}
						}
						else {
							sprintf( &str[i][0], "DISABLED  [S%u]", i+1 );
							pFunc->pMsgBox->Msg[i].tClr = RGB.DARKGRAY;
						}
						pFunc->pMsgBox->Msg[i].pStr = &str[i][0];
					}
					pFunc->pMsgBox->Msg[pFunc->Item.Lv4].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = MEASURING_GAS_SENSOR_MAX;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 3: // Base Zero
					pFunc->pMsgBox->Title.pStr = "BASE ZERO";
					pFunc->pMsgBox->Msg[0].pStr = "CAL";
					pFunc->pMsgBox->Msg[1].pStr = "SET";
					pFunc->pMsgBox->Msg[pFunc->Item.Lv4].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 2;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 4: // View Info
					pFunc->pMsgBox->Title.pStr = "VIEW CAL INFO";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font07x12 );
					i = pFunc->Item.Lv4;
					pGas = &pSet->pMeas->Gas[i];
					CalRsp = Measuring_Gas_Response_Calculation( i, pGas->Inf.SpanGas, pGas->Inf.bSpan.f );
					if( pGas->IsPresent == true ){
						sprintf( &str[0][0], "S%u: %s", i+1, (char *)pGas->Inf.GasName );
						rgb[0] = RGB.YELLOW;
						rgb[1] = RGB.LIGHTSKYBLUE;
						rgb[2] = RGB.LIGHTSKYBLUE;
						rgb[3] = RGB.PINK;
						rgb[4] = RGB.PINK;
						rgb[5] = RGB.PINK;
						rgb[6] = RGB.SPRINGGREEN;
						if( pGas->Inf.Func == MEASURING_GAS_FUNC_IR_REF ){ rgb[7] = RGB.DARKGRAY; }
						else { rgb[7] = RGB.LIGHTSKYBLUE; }
						rgb[8] = RGB.LIGHTSKYBLUE;
					}
					else {
						sprintf( &str[0][0], "S%u: DISABLED  ", i+1 );
						rgb[0] = RGB.YELLOW;
						rgb[1] = RGB.DARKGRAY;
						rgb[2] = RGB.DARKGRAY;
						rgb[3] = RGB.DARKGRAY;
						rgb[4] = RGB.DARKGRAY;
						rgb[5] = RGB.DARKGRAY;
						rgb[6] = RGB.DARKGRAY;
						rgb[7] = RGB.DARKGRAY;
						rgb[8] = RGB.DARKGRAY;
					}
					sprintf( &str[1][0], "mZ:%7.4fV,%6.1f`C", pGas->Inf.mZero.f, pGas->Inf.mZeroTemp.f );
					sprintf( &str[2][0], "mS:%7.4fV,%6.1f`C", pGas->Inf.mSpan.f, pGas->Inf.mSpanTemp.f );
					sprintf( &str[3][0], "bZ:%7.4fV,%6.1f`C", pGas->Inf.bZero.f, pGas->Inf.bZeroTemp.f );
					sprintf( &str[4][0], "bS:%7.4fV,%6.1f`C", pGas->Inf.bSpan.f, pGas->Inf.bSpanTemp.f );
					sprintf( &str[5][0], "bZ COUNTS:     %5u", pGas->Inf.bZeroCnt );
					if( i < CARTRIDGE_SEN_CH_MAX ){
						sprintf( &str[6][0], "R-GAIN:         %s", pCtgRGAIN[pGas->Inf.TIA.Item.RGAIN] );
						if( pGas->Inf.SenNo < CT_SEN_NAP_100AM ){ // EC Sensor
							sprintf( &str[8][0], "RESPONSE:%8.0f nA", CalRsp );
						}
						else {
							sprintf( &str[8][0], "RESPONSE:%8.1f mV", CalRsp );
						}
					}
					else {
						sprintf( &str[6][0], "OFFS: %3u, GAIN: %3u",
							pSet->pMeas->pSen->pCvt->PtmOffs[i - CARTRIDGE_SEN_CH_MAX],
							pSet->pMeas->pSen->pCvt->PtmGain[i - CARTRIDGE_SEN_CH_MAX]
						);
						sprintf( &str[8][0], "RESPONSE:%9.1f %%", CalRsp );
					}
					sprintf( &str[7][0], "SPAN GAS: %s %s",
						vStrGasConc( pGas->Inf.Dp, pGas->Inf.SpanGas ),
						pUnit[pGas->Inf.Unit]
					);
					
					GDI_StringColor( 39,  45, GDI_STR_ALIGN_L, rgb[0], &str[0][0] );
					GDI_StringColor( 39,  63, GDI_STR_ALIGN_L, rgb[1], &str[1][0] );
					GDI_StringColor( 39,  75, GDI_STR_ALIGN_L, rgb[2], &str[2][0] );
					GDI_StringColor( 39,  87, GDI_STR_ALIGN_L, rgb[3], &str[3][0] );
					GDI_StringColor( 39,  99, GDI_STR_ALIGN_L, rgb[4], &str[4][0] );
					GDI_StringColor( 39, 111, GDI_STR_ALIGN_L, rgb[5], &str[5][0] );
					GDI_StringColor( 39, 123, GDI_STR_ALIGN_L, rgb[6], &str[6][0] );
					GDI_StringColor( 39, 135, GDI_STR_ALIGN_L, rgb[7], &str[7][0] );
					GDI_StringColor( 39, 147, GDI_STR_ALIGN_L, rgb[8], &str[8][0] );
					break;
			}
			break;
			
		case FUNC_LEVEL_5:
			switch( pFunc->Item.Lv3 ){
				case 0: // Gain
					switch( pFunc->Item.Lv4 ){
						case 0: // Cartridge
							pFunc->pMsgBox->Title.pStr = "CARTRIDGE GAIN";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine(  19,  59, 182 );
							GDI_DrawVLine(  93,  45,  91 );
							GDI_DrawVLine( 150,  45,  91 );
							//GDI_SetFont( &Fimg07x12 );
							//GDI_StringColor(  62,  65, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "[O]" );
							GDI_SetFont( &Font07x12 );
							GDI_StringColor(  20,  45, GDI_STR_ALIGN_L, RGB.PINK		, "ITEM" );
							GDI_StringColor( 143,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "S1" );
							GDI_StringColor( 200,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "S2" );
							GDI_StringColor(  20,  65, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "R-GAIN" );
							GDI_StringColor(  20,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "T-VOLT[V]" );
							GDI_StringColor(  20,  95, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "C-VOLT[V]" );
							GDI_StringColor(  20, 110, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "T-RATE[%]" );
							GDI_StringColor(  20, 125, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "C-RATE[%]" );
							rgb[0] = RGB.WHITE;
							rgb[1] = RGB.WHITE;
							rgb[2] = RGB.WHITE;
							rgb[pFunc->Item.Lv5] = RGB.YELLOW;
							for( i=3; i<11; i++ ){ rgb[i] = RGB.ORANGE; }
							tOffs = (float)pSet->pMeas->Gas[0].Inf.TargetOffs * 0.001f;
							tGain = (float)pSet->pMeas->Gas[0].Inf.TargetGain * 0.001f;
							ztos = tGain - tOffs;
							SpanScale = (float)pSet->pMeas->Gas[0].Inf.SpanGas / (float)pSet->pMeas->Gas[0].Inf.HighScale;
							SpanScale = SpanScale * (float)pSet->pMeas->Gas[0].Inf.CrossScale / 100.f;
							tFull = ztos * SpanScale;
							tVolt = tFull + tOffs;
							tRate = SpanScale * 100.f;
							cRate = (pSet->pMeas->pCur->nDet[0] - pSet->pMeas->Gas[0].Inf.bZero.f) / ztos * 100.f;
							sprintf( &str[0][0], "%s", pCtgRGAIN[pSet->Cal.TIA[0].Item.RGAIN] );
							sprintf( &str[3][0], "%5.3f", tVolt );
							sprintf( &str[4][0], "%5.3f", pSet->pMeas->pCur->nDet[0] );
							sprintf( &str[5][0], "%6.1f", tRate );
							sprintf( &str[6][0], "%6.1f", cRate );
							if( pSet->pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_CHANNELS] > 1 ){
								tOffs = (float)pSet->pMeas->Gas[1].Inf.TargetOffs * 0.001f;
								tGain = (float)pSet->pMeas->Gas[1].Inf.TargetGain * 0.001f;
								ztos = tGain - tOffs;
								SpanScale = (float)pSet->pMeas->Gas[1].Inf.SpanGas / (float)pSet->pMeas->Gas[1].Inf.HighScale;
								SpanScale = SpanScale * (float)pSet->pMeas->Gas[1].Inf.CrossScale / 100.f;
								tFull = ztos * SpanScale;
								tVolt = tFull + tOffs;
								tRate = SpanScale * 100.f;
								cRate = (pSet->pMeas->pCur->nDet[1] - pSet->pMeas->Gas[1].Inf.bZero.f) / tFull * 100.f;
								sprintf( &str[ 1][0], "%s", pCtgRGAIN[pSet->Cal.TIA[1].Item.RGAIN] );
								sprintf( &str[ 7][0], "%5.3f", tVolt );
								sprintf( &str[ 8][0], "%5.3f", pSet->pMeas->pCur->nDet[1] );
								sprintf( &str[ 9][0], "%6.1f", tRate );
								sprintf( &str[10][0], "%6.1f", cRate );
							}
							else {
								strcpy( &str[ 1][0], "   DIS" );
								strcpy( &str[ 7][0], "   DIS" );
								strcpy( &str[ 8][0], "   DIS" );
								strcpy( &str[ 9][0], "   DIS" );
								strcpy( &str[10][0], "   DIS" );
								rgb[ 1] = RGB.DARKGRAY;
								rgb[ 7] = RGB.DARKGRAY;
								rgb[ 8] = RGB.DARKGRAY;
								rgb[ 9] = RGB.DARKGRAY;
								rgb[10] = RGB.DARKGRAY;
							}
							if( pFunc->IsApproved == true )	{ strcpy( &str[2][0], "[   SAVE   ]" ); }
							else							{ strcpy( &str[2][0], "[  CANCEL  ]" ); }
							GDI_StringColor( 143,  65, GDI_STR_ALIGN_R, rgb[ 0], &str[ 0][0] );
							GDI_StringColor( 143,  80, GDI_STR_ALIGN_R, rgb[ 3], &str[ 3][0] );
							GDI_StringColor( 143,  95, GDI_STR_ALIGN_R, rgb[ 4], &str[ 4][0] );
							GDI_StringColor( 143, 110, GDI_STR_ALIGN_R, rgb[ 5], &str[ 5][0] );
							GDI_StringColor( 143, 125, GDI_STR_ALIGN_R, rgb[ 6], &str[ 6][0] );
							GDI_StringColor( 200,  65, GDI_STR_ALIGN_R, rgb[ 1], &str[ 1][0] );
							GDI_StringColor( 200,  80, GDI_STR_ALIGN_R, rgb[ 7], &str[ 7][0] );
							GDI_StringColor( 200,  95, GDI_STR_ALIGN_R, rgb[ 8], &str[ 8][0] );
							GDI_StringColor( 200, 110, GDI_STR_ALIGN_R, rgb[ 9], &str[ 9][0] );
							GDI_StringColor( 200, 125, GDI_STR_ALIGN_R, rgb[10], &str[10][0] );
							GDI_StringColor( 110, 150, GDI_STR_ALIGN_C, rgb[ 2], &str[ 2][0] );
							break;
							
						case 1: // Cavity
							pFunc->pMsgBox->Title.pStr = "CAVITY-IR GAIN";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 16, 68, 189 );
							GDI_SetFont( &Font07x12 );
							GDI_StringColor( 110, 52, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH GAS NAME   P/O P/G V-OUT" );
							for( i=0; i<5; i++ ){ rgb[i] = RGB.WHITE; }
							for( i=0; i<CAVITY_SEN_CH_MAX; i++ ){
								pGas = &pSet->pMeas->Gas[CARTRIDGE_SEN_CH_MAX+i];
								if( pSet->pMeas->Gas[CARTRIDGE_SEN_CH_MAX+i].IsPresent == true ){
									sprintf( &str[i][0], "S%u %s %3u %3u %5.3f",
										i+3,
										(char *)pGas->Inf.GasName,
										pSet->pMeas->pSen->pCvt->PtmOffs[i],
										pSet->pMeas->pSen->pCvt->PtmGain[i],
										pSet->pMeas->pCur->nDet[CARTRIDGE_SEN_CH_MAX+i]
									);
									if( (pSet->Cal.Status == FACTORY_CAL_STATUS_PROGRESS_GAIN) ||
										(pSet->Cal.Status == FACTORY_CAL_STATUS_FINISHED_GAIN) )
									{
										if( pSet->Cal.Select[CARTRIDGE_SEN_CH_MAX+i] == true ){
											if( !(pSet->pMeas->pSen->pCvt->AmpCalRunFlag & (1 << i)) ){
												if( pSet->pMeas->pSen->pCvt->AmpCalErrFlag & (1 << i) ){
													rgb[i] = RGB.ORANGE;
												}
												else {
													rgb[i] = RGB.LIGHTSKYBLUE;
												}
											}
										}
									}
								}
								else {
									sprintf( &str[i][0], "S%u DISABLED     0   0 0.000", i+3 );
									rgb[i] = RGB.DARKGRAY;
								}
							}
							switch( pSet->Cal.Status ){
								case FACTORY_CAL_STATUS_READY:
									strcpy( &str[4][0], "READY          " );
									rgb[pFunc->Item.Lv5] = RGB.YELLOW;
									break;
									
								case FACTORY_CAL_STATUS_PACKET_SEND_GAIN:
								case FACTORY_CAL_STATUS_PROGRESS_GAIN:
									strcpy( &str[4][0], "PROGRESS...    " );
									break;
									
								case FACTORY_CAL_STATUS_FINISHED_GAIN:
									if( pSet->pMeas->pSen->pCvt->AmpCalErrFlag ){
										strcpy( &str[4][0], "FAILURE        " );
									}
									else {
										strcpy( &str[4][0], "SUCCESS        " );
									}
									rgb[4] = RGB.YELLOW;
									break;
							}
							sprintf( &str[5][0], "%5.1f`C", pSet->pMeas->pCur->CvtSenTemp );
							if( (pSet->pMeas->pCur->CvtSenTemp < 30.f) || ( pSet->pMeas->pCur->CvtSenTemp > 50.f ) ){
								rgb[5] = RGB.ORANGE;
								if( pFunc->ViewBlk == true ){ strcpy( &str[5][0], "       " ); }
							}
							else {
								rgb[5] = RGB.GREEN;
							}
							for( i=0; i<4; i++ ){
								if( pSet->Cal.Select[CARTRIDGE_SEN_CH_MAX+i] == true ){ GDI_SetBackColor( RGB.SELECTED ); }
								else { GDI_SetBackColor( RGB.BLACK ); }
								GDI_StringColor( 110, 75+i*15, GDI_STR_ALIGN_C, rgb[i], &str[i][0] );
							}
							GDI_SetBackColor( RGB.BLACK );
							GDI_StringColor(  16, 143, GDI_STR_ALIGN_L, rgb[4], &str[4][0] );
							GDI_StringColor( 204, 143, GDI_STR_ALIGN_R, rgb[5], &str[5][0] );
							break;
							
						case 2: // Target Voltage
							pFunc->pMsgBox->Title.pStr = "TARGET VOLTAGE";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 26, 61, 168 );
							GDI_SetFont( &Font07x12 );
							GDI_StringColor(  26, 47, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "CH GAS NAME" );
							GDI_StringColor( 152, 47, GDI_STR_ALIGN_R, RGB.SPRINGGREEN, "OFFS" );
							GDI_StringColor( 194, 47, GDI_STR_ALIGN_R, RGB.SPRINGGREEN, "GAIN" );
							for( i=0; i<19; i++ ){ rgb[i] = RGB.WHITE; }
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( pSet->pMeas->Gas[i].IsPresent == true ){
									sprintf( &str[i*3+0][0], "S%u %s", i+1, (char *)pSet->pMeas->Gas[i].Inf.GasName );
									sprintf( &str[i*3+1][0], "%5.3f", pSet->Cal.TargetOffs[i] * 0.001f );
									sprintf( &str[i*3+2][0], "%5.3f", pSet->Cal.TargetGain[i] * 0.001f );
									rgb[i*3+0] = RGB.LIGHTSKYBLUE;
								}
								else {
									sprintf( &str[i*3+0][0], "S%u DISABLED  ", 1+i );
									strcpy( &str[i*3+1][0], "0.000" );
									strcpy( &str[i*3+2][0], "0.000" );
									rgb[i*3+0] = RGB.DARKGRAY;
									rgb[i*3+1] = RGB.DARKGRAY;
									rgb[i*3+2] = RGB.DARKGRAY;
								}
							}
							if( pFunc->IsApproved == true )	{ strcpy( &str[18][0], "[   SAVE   ]" ); }
							else							{ strcpy( &str[18][0], "[  CANCEL  ]" ); }
							switch( pFunc->Item.Lv5 ){
								case  0: rgb[ 1] = RGB.YELLOW; break;
								case  1: rgb[ 2] = RGB.YELLOW; break;
								case  2: rgb[ 4] = RGB.YELLOW; break;
								case  3: rgb[ 5] = RGB.YELLOW; break;
								case  4: rgb[ 7] = RGB.YELLOW; break;
								case  5: rgb[ 8] = RGB.YELLOW; break;
								case  6: rgb[10] = RGB.YELLOW; break;
								case  7: rgb[11] = RGB.YELLOW; break;
								case  8: rgb[13] = RGB.YELLOW; break;
								case  9: rgb[14] = RGB.YELLOW; break;
								case 10: rgb[16] = RGB.YELLOW; break;
								case 11: rgb[17] = RGB.YELLOW; break;
								case 12: rgb[18] = RGB.YELLOW; break;
							}
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								GDI_StringColor(  26, 67+i*13, GDI_STR_ALIGN_L, rgb[i*3+0], &str[i*3+0][0] );
								GDI_StringColor( 152, 67+i*13, GDI_STR_ALIGN_R, rgb[i*3+1], &str[i*3+1][0] );
								GDI_StringColor( 194, 67+i*13, GDI_STR_ALIGN_R, rgb[i*3+2], &str[i*3+2][0] );
							}
							GDI_StringColor( 110, 150, GDI_STR_ALIGN_C, rgb[18], &str[18][0] );
							break;
					}
					break;
					
				case 2: // Span
					pFunc->pMsgBox->Title.pStr = "SPAN CAL";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 29,  61, 162 );
					GDI_DrawHLine( 29, 144, 162 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 30,  68, GDI_STR_ALIGN_L, RGB.SPRINGGREEN	, "HIGH SCALE:" );
					GDI_StringColor( 30,  80, GDI_STR_ALIGN_L, RGB.SPRINGGREEN	, "SPAN GAS  :" );
					GDI_StringColor( 30,  92, GDI_STR_ALIGN_L, RGB.AQUAMARINE	, "ZERO TEMP :" );
					GDI_StringColor( 30, 104, GDI_STR_ALIGN_L, RGB.AQUAMARINE	, "CURR TEMP :" );
					GDI_StringColor( 30, 116, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE	, "RESPONSE  :" );
					GDI_StringColor( 30, 128, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE	, "CURR GAS  :" );
					pGas = &pSet->pMeas->Gas[pFunc->Item.Lv4];
					rgb[0] = RGB.PINK;
					rgb[1] = RGB.ORANGE;
					rgb[2] = RGB.SPRINGGREEN;
					rgb[4] = RGB.AQUAMARINE;
					rgb[5] = RGB.AQUAMARINE;
					rgb[6] = RGB.LIGHTSKYBLUE;
					rgb[7] = RGB.LIGHTSKYBLUE;
					rgb[9] = RGB.GREEN;
					if( pFunc->Item.Lv5 == 4 ){
						rgb[3] = RGB.WHITE, rgb[8] = RGB.YELLOW;
					}
					else {
						rgb[8] = RGB.WHITE, rgb[3] = RGB.YELLOW;
					}
					sprintf( &str[0][0], "[S%u] %s", pFunc->Item.Lv4+1, (char *)pGas->Inf.GasName );
					sprintf( &str[1][0], "x%4.2f", pGas->Inf.CrossScale * 0.01f );
					sprintf( &str[2][0], "%s %s", vStrGasConc( pGas->Inf.Dp, pGas->Inf.HighScale ), pUnit[pGas->Inf.Unit] );
					sprintf( &str[3][0], "%s %s", vStrGasConc( pGas->Inf.Dp, pSet->Cal.SpanGas ),	pUnit[pGas->Inf.Unit] );
					if( pFunc->Item.Lv5 < 4 ){
						switch( pGas->Inf.Dp ){
							case 0: Digit[0] = 1, Digit[1] = 2, Digit[2] = 3, Digit[3] = 4; break;
							case 1: Digit[0] = 0, Digit[1] = 1, Digit[2] = 2, Digit[3] = 4; break;
							case 2: Digit[0] = 0, Digit[1] = 1, Digit[2] = 3, Digit[3] = 4; break;
							case 3: Digit[0] = 0, Digit[1] = 2, Digit[2] = 3, Digit[3] = 4; break;
						}
						for( i=pFunc->Item.Lv5; i<4; i++ ){
							if( i == pFunc->Item.Lv5 ){
								if( pFunc->ViewBlk == true ){
									if( str[3][Digit[i]] != ' ' ){ str[3][Digit[i]] = ' '; }
								}
								else {
									if( str[3][Digit[i]] == ' ' ){ str[3][Digit[i]] = '0'; }
								}
							}
							else {
								if( str[3][Digit[i]] == ' ' ){ str[3][Digit[i]] = '0'; }
							}
						}
					}
					sprintf( &str[4][0], "%5.1f   `C", pGas->Inf.mZeroTemp.f );
					if( pFunc->Item.Lv4 == 0 ){
						sprintf( &str[5][0], "%5.1f   `C", pSet->pMeas->pCur->CtgSenTemp );
					}
					else {
						sprintf( &str[5][0], "%5.1f   `C", pSet->pMeas->pCur->CvtSenTemp );
					}
					CalRsp =
					Measuring_Gas_Response_Calculation(
						pFunc->Item.Lv4,
						pSet->Cal.SpanGas,
						pSet->pMeas->pCur->nDet[pFunc->Item.Lv4]
					);
					if( pGas->Inf.Method == MEASURING_METHOD_CT ){
						if( pGas->Inf.SenNo < CT_SEN_NAP_100AM ){ // EC Sensor
							sprintf( &str[6][0], "%6.0f   nA", CalRsp );
						}
						else {
							sprintf( &str[6][0], "%6.1f   mV", CalRsp );
						}
					}
					else {
						sprintf( &str[6][0], "%6.1f    %%", CalRsp );
					}
					sprintf( &str[7][0], "%s %s", vStrGasConc( pGas->Inf.Dp, pGas->Act ), pUnit[pGas->Inf.Unit] );
					switch( pSet->Cal.Status ){
						case FACTORY_CAL_STATUS_READY:
							if( pFunc->IsApproved == true )	{			strcpy( &str[8][0], "READY        " ); }
							else							{			strcpy( &str[8][0], "CANCEL       " ); }
							break;
							
						case FACTORY_CAL_STATUS_INJECT_GAS:				strcpy( &str[8][0], "INJECT GAS..." ); break;
						case FACTORY_CAL_STATUS_DECISION:				strcpy( &str[8][0], "DECISION...  " ); break;
						case FACTORY_CAL_STATUS_FINISHED:
							switch( pSet->Cal.Result[pFunc->Item.Lv4] ){
								case FACTORY_CAL_RESULT_SUCCESS:		strcpy( &str[8][0], "SUCCESS      " ); break;
								case FACTORY_CAL_RESULT_TIMEOUT:		strcpy( &str[8][0], "TIMEOUT      " ); break;
								case FACTORY_CAL_RESULT_LO_RESPONSE:	strcpy( &str[8][0], "LO RESPONSE  " ); break;
								case FACTORY_CAL_RESULT_HI_RESPONSE:	strcpy( &str[8][0], "HI RESPONSE  " ); break;
								case FACTORY_CAL_RESULT_OUTPUT_REVERSE:	strcpy( &str[8][0], "REVERSE      " ); break;
							}
							break;
					}
					sprintf( &str[9][0], "%3u  SEC", pSet->Cal.InjectionTmr );		
					GDI_StringColor(  30,  45, GDI_STR_ALIGN_L, rgb[0], &str[0][0] );
					GDI_StringColor( 190,  45, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
					GDI_StringColor( 190,  68, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
					GDI_StringColor( 190,  80, GDI_STR_ALIGN_R, rgb[3], &str[3][0] );
					GDI_StringColor( 190,  92, GDI_STR_ALIGN_R, rgb[4], &str[4][0] );
					GDI_StringColor( 190, 104, GDI_STR_ALIGN_R, rgb[5], &str[5][0] );
					GDI_StringColor( 190, 116, GDI_STR_ALIGN_R, rgb[6], &str[6][0] );
					GDI_StringColor( 190, 128, GDI_STR_ALIGN_R, rgb[7], &str[7][0] );
					GDI_StringColor(  30, 151, GDI_STR_ALIGN_L, rgb[8], &str[8][0] );
					GDI_StringColor( 190, 151, GDI_STR_ALIGN_R, rgb[9], &str[9][0] );
					break;
					
				case 3: // Base Zero
					switch( pFunc->Item.Lv4 ){
						case 0: // Cal
							pFunc->pMsgBox->Title.pStr = "BASE ZERO CAL";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 26, 61, 168 );
							GDI_SetFont( &Font07x12 );
							GDI_StringColor( 110, 47, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH GAS NAME    CONC UNIT" );
							for( i=0; i<7; i++ ){ rgb[i] = RGB.WHITE; }
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								pGas = &pSet->pMeas->Gas[i];
								if( pGas->IsPresent == true ){
									if( pGas->Inf.Func == MEASURING_GAS_FUNC_IR_REF ){
										sprintf( &str[i][0], "S%u %s     0 ----", 1+i, (char *)pGas->Inf.GasName );
										rgb[i] = RGB.DARKGRAY;
									}
									else {
										sprintf( &str[i][0], "S%u %s %s %s",
											1+i,
											(char *)pGas->Inf.GasName,
											vStrGasConc( pGas->Inf.Dp, pGas->Act ),
											pUnit[pGas->Inf.Unit]
										);
										if( pGas->Inf.bZeroUse == true ){
											if( i == pFunc->Item.Lv5 ){
												rgb[i] = RGB.YELLOW;
											}
											else {
												if( (pSet->Cal.Status == FACTORY_CAL_STATUS_FINISHED) &&
													(pSet->Cal.Select[i] == true) )
												{
													switch( pSet->Cal.Result[i] ){
														case FACTORY_CAL_RESULT_SUCCESS:
															rgb[i] = RGB.LIGHTSKYBLUE;
															break;
															
														case FACTORY_CAL_RESULT_TIMEOUT:
															sprintf( &str[i][0], "S%u TIMEOUT    %s %s",
																1+i,
																vStrGasConc( pGas->Inf.Dp, pGas->Act ),
																pUnit[pGas->Inf.Unit]
															);
															rgb[i] = RGB.ORANGE;
															break;
													}
												}
											}
										}
										else {
											rgb[i] = RGB.DARKGRAY;
										}
									}
								}
								else {
									sprintf( &str[i][0], "S%u DISABLED       0 ----", 1+i );
									rgb[i] = RGB.DARKGRAY;
								}
							}
							switch( pSet->Cal.Status ){
								case FACTORY_CAL_STATUS_READY:		strcpy( &str[6][0], "READY        " ); break;
								case FACTORY_CAL_STATUS_INJECT_GAS:	strcpy( &str[6][0], "INJECT GAS..." ); break;
								case FACTORY_CAL_STATUS_DECISION:	strcpy( &str[6][0], "DECISION...  " ); break;
								case FACTORY_CAL_STATUS_FINISHED:
									for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
										if( (pSet->Cal.Select[i] == true) && (pSet->Cal.Result[i] != FACTORY_CAL_RESULT_SUCCESS) ){
											IsSuccessful = false;
											break;
										}
									}
									if( IsSuccessful == true )	{ strcpy( &str[6][0], "SUCCESS      " ); }
									else						{ strcpy( &str[6][0], "FAILURE      " ); }
									break;
							}
							if( (pFunc->Item.Lv5 == 6) && (pSet->Cal.Status != FACTORY_CAL_STATUS_DECISION) ){
								rgb[6] = RGB.YELLOW;
							}
							sprintf( &str[7][0], "%3u  SEC", pSet->Cal.InjectionTmr );
							rgb[7] = RGB.GREEN;
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( pSet->Cal.Select[i] == true ){ GDI_SetBackColor( RGB.SELECTED ); }
								else { GDI_SetBackColor( RGB.BLACK ); }
								GDI_StringColor( 110, 67+i*13, GDI_STR_ALIGN_C, rgb[i], &str[i][0] );
							}
							GDI_SetBackColor( RGB.BLACK );
							GDI_StringColor(  26, 150, GDI_STR_ALIGN_L, rgb[6], &str[6][0] );
							GDI_StringColor( 194, 150, GDI_STR_ALIGN_R, rgb[7], &str[7][0] );
							break;
							
						case 1: // Set
							pFunc->pMsgBox->Title.pStr = "BASE ZERO SET";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetTextColor( RGB.DARKGRAY );
							GDI_DrawHLine( 19, 61, 182 );
							GDI_SetFont( &Font07x12 );
							GDI_StringColor(  19, 47, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "CH GAS NAME" );
							GDI_StringColor( 138, 47, GDI_STR_ALIGN_R, RGB.SPRINGGREEN, "USE" );
							GDI_StringColor( 173, 47, GDI_STR_ALIGN_R, RGB.SPRINGGREEN, "PER" );
							GDI_StringColor( 201, 47, GDI_STR_ALIGN_R, RGB.SPRINGGREEN, "SEC" );
							for( i=0; i<25; i++ ){ rgb[i] = RGB.WHITE; }
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								if( pSet->pMeas->Gas[i].IsPresent == true ){
									sprintf( &str[i*4+0][0], "S%u %s", 1+i, (char *)pSet->pMeas->Gas[i].Inf.GasName );
									sprintf( &str[i*4+1][0], "%s", pUse[pSet->Cal.bZeroUse[i]] );
									sprintf( &str[i*4+2][0], "%4.1f", pSet->Cal.bZeroPer[i] * 0.1f );
									sprintf( &str[i*4+3][0], "%3u", pSet->Cal.bZeroSec[i] );
									if( pSet->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_REF ){
										rgb[i*4+0] = RGB.DARKGRAY;
										rgb[i*4+1] = RGB.DARKGRAY;
										rgb[i*4+2] = RGB.DARKGRAY;
										rgb[i*4+3] = RGB.DARKGRAY;
									}
									else {
										rgb[i*4+0] = RGB.LIGHTSKYBLUE;
									}
								}
								else {
									sprintf( &str[i*4+0][0], "S%u DISABLED  ", 1+i );
									strcpy( &str[i*4+1][0], "---" );
									strcpy( &str[i*4+2][0], "0.0" );
									strcpy( &str[i*4+3][0], "0" );
									rgb[i*4+0] = RGB.DARKGRAY;
									rgb[i*4+1] = RGB.DARKGRAY;
									rgb[i*4+2] = RGB.DARKGRAY;
									rgb[i*4+3] = RGB.DARKGRAY;
								}
							}
							if( pFunc->IsApproved == true )	{ strcpy( &str[24][0], "[   SAVE   ]" ); }
							else							{ strcpy( &str[24][0], "[  CANCEL  ]" ); }
							switch( pFunc->Item.Lv5 ){
								case  0: rgb[ 1] = RGB.YELLOW; break;
								case  1: rgb[ 2] = RGB.YELLOW; break;
								case  2: rgb[ 3] = RGB.YELLOW; break;
								case  3: rgb[ 5] = RGB.YELLOW; break;
								case  4: rgb[ 6] = RGB.YELLOW; break;
								case  5: rgb[ 7] = RGB.YELLOW; break;
								case  6: rgb[ 9] = RGB.YELLOW; break;
								case  7: rgb[10] = RGB.YELLOW; break;
								case  8: rgb[11] = RGB.YELLOW; break;
								case  9: rgb[13] = RGB.YELLOW; break;
								case 10: rgb[14] = RGB.YELLOW; break;
								case 11: rgb[15] = RGB.YELLOW; break;
								case 12: rgb[17] = RGB.YELLOW; break;
								case 13: rgb[18] = RGB.YELLOW; break;
								case 14: rgb[19] = RGB.YELLOW; break;
								case 15: rgb[21] = RGB.YELLOW; break;
								case 16: rgb[22] = RGB.YELLOW; break;
								case 17: rgb[23] = RGB.YELLOW; break;
								case 18: rgb[24] = RGB.YELLOW; break;
							}
							for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
								GDI_StringColor(  19, 67+i*13, GDI_STR_ALIGN_L, rgb[i*4+0], &str[i*4+0][0] );
								GDI_StringColor( 138, 67+i*13, GDI_STR_ALIGN_R, rgb[i*4+1], &str[i*4+1][0] );
								GDI_StringColor( 173, 67+i*13, GDI_STR_ALIGN_R, rgb[i*4+2], &str[i*4+2][0] );
								GDI_StringColor( 201, 67+i*13, GDI_STR_ALIGN_R, rgb[i*4+3], &str[i*4+3][0] );
							}
							GDI_StringColor( 110, 150, GDI_STR_ALIGN_C, rgb[24], &str[24][0] );
							break;
					}
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vFunc_Factory_SysInit( Func_HandleType *pFunc, Factory_SetType *pSet )
{
	char				str[7][30];
	uint16_t			rgb[7];
	U8					i;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			pFunc->pMsgBox->Title.pStr = "INIT MENU";
			pFunc->pMsgBox->Msg[0].pStr = "MAIN";
			pFunc->pMsgBox->Msg[1].pStr = "SENSOR SET";
			pFunc->pMsgBox->Msg[2].pStr = "SENSOR CAL";
			pFunc->pMsgBox->Msg[3].pStr = "SYSTEM ALL";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv3].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 4;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_4:
			if( pSet->Init.IsConfirmMode == true ){
				pFunc->pMsgBox->Title.pStr = "INIT NOW?";
				if( pFunc->IsApproved == true ){ pFunc->pMsgBox->Msg[0].pStr = "YES"; }
				else { pFunc->pMsgBox->Msg[0].pStr = "NO"; }
				pFunc->pMsgBox->Msg[0].tClr = RGB.YELLOW;
				pFunc->pMsgBox->LineColor = RGB.RED;
				pFunc->pMsgBox->TboxColor = RGB.RED;
				pFunc->pMsgBox->BackColor = 0x2800;
				pFunc->pMsgBox->Width = 150;
				pFunc->pMsgBox->MsgLine = 1;
				pFunc->pMsgBox->OutLine = 1;
				vMessageBox( pFunc->pMsgBox );
			}
			else {
				switch( pFunc->Item.Lv3 ){
					case 0: // Main
						pFunc->pMsgBox->Title.pStr = "SELECT ITEM";
						pFunc->pMsgBox->Width = 216;
						pFunc->pMsgBox->MsgLine = 0;
						pFunc->pMsgBox->OutLine = 6;
						vMessageBox( pFunc->pMsgBox );
						GDI_SetFont( &Font07x12 );
						strcpy( &str[0][0], "[    mA CH-1    ]" );
						strcpy( &str[1][0], "[    mA CH-2    ]" );
						strcpy( &str[2][0], "[     RELAY     ]" );
						strcpy( &str[3][0], "[   FLOW RATE   ]" );
						for( i=0; i<5; i++ ){
							if( i == pFunc->Item.Lv4 ){
								rgb[i] = RGB.YELLOW;
							}
							else {
								if		( (SystemOption.Item.IsPoeMode == true)	&& (i  < 2) ){ rgb[i] = RGB.DARKGRAY;	}
								else if	( (pSet->pMeas->Conf.OutMax < 2)		&& (i == 1) ){ rgb[i] = RGB.DARKGRAY;	}
								else														 { rgb[i] = RGB.WHITE;		}
							}
							GDI_SetBackColor( RGB.BLACK );
							switch( i ){
								case 0: if( pSet->Init.Set.Bit.mA_1		== true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 1: if( pSet->Init.Set.Bit.mA_2		== true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 2: if( pSet->Init.Set.Bit.Relay	== true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 3: if( pSet->Init.Set.Bit.Flow		== true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 4:
									GDI_StringColor( 110, 135, GDI_STR_ALIGN_C, rgb[4], "CONFIRM" );
									GDI_DrawRect( 53, 129, 115, 22 );
									return;
							}
							GDI_StringColor( 110, 54+i*15, GDI_STR_ALIGN_C, rgb[i], &str[i][0] );
						}
						break;
					
					case 1: // Sensor Set
						pFunc->pMsgBox->Title.pStr = "SELECT ITEM";
						pFunc->pMsgBox->Width = 216;
						pFunc->pMsgBox->MsgLine = 0;
						pFunc->pMsgBox->OutLine = 6;
						vMessageBox( pFunc->pMsgBox );
						GDI_SetFont( &Font07x12 );
						for( i=0; i<7; i++ ){ rgb[i] = RGB.WHITE; }
						for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
							if( pSet->pMeas->Gas[i].IsPresent == true ){
								sprintf( &str[i][0], "[ S%u %s ]", i+1,  (char *)pSet->pMeas->Gas[i].Inf.GasName );
								if( i == pFunc->Item.Lv4 ){ rgb[i] = RGB.YELLOW; }
							}
							else {
								sprintf( &str[i][0], "[ S%u DISABLED   ]", i+1 );
								rgb[i] = RGB.DARKGRAY;
							}
							GDI_SetBackColor( RGB.BLACK );
							switch( i ){
								case 0: if( pSet->Init.Set.Bit.S1_Set == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 1: if( pSet->Init.Set.Bit.S2_Set == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 2: if( pSet->Init.Set.Bit.S3_Set == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 3: if( pSet->Init.Set.Bit.S4_Set == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 4: if( pSet->Init.Set.Bit.S5_Set == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 5: if( pSet->Init.Set.Bit.S6_Set == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
							}
							GDI_StringColor( 110, 45+i*14, GDI_STR_ALIGN_C, rgb[i], &str[i][0] );
						}
						GDI_SetBackColor( RGB.BLACK );
						if( pFunc->Item.Lv4 == 6 ){ rgb[6] = RGB.YELLOW; }
						GDI_StringColor( 110, 141, GDI_STR_ALIGN_C, rgb[6], "CONFIRM" );
						GDI_DrawRect( 53, 135, 115, 22 );
						break;
						
					case 2: // Sensor Cal
						pFunc->pMsgBox->Title.pStr = "SELECT ITEM";
						pFunc->pMsgBox->Width = 216;
						pFunc->pMsgBox->MsgLine = 0;
						pFunc->pMsgBox->OutLine = 6;
						vMessageBox( pFunc->pMsgBox );
						GDI_SetFont( &Font07x12 );
						for( i=0; i<7; i++ ){ rgb[i] = RGB.WHITE; }
						for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
							if( pSet->pMeas->Gas[i].IsPresent == true ){
								sprintf( &str[i][0], "[ S%u %s ]", i+1,  (char *)pSet->pMeas->Gas[i].Inf.GasName );
								if( i == pFunc->Item.Lv4 ){ rgb[i] = RGB.YELLOW; }
							}
							else {
								sprintf( &str[i][0], "[ S%u DISABLED   ]", i+1 );
								rgb[i] = RGB.DARKGRAY;
							}
							GDI_SetBackColor( RGB.BLACK );
							switch( i ){
								case 0: if( pSet->Init.Set.Bit.S1_Cal == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 1: if( pSet->Init.Set.Bit.S2_Cal == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 2: if( pSet->Init.Set.Bit.S3_Cal == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 3: if( pSet->Init.Set.Bit.S4_Cal == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 4: if( pSet->Init.Set.Bit.S5_Cal == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
								case 5: if( pSet->Init.Set.Bit.S6_Cal == true ){ GDI_SetBackColor( RGB.SELECTED ); } break;
							}
							GDI_StringColor( 110, 45+i*14, GDI_STR_ALIGN_C, rgb[i], &str[i][0] );
						}
						GDI_SetBackColor( RGB.BLACK );
						if( pFunc->Item.Lv4 == 6 ){ rgb[6] = RGB.YELLOW; }
						GDI_StringColor( 110, 141, GDI_STR_ALIGN_C, rgb[6], "CONFIRM" );
						GDI_DrawRect( 53, 135, 115, 22 );
						break;
					
					case 3: // System All
						pFunc->pMsgBox->Title.pStr = "WARNING";
						pFunc->pMsgBox->Width = 216;
						pFunc->pMsgBox->MsgLine = 0;
						pFunc->pMsgBox->OutLine = 6;
						vMessageBox( pFunc->pMsgBox );
						GDI_SetFont( &Font07x12 );
						GDI_StringColor( 110,  65, GDI_STR_ALIGN_C, RGB.ORANGERED, "ALL DATA IN THE SYSTEM" );
						GDI_StringColor( 110,  80, GDI_STR_ALIGN_C, RGB.ORANGERED, "IS INITIALIZED!       " );
						GDI_StringColor( 110, 126, GDI_STR_ALIGN_C, RGB.YELLOW, "CONFIRM" );
						GDI_DrawRect( 53, 121, 115, 22 );
						break;
				}
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vFunc_Factory_SysInfo( Func_HandleType *pFunc, Factory_SetType *pSet )
{
	char		str[10][30];
	uint16_t	rgb[10];
	U8			i;
	char		*pIsEnabled[2] = { "OFF", " ON" };
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv3 ){
				case 0: // Version
					pFunc->pMsgBox->Title.pStr = "VERSION";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 32, 85, 156 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 110, 70, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DEVICE       H/W   F/W" );
					sprintf( &str[0][0], "MAIN         %3.1f  %4.2f", HardwareVersion, FirmwareVersion );
					rgb[0] = RGB.LIGHTSKYBLUE;
					if( pSet->pMeas->pSen->pCtg->IsPresent == true ){
						sprintf( &str[1][0], "CARTRIDGE    %3.1f  %4.2f",
							pSet->pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_HW_VER] * 0.1f,
							pSet->pMeas->pSen->pCtg->Inf[CARTRIDGE_INF_FW_VER] * 0.01f
						);
						rgb[1] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[1][0], "CARTRIDGE    -.-  -.--" );
						rgb[1] = RGB.DARKGRAY;
					}
					if( pSet->pMeas->pSen->pCvt->IsPresent == true ){
						sprintf( &str[2][0], "CAVITY-IR    %3.1f  %4.2f",
							pSet->pMeas->pSen->pCvt->Inf[CAVITY_INF_HW_VER] * 0.1f,
							pSet->pMeas->pSen->pCvt->Inf[CAVITY_INF_FW_VER] * 0.01f
						);
						rgb[2] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[2][0], "CAVITY-IR    -.-  -.--" );
						rgb[2] = RGB.DARKGRAY;
					}
					for( i=0; i<3; i++ ){ GDI_StringColor( 110, 93+i*15, GDI_STR_ALIGN_C, rgb[i], &str[i][0] ); }
					break;
					
				case 1: // Temperature
					pFunc->pMsgBox->Title.pStr = "TEMPERATURE";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 22, 77, 176 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 110, 62, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DEVICE        SEN     MCU" );
					sprintf( &str[0][0], "MAIN      %5.1f`C %5.1f`C", pSet->pMeas->pCur->SysSenTemp, pSet->pMeas->pCur->SysMcuTemp );
					rgb[0] = RGB.LIGHTSKYBLUE;
					if( pSet->pMeas->pSen->pCtg->IsPresent == true ){
						sprintf( &str[1][0], "CARTRIDGE %5.1f`C %5.1f`C",
							pSet->pMeas->pCur->CtgSenTemp,
							pSet->pMeas->pCur->CtgMcuTemp
						);
						rgb[1] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[1][0], "CARTRIDGE  --.-`C  --.-`C" );
						rgb[1] = RGB.DARKGRAY;
					}
					if( pSet->pMeas->pSen->pCvt->IsPresent == true ){
						sprintf( &str[2][0], "CAVITY-IR %5.1f`C %5.1f`C",
							pSet->pMeas->pCur->CvtSenTemp,
							pSet->pMeas->pCur->CvtMcuTemp
						);
						rgb[2] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[2][0], "CAVITY-IR  --.-`C  --.-`C" );
						rgb[2] = RGB.DARKGRAY;
					}
					if( SystemOption.Item.IsBatMode == true ){
						sprintf( &str[3][0], "BATTERY   %5.1f`C %5.1f %%",
							pSet->pMeas->pCur->BatSenTemp,
							pSet->pMeas->pCur->BatSenJper
						);
						rgb[3] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[3][0], "BATTERY    --.-`C  --.- %" );
						rgb[3] = RGB.DARKGRAY;
					}
					for( i=0; i<4; i++ ){ GDI_StringColor( 110, 85+i*15, GDI_STR_ALIGN_C, rgb[i], &str[i][0] ); }
					break;
					
				case 2: // Power
					pFunc->pMsgBox->Title.pStr = "POWER";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  20,  47, GDI_STR_ALIGN_L, RGB.SPRINGGREEN , "ITEM" );
					GDI_StringColor( 200,  47, GDI_STR_ALIGN_R, RGB.SPRINGGREEN , "DESCRIPTION" );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 20, 61, 180 );
					if( SystemOption.Item.IsBatMode == true ){
						switch( PwrCtrl.BQ.Status.Item.Charge_Status ){
							case BQ2579x_CHRG_STAT_Not_Charging					: strcpy( &str[0][0], "  NOT CHARGING" ); break;
							case BQ2579x_CHRG_STAT_Trickle_Charge				: strcpy( &str[0][0], "TRICKLE CHARGE" ); break;
							case BQ2579x_CHRG_STAT_Pre_Charge					: strcpy( &str[0][0], "    PRE-CHARGE" ); break;
							case BQ2579x_CHRG_STAT_Fast_Charge					: strcpy( &str[0][0], "   FAST CHARGE" ); break;
							case BQ2579x_CHRG_STAT_Taper_Charge					: strcpy( &str[0][0], "  TAPER CHARGE" ); break;
							case BQ2579x_CHRG_STAT_Reserved						: strcpy( &str[0][0], "      RESERVED" ); break;
							case BQ2579x_CHRG_STAT_Top_Off_Timer_Active_Charging: strcpy( &str[0][0], "TOP-OFF TM ACT" ); break;
							case BQ2579x_CHRG_STAT_Charge_Termination_Done		: strcpy( &str[0][0], " FULLY CHARGED" ); break;
						}
						sprintf( &str[1][0], "%02X %02X %02X %02X %02X",
							PwrCtrl.BQ.Status.Byte[0],
							PwrCtrl.BQ.Status.Byte[1],
							PwrCtrl.BQ.Status.Byte[2],
							PwrCtrl.BQ.Status.Byte[3],
							PwrCtrl.BQ.Status.Byte[4]
						);
						sprintf( &str[2][0], "%02X %02X", PwrCtrl.BQ.Faults.Byte[0], PwrCtrl.BQ.Faults.Byte[1] );
						sprintf( &str[3][0], "%5.2f[A] %5.2f[V]", pSet->pMeas->pCur->IBUS, pSet->pMeas->pCur->VBUS );
						sprintf( &str[4][0], "%5.2f[A] %5.2f[V]", pSet->pMeas->pCur->IBAT, pSet->pMeas->pCur->VBAT );
						rgb[0] = RGB.AQUAMARINE;
						rgb[1] = RGB.LIGHTGREEN;
						rgb[2] = RGB.LIGHTGREEN;
						rgb[3] = RGB.LIGHTSKYBLUE;
						rgb[4] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[0][0], "DISABLED" );
						strcpy( &str[1][0], "-- -- -- -- --" );
						strcpy( &str[2][0], "-- --" );
						strcpy( &str[3][0], " -.--[A] --.--[V]" );
						strcpy( &str[4][0], " -.--[A] --.--[V]" );
						rgb[0] = RGB.DARKGRAY;
						rgb[1] = RGB.DARKGRAY;
						rgb[2] = RGB.DARKGRAY;
						rgb[3] = RGB.DARKGRAY;
						rgb[4] = RGB.DARKGRAY;
					}
					sprintf( &str[5][0], "%5.2f[V]", pSet->pMeas->pCur->VSUP );
					sprintf( &str[6][0], "%5.2f[V]", pSet->pMeas->pCur->VBACKUP );
					rgb[5] = RGB.LIGHTSKYBLUE;
					rgb[6] = RGB.PINK;
					GDI_StringColor(  20,  68, GDI_STR_ALIGN_L, rgb[0], "CHARGE" );
					GDI_StringColor(  20,  81, GDI_STR_ALIGN_L, rgb[1], "STATUS" );
					GDI_StringColor(  20,  94, GDI_STR_ALIGN_L, rgb[2], "FAULTS" );
					GDI_StringColor(  20, 107, GDI_STR_ALIGN_L, rgb[3], "BUS" );
					GDI_StringColor(  20, 120, GDI_STR_ALIGN_L, rgb[4], "BAT" );
					GDI_StringColor(  20, 133, GDI_STR_ALIGN_L, rgb[5], "SYSTEM" );
					GDI_StringColor(  20, 146, GDI_STR_ALIGN_L, rgb[6], "BACKUP" );
					GDI_StringColor( 200,  68, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
					GDI_StringColor( 200,  81, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
					GDI_StringColor( 200,  94, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
					GDI_StringColor( 200, 107, GDI_STR_ALIGN_R, rgb[3], &str[3][0] );
					GDI_StringColor( 200, 120, GDI_STR_ALIGN_R, rgb[4], &str[4][0] );
					GDI_StringColor( 200, 133, GDI_STR_ALIGN_R, rgb[5], &str[5][0] );
					GDI_StringColor( 200, 146, GDI_STR_ALIGN_R, rgb[6], &str[6][0] );
					break;
					
				case 3: // Measuring Range
					pFunc->pMsgBox->Title.pStr = "MEASURING RANGE";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 26, 90, 169 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 110, 75, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "CH GAS NAME     H/S UNIT" );
					rgb[0] = RGB.LIGHTSKYBLUE;
					sprintf( &str[0][0], "1. %s %s %s",
						(char *)pSet->pMeas->pGas[0]->Inf.GasName,
						vStrGasConc( pSet->pMeas->pGas[0]->Inf.Dp, pSet->pMeas->pGas[0]->Inf.HighScale ),
						pUnit[pSet->pMeas->pGas[0]->Inf.Unit]
					);
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[1][0], "2. %s %s %s",
							(char *)pSet->pMeas->pGas[1]->Inf.GasName,
							vStrGasConc( pSet->pMeas->pGas[1]->Inf.Dp, pSet->pMeas->pGas[1]->Inf.HighScale ),
							pUnit[pSet->pMeas->pGas[1]->Inf.Unit]
						);
						rgb[1] = RGB.LIGHTSKYBLUE;
					}
					else {
						strcpy( &str[1][0], "2. DISABLED       0 ----" );
						rgb[1] = RGB.DARKGRAY;
					}
					for( i=0; i<2; i++ ){ GDI_StringColor( 110, 98+i*15, GDI_STR_ALIGN_C, rgb[i], &str[i][0] ); }
					break;
					
				case 4: // Measuring Option
					pFunc->pMsgBox->Title.pStr = "MEASURING OPTION";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 22, 70, 174 );
					GDI_SetFont( &Font07x12 );
					for( i=0; i<10; i++ ){ rgb[i] = RGB.LIGHTSKYBLUE; }
					GDI_StringColor(  22,  55, GDI_STR_ALIGN_L, RGB.SPRINGGREEN , "ITEM" );
					GDI_StringColor( 159,  55, GDI_STR_ALIGN_R, RGB.SPRINGGREEN , "CH1" );
					GDI_StringColor( 196,  55, GDI_STR_ALIGN_R, RGB.SPRINGGREEN , "CH2" );
					GDI_StringColor(  22,  75, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "ZERO SKIP[ % ]" );
					GDI_StringColor(  22,  90, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "SPAN SKIP[ % ]" );
					GDI_StringColor(  22, 105, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "OUT DELAY[ % ]" );
					GDI_StringColor(  22, 120, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "OUT DELAY[SEC]" );
					GDI_StringColor(  22, 135, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "UNDER CHECK" );
					sprintf( &str[0][0], "%4.1f", pSet->pMeas->pGas[0]->Inf.ZeroSkipPer * 0.1f );
					sprintf( &str[1][0], "%4.1f", pSet->pMeas->pGas[0]->Inf.SpanSkipPer * 0.1f );
					sprintf( &str[2][0], "%4.1f", pSet->pMeas->pGas[0]->Inf.OutDelayPer * 0.1f );
					if( pSet->pMeas->pGas[0]->Inf.OutDelaySec ){
						sprintf( &str[3][0], "%4u", pSet->pMeas->pGas[0]->Inf.OutDelaySec );
					}
					else {
						strcpy( &str[3][0], " OFF" );
					}
					sprintf( &str[4][0], "%s", pIsEnabled[pSet->pMeas->pAlr[0]->Inf.IsUnderCheckEnabled] );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[5][0], "%4.1f", pSet->pMeas->pGas[1]->Inf.ZeroSkipPer * 0.1f );
						sprintf( &str[6][0], "%4.1f", pSet->pMeas->pGas[1]->Inf.SpanSkipPer * 0.1f );
						sprintf( &str[7][0], "%4.1f", pSet->pMeas->pGas[1]->Inf.OutDelayPer * 0.1f );
						if( pSet->pMeas->pGas[1]->Inf.OutDelaySec ){
							sprintf( &str[8][0], "%4u", pSet->pMeas->pGas[1]->Inf.OutDelaySec );
						}
						else {
							strcpy( &str[8][0], " OFF" );
						}
						sprintf( &str[9][0], "%s", pIsEnabled[pSet->pMeas->pAlr[1]->Inf.IsUnderCheckEnabled] );
					}
					else {
						for( i=5; i<10; i++ ){
							strcpy( &str[i][0], "DIS" );
							rgb[i] = RGB.DARKGRAY;
						}
					}
					for( i=0; i<5; i++ ){
						GDI_StringColor( 159, 75+i*15, GDI_STR_ALIGN_R, rgb[0+i], &str[0+i][0] );
						GDI_StringColor( 196, 75+i*15, GDI_STR_ALIGN_R, rgb[5+i], &str[5+i][0] );
					}
					break;
					
				case 5: // Maintenance Set
					pFunc->pMsgBox->Title.pStr = "MAINTENANCE SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  27,  71, 164 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  108,  55, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "ITEM          SET  UNIT" );
					for( i=0; i<5; i++ ){ rgb[i] = RGB.LIGHTSKYBLUE; }
					sprintf( &str[0][0], "CH-1 LEVEL  %s  %s",
						vStrGasConc( pSet->pMeas->pGas[0]->Inf.Dp, pSet->pMeas->pGas[0]->Inf.MaintenanceGas ),
						pUnit[pSet->pMeas->pGas[0]->Inf.Unit]
					);
					sprintf( &str[1][0], "           %6.3f    mA",
						EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH1] * 0.001f
					);
					sprintf( &str[2][0], "CH-2 LEVEL  %s  %s",
						vStrGasConc( pSet->pMeas->pGas[1]->Inf.Dp, pSet->pMeas->pGas[1]->Inf.MaintenanceGas ),
						pUnit[pSet->pMeas->pGas[1]->Inf.Unit]
					);
					sprintf( &str[3][0], "           %6.3f    mA",
						EepHandle.Sram[SYSTEM_EEP_mA_OUTPUT_MAINTENANCE_CH2] * 0.001f
					);
					if( pSet->pMeas->Mode.Item.Maintenance_QuitMin ){
						sprintf( &str[4][0], "QUIT TIMER    %3u   MIN", pSet->pMeas->Mode.Item.Maintenance_QuitMin );
					}
					else {
						sprintf( &str[4][0], "QUIT TIMER    OFF   MIN" );
					}
					if( pSet->pMeas->Conf.OutMax == 1 ){
						rgb[2] = RGB.DARKGRAY;
						rgb[3] = RGB.DARKGRAY;
					}
					if( SystemOption.Item.IsPoeMode == true ){
						rgb[1] = RGB.DARKGRAY;
						rgb[3] = RGB.DARKGRAY;
					}
					GDI_StringColor( 108,  78, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
					GDI_StringColor( 108,  93, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
					GDI_StringColor( 108, 108, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
					GDI_StringColor( 108, 123, GDI_STR_ALIGN_C, rgb[3], &str[3][0] );
					GDI_StringColor( 108, 138, GDI_STR_ALIGN_C, rgb[4], &str[4][0] );
					break;
					                                       
				case 6: // Serial Number
					pFunc->pMsgBox->Title.pStr = "SERIAL NUMBER";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font07x12 );
					sprintf( &str[0][0], "[    %s    ]", (char *)EepHandle.Tag.SerialNumber );
					GDI_StringColor( 110, 95, GDI_STR_ALIGN_C, RGB.LIGHTSKYBLUE, &str[0][0] );
					break;
			}
			break;
	}
}


