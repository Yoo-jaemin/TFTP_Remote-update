#define __VFUNC_MEASURING_C__
    #include "vfunc_measuring.h"
#undef  __VFUNC_MEASURING_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vmeasuring.h"
#include "vimage.h"
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

void vFunc_Measuring( Func_HandleType *pFunc, Measuring_SetType *pSet )
{
	char					str[12][26];
	U8						Digit[4];
	U8						i, x, y, ic, ir;
	U8						id;
	uint16_t				rgb[12];
	Measuring_GasInfo		*pGas;
	Measuring_AlarmInfo		*pAlr;
	char					*pUnit[4] = { " PPB", " PPM", "%LEL", "%VOL" };
	char					*pLat[2] = { "DISABLE", " ENABLE" };
	char					*pInc[2] = { "DEC", "INC" };
	char					*pBlk;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
		case FUNC_LEVEL_2:
			pFunc->pMsgBox->Title.pStr = "MEASURING MENU";
			pFunc->pMsgBox->Msg[0].pStr = "RANGE";
			pFunc->pMsgBox->Msg[1].pStr = "ALARM";
			pFunc->pMsgBox->Msg[2].pStr = "CROSS SCALE";
			pFunc->pMsgBox->Msg[3].pStr = "GAS NAME";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv2].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 4;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv2 ){
				case 0: // Range
					pFunc->pMsgBox->Title.pStr = "RANGE SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 25, 80, 170 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 110, 63, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "GAS NAME   DP    HS UNIT" );
					for( i=0; i<3; i++ ){ rgb[i] = RGB.WHITE; }
					sprintf( &str[0][0], "%s %2u %s %s",
							(char *)pSet->sGas[0].GasName,
							pSet->sGas[0].Dp,
							vStrGasConc( pSet->sGas[0].Dp, pSet->sGas[0].HighScale ),
							pUnit[pSet->sGas[0].Unit]
					);
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[1][0], "%s %2u %s %s",
							(char *)pSet->sGas[1].GasName,
							pSet->sGas[1].Dp,
							vStrGasConc( pSet->sGas[1].Dp, pSet->sGas[1].HighScale ),
							pUnit[pSet->sGas[1].Unit]
						);
					}
					else {
						strcpy( &str[1][0], "DISABLED    0     0 ----" );
						rgb[1] = RGB.DARKGRAY;
					}
					if( pFunc->IsApproved == true )	{ strcpy( &str[2][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[2][0], "[  CANCEL  ]" ); }
					switch( pFunc->Item.Lv3 ){
						case  0:
							if( pFunc->ViewBlk == true ){ str[0][12] = ' '; }
							rgb[0] = RGB.YELLOW;
							break;
							
						case  1:
						case  2:
						case  3:
						case  4:
							switch( pSet->sGas[0].Dp ){
								case 0: Digit[0] = 15, Digit[1] = 16, Digit[2] = 17, Digit[3] = 18; break;
								case 1: Digit[0] = 14, Digit[1] = 15, Digit[2] = 16, Digit[3] = 18; break;
								case 2: Digit[0] = 14, Digit[1] = 15, Digit[2] = 17, Digit[3] = 18; break;
								case 3: Digit[0] = 14, Digit[1] = 16, Digit[2] = 17, Digit[3] = 18; break;
							}
							id = (pFunc->Item.Lv3-1) % 4;
							for( i=id; i<4; i++ ){
								if( i == id ){
									if( pFunc->ViewBlk == true ){
										if( str[0][Digit[i]] != ' ' ){ str[0][Digit[i]] = ' '; }
									}
									else {
										if( str[0][Digit[i]] == ' ' ){ str[0][Digit[i]] = '0'; }
									}
								}
								else {
									if( str[0][Digit[i]] == ' ' ){ str[0][Digit[i]] = '0'; }
								}
							}
							rgb[0] = RGB.YELLOW;
							break;
							
						case  5:
							if( pFunc->ViewBlk == true ){ strcpy( &str[0][20], "    " ); }
							rgb[0] = RGB.YELLOW;
							break;
							
						case  6:
							if( pFunc->ViewBlk == true ){ str[1][12] = ' '; }
							rgb[1] = RGB.YELLOW;
							break;
						
						case  7:
						case  8:
						case  9:
						case 10:
							switch( pSet->sGas[1].Dp ){
								case 0: Digit[0] = 15, Digit[1] = 16, Digit[2] = 17, Digit[3] = 18; break;
								case 1: Digit[0] = 14, Digit[1] = 15, Digit[2] = 16, Digit[3] = 18; break;
								case 2: Digit[0] = 14, Digit[1] = 15, Digit[2] = 17, Digit[3] = 18; break;
								case 3: Digit[0] = 14, Digit[1] = 16, Digit[2] = 17, Digit[3] = 18; break;
							}
							id = (pFunc->Item.Lv3-7) % 4;
							for( i=id; i<4; i++ ){
								if( i == id ){
									if( pFunc->ViewBlk == true ){
										if( str[1][Digit[i]] != ' ' ){ str[1][Digit[i]] = ' '; }
									}
									else {
										if( str[1][Digit[i]] == ' ' ){ str[1][Digit[i]] = '0'; }
									}
								}
								else {
									if( str[1][Digit[i]] == ' ' ){ str[1][Digit[i]] = '0'; }
								}
							}
							rgb[1] = RGB.YELLOW;
							break;
							
						case 11:
							if( pFunc->ViewBlk == true ){ strcpy( &str[1][20], "    " ); }
							rgb[1] = RGB.YELLOW;
							break;
							
						case 12:
							rgb[2] = RGB.YELLOW;
							break;
					}
					GDI_StringColor( 110,  87, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
					GDI_StringColor( 110, 102, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
					GDI_StringColor( 110, 135, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
					break;
					
				case 1: // Alarm
					pFunc->pMsgBox->Title.pStr = "ALARM SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine(  11,  60, 199 );
					GDI_DrawVLine(  88,  43,  96 );
					GDI_DrawVLine( 150,  43,  96 );
					GDI_SetFont( &Font07x12 );
					pGas = &pSet->sGas[pSet->sCh];
					pAlr = &pSet->sAlr[pSet->sCh];
					GDI_StringColor( 144,  43, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "1'ST" );
					GDI_StringColor( 208,  43, GDI_STR_ALIGN_R, RGB.SPRINGGREEN	, "2'ND" );
					GDI_StringColor(  12,  68, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "LEVEL[" );
					GDI_StringColor(  54,  68, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, pUnit[pGas->Unit]+1 );
					GDI_StringColor(  75,  68, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "]" );
					GDI_StringColor(  12,  83, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "DEAD [ % ]" );
					GDI_StringColor(  12,  98, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "DELAY[SEC]" );
					GDI_StringColor(  12, 113, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "LATCH" );
					GDI_StringColor(  12, 128, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "DIRECTION" );
					for( i=0; i<12; i++ ){ rgb[i] = RGB.WHITE; }
					if( pSet->pMeas->Conf.OutMax < 2 ){ rgb[0] = RGB.PINK; }
					sprintf( &str[ 0][0], "%s", (char *)pGas->GasName );
					sprintf( &str[ 1][0], "%s", vStrGasConc( pGas->Dp, pAlr->_1stLevel ) );
					sprintf( &str[ 2][0], "%2u", pAlr->_1stDead );
					sprintf( &str[ 3][0], "%2u", pAlr->_1stDelaySec );
					sprintf( &str[ 4][0], "%s", pLat[pAlr->_1stLatchEnabled] );
					sprintf( &str[ 5][0], "%s", pInc[pAlr->_1stTypeIncrease] );
					sprintf( &str[ 6][0], "%s", vStrGasConc( pGas->Dp, pAlr->_2ndLevel ) );
					sprintf( &str[ 7][0], "%2u", pAlr->_2ndDead );
					sprintf( &str[ 8][0], "%2u", pAlr->_2ndDelaySec );
					sprintf( &str[ 9][0], "%s", pLat[pAlr->_2ndLatchEnabled] );
					sprintf( &str[10][0], "%s", pInc[pAlr->_2ndTypeIncrease] );
					if( pFunc->IsApproved == true )	{ sprintf( &str[11][0], "[   SAVE   ]" ); }
					else							{ sprintf( &str[11][0], "[  CANCEL  ]" ); }
					switch( pGas->Dp ){
						case 0: Digit[0] = 1, Digit[1] = 2, Digit[2] = 3, Digit[3] = 4; break;
						case 1: Digit[0] = 0, Digit[1] = 1, Digit[2] = 2, Digit[3] = 4; break;
						case 2: Digit[0] = 0, Digit[1] = 1, Digit[2] = 3, Digit[3] = 4; break;
						case 3: Digit[0] = 0, Digit[1] = 2, Digit[2] = 3, Digit[3] = 4; break;
					}
					switch( pFunc->Item.Lv3 ){
						case  0:
							rgb[0] = RGB.YELLOW;
							break;
							
						case  1:
						case  2:
						case  3:
						case  4:
							id = (pFunc->Item.Lv3-1) % 4;
							for( i=id; i<4; i++ ){
								if( i == id ){
									if( pFunc->ViewBlk == true ){
										if( str[1][Digit[i]] != ' ' ){ str[1][Digit[i]] = ' '; }
									}
									else {
										if( str[1][Digit[i]] == ' ' ){ str[1][Digit[i]] = '0'; }
									}
								}
								else {
									if( str[1][Digit[i]] == ' ' ){ str[1][Digit[i]] = '0'; }
								}
							}
							rgb[1] = RGB.YELLOW;
							break;
							
						case  5:
						case  6:
						case  7:
						case  8:
							rgb[pFunc->Item.Lv3-3] = RGB.YELLOW;
							break;
							
						case  9:
						case 10:
						case 11:
						case 12:
							id = (pFunc->Item.Lv3-9) % 4;
							for( i=id; i<4; i++ ){
								if( i == id ){
									if( pFunc->ViewBlk == true ){
										if( str[6][Digit[i]] != ' ' ){ str[6][Digit[i]] = ' '; }
									}
									else {
										if( str[6][Digit[i]] == ' ' ){ str[6][Digit[i]] = '0'; }
									}
								}
								else {
									if( str[6][Digit[i]] == ' ' ){ str[6][Digit[i]] = '0'; }
								}
							}
							rgb[6] = RGB.YELLOW;
							break;
							
						case 13:
						case 14:
						case 15:
						case 16:
							rgb[pFunc->Item.Lv3-6] = RGB.YELLOW;
							break;
							
						case 17:
							rgb[11] = RGB.YELLOW;
							break;
					}
					GDI_StringColor(  12,  43, GDI_STR_ALIGN_L, rgb[ 0], &str[ 0][0] );
					GDI_StringColor( 144,  68, GDI_STR_ALIGN_R, rgb[ 1], &str[ 1][0] );
					GDI_StringColor( 144,  83, GDI_STR_ALIGN_R, rgb[ 2], &str[ 2][0] );
					GDI_StringColor( 144,  98, GDI_STR_ALIGN_R, rgb[ 3], &str[ 3][0] );
					GDI_StringColor( 144, 113, GDI_STR_ALIGN_R, rgb[ 4], &str[ 4][0] );
					GDI_StringColor( 144, 128, GDI_STR_ALIGN_R, rgb[ 5], &str[ 5][0] );
					GDI_StringColor( 208,  68, GDI_STR_ALIGN_R, rgb[ 6], &str[ 6][0] );
					GDI_StringColor( 208,  83, GDI_STR_ALIGN_R, rgb[ 7], &str[ 7][0] );
					GDI_StringColor( 208,  98, GDI_STR_ALIGN_R, rgb[ 8], &str[ 8][0] );
					GDI_StringColor( 208, 113, GDI_STR_ALIGN_R, rgb[ 9], &str[ 9][0] );
					GDI_StringColor( 208, 128, GDI_STR_ALIGN_R, rgb[10], &str[10][0] );
					GDI_StringColor( 110, 150, GDI_STR_ALIGN_C, rgb[11], &str[11][0] );
					break;
					
				case 2: // Cross Scale
					pFunc->pMsgBox->Title.pStr = "CROSS SCALE SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawHLine( 49, 80, 121 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 110, 63, GDI_STR_ALIGN_C, RGB.SPRINGGREEN, "GAS NAME    SCALE" );
					for( i=0; i<3; i++ ){ rgb[i] = RGB.WHITE; }
					sprintf( &str[0][0], "%s x%5.2f", (char *)pSet->sGas[0].GasName, pSet->sGas[0].CrossScale * 0.01f );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[1][0], "%s x%5.2f", (char *)pSet->sGas[1].GasName, pSet->sGas[1].CrossScale * 0.01f );
					}
					else {
						strcpy( &str[1][0], "DISABLED   x 0.00" );
						rgb[1] = RGB.DARKGRAY;
					}
					if( pFunc->IsApproved == true )	{ strcpy( &str[2][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[2][0], "[  CANCEL  ]" ); }
					if( pFunc->Item.Lv3 < 8 ){
						Digit[0] = 12;
						Digit[1] = 13;
						Digit[2] = 15;
						Digit[3] = 16;
						id = pFunc->Item.Lv3 % 4;
						pBlk = &str[pFunc->Item.Lv3 / 4][Digit[id]];
						if( pFunc->ViewBlk == true ){
							*pBlk = ' ';
						}
						else {
							if( (id == 0) && (*pBlk == ' ') ){ *pBlk = '0'; }
						}
					}
					rgb[pFunc->Item.Lv3 / 4] = RGB.YELLOW;
					GDI_StringColor( 110,  87, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
					GDI_StringColor( 110, 102, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
					GDI_StringColor( 110, 135, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
					break;
					
				case 3: // Gas Name
					pFunc->pMsgBox->Title.pStr = "GAS NAME MENU";
					pFunc->pMsgBox->Msg[0].pStr = "GAS1";
					pFunc->pMsgBox->Msg[1].pStr = "GAS2";
					pFunc->pMsgBox->Msg[2].pStr = "INIT";
					if( pSet->pMeas->Conf.OutMax == 1 ){
						pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
					}
					pFunc->pMsgBox->Msg[pFunc->Item.Lv3].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 3;
					vMessageBox( pFunc->pMsgBox );
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->Item.Lv2 ){
				case 3: // Gas Name
					switch( pFunc->Item.Lv3 ){
						case 0:
						case 1:
							pFunc->pMsgBox->Title.pStr = "GAS NAME SET";
							pFunc->pMsgBox->MsgLine = 0;
							vMessageBox( pFunc->pMsgBox );
							GDI_SetFont( &Font07x12 );
							strcpy( &str[0][0], "_ #%&()-./012" );
							strcpy( &str[1][0], "3456789:;<>[]" );
							strcpy( &str[2][0], "ABCDEFGHIJKLM" );
							strcpy( &str[3][0], "NOPQRSTUVWXYZ" );
							strcpy( &str[4][0], "abcdefghijklm" );
							strcpy( &str[5][0], "nopqrstuvwxyz" );
							GDI_SetTextColor( RGB.SPRINGGREEN );
							x = 23;
							y = 45;
							for( ir=0; ir<6; ir++ ){
								for( ic=0; ic<13; ic++ ){
									if( pSet->GasName[pFunc->Item.Lv4] == str[ir][ic] ){
										GDI_SetBackColor( RGB.NAVY );
										GDI_SetTextColor( RGB.YELLOW );
										GDI_DisplayChar( x, y, str[ir][ic] );
										GDI_SetBackColor( RGB.BLACK );
										GDI_SetTextColor( RGB.SPRINGGREEN );
									}
									else {
										GDI_DisplayChar( x, y, str[ir][ic] );
									}
									x += 14;
								}
								x  = 23;
								y += 15;
							}
							y = 145;
							for( i=0; i<10; i++ ){
								if( pFunc->Item.Lv4 == i )	{ GDI_SetTextColor( RGB.YELLOW ); }
								else						{ GDI_SetTextColor( RGB.WHITE  ); }
								GDI_DisplayChar( x, y, pSet->GasName[i] );
								GDI_SetFont( &Fimg07x12 );
								GDI_DisplayChar( x, 157, '-' );
								GDI_SetFont( &Font07x12 );
								x += 9;
							}
							if( pFunc->IsApproved == true )	{ strcpy( &str[0][0], " SAVE " ); }
							else							{ strcpy( &str[0][0], "CANCEL" ); }
							if( pFunc->Item.Lv4 == 10 )	{ GDI_SetTextColor( RGB.YELLOW ); }
							else						{ GDI_SetTextColor( RGB.WHITE  ); }
							GDI_DisplayChar	( 123, y, '[' );
							GDI_String		( 182, y, GDI_STR_ALIGN_R, &str[0][0] );
							GDI_DisplayChar	( 192, y, ']' );
							break;
							
						case 2:
							if( pSet->IsConfirmMode == true ){
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
								pFunc->pMsgBox->Title.pStr = "SELECT ITEM";
								pFunc->pMsgBox->Width = 216;
								pFunc->pMsgBox->MsgLine = 0;
								pFunc->pMsgBox->OutLine = 6;
								vMessageBox( pFunc->pMsgBox );
								GDI_SetFont( &Font07x12 );
								sprintf( &str[0][0], "[ G1 %s ]", (char *)pSet->pMeas->pGas[0]->Inf.GasName );
								sprintf( &str[1][0], "[ G2 %s ]", (char *)pSet->pMeas->pGas[1]->Inf.GasName );
								strcpy ( &str[2][0], "CONFIRM" );
								for( i=0; i<3; i++ ){ rgb[i] = RGB.WHITE; }
								for( i=3; i<6; i++ ){ rgb[i] = RGB.BLACK; }
								if( pSet->pMeas->Conf.OutMax < 2 ){
									strcpy( &str[1][0], "[ G2 DISABLED   ]" );
									rgb[1] = RGB.DARKGRAY;
								}
								rgb[pFunc->Item.Lv4] = RGB.YELLOW;
								if( pSet->IsSelected[0] == true ){ rgb[3] = RGB.SELECTED; }
								if( pSet->IsSelected[1] == true ){ rgb[4] = RGB.SELECTED; }
								GDI_SetBackColor( rgb[3] ), GDI_StringColor( 110,  65, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
								GDI_SetBackColor( rgb[4] ), GDI_StringColor( 110,  82, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
								GDI_SetBackColor( rgb[5] ), GDI_StringColor( 110, 120, GDI_STR_ALIGN_C, rgb[2], &str[2][0] );
								GDI_DrawRect( 53, 112, 115, 26 );
							}
							break;
					}
					break;
			}
			break;
	}
}


