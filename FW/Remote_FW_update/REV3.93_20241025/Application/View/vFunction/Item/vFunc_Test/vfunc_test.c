#define __VFUNC_TEST_C__
    #include "vfunc_test.h"
#undef  __VFUNC_TEST_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "option.h"
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
static void GasName_Right_Align( char *pStr );
/* End Function */

/* Start Variable */
#if 0
static vPoint	vp[20];
static vLine	vl[20];
#endif
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vFunc_Test( Func_HandleType *pFunc, Test_SetType *pSet )
{
	char					str[15][30];
	uint16_t				rgb[15];
	char					*pUnit[4] = { " PPB", " PPM", "%LEL", "%VOL" };
	char					*pType[2] = { "D", "I" };
	char					*pTest[2] = { "OFF", " ON " };
	Measuring_GasType		*pGas;
	Measuring_AlarmType		*pAlr;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
		case FUNC_LEVEL_2:
			pFunc->pMsgBox->Title.pStr = "TEST MENU";
			pFunc->pMsgBox->Msg[0].pStr = "mA OUTPUT";
			pFunc->pMsgBox->Msg[1].pStr = "RELAY";
			pFunc->pMsgBox->Msg[2].pStr = "DISPLAY";
			pFunc->pMsgBox->Msg[3].pStr = "GAS SIMULATION";
			pFunc->pMsgBox->Msg[4].pStr = "MAX POWER";
			if( pSet->pMeas->Mode.Item.Maintenance == true ){
				pFunc->pMsgBox->Msg[0].tClr = RGB.DARKGRAY;
				pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
				pFunc->pMsgBox->Msg[2].tClr = RGB.YELLOW;
				pFunc->pMsgBox->Msg[3].tClr = RGB.DARKGRAY;
				pFunc->pMsgBox->Msg[4].tClr = RGB.DARKGRAY;
			}
			else {
				if( SystemOption.Item.IsBatMode == true ){
					pFunc->pMsgBox->Msg[0].tClr = RGB.DARKGRAY;
					pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
				}
				else if( SystemOption.Item.IsPoeMode == true ){
					pFunc->pMsgBox->Msg[0].tClr = RGB.DARKGRAY;
				}
				pFunc->pMsgBox->Msg[pFunc->Item.Lv2].tClr = RGB.YELLOW;
			}
			pFunc->pMsgBox->MsgLine = 5;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv2 ){
				case 0: // mA Output Test
					pFunc->pMsgBox->Title.pStr = "mA OUTPUT TEST";
					sprintf( &str[0][0], "CH1: %6.3f mA", pSet->pmA[0]->Set * 0.001f );
					if( pSet->pMeas->Conf.OutMax > 1 ){
						sprintf( &str[1][0], "CH2: %6.3f mA", pSet->pmA[1]->Set * 0.001f );
					}
					else {
						strcpy( &str[1][0], "CH2:  DISABLED" );
						pFunc->pMsgBox->Msg[1].tClr = RGB.DARKGRAY;
					}
					if( pFunc->ViewBlk == true ){
						switch( pFunc->Item.Lv3 ){
							case 0: str[0][ 5] = ' '; str[0][6] = ' '; break;
							case 1: str[0][ 8] = ' '; break;
							case 2: str[0][ 9] = ' '; break;
							case 3: str[0][10] = ' '; break;
							case 4: str[1][ 5] = ' '; str[1][6] = ' '; break;
							case 5: str[1][ 8] = ' '; break;
							case 6: str[1][ 9] = ' '; break;
							case 7: str[1][10] = ' '; break;
						}
					}
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					pFunc->pMsgBox->Msg[1].pStr = &str[1][0];
					pFunc->pMsgBox->Msg[3].pStr = "[  END TEST  ]";
					if( pFunc->Item.Lv3 == 8 ){
						pFunc->pMsgBox->Msg[3].tClr = RGB.YELLOW;
					}
					else {
						pFunc->pMsgBox->Msg[pFunc->Item.Lv3/4].tClr = RGB.YELLOW;
					}
					pFunc->pMsgBox->MsgLine = 4;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 1: // Relay Test
					pFunc->pMsgBox->Title.pStr =  "RELAY TEST";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font11x16 );
					rgb[0] = RGB.WHITE;
					rgb[1] = RGB.WHITE;
					rgb[2] = RGB.WHITE;
					rgb[3] = RGB.WHITE;
					rgb[pFunc->Item.Lv3] = RGB.YELLOW;
					GDI_StringColor(  34,  60, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "TROUBLE" );
					GDI_StringColor(  34,  80, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "ALARM-1" );
					GDI_StringColor(  34, 100, GDI_STR_ALIGN_L, RGB.LIGHTSKYBLUE, "ALARM-2" );
					GDI_StringColor( 137,  60, GDI_STR_ALIGN_R, rgb[0], "[" );
					GDI_StringColor( 137,  80, GDI_STR_ALIGN_R, rgb[1], "[" );
					GDI_StringColor( 137, 100, GDI_STR_ALIGN_R, rgb[2], "[" );
					GDI_StringColor( 156,  60, GDI_STR_ALIGN_C, rgb[0], pTest[pSet->pRelay->TRB.Set] );
					GDI_StringColor( 156,  80, GDI_STR_ALIGN_C, rgb[1], pTest[pSet->pRelay->AL1.Set] );
					GDI_StringColor( 156, 100, GDI_STR_ALIGN_C, rgb[2], pTest[pSet->pRelay->AL2.Set] );
					GDI_StringColor( 187,  60, GDI_STR_ALIGN_R, rgb[0], "]" ); 
					GDI_StringColor( 187,  80, GDI_STR_ALIGN_R, rgb[1], "]" );
					GDI_StringColor( 187, 100, GDI_STR_ALIGN_R, rgb[2], "]" );
					GDI_StringColor( 110, 130, GDI_STR_ALIGN_C, rgb[3], "[  END TEST  ]" );
					break;
					
				case 2: // Display Test
					switch( pSet->TestColor ){
						case 0: GDI_Clear( RGB.RED   ); break;
						case 1: GDI_Clear( RGB.GREEN ); break;
						case 2: GDI_Clear( RGB.BLUE  ); break;
						case 3: GDI_Clear( RGB.WHITE ); break;
					}
					break;
					
				case 3: // Gas Simulation
					pFunc->pMsgBox->Title.pStr =  "SELECT OUTPUT";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font11x16 );
					rgb[0] = RGB.WHITE;
					rgb[1] = RGB.WHITE;
					rgb[2] = RGB.WHITE;
					rgb[3] = RGB.WHITE;
					rgb[4] = RGB.BLACK;
					rgb[5] = RGB.BLACK;
					rgb[6] = RGB.BLACK;
					rgb[pFunc->Item.Lv3] = RGB.YELLOW;
					if( SystemOption.Item.IsBatMode == true ){
						rgb[0] = RGB.DARKGRAY;
						rgb[1] = RGB.DARKGRAY;
					}
					else {
						if( SystemOption.Item.IsPoeMode == true ){
							rgb[0] = RGB.DARKGRAY;
						}
						if( SystemOption.Item.BuzPresent == false ){
							rgb[2] = RGB.DARKGRAY;
						}
					}
					if( pSet->With_mA		== true ){ rgb[4] = RGB.SELECTED; }
					if( pSet->With_Relay	== true ){ rgb[5] = RGB.SELECTED; }
					if( pSet->With_Buz		== true ){ rgb[6] = RGB.SELECTED; }
					GDI_SetBackColor( rgb[4] );
					GDI_StringColor(  39,  50, GDI_STR_ALIGN_L, rgb[0], "[ " );
					GDI_StringColor( 182,  50, GDI_STR_ALIGN_R, rgb[0], " ]" );
					GDI_StringColor( 110,  50, GDI_STR_ALIGN_C, rgb[0], "    mA    " );
					GDI_SetBackColor( rgb[5] );
					GDI_StringColor( 110,  70, GDI_STR_ALIGN_C, rgb[1], "[   RELAY   ]" );
					GDI_SetBackColor( rgb[6] );
					GDI_StringColor( 110,  90, GDI_STR_ALIGN_C, rgb[2], "[    BUZ    ]" );
					GDI_SetBackColor( RGB.BLACK );
					GDI_StringColor( 110, 130, GDI_STR_ALIGN_C, rgb[3], "START TEST" );
					GDI_DrawRect( 42, 120, 137, 33 );
					break;
					
				case 4: // Maximum Power Test
					pFunc->pMsgBox->Title.pStr =  "MAX POWER TEST";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetFont( &Font11x16 );
					rgb[ 0] = RGB.WHITE;
					rgb[ 1] = RGB.WHITE;
					rgb[ 2] = RGB.WHITE;
					rgb[ 3] = RGB.WHITE;
					rgb[ 4] = RGB.WHITE;
					rgb[ 5] = RGB.WHITE;
					rgb[ 6] = RGB.BLACK;
					rgb[ 7] = RGB.BLACK;
					rgb[ 8] = RGB.BLACK;
					rgb[ 9] = RGB.BLACK;
					rgb[10] = RGB.BLACK;
					rgb[pFunc->Item.Lv3] = RGB.YELLOW;
					if( SystemOption.Item.IsBatMode == true ){
						rgb[0] = RGB.DARKGRAY;
						rgb[1] = RGB.DARKGRAY;
					}
					else {
						if( SystemOption.Item.IsPoeMode == true ){
							rgb[0] = RGB.DARKGRAY;
						}
						if( SystemOption.Item.BuzPresent == false ){
							rgb[2] = RGB.DARKGRAY;
						}
					}
					if( pSet->With_mA		== true ){ rgb[ 6] = RGB.SELECTED; }
					if( pSet->With_Relay	== true ){ rgb[ 7] = RGB.SELECTED; }
					if( pSet->With_Buz		== true ){ rgb[ 8] = RGB.SELECTED; }
					if( pSet->With_Led		== true ){ rgb[ 9] = RGB.SELECTED; }
					if( pSet->With_Pump		== true ){ rgb[10] = RGB.SELECTED; }
					GDI_SetBackColor( rgb[6] );
					GDI_StringColor(  39,  42, GDI_STR_ALIGN_L, rgb[0], "[ " );
					GDI_StringColor( 182,  42, GDI_STR_ALIGN_R, rgb[0], " ]" );
					GDI_StringColor( 110,  42, GDI_STR_ALIGN_C, rgb[0], "    mA    " );
					GDI_SetBackColor( rgb[7] );
					GDI_StringColor( 110,  59, GDI_STR_ALIGN_C, rgb[1], "[   RELAY   ]" );
					GDI_SetBackColor( rgb[8] );
					GDI_StringColor( 110,  76, GDI_STR_ALIGN_C, rgb[2], "[    BUZ    ]" );
					GDI_SetBackColor( rgb[9] );
					GDI_StringColor( 110,  93, GDI_STR_ALIGN_C, rgb[3], "[    LED    ]" );
					GDI_SetBackColor( rgb[10] );
					GDI_StringColor(  39, 110, GDI_STR_ALIGN_L, rgb[4], "[ " );
					GDI_StringColor( 182, 110, GDI_STR_ALIGN_R, rgb[4], " ]" );
					GDI_StringColor( 110, 110, GDI_STR_ALIGN_C, rgb[4], "   PUMP   " );
					GDI_SetBackColor( RGB.BLACK );
					GDI_StringColor( 110, 140, GDI_STR_ALIGN_C, rgb[5], "END TEST" );
					GDI_DrawRect( 42, 133, 137, 28 );
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			switch( pFunc->Item.Lv2 ){
				case 3: // Gas Simulation
					pFunc->pMsgBox->Title.pStr = "GAS SIMULATION";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					GDI_SetTextColor( RGB.DARKGRAY );
					GDI_DrawRect ( 14, 43, 192, 98 );
					GDI_DrawHLine( 14, 92, 192 );
					pGas = pSet->pMeas->pGas[0];
					pAlr = pSet->pMeas->pAlr[0];
					sprintf( &str[0][0], "%s", (char *)pGas->Inf.GasName );
					GasName_Right_Align( &str[0][0] );
					sprintf( &str[1][0], "H-S: %s", vStrGasConc( pGas->Inf.Dp, pGas->Inf.HighScale ) );
					sprintf( &str[2][0], "AL1: %s", vStrGasConc( pGas->Inf.Dp, pAlr->Inf._1stLevel ) );
					sprintf( &str[3][0], "%s", pType[pAlr->Inf._1stTypeIncrease] );
					sprintf( &str[4][0], "AL2: %s", vStrGasConc( pGas->Inf.Dp, pAlr->Inf._2ndLevel ) );
					sprintf( &str[5][0], "%s", pType[pAlr->Inf._2ndTypeIncrease] );
					if( pSet->Trouble[0] == true ){
						sprintf( &str[6][0], " E-01 %s", pUnit[pGas->Inf.Unit] );
					}
					else if( pAlr->Item.Bit.aOvr == true ){
						sprintf( &str[6][0], " OVER %s", pUnit[pGas->Inf.Unit] );
					}
					else {
						sprintf( &str[6][0], "%s %s", vStrGasConc( pGas->Inf.Dp, pGas->Set ), pUnit[pGas->Inf.Unit] );
					}
					rgb[0] = RGB.LIGHTSKYBLUE;
					rgb[1] = RGB.SPRINGGREEN;
					rgb[2] = RGB.PINK;
					if( pAlr->Inf._1stTypeIncrease == true ){ rgb[3] = RGB.RED; }
					else { rgb[3] = RGB.BLUE; }
					rgb[4] = RGB.PINK;
					if( pAlr->Inf._2ndTypeIncrease == true ){ rgb[5] = RGB.RED; }
					else { rgb[5] = RGB.BLUE; }
					if( MeasHandle.Conf.OutMax == 1 ){
						strcpy( &str[ 7][0], "  DISABLED" );
						strcpy( &str[ 8][0], "H-S:     0" );
						strcpy( &str[ 9][0], "AL1:     0" );
						strcpy( &str[10][0], "I" );
						strcpy( &str[11][0], "AL2:     0" );
						strcpy( &str[12][0], "I" );
						strcpy( &str[13][0], "    0 %VOL" );
						rgb[ 7] = RGB.DARKGRAY;
						rgb[ 8] = RGB.DARKGRAY;
						rgb[ 9] = RGB.DARKGRAY;
						rgb[10] = RGB.DARKGRAY;
						rgb[11] = RGB.DARKGRAY;
						rgb[12] = RGB.DARKGRAY;
						rgb[13] = RGB.DARKGRAY;
						switch( pFunc->Item.Lv4 ){
							case 0: rgb[6] = RGB.YELLOW; rgb[14] = RGB.WHITE ; break;
							case 1: rgb[6] = RGB.WHITE ; rgb[14] = RGB.YELLOW; break;
						}
					}
					else {
						pGas = pSet->pMeas->pGas[1];
						pAlr = pSet->pMeas->pAlr[1];
						sprintf( &str[ 7][0], "%s", (char *)pGas->Inf.GasName );
						GasName_Right_Align( &str[7][0] );
						sprintf( &str[ 8][0], "H-S: %s", vStrGasConc( pGas->Inf.Dp, pGas->Inf.HighScale ) );
						sprintf( &str[ 9][0], "AL1: %s", vStrGasConc( pGas->Inf.Dp, pAlr->Inf._1stLevel ) );
						sprintf( &str[10][0], "%s", pType[pAlr->Inf._1stTypeIncrease] );
						sprintf( &str[11][0], "AL2: %s", vStrGasConc( pGas->Inf.Dp, pAlr->Inf._2ndLevel ) );
						sprintf( &str[12][0], "%s", pType[pAlr->Inf._2ndTypeIncrease] );
						if( pSet->Trouble[1] == true ){
							sprintf( &str[13][0], " E-01 %s", pUnit[pGas->Inf.Unit] );
						}
						else if( pAlr->Item.Bit.aOvr == true ){
							sprintf( &str[13][0], " OVER %s", pUnit[pGas->Inf.Unit] );
						}
						else {
							sprintf( &str[13][0], "%s %s", vStrGasConc( pGas->Inf.Dp, pGas->Set ), pUnit[pGas->Inf.Unit] );
						}
						rgb[ 7] = RGB.LIGHTSKYBLUE;
						rgb[ 8] = RGB.SPRINGGREEN;
						rgb[ 9] = RGB.PINK;
						if( pAlr->Inf._1stTypeIncrease == true ){ rgb[10] = RGB.RED; }
						else { rgb[10] = RGB.BLUE; }
						rgb[11] = RGB.PINK;
						if( pAlr->Inf._2ndTypeIncrease == true ){ rgb[12] = RGB.RED; }
						else { rgb[12] = RGB.BLUE; }
						rgb[13] = RGB.WHITE;
						switch( pFunc->Item.Lv4 ){
							case 0: rgb[6] = RGB.YELLOW; rgb[13] = RGB.WHITE ; rgb[14] = RGB.WHITE ; break;
							case 1: rgb[6] = RGB.WHITE ; rgb[13] = RGB.YELLOW; rgb[14] = RGB.WHITE ; break;
							case 2: rgb[6] = RGB.WHITE ; rgb[13] = RGB.WHITE ; rgb[14] = RGB.YELLOW; break;
						}
					}
					strcpy( &str[14][0], "[  END TEST  ]" );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor( 197,  51, GDI_STR_ALIGN_R, rgb[ 0], &str[ 0][0] );
					GDI_StringColor(  23,  51, GDI_STR_ALIGN_L, rgb[ 1], &str[ 1][0] );
					GDI_StringColor(  23,  64, GDI_STR_ALIGN_L, rgb[ 2], &str[ 2][0] );
					GDI_StringColor(  23,  75, GDI_STR_ALIGN_L, rgb[ 4], &str[ 4][0] );
					GDI_StringColor( 197,  75, GDI_STR_ALIGN_R, rgb[ 6], &str[ 6][0] );
					GDI_StringColor( 197,  99, GDI_STR_ALIGN_R, rgb[ 7], &str[ 7][0] );
					GDI_StringColor(  23,  99, GDI_STR_ALIGN_L, rgb[ 8], &str[ 8][0] );
					GDI_StringColor(  23, 112, GDI_STR_ALIGN_L, rgb[ 9], &str[ 9][0] );
					GDI_StringColor(  23, 123, GDI_STR_ALIGN_L, rgb[11], &str[11][0] );
					GDI_StringColor( 197, 123, GDI_STR_ALIGN_R, rgb[13], &str[13][0] );
					GDI_StringColor( 110, 151, GDI_STR_ALIGN_C, rgb[14], &str[14][0] );
					GDI_SetFont( &Fimg07x12 );
					GDI_StringColor(  95,  64, GDI_STR_ALIGN_L, rgb[ 3], &str[ 3][0] );
					GDI_StringColor(  95,  75, GDI_STR_ALIGN_L, rgb[ 5], &str[ 5][0] );
					GDI_StringColor(  95, 112, GDI_STR_ALIGN_L, rgb[10], &str[10][0] );
					GDI_StringColor(  95, 123, GDI_STR_ALIGN_L, rgb[12], &str[12][0] );
					break;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GasName_Right_Align( char *pStr )
{
	U8		i;
	char	str[11];
	
	for( i=0; i<10; i++ ){
		if( (*(pStr+i) == ' ') && (*(pStr+i+1) == ' ') ){
			str[i] = 0;
			break;
		}
		else {
			str[i] = *(pStr+i);
		}
	}
	
	str[10] = 0;
	
	sprintf( pStr, "%10s", str );
}


