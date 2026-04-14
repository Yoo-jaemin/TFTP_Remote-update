#define __VALLBASE_MODE_C__
    #include "vallbase_mode.h"
#undef  __VALLBASE_MODE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gdi.h"
#include "vmeasuring.h"
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

void vAllBase_Mode( AllBase_HandleType *pHandle )
{
	char		str[30];
	uint16_t	rgb;
	U8			i;
	char		*pUnit[MEASURING_UNIT_MAX] = { " PPB", " PPM", "%LEL", "%VOL" };
	
	pHandle->pMsgBox->Title.pStr = "ALL BASE ZERO";
	pHandle->pMsgBox->LineColor = RGB.SLATEBLUE;
	pHandle->pMsgBox->TboxColor = RGB.SLATEBLUE;
	pHandle->pMsgBox->BackColor = 3;
	pHandle->pMsgBox->Width = 190;
	pHandle->pMsgBox->MsgLine = 0;
	pHandle->pMsgBox->OutLine = 5;
	vMessageBox( pHandle->pMsgBox );
	
	GDI_SetFont( &Font07x12 );
	for( i=0; i<MEASURING_GAS_SENSOR_MAX; i++ ){
		if( pHandle->pMeas->Gas[i].IsPresent == true ){
			if( pHandle->pMeas->Gas[i].Inf.Func == MEASURING_GAS_FUNC_IR_REF ){
				sprintf( str, "S%u %s     0 ----", 1+i, (char *)pHandle->pMeas->Gas[i].Inf.GasName );
				rgb = RGB.DARKGRAY;
			}
			else {
				sprintf( str, "S%u %s %s %s", 1+i,
					(char *)pHandle->pMeas->Gas[i].Inf.GasName,
					vStrGasConc( pHandle->pMeas->Gas[i].Inf.Dp, pHandle->pMeas->Gas[i].Act ),
					pUnit[pHandle->pMeas->Gas[i].Inf.Unit]
				);
				if( pHandle->pMeas->Gas[i].Inf.bZeroUse == true ){
					switch( pHandle->State ){
						case ALLBASE_STATE_READY:
						case ALLBASE_STATE_PROGRESS:	rgb = RGB.WHITE;		break;
						case ALLBASE_STATE_COMPLETE:	rgb = RGB.LIGHTSKYBLUE; break;
					}
				}
				else {
					rgb = RGB.DARKGRAY;
				}
			}
		}
		else {
			sprintf( str, "S%u DISABLED       0 ----", 1+i );
			rgb = RGB.DARKGRAY;
		}
		GDI_StringColor( 110, 53+i*13, GDI_STR_ALIGN_C, rgb, str );
	}
	
	switch( pHandle->State ){
		case ALLBASE_STATE_READY:
			if( pHandle->IsApproved == true ){	strcpy( str, "[  CAL NOW?  ]" ); }
			else							 {	strcpy( str, "[   CANCEL   ]" ); }
			break;
			
		case ALLBASE_STATE_PROGRESS:			strcpy( str, "[  PROGRESS  ]" ); break;
		case ALLBASE_STATE_COMPLETE:			strcpy( str, "[  COMPLETE  ]" ); break;
	}
	GDI_StringColor( 110, 139, GDI_STR_ALIGN_C, RGB.YELLOW, str );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vAllBase_Clr( void )
{
	GDI_FillRectColor( 15, 12, 190, 152, RGB.BLACK );
}


