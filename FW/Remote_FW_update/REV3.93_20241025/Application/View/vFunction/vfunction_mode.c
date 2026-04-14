#define __VFUNC_C__
    #include "vfunction_mode.h"
#undef  __VFUNC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "vimage.h"
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
static const char	*spFuncItem[] =
{                    
	"",
	"",
	"MEASURING",
	"FLOW RATE",
	"INTERFACE",
	"SYSTEM",
	"TEST",
	"FACTORY",
	"",
	""
};
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
void vFuncItemSet( Func_HandleType *pFunc )
{
	if( pFunc->ViewEvt == true ){
		
		pFunc->pMsgBox->Title.pStr = "FUNCTION MENU";
		
		pFunc->pMsgBox->Msg[0].pStr = (char *)spFuncItem[pFunc->Item.Lv1  ];
		pFunc->pMsgBox->Msg[1].pStr = (char *)spFuncItem[pFunc->Item.Lv1+1];
		pFunc->pMsgBox->Msg[2].pStr = (char *)spFuncItem[pFunc->Item.Lv1+2];
		pFunc->pMsgBox->Msg[3].pStr = (char *)spFuncItem[pFunc->Item.Lv1+3];
		pFunc->pMsgBox->Msg[4].pStr = (char *)spFuncItem[pFunc->Item.Lv1+4];
		
		pFunc->pMsgBox->Msg[2].tClr = RGB.YELLOW;
		
		if( (SystemOption.Item.IsBatMode == true) && (pFunc->Item.Lv1 < FUNC_ITEM_LV1_FACTORY) ){
			pFunc->pMsgBox->Msg[4-pFunc->Item.Lv1].tClr = RGB.DARKGRAY;
		}
		
		pFunc->pMsgBox->MsgLine = 5;
		
		vMessageBox( pFunc->pMsgBox );
	}
}


