#define __VPASSWORD_MODE_C__
    #include "vpassword_mode.h"
#undef  __VPASSWORD_MODE_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "gdi.h"
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

void vPassword_Mode( Password_HandleType *pHandle )
{                     
	char		str[11] = "[ ** ]";
	
	if( pHandle->IsHideMode == false ){
		if( pHandle->IsMisMatch == true ){
			pHandle->IsMisMatch = false;
			
			sprintf( str, "MISMATCH!" );
			pHandle->pMsgBox->Msg[0].tClr = RGB.ORANGE;
		}
		else {
			sprintf( str, "[ %02u ]", pHandle->SetPW );
			pHandle->pMsgBox->Msg[0].tClr = RGB.YELLOW;
		}
	}
	
	pHandle->pMsgBox->Title.pStr = "ENTER P/W";
	pHandle->pMsgBox->Msg[0].pStr = str;
	pHandle->pMsgBox->LineColor = RGB.SLATEBLUE;
	pHandle->pMsgBox->TboxColor = RGB.SLATEBLUE;
	pHandle->pMsgBox->BackColor = 5;
	pHandle->pMsgBox->Width = 170;                     
	pHandle->pMsgBox->MsgLine = 1;
	pHandle->pMsgBox->OutLine = 1;
	
	vMessageBox( pHandle->pMsgBox );
}


