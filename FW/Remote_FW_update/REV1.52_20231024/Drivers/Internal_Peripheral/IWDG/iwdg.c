#define __IWDG_C__
    #include "iwdg.h"
#undef  __IWDG_C__

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

void MX_IWDG_Init(void)
{
	
#if 0
	if( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET ){
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
#endif
	IWDG_Handle.Instance		= IWDG;
	IWDG_Handle.Init.Prescaler	= IWDG_PRESCALER_32; // 40KHz / 32 = 1.25KHz
	IWDG_Handle.Init.Reload		= 2500; // 1 / 1.25KHz * 2500 = 2sec.
	
	if( HAL_IWDG_Init( &IWDG_Handle ) != HAL_OK ){ Error_Handler(); }

}
