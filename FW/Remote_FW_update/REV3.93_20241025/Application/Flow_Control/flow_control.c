#define __FLOW_CONTROL_C__
    #include "flow_control.h"
#undef  __FLOW_CONTROL_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "eeprom.h"
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

bool Flow_Control_Init( void )
{
	FlowCtrl.pSen					= &SensingProcess;
	FlowCtrl.pPump					= &PumpDrive;
	
	FlowCtrl.Cal.Ptm.pPtmOffs		= &MCP46x1_Wiper[FLOW_PTM_OFFS_WIPER_INDEX];
	FlowCtrl.Cal.Ptm.pPtmGain		= &MCP46x1_Wiper[FLOW_PTM_GAIN_WIPER_INDEX];
	FlowCtrl.Cal.Ptm.SysOffs.Word	= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_PTM_OFFS];
	FlowCtrl.Cal.Ptm.SysGain.Word	= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_PTM_GAIN];
	FlowCtrl.Cal.Span_mL			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_SPAN_mL];
	FlowCtrl.Cal.Targ_mL			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_TARG_mL];
	FlowCtrl.Cal.SpanFreq			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_SPAN_FREQ_x100] * 0.01f;
	FlowCtrl.Cal.SpanDuty			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_SPAN_DUTY_x100] * 0.01f;
	FlowCtrl.Cal.Span.w.l			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_SPAN_L];
	FlowCtrl.Cal.Span.w.h			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_SPAN_H];
	FlowCtrl.Cal.Zero.w.l			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_ZERO_L];
	FlowCtrl.Cal.Zero.w.h			= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_ZERO_H];
	
	FlowCtrl.P_Value				= 0.f;
	FlowCtrl.P_Scale				= EepHandle.Sram[SYSTEM_EEP_FLOW_CTRL_P_CALCULATION_SCALE];
	                                    
	FlowCtrl.TroubleSec				= EepHandle.Sram[SYSTEM_EEP_FLOW_TRB_CHECK_SEC];                                                    
	FlowCtrl.TroubleTmr				= FlowCtrl.TroubleSec * 10;
	
	PTM_Offs_Update();
	PTM_Gain_Update();
	
	return SystemError.Peripheral.Bit.Psen_Potentiometer | Pump_Init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Flow_Control_ReInit( void )
{
	FlowCtrl.Cal.Ptm.pPtmOffs		= &MCP46x1_Wiper[FLOW_PTM_OFFS_WIPER_INDEX];
	FlowCtrl.Cal.Ptm.pPtmGain		= &MCP46x1_Wiper[FLOW_PTM_GAIN_WIPER_INDEX];
	FlowCtrl.Cal.Ptm.SysOffs.Word	= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_PTM_OFFS];
	FlowCtrl.Cal.Ptm.SysGain.Word	= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_PTM_GAIN];
	FlowCtrl.Cal.Span_mL			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_SPAN_mL];
	FlowCtrl.Cal.Targ_mL			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_TARG_mL];
	FlowCtrl.Cal.SpanFreq			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_SPAN_FREQ_x100] * 0.01f;
	FlowCtrl.Cal.SpanDuty			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_SPAN_DUTY_x100] * 0.01f;
	FlowCtrl.Cal.Span.w.l			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_SPAN_L];
	FlowCtrl.Cal.Span.w.h			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_SPAN_H];
	FlowCtrl.Cal.Zero.w.l			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_ZERO_L];
	FlowCtrl.Cal.Zero.w.h			= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_ZERO_H];
	
	FlowCtrl.P_Value				= 0.f;
	FlowCtrl.P_Scale				= EepHandle.pDefault[SYSTEM_EEP_FLOW_CTRL_P_CALCULATION_SCALE];
	                                    
	FlowCtrl.TroubleSec				= EepHandle.pDefault[SYSTEM_EEP_FLOW_TRB_CHECK_SEC];                                                    
	FlowCtrl.TroubleTmr				= FlowCtrl.TroubleSec * 10;
	
	PTM_Offs_Update();
	PTM_Gain_Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Flow_Control( void )
{
	float	Slope = FlowCtrl.Cal.Span_mL / (FlowCtrl.Cal.Span.f - FlowCtrl.Cal.Zero.f);
	
	FlowCtrl.FlowRate	= (FlowCtrl.pSen->Raw.Pressure.AmpOut - FlowCtrl.Cal.Zero.f ) * Slope;
	FlowCtrl.P_Value	= (FlowCtrl.Cal.Targ_mL - FlowCtrl.FlowRate) / FlowCtrl.P_Scale;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PTM_Offs_Update( void )
{
	if( FlowCtrl.Cal.Ptm.pPtmOffs->Word != FlowCtrl.Cal.Ptm.SysOffs.Word ){
		FlowCtrl.Cal.Ptm.pPtmOffs->Word  = FlowCtrl.Cal.Ptm.SysOffs.Word;
		
		MCP46X1_Wr_Wiper( FLOW_PTM_OFFS_SET_ADDRESS, FlowCtrl.Cal.Ptm.pPtmOffs->Word );
		MCP46X1_Wr_Wiper( FLOW_PTM_OFFS_UPD_ADDRESS, FlowCtrl.Cal.Ptm.pPtmOffs->Word );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PTM_Gain_Update( void )
{
	if( FlowCtrl.Cal.Ptm.pPtmGain->Word != FlowCtrl.Cal.Ptm.SysGain.Word ){
		FlowCtrl.Cal.Ptm.pPtmGain->Word  = FlowCtrl.Cal.Ptm.SysGain.Word;
		
		MCP46X1_Wr_Wiper( FLOW_PTM_GAIN_SET_ADDRESS, FlowCtrl.Cal.Ptm.pPtmGain->Word );
		MCP46X1_Wr_Wiper( FLOW_PTM_GAIN_UPD_ADDRESS, FlowCtrl.Cal.Ptm.pPtmGain->Word );
	}
}

