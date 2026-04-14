#ifndef   __FLOW_CONTROL_H__
#define   __FLOW_CONTROL_H__


#ifdef __FLOW_CONTROL_C__
	#define FLOW_CONTROL_EXT
#else
	#define FLOW_CONTROL_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"  
#include "mcp46x1.h"
#include "sensing_process.h" 
#include "pump.h" 
/* End Include */

/* Start Define */
#define FLOW_CTRL_MIN				(  100.f ) // mL
#define FLOW_CTRL_MAX				( 1000.f ) // mL
/* End Define */

/* Start Enum */
enum {
	FLOW_PTM_OFFS_SET_ADDRESS,
	FLOW_PTM_GAIN_SET_ADDRESS,
	FLOW_PTM_OFFS_UPD_ADDRESS,
	FLOW_PTM_GAIN_UPD_ADDRESS
};

enum {
	FLOW_PTM_OFFS_WIPER_INDEX,
	FLOW_PTM_GAIN_WIPER_INDEX
};
/* End Enum */

/* Start Struct */
typedef struct {
	MCP46x1_WiperType			*pPtmOffs;
	MCP46x1_WiperType			*pPtmGain;
	MCP46x1_WiperType			SysOffs;
	MCP46x1_WiperType			SysGain;
}Flow_PtmType;

typedef struct {
	Flow_PtmType				Ptm;
	U16							Span_mL;
	U16							Targ_mL;
	float						SpanFreq;	// Hz
	float						SpanDuty;	// %
	ufloat						Zero;		// mV
	ufloat						Span;		// mV
}Flow_CalType;

typedef struct {
	SensingProcess_HandleType	*pSen;
	Pump_DriveType				*pPump;
	Flow_CalType				Cal;
	float						FlowRate;
	float						P_Value;
	float						P_Scale;
	U16							TroubleSec;
	U16							TroubleTmr;
}Flow_ControlType;
/* End Struct */

/* Start Function */
FLOW_CONTROL_EXT bool Flow_Control_Init( void );
FLOW_CONTROL_EXT void Flow_Control_ReInit( void );
FLOW_CONTROL_EXT void Flow_Control( void );
FLOW_CONTROL_EXT void PTM_Offs_Update( void );
FLOW_CONTROL_EXT void PTM_Gain_Update( void );
/* End Function */

/* Start Variable */
FLOW_CONTROL_EXT Flow_ControlType		FlowCtrl;
/* End Variable */


#endif // __FLOW_CONTROL_H__
