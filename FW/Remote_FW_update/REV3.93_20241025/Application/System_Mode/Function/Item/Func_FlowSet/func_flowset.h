#ifndef   __FUNC_FLOWSET_H__
#define   __FUNC_FLOWSET_H__


#ifdef __FUNC_FLOWSET_C__
	#define FUNC_FLOWSET_EXT
#else
	#define FUNC_FLOWSET_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
#include "function_mode.h" 
#include "flow_control.h"
/* End Include */

/* Start Define */
#define FLOW_SET_PTM_RESOLUTION				( 256.f )
#define FLOW_SET_PTM_OFFS_VREF				( 2500.f ) // mV
#define FLOW_SET_PTM_OFFS_VLSB				( FLOW_SET_PTM_OFFS_VREF / FLOW_SET_PTM_RESOLUTION ) // 9.765625 mV
/* End Define */

/* Start Enum */
typedef enum {
	FLOW_SET_STATUS_READY,
	FLOW_SET_STATUS_ITEM_SET,
	FLOW_SET_STATUS_SPAN_SET,
	FLOW_SET_STATUS_TARG_SET,
	FLOW_SET_STATUS_TRB_CHECK_TIME_SET,
	FLOW_SET_STATUS_CAL_GAIN_SET_OFFS_SCAN,
	FLOW_SET_STATUS_CAL_GAIN_SET_ZERO_SCAN,
	FLOW_SET_STATUS_CAL_GAIN_SET_SPAN_SCAN,
	FLOW_SET_STATUS_CAL_OFFS_SCAN,
	FLOW_SET_STATUS_CAL_OFFS_INCREASE,
	FLOW_SET_STATUS_CAL_OFFS_DECREASE,
	FLOW_SET_STATUS_CAL_ZERO_DECISION,
	FLOW_SET_STATUS_CAL_SPAN_DECISION,
	FLOW_SET_STATUS_CAL_END
}FlowSet_StatusType;

typedef enum {
	FLOW_SET_RESULT_SUCCESS,
	FLOW_SET_RESULT_PTM_HW_ERROR,
	FLOW_SET_RESULT_PUMP_ERROR,
	FLOW_SET_RESULT_EXCEED_ZERO_RANGE,
	FLOW_SET_RESULT_EXCEED_SPAN_RANGE
}FlowSet_ResultType;
/* End Enum */

/* Start Struct */
typedef struct {
	Flow_ControlType			*pFlow;
	Flow_CalType				Cal;
	U16							Tick;
	FlowSet_StatusType			Status;
	FlowSet_ResultType			Result;
	float						AmpOut;
	float						AmpRef;
	U16							Span;
	U16							TroubleSec;
	float						TargetZero;
	float						TargetZeroMin;
	float						TargetZeroMax;
}FlowSet_HandleType;
/* End Struct */

/* Start Function */
FUNC_FLOWSET_EXT void Func_FlowSet( Func_HandleType *pFunc );
FUNC_FLOWSET_EXT void FlowSet_Handler( void );
FUNC_FLOWSET_EXT float FlowSet_Amp_Gain( U16 Wiper );
/* End Function */

/* Start Variable */
FUNC_FLOWSET_EXT FlowSet_HandleType		FlowSet;
/* End Variable */


#endif // __FUNC_FLOWSET_H__
