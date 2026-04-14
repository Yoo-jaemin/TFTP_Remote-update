#define __VFUNC_SYSTEM_C__
    #include "vfunc_system.h"
#undef  __VFUNC_SYSTEM_C__

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
static vPoint	vp[10];
static vLine	vl[10];
static vArea	va[10];
#endif
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vFunc_System( Func_HandleType *pFunc, System_SetType *pSet )
{
	char		str[4][19];
	U16			rgb[4];
	U16			fw;
	U16			i;
	
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
		case FUNC_LEVEL_2:                                    
			pFunc->pMsgBox->Title.pStr = "SYSTEM MENU";
			pFunc->pMsgBox->Msg[0].pStr = "CALENDAR";
			pFunc->pMsgBox->Msg[1].pStr = "PASSWORD";
			pFunc->pMsgBox->Msg[2].pStr = "DISPLAY";
			pFunc->pMsgBox->Msg[3].pStr = "BUZZER";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv2].tClr = RGB.YELLOW;
			if( SystemOption.Item.BuzPresent == false ){
				pFunc->pMsgBox->Msg[3].tClr = RGB.DARKGRAY;
			}
			pFunc->pMsgBox->MsgLine = 4;
			vMessageBox( pFunc->pMsgBox );
			break;
			
		case FUNC_LEVEL_3:
			switch( pFunc->Item.Lv2 ){
				case 0: // Calendar Set
					pFunc->pMsgBox->Title.pStr = "CALENDAR SET";
					if( pFunc->Item.Lv3 == 7 ){
						sprintf( &str[0][0], "%02u / %02u / %02u",
							pSet->pDate->Year,
							pSet->pDate->Month,
							pSet->pDate->Date
						);
						sprintf( &str[1][0], "%02u : %02u : %02u",
							pSet->pTime->Hours,
							pSet->pTime->Minutes,
							pSet->pTime->Seconds
						);
						pFunc->pMsgBox->Msg[3].pStr = "[   QUIT   ]";
						pFunc->pMsgBox->Msg[0].tClr = RGB.LIGHTSKYBLUE;
						pFunc->pMsgBox->Msg[1].tClr = RGB.LIGHTSKYBLUE;
						pFunc->pMsgBox->Msg[3].tClr = RGB.YELLOW;
					}
					else {
						sprintf( &str[0][0], "%02u / %02u / %02u",
							pSet->sDate.Year,
							pSet->sDate.Month,
							pSet->sDate.Date
						);
						sprintf( &str[1][0], "%02u : %02u : %02u",
							pSet->sTime.Hours,
							pSet->sTime.Minutes,
							pSet->sTime.Seconds
						);
						if( pFunc->IsApproved == true )	{ pFunc->pMsgBox->Msg[3].pStr = "[   SAVE   ]"; }
						else							{ pFunc->pMsgBox->Msg[3].pStr = "[  CANCEL  ]"; }
						if( pFunc->ViewBlk == true ){
							switch( pFunc->Item.Lv3 ){
								case 0: str[0][ 0] = ' '; str[0][ 1] = ' '; break; 
								case 1: str[0][ 5] = ' '; str[0][ 6] = ' '; break; 
								case 2: str[0][10] = ' '; str[0][11] = ' '; break; 
								case 3: str[1][ 0] = ' '; str[1][ 1] = ' '; break; 
								case 4: str[1][ 5] = ' '; str[1][ 6] = ' '; break; 
								case 5: str[1][10] = ' '; str[1][11] = ' '; break;
							}
						}
						switch( pFunc->Item.Lv3 ){
							case 0:
							case 1:
							case 2: pFunc->pMsgBox->Msg[0].tClr = RGB.YELLOW; break;
							case 3:
							case 4:
							case 5: pFunc->pMsgBox->Msg[1].tClr = RGB.YELLOW; break; 
							case 6: pFunc->pMsgBox->Msg[3].tClr = RGB.YELLOW; break;
						}
					} 
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					pFunc->pMsgBox->Msg[1].pStr = &str[1][0];
					pFunc->pMsgBox->MsgLine = 4;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 1: // Password Set
					pFunc->pMsgBox->Title.pStr = "PASSWORD SET";
					sprintf( &str[0][0], "NEW P/W : %02u", pSet->sPW );
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					if( pFunc->IsApproved == true )	{ pFunc->pMsgBox->Msg[2].pStr = "[   SAVE   ]"; }
					else							{ pFunc->pMsgBox->Msg[2].pStr = "[  CANCEL  ]"; }
					switch( pFunc->Item.Lv3 ){
						case 0: pFunc->pMsgBox->Msg[0].tClr = RGB.YELLOW; break;
						case 1: pFunc->pMsgBox->Msg[2].tClr = RGB.YELLOW; break;
					}
					pFunc->pMsgBox->MsgLine = 3;
					vMessageBox( pFunc->pMsgBox );
					break;
					
				case 2: // Display Set
					pFunc->pMsgBox->Title.pStr = "DISPLAY SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					sprintf( &str[0][0], "%3u [%%]", pSet->sBrightness );
					if( pFunc->IsApproved == true )	{ strcpy( &str[1][0], "[   SAVE   ]" ); }
					else							{ strcpy( &str[1][0], "[  CANCEL  ]" ); }
					switch( pFunc->Item.Lv3 ){
						case 0: rgb[0] = RGB.YELLOW; rgb[1] = RGB.WHITE; break;
						case 1: rgb[1] = RGB.YELLOW; rgb[0] = RGB.WHITE; break;
					}
					fw = (U16)(pSet->sBrightness * 1.5f);
					GDI_SetTextColor( RGB.BLUE );
					GDI_FillRect( 33   ,  98,     fw,  5 );
					GDI_FillRect( 33+fw,  93,      5, 15 );
					GDI_SetTextColor( RGB.BLACK );
					GDI_FillRect( 30+fw,  93,      3,  5 );
					GDI_FillRect( 38+fw,  93,      3,  5 );
					GDI_FillRect( 30+fw, 103,      3,  5 );
					GDI_FillRect( 38+fw, 103,      3,  5 );
					GDI_SetTextColor( RGB.GRAY );
					GDI_FillRect( 38+fw,  98, 150-fw,  5 );
					GDI_SetFont( &Font07x12 );
					GDI_StringColor(  34,  70, GDI_STR_ALIGN_L, RGB.SPRINGGREEN, "BRIGHTNESS " );
					GDI_StringColor( 188,  70, GDI_STR_ALIGN_R, rgb[0], &str[0][0] );
					GDI_StringColor( 110, 130, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
					break;
					
				case 3: // Buzzer Set
					pFunc->pMsgBox->Title.pStr = "BUZZER SET";
					pFunc->pMsgBox->MsgLine = 0;
					vMessageBox( pFunc->pMsgBox );
					switch( pFunc->Item.Lv3 ){
						case 0: rgb[0] = RGB.YELLOW; rgb[1] = RGB.WHITE; break;
						case 1: rgb[1] = RGB.YELLOW; rgb[0] = RGB.WHITE; break;
					}
					if( pSet->sBuzVolume ){ sprintf( &str[0][0], "%u  ", pSet->sBuzVolume ); }
					else { strcpy( &str[0][0], "OFF" ); }
					for( i=0; i<8; i++ ){
						GDI_SetTextColor( rgb[0] );
						GDI_DrawRect( 55+14*i, 113-8*i, 9, 8+8*i );
						if( i >= pSet->sBuzVolume ){
							GDI_SetTextColor( RGB.BLACK );
						}
						GDI_FillRect( 57+14*i, 115-8*i, 5, 4+8*i );
					}
					if( pFunc->IsApproved == true )	{ strcpy( &str[1][0], "[  SAVE  ]" ); }
					else							{ strcpy( &str[1][0], "[ CANCEL ]" ); }
					GDI_StringColor(  55,  54, GDI_STR_ALIGN_L, rgb[0], &str[0][0] );
					GDI_StringColor( 109, 138, GDI_STR_ALIGN_C, rgb[1], &str[1][0] );
					break;
			}
			break;
	}
}


