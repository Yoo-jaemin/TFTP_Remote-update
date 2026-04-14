#define __VSELF_TEST_C__
    #include "vself_test.h"
#undef  __VSELF_TEST_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vlibrary.h"
#include "measuring.h"
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
static vMessageBoxType		*pMsgBox = &vMsgBox;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vSelf_Test( SelfTest_HandleType *pTest )
{
	char	str[6][17];
	U16		rgb[5];
	
	pMsgBox->Title.pStr = "SELF-TEST";
	pMsgBox->Width = 216;
	pMsgBox->MsgLine = 0;
	pMsgBox->OutLine = 6;
	vMessageBox( pMsgBox );
	GDI_SetFont( &Font07x12 );
	switch( pTest->State ){
		case SELF_TEST_STATE_SENSOR_CHECK:
			rgb[0] = RGB.YELLOW;
			rgb[1] = RGB.YELLOW;
			rgb[2] = RGB.WHITE;
		    rgb[3] = RGB.WHITE;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[4] = RGB.LIGHTSKYBLUE;
			}
			else {
				rgb[4] = RGB.WHITE;
			}
			break;
			
		case SELF_TEST_STATE_IR_SRC_CHECK:
			rgb[0] = RGB.YELLOW;
			rgb[1] = RGB.LIGHTSKYBLUE;
			rgb[2] = RGB.YELLOW;
		    rgb[3] = RGB.WHITE;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[4] = RGB.LIGHTSKYBLUE;
			}
			else {
				rgb[4] = RGB.WHITE;
			}
			break;
			
		case SELF_TEST_STATE_FLOW_CHECK:
			rgb[0] = RGB.YELLOW;
			rgb[1] = RGB.LIGHTSKYBLUE;
			rgb[2] = RGB.LIGHTSKYBLUE;
			rgb[3] = RGB.YELLOW;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[4] = RGB.LIGHTSKYBLUE;
			}
			else {
				rgb[4] = RGB.WHITE;
			}
			break;
			
		case SELF_TEST_STATE_WARM_UP:
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[2] = RGB.WHITE;
				rgb[3] = RGB.WHITE;
				if( pTest->WarmUpSec ){
					rgb[0] = RGB.WHITE;
					rgb[1] = RGB.WHITE;
					rgb[4] = RGB.YELLOW;
				}
				else {
					rgb[0] = RGB.YELLOW;
					rgb[1] = RGB.YELLOW;
					rgb[4] = RGB.LIGHTSKYBLUE;
				}
			}
			else {
				rgb[0] = RGB.LIGHTSKYBLUE;
				rgb[1] = RGB.LIGHTSKYBLUE;
				rgb[2] = RGB.LIGHTSKYBLUE;
				rgb[3] = RGB.LIGHTSKYBLUE;
				if( pTest->WarmUpSec ){
					rgb[4] = RGB.YELLOW;
				}
				else {
					rgb[4] = RGB.LIGHTSKYBLUE;
				}
			}
			break;
			
		case SELF_TEST_STATE_SUCCESS:
			rgb[0] = RGB.LIGHTSKYBLUE;
			rgb[1] = RGB.LIGHTSKYBLUE;
			rgb[2] = RGB.LIGHTSKYBLUE;
			rgb[3] = RGB.LIGHTSKYBLUE;
			rgb[4] = RGB.LIGHTSKYBLUE;
			break;
			
		case SELF_TEST_STATE_PREPARING_FAILURE:
			rgb[2] = RGB.WHITE;
		    rgb[3] = RGB.WHITE;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[0] = RGB.WHITE;
				rgb[1] = RGB.WHITE;
				rgb[4] = RGB.RED;
			}
			else {
				rgb[0] = RGB.RED;
				rgb[1] = RGB.RED;
				rgb[4] = RGB.WHITE;
			}
			break;
			
		case SELF_TEST_STATE_SENSOR_FAILURE:
			rgb[0] = RGB.RED;
			rgb[1] = RGB.RED;
			rgb[2] = RGB.WHITE;
		    rgb[3] = RGB.WHITE;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[4] = RGB.LIGHTSKYBLUE;
			}
			else {
				rgb[4] = RGB.WHITE;
			}
			break;
			
		case SELF_TEST_STATE_IR_SRC_FAILURE:
			rgb[0] = RGB.RED;
			rgb[1] = RGB.LIGHTSKYBLUE;
			rgb[2] = RGB.RED;
		    rgb[3] = RGB.WHITE;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[4] = RGB.LIGHTSKYBLUE;
			}
			else {
				rgb[4] = RGB.WHITE;
			}
			break;
			
		case SELF_TEST_STATE_FLOW_FAILURE:
			rgb[0] = RGB.RED;
			rgb[1] = RGB.LIGHTSKYBLUE;
			rgb[2] = RGB.LIGHTSKYBLUE;
		    rgb[3] = RGB.RED;
			if( pTest->IsWarmUpZeroEnabled == true ){
				rgb[4] = RGB.LIGHTSKYBLUE;
			}
			else {
				rgb[4] = RGB.WHITE;
			}
			break;
	}
	if( (MeasHandle.pSen->pCtg->IsPresent == false) &&
		(MeasHandle.pSen->pCvt->IsPresent == false) )
	{
		rgb[1] = RGB.DARKGRAY;
	}
	sprintf( &str[0][0], "CHECK [%03u]" , pTest->Timeout );
	sprintf( &str[1][0], "%08X" , pTest->Item.Word );
	sprintf( &str[2][0], "%5.0f mW", MeasHandle.pCur->CvtSrcPwr );
	sprintf( &str[3][0], "%5.0f mL", MeasHandle.pCur->FlowRate );
	sprintf( &str[4][0], "%6.1f  `C", MeasHandle.pCur->SysSenTemp );
	sprintf( &str[5][0], " %4u SEC", pTest->WarmUpSec );
	
	GDI_SetTextColor( rgb[0] );
	GDI_DrawHLine(  35,  47,  29 );
	GDI_DrawHLine( 156,  47,  29 );
	GDI_DrawHLine(  35, 100, 151 );
	GDI_DrawVLine(  35,  47,  54 );
	GDI_DrawVLine( 185,  47,  54 );
	GDI_SetTextColor( rgb[4] );
	GDI_DrawHLine(  35, 117,  29 );
	GDI_DrawHLine( 156, 117,  29 );
	GDI_DrawHLine(  35, 158, 151 );
	GDI_DrawVLine(  35, 117,  42 );
	GDI_DrawVLine( 185, 117,  42 );
	
	GDI_StringColor(  45,  58, GDI_STR_ALIGN_L, RGB.ORANGE,			"SENSOR:" );
	GDI_StringColor(  45,  70, GDI_STR_ALIGN_L, RGB.PINK,			"IR SRC:" );
	GDI_StringColor(  45,  82, GDI_STR_ALIGN_L, RGB.SPRINGGREEN,	"FLOW  :" );
	GDI_StringColor(  45, 129, GDI_STR_ALIGN_L, RGB.PINK,			"TEMP  :" );
	GDI_StringColor(  45, 141, GDI_STR_ALIGN_L, RGB.SPRINGGREEN,	"TIME  :" );
	
	GDI_StringColor( 110,  42, GDI_STR_ALIGN_C, rgb[0], &str[0][0] );
	GDI_StringColor( 175,  58, GDI_STR_ALIGN_R, rgb[1], &str[1][0] );
	GDI_StringColor( 175,  70, GDI_STR_ALIGN_R, rgb[2], &str[2][0] );
	GDI_StringColor( 175,  82, GDI_STR_ALIGN_R, rgb[3], &str[3][0] );
	
	GDI_StringColor( 110, 112, GDI_STR_ALIGN_C, rgb[4], "WARMING-UP" );
	GDI_StringColor( 175, 128, GDI_STR_ALIGN_R, rgb[4], &str[4][0] );
	GDI_StringColor( 175, 140, GDI_STR_ALIGN_R, rgb[4], &str[5][0] );
}


