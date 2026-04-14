#define __VFUNC_INTERFACE_C__
    #include "vfunc_interface.h"
#undef  __VFUNC_INTERFACE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "option.h"
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
#if 0
static vPoint	vp[30];
static vLine	vl[30];
#endif
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vFunc_Poe( Func_HandleType *pFunc, Interface_SetType *pSet )
{
	char		str[20][15];
	uint16_t	rgb[20]; 
	uint16_t	i;
	char		*sp[2] = { "DIS", "ENA" };
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
		case FUNC_LEVEL_2:
			pFunc->pMsgBox->Title.pStr = "INTERFACE MENU";
			pFunc->pMsgBox->Msg[0].pStr = "PoE";
			pFunc->pMsgBox->Msg[1].pStr = "RELAY";
			pFunc->pMsgBox->Msg[2].pStr = "RS485";
			pFunc->pMsgBox->Msg[3].pStr = "mA CALIBRATION";
			pFunc->pMsgBox->Msg[4].pStr = "mA ZERO OFFSET";
			if( SystemOption.Item.IsPoeMode == true ){
				pFunc->pMsgBox->Msg[2].tClr = RGB.DARKGRAY;
				pFunc->pMsgBox->Msg[3].tClr = RGB.DARKGRAY;
				pFunc->pMsgBox->Msg[4].tClr = RGB.DARKGRAY;
				if( pSet->pMeas->Mode.Item.Maintenance == true ){
					pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
				}
			}
			else {
				if( SystemOption.Item.IsUniMode == false ){ pFunc->pMsgBox->Msg[0].tClr = RGB.DARKGRAY; }
				if( pSet->pMeas->Mode.Item.Maintenance == true ){
					pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
					pFunc->pMsgBox->Msg[3].tClr = RGB.DARKGRAY;
					pFunc->pMsgBox->Msg[4].tClr = RGB.DARKGRAY;
				}
			}
			pFunc->pMsgBox->Msg[pFunc->Item.Lv2].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 5;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv2 ){
				case 0: // PoE
					pFunc->pMsgBox->Title.pStr = "PoE CONFIG";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font07x12 );
					GDI_SetTextColor( RGB.SPRINGGREEN );
					GDI_String(  16,  63, GDI_STR_ALIGN_L, "MAC     " );
					GDI_String(  16,  78, GDI_STR_ALIGN_L, "IP      " );
					GDI_String(  16,  93, GDI_STR_ALIGN_L, "SUBNET  " );
					GDI_String(  16, 108, GDI_STR_ALIGN_L, "GATE WAY" );
					GDI_SetTextColor( RGB.LIGHTGRAY );
					GDI_String( 102,  63, GDI_STR_ALIGN_L, ":" );
					GDI_String( 123,  63, GDI_STR_ALIGN_L, ":" );
					GDI_String( 144,  63, GDI_STR_ALIGN_L, ":" );
					GDI_String( 165,  63, GDI_STR_ALIGN_L, ":" );
					GDI_String( 186,  63, GDI_STR_ALIGN_L, ":" );
					GDI_String( 109,  78, GDI_STR_ALIGN_L, "." );
					GDI_String( 137,  78, GDI_STR_ALIGN_L, "." );
					GDI_String( 165,  78, GDI_STR_ALIGN_L, "." );
					GDI_String( 109,  93, GDI_STR_ALIGN_L, "." );
					GDI_String( 137,  93, GDI_STR_ALIGN_L, "." );
					GDI_String( 165,  93, GDI_STR_ALIGN_L, "." );
					GDI_String( 109, 108, GDI_STR_ALIGN_L, "." );
					GDI_String( 137, 108, GDI_STR_ALIGN_L, "." );
					GDI_String( 165, 108, GDI_STR_ALIGN_L, "." );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawVLine( 80, 63, 56 );
					sprintf( &str[ 0][0], "%02X", pSet->sPoe.MC[0] );
					sprintf( &str[ 1][0], "%02X", pSet->sPoe.MC[1] );
					sprintf( &str[ 2][0], "%02X", pSet->sPoe.MC[2] );
					sprintf( &str[ 3][0], "%02X", pSet->sPoe.MC[3] );
					sprintf( &str[ 4][0], "%02X", pSet->sPoe.MC[4] );
					sprintf( &str[ 5][0], "%02X", pSet->sPoe.MC[5] );
					sprintf( &str[ 6][0], "%03u", pSet->sPoe.IP[0] );
					sprintf( &str[ 7][0], "%03u", pSet->sPoe.IP[1] );
					sprintf( &str[ 8][0], "%03u", pSet->sPoe.IP[2] );
					sprintf( &str[ 9][0], "%03u", pSet->sPoe.IP[3] );
					sprintf( &str[10][0], "%03u", pSet->sPoe.NM[0] );
					sprintf( &str[11][0], "%03u", pSet->sPoe.NM[1] );
					sprintf( &str[12][0], "%03u", pSet->sPoe.NM[2] );
					sprintf( &str[13][0], "%03u", pSet->sPoe.NM[3] );
					sprintf( &str[14][0], "%03u", pSet->sPoe.GW[0] );
					sprintf( &str[15][0], "%03u", pSet->sPoe.GW[1] );
					sprintf( &str[16][0], "%03u", pSet->sPoe.GW[2] );
					sprintf( &str[17][0], "%03u", pSet->sPoe.GW[3] );
					if( pFunc->IsApproved == true ){ sprintf( &str[18][0], "[   SAVE   ]" ); }
					else { sprintf( &str[18][0], "[  CANCEL  ]" ); }
					for( i=0; i<19; i++ ){
						if( i == pFunc->Item.Lv3 ){ rgb[i] = RGB.YELLOW; }
						else { rgb[i] = RGB.WHITE; }
					}
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  88,  63, GDI_STR_ALIGN_L, rgb[ 0], &str[ 0][0] );
					GDI_StringColor( 109,  63, GDI_STR_ALIGN_L, rgb[ 1], &str[ 1][0] );
					GDI_StringColor( 130,  63, GDI_STR_ALIGN_L, rgb[ 2], &str[ 2][0] );
					GDI_StringColor( 151,  63, GDI_STR_ALIGN_L, rgb[ 3], &str[ 3][0] );
					GDI_StringColor( 172,  63, GDI_STR_ALIGN_L, rgb[ 4], &str[ 4][0] );
					GDI_StringColor( 193,  63, GDI_STR_ALIGN_L, rgb[ 5], &str[ 5][0] );
					GDI_StringColor(  88,  78, GDI_STR_ALIGN_L, rgb[ 6], &str[ 6][0] );
					GDI_StringColor( 116,  78, GDI_STR_ALIGN_L, rgb[ 7], &str[ 7][0] );
					GDI_StringColor( 144,  78, GDI_STR_ALIGN_L, rgb[ 8], &str[ 8][0] );
					GDI_StringColor( 172,  78, GDI_STR_ALIGN_L, rgb[ 9], &str[ 9][0] );
					GDI_StringColor(  88,  93, GDI_STR_ALIGN_L, rgb[10], &str[10][0] );
					GDI_StringColor( 116,  93, GDI_STR_ALIGN_L, rgb[11], &str[11][0] );
					GDI_StringColor( 144,  93, GDI_STR_ALIGN_L, rgb[12], &str[12][0] );
					GDI_StringColor( 172,  93, GDI_STR_ALIGN_L, rgb[13], &str[13][0] );
					GDI_StringColor(  88, 108, GDI_STR_ALIGN_L, rgb[14], &str[14][0] );
					GDI_StringColor( 116, 108, GDI_STR_ALIGN_L, rgb[15], &str[15][0] );
					GDI_StringColor( 144, 108, GDI_STR_ALIGN_L, rgb[16], &str[16][0] );
					GDI_StringColor( 172, 108, GDI_STR_ALIGN_L, rgb[17], &str[17][0] );
					GDI_StringColor( 110, 140, GDI_STR_ALIGN_C, rgb[18], &str[18][0] );
					break;
					
				case 1: // Relay
					pFunc->pMsgBox->Title.pStr = "RELAY CONFIG";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  15,  59, 190 );
					GDI_DrawVLine(  79,  45,  91 );
					GDI_DrawVLine( 121,  45,  91 );
					GDI_DrawVLine( 163,  45,  91 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  15,  45, GDI_STR_ALIGN_L, RGB.PINK		, "ITEM" );
					GDI_StringColor( 112,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "TRB" );
					GDI_StringColor( 154,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "AL1" );
					GDI_StringColor( 196,  45, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "AL2" );
					GDI_StringColor(  15,  65, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "OUTPUT" );
					GDI_StringColor(  15,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "ENERGIZE" );
					GDI_StringColor(  15,  95, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "BLINKING" );
					GDI_StringColor(  15, 110, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "ACT" );
					GDI_StringColor(  15, 125, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "REL" );
					GDI_StringColor(  37, 110, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "[SEC]" );
					GDI_StringColor(  37, 125, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "[SEC]" );
					sprintf( &str[ 0][0], "%s", sp[pSet->sRly.TRB.Opt.Item.IsOutputEnable] );
					sprintf( &str[ 1][0], "%s", sp[pSet->sRly.TRB.Opt.Item.IsEnergizeMode] );
					sprintf( &str[ 2][0], "%s", sp[pSet->sRly.TRB.Opt.Item.IsBlinkingMode] );
					sprintf( &str[ 3][0], "%3u", pSet->sRly.TRB.Blk.Item.Act_Time );
					sprintf( &str[ 4][0], "%3u", pSet->sRly.TRB.Blk.Item.Rel_Time );
					sprintf( &str[ 5][0], "%s", sp[pSet->sRly.AL1.Opt.Item.IsOutputEnable] );
					sprintf( &str[ 6][0], "%s", sp[pSet->sRly.AL1.Opt.Item.IsEnergizeMode] );
					sprintf( &str[ 7][0], "%s", sp[pSet->sRly.AL1.Opt.Item.IsBlinkingMode] );
					sprintf( &str[ 8][0], "%3u", pSet->sRly.AL1.Blk.Item.Act_Time );
					sprintf( &str[ 9][0], "%3u", pSet->sRly.AL1.Blk.Item.Rel_Time );
					sprintf( &str[10][0], "%s", sp[pSet->sRly.AL2.Opt.Item.IsOutputEnable] );
					sprintf( &str[11][0], "%s", sp[pSet->sRly.AL2.Opt.Item.IsEnergizeMode] );
					sprintf( &str[12][0], "%s", sp[pSet->sRly.AL2.Opt.Item.IsBlinkingMode] );
					sprintf( &str[13][0], "%3u", pSet->sRly.AL2.Blk.Item.Act_Time );
					sprintf( &str[14][0], "%3u", pSet->sRly.AL2.Blk.Item.Rel_Time );
					if( pFunc->IsApproved == true ){ strcpy( &str[15][0], "[   SAVE   ]" ); }
					else { strcpy( &str[15][0], "[  CANCEL  ]" ); }
					for( i=0; i<16; i++ ){
						if( i == pFunc->Item.Lv3 ){ rgb[i] = RGB.YELLOW; }
						else { rgb[i] = RGB.WHITE; }
					}
					GDI_StringColor( 112,  65, GDI_STR_ALIGN_R, rgb[ 0], &str[ 0][0] );
					GDI_StringColor( 112,  80, GDI_STR_ALIGN_R, rgb[ 1], &str[ 1][0] );
					GDI_StringColor( 112,  95, GDI_STR_ALIGN_R, rgb[ 2], &str[ 2][0] );
					GDI_StringColor( 112, 110, GDI_STR_ALIGN_R, rgb[ 3], &str[ 3][0] );
					GDI_StringColor( 112, 125, GDI_STR_ALIGN_R, rgb[ 4], &str[ 4][0] );
					GDI_StringColor( 154,  65, GDI_STR_ALIGN_R, rgb[ 5], &str[ 5][0] );
					GDI_StringColor( 154,  80, GDI_STR_ALIGN_R, rgb[ 6], &str[ 6][0] );
					GDI_StringColor( 154,  95, GDI_STR_ALIGN_R, rgb[ 7], &str[ 7][0] );
					GDI_StringColor( 154, 110, GDI_STR_ALIGN_R, rgb[ 8], &str[ 8][0] );
					GDI_StringColor( 154, 125, GDI_STR_ALIGN_R, rgb[ 9], &str[ 9][0] );
					GDI_StringColor( 196,  65, GDI_STR_ALIGN_R, rgb[10], &str[10][0] );
					GDI_StringColor( 196,  80, GDI_STR_ALIGN_R, rgb[11], &str[11][0] );
					GDI_StringColor( 196,  95, GDI_STR_ALIGN_R, rgb[12], &str[12][0] );
					GDI_StringColor( 196, 110, GDI_STR_ALIGN_R, rgb[13], &str[13][0] );
					GDI_StringColor( 196, 125, GDI_STR_ALIGN_R, rgb[14], &str[14][0] );
					GDI_StringColor( 110, 148, GDI_STR_ALIGN_C, rgb[15], &str[15][0] );
					break;
					
				case 2:
					pFunc->pMsgBox->Title.pStr = "RS485 CONFIG";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font07x12 );
					GDI_SetTextColor( RGB.LIGHTSKYBLUE );
					GDI_String( 45, 43, GDI_STR_ALIGN_L, "UNIT ID  " );
					GDI_String( 45, 58, GDI_STR_ALIGN_L, "BAUD RATE" );
					sprintf( &str[0][0], "%3u", pSet->s485.UnitID );
					switch( pSet->s485.Baud ){
						case 0: strcpy( &str[1][0], "  2400" ); break;
						case 1: strcpy( &str[1][0], "  4800" ); break;
						case 2: strcpy( &str[1][0], "  9600" ); break;
						case 3: strcpy( &str[1][0], " 19200" ); break;
						case 4: strcpy( &str[1][0], " 38400" ); break;
						case 5: strcpy( &str[1][0], " 57600" ); break;
						case 6: strcpy( &str[1][0], "115200" ); break;
					}
					sprintf( &str[2][0], "TYPE-%u", pSet->s485.Type+1 );
					if( pFunc->IsApproved == true )	{ strcpy( &str[3][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[3][0], "[  CANCEL  ]" ); }
					for( i=0; i<4; i++ ){
						if( i == pFunc->Item.Lv3 ){ rgb[i] = RGB.YELLOW; }
						else { rgb[i] = RGB.WHITE; }
					}
					GDI_StringColor( 175, 43, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
					GDI_StringColor( 175, 58, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
					GDI_StringColor( 111, 78, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
					GDI_SetTextColor( rgb[2] );
					GDI_DrawHLine(  45,  82,  35 );
					GDI_DrawHLine( 140,  82,  34 );
					GDI_DrawHLine(  45, 140, 129 );
					GDI_DrawVLine(  45,  82,  59 );
					GDI_DrawVLine( 174,  82,  59 );
					switch( pSet->s485.Type ){
						case 0:
							GDI_StringColor( 110,  95, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DATA BITS:     8" );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "STOP BITS:     1" );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "PARITY   :  NONE" );
							break;
							
						case 1:
							GDI_StringColor( 110,  95, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DATA BITS:     8" );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "STOP BITS:     1" );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "PARITY   :   ODD" );
							break;
							
						case 2:
							GDI_StringColor( 110,  95, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DATA BITS:     8" );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "STOP BITS:     1" );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "PARITY   :  EVEN" );
							break;
							
						case 3:
							GDI_StringColor( 110,  95, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DATA BITS:     8" );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "STOP BITS:     2" );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "PARITY   :  NONE" );
							break;
							
						case 4:
							GDI_StringColor( 110,  95, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DATA BITS:     8" );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "STOP BITS:     2" );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "PARITY   :   ODD" );
							break;
							
						case 5:
							GDI_StringColor( 110,  95, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "DATA BITS:     8" );
							GDI_StringColor( 110, 108, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "STOP BITS:     2" );
							GDI_StringColor( 110, 121, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "PARITY   :  EVEN" );
							break;
					}
					GDI_StringColor( 110, 152, GDI_STR_ALIGN_C, rgb[3], &str[3][0] );
					break;
					
				case 3: // mA Calibration
					pFunc->pMsgBox->Title.pStr = "mA CALIBRATION";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  33,  72, 152 );
					GDI_DrawVLine(  73,  55,  69 );
					GDI_DrawVLine( 133,  55,  69 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  33,  55, GDI_STR_ALIGN_L, RGB.PINK		, "ITEM" );
					GDI_StringColor( 125,  55, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "CH-1" );
					GDI_StringColor( 185,  55, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "CH-2" );
					GDI_StringColor(  33,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, " 4mA" );
					GDI_StringColor(  33,  96, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "20mA" );
					GDI_StringColor(  33, 112, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "TEST" );
					for( i=0; i<7; i++ ){ rgb[i] = RGB.WHITE; }
					sprintf( &str[0][0], "%6.3f", pSet->smA[0].Inf.Zero * 0.001f );
					sprintf( &str[1][0], "%6.3f", pSet->smA[0].Inf.Span * 0.001f );
					sprintf( &str[2][0], "%6.3f", pSet->smA[0].Set * 0.001f );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[3][0], "%6.3f", pSet->smA[1].Inf.Zero * 0.001f );
						sprintf( &str[4][0], "%6.3f", pSet->smA[1].Inf.Span * 0.001f );
						sprintf( &str[5][0], "%6.3f", pSet->smA[1].Set * 0.001f );
					}
					else {
						strcpy( &str[3][0], "DIS" );
						strcpy( &str[4][0], "DIS" );
						strcpy( &str[5][0], "DIS" );
						rgb[3] = RGB.DARKGRAY;
						rgb[4] = RGB.DARKGRAY;
						rgb[5] = RGB.DARKGRAY;
					}
					if( pFunc->IsApproved == true )	{ strcpy( &str[6][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[6][0], "[  CANCEL  ]" ); }
					if( pFunc->ViewBlk == true ){  
						switch( pFunc->Item.Lv3 ){
							case  0: str[0][3] = ' '; break;
							case  1: str[0][4] = ' '; break;
							case  2: str[0][5] = ' '; break;
							case  3: str[1][3] = ' '; break;
							case  4: str[1][4] = ' '; break;
							case  5: str[1][5] = ' '; break;
							case  6: str[2][0] = ' '; str[2][1] = ' '; break;
							case  7: str[2][3] = ' '; break;
							case  8: str[2][4] = ' '; break;
							case  9: str[3][3] = ' '; break;
							case 10: str[3][4] = ' '; break;
							case 11: str[3][5] = ' '; break;
							case 12: str[4][3] = ' '; break;
							case 13: str[4][4] = ' '; break;
							case 14: str[4][5] = ' '; break;
							case 15: str[5][0] = ' '; str[5][1] = ' '; break;
							case 16: str[5][3] = ' '; break;
							case 17: str[5][4] = ' '; break;
						}
					}
					rgb[pFunc->Item.Lv3/3] = RGB.YELLOW;
					GDI_StringColor( 125,  80, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
					GDI_StringColor( 125,  96, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
					GDI_StringColor( 125, 112, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
					GDI_StringColor( 185,  80, GDI_STR_ALIGN_R, rgb[3], &str[3][0] );
					GDI_StringColor( 185,  96, GDI_STR_ALIGN_R, rgb[4], &str[4][0] );
					GDI_StringColor( 185, 112, GDI_STR_ALIGN_R, rgb[5], &str[5][0] );
					GDI_StringColor( 110, 143, GDI_STR_ALIGN_C, rgb[6], &str[6][0] );
					break;
					
				case 4: // mA Zero Offset
					pFunc->pMsgBox->Title.pStr = "mA ZERO OFFSET";
					sprintf( &str[0][0], "CH1: %6.3f mA", pSet->smA[0].Inf.Offs * 0.001f );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[1][0], "CH2: %6.3f mA", pSet->smA[1].Inf.Offs * 0.001f );
					}
					else {
						strcpy( &str[1][0], "CH2:  DISABLED" );
						pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
					}
					if( pFunc->ViewBlk == true ){
						if( pFunc->Item.Lv3 < 6 ){
							str[pFunc->Item.Lv3/3][pFunc->Item.Lv3%3+8] = ' ';
						}
					}
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					pFunc->pMsgBox->Msg[1].pStr = &str[1][0];
					if( pFunc->IsApproved == true )	{ pFunc->pMsgBox->Msg[3].pStr = "[    SAVE    ]"; }
					else							{ pFunc->pMsgBox->Msg[3].pStr = "[   CANCEL   ]"; }
					switch( pFunc->Item.Lv3 ){
						case  0:
						case  1:
						case  2: pFunc->pMsgBox->Msg[0].tClr = RGB.YELLOW; break;
						case  3:
						case  4:
						case  5: pFunc->pMsgBox->Msg[1].tClr = RGB.YELLOW; break;
						case  6: pFunc->pMsgBox->Msg[3].tClr = RGB.YELLOW; break;
					}
					pFunc->pMsgBox->MsgLine = 4;
					vMessageBox( pFunc->pMsgBox );
					break;
			}
			break;
	}
}


