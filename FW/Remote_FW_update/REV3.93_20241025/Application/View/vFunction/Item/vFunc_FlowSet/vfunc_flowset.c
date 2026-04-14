#define __VFUNC_FLOWSET_C__
    #include "vfunc_flowset.h"
#undef  __VFUNC_FLOWSET_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
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

void vFunc_FlowSet( Func_HandleType *pFunc, FlowSet_HandleType *pSet )
{
	char	str[5][19];
	                       
	switch( pFunc->Level ){
		case FUNC_LEVEL_1:
		case FUNC_LEVEL_2:
			pFunc->pMsgBox->Title.pStr = "FLOW RATE MENU";
			pFunc->pMsgBox->Msg[0].pStr = "CALIBRATION";
			pFunc->pMsgBox->Msg[1].pStr = "TARGET FLOW";
			pFunc->pMsgBox->Msg[2].pStr = "TROUBLE";
			pFunc->pMsgBox->Msg[pFunc->Item.Lv2].tClr = RGB.YELLOW;
			pFunc->pMsgBox->MsgLine = 3;
			break;
			
		case FUNC_LEVEL_3:
			switch( pSet->Status ){
				case FLOW_SET_STATUS_SPAN_SET:
					pFunc->pMsgBox->Title.pStr = "SPAN SET";
					sprintf( &str[0][0], "FLOW: %5u mL", pSet->Cal.Span_mL );
					sprintf( &str[1][0], "CTRL: %5.1f  %%",
						PUMP_CTRL_FREQ_MAX - pSet->pFlow->pPump->SetFreq + pSet->pFlow->pPump->SetDuty - PUMP_CTRL_DUTY_MIN
					);
					if( pFunc->ViewBlk == true ){
						switch( pFunc->Item.Lv3 ){
							case 1: str[1][ 6] = ' '; str[1][7] = ' '; str[1][8] = ' ';	break;
							case 2: str[1][10] = ' ';									break;
						}
					}
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					pFunc->pMsgBox->Msg[1].pStr = &str[1][0];
					if( pFunc->IsApproved == true )	{ pFunc->pMsgBox->Msg[3].pStr = "[  PROGRESS  ]"; }
					else							{ pFunc->pMsgBox->Msg[3].pStr = "[   CANCEL   ]"; }
					switch( pFunc->Item.Lv3 ){
						case 0: pFunc->pMsgBox->Msg[0].tClr = RGB.YELLOW; break;
						case 1:
						case 2: pFunc->pMsgBox->Msg[1].tClr = RGB.YELLOW; break;
						case 3: pFunc->pMsgBox->Msg[3].tClr = RGB.YELLOW; break;
					}
					pFunc->pMsgBox->MsgLine = 4;
					break;
					
				case FLOW_SET_STATUS_TARG_SET:
					pFunc->pMsgBox->Title.pStr = "TARGET SET";
					sprintf( &str[0][0], "FLOW: %5u mL", pSet->Cal.Targ_mL );
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					if( pFunc->IsApproved == true )	{ pFunc->pMsgBox->Msg[2].pStr = "[    SAVE    ]"; }
					else							{ pFunc->pMsgBox->Msg[2].pStr = "[   CANCEL   ]"; }
					if( pFunc->Item.Lv3 == 1 ){ pFunc->pMsgBox->Msg[2].tClr = RGB.YELLOW; }
					else { pFunc->pMsgBox->Msg[pFunc->Item.Lv3].tClr = RGB.YELLOW; }
					pFunc->pMsgBox->MsgLine = 3;
					break;
					
				case FLOW_SET_STATUS_TRB_CHECK_TIME_SET:
					pFunc->pMsgBox->Title.pStr = "TROUBLE SET";
					sprintf( &str[0][0], "TIME: %4u SEC", pSet->TroubleSec );
					pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
					if( pFunc->IsApproved == true )	{ pFunc->pMsgBox->Msg[2].pStr = "[    SAVE    ]"; }
					else							{ pFunc->pMsgBox->Msg[2].pStr = "[   CANCEL   ]"; }
					if( pFunc->Item.Lv3 == 1 ){ pFunc->pMsgBox->Msg[2].tClr = RGB.YELLOW; }
					else { pFunc->pMsgBox->Msg[pFunc->Item.Lv3].tClr = RGB.YELLOW; }
					pFunc->pMsgBox->MsgLine = 3;
					break;
			}
			break;
			
		case FUNC_LEVEL_4:
			sprintf( &str[0][0], "CURR: %5.0f mV", pSet->AmpOut );
			sprintf( &str[1][0], "GAIN: %5.1f x1", FlowSet_Amp_Gain( pSet->Cal.Ptm.SysGain.Word ) );
			sprintf( &str[2][0], "OFFS: %5.0f mV", pSet->Cal.Ptm.SysOffs.Word * FLOW_SET_PTM_OFFS_VLSB );
			sprintf( &str[3][0], "ZERO: %5.0f mV", pSet->Cal.Zero.f );
			sprintf( &str[4][0], "SPAN: %5.0f mV", pSet->Cal.Span.f );
			pFunc->pMsgBox->Msg[0].pStr = &str[0][0];
			pFunc->pMsgBox->Msg[1].pStr = &str[1][0];
			pFunc->pMsgBox->Msg[2].pStr = &str[2][0];
			pFunc->pMsgBox->Msg[3].pStr = &str[3][0];
			pFunc->pMsgBox->Msg[4].pStr = &str[4][0];
			pFunc->pMsgBox->Msg[0].tClr = RGB.SPRINGGREEN;
			switch( pSet->Status ){
				case FLOW_SET_STATUS_CAL_GAIN_SET_OFFS_SCAN:
				case FLOW_SET_STATUS_CAL_GAIN_SET_ZERO_SCAN:
				case FLOW_SET_STATUS_CAL_GAIN_SET_SPAN_SCAN:
					pFunc->pMsgBox->Title.pStr = "PROGRESS [1/4]";
					pFunc->pMsgBox->Msg[1].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 5;
					break;
					
				case FLOW_SET_STATUS_CAL_OFFS_SCAN:
				case FLOW_SET_STATUS_CAL_OFFS_INCREASE:
				case FLOW_SET_STATUS_CAL_OFFS_DECREASE:
					pFunc->pMsgBox->Title.pStr = "PROGRESS [2/4]";
					pFunc->pMsgBox->Msg[1].tClr = RGB.LIGHTSKYBLUE;
					pFunc->pMsgBox->Msg[2].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 5;
					break;
					
				case FLOW_SET_STATUS_CAL_ZERO_DECISION:
					pFunc->pMsgBox->Title.pStr = "PROGRESS [3/4]";
					pFunc->pMsgBox->Msg[1].tClr = RGB.LIGHTSKYBLUE;
					pFunc->pMsgBox->Msg[2].tClr = RGB.LIGHTSKYBLUE;
					pFunc->pMsgBox->Msg[3].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 5;
					break;
					
				case FLOW_SET_STATUS_CAL_SPAN_DECISION:
					pFunc->pMsgBox->Title.pStr = "PROGRESS [4/4]";
					pFunc->pMsgBox->Msg[1].tClr = RGB.LIGHTSKYBLUE;
					pFunc->pMsgBox->Msg[2].tClr = RGB.LIGHTSKYBLUE;
					pFunc->pMsgBox->Msg[3].tClr = RGB.LIGHTSKYBLUE;
					pFunc->pMsgBox->Msg[4].tClr = RGB.YELLOW;
					pFunc->pMsgBox->MsgLine = 5;
					break;
					
				case FLOW_SET_STATUS_CAL_END:
					switch( pSet->Result ){
						case FLOW_SET_RESULT_SUCCESS:
							pFunc->pMsgBox->Title.pStr = "SUCCESS";
							pFunc->pMsgBox->Msg[1].tClr = RGB.LIGHTSKYBLUE;
							pFunc->pMsgBox->Msg[2].tClr = RGB.LIGHTSKYBLUE;
							pFunc->pMsgBox->Msg[3].tClr = RGB.LIGHTSKYBLUE;
							pFunc->pMsgBox->Msg[4].tClr = RGB.LIGHTSKYBLUE;
							pFunc->pMsgBox->MsgLine = 5;
							break;
							
						case FLOW_SET_RESULT_PTM_HW_ERROR:
							pFunc->pMsgBox->Msg[0].pStr = "PTM H/W ERROR";
							pFunc->pMsgBox->Msg[0].tClr = RGB.RED;
							pFunc->pMsgBox->MsgLine = 1;
							break;
							
						case FLOW_SET_RESULT_PUMP_ERROR:
							pFunc->pMsgBox->Msg[0].pStr = "PUMP ERROR";
							pFunc->pMsgBox->Msg[0].tClr = RGB.RED;
							pFunc->pMsgBox->MsgLine = 1;
							break;
							
						case FLOW_SET_RESULT_EXCEED_ZERO_RANGE:
							pFunc->pMsgBox->Msg[0].pStr = "EXCEED ZERO RANGE";
							pFunc->pMsgBox->Msg[0].tClr = RGB.RED;
							pFunc->pMsgBox->MsgLine = 1;
							break;
							
						case FLOW_SET_RESULT_EXCEED_SPAN_RANGE:
							pFunc->pMsgBox->Msg[0].pStr = "EXCEED SPAN RANGE";
							pFunc->pMsgBox->Msg[0].tClr = RGB.RED;
							pFunc->pMsgBox->MsgLine = 1;
							break;
					}
					break;
			}
			break;
	}
	vMessageBox( pFunc->pMsgBox );
}


