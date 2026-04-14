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
static IWDG_HandleTypeDef		IWDG_Handle;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IWDG_Init( void )
{
	bool	err = false;
	
	if( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET ){
		/* Clear reset flags */
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
	
	IWDG_Handle.Instance		= IWDG;
	IWDG_Handle.Init.Prescaler	= IWDG_PRESCALER_256; // LSI(32Khz) / 256 = 125Hz
	IWDG_Handle.Init.Reload		= 125;
	
	if( HAL_IWDG_Init( &IWDG_Handle ) != HAL_OK ){ err = true; }
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IWDG_Task( void )
{
	HAL_IWDG_Refresh( &IWDG_Handle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IWDG_Reload_Counter_Set( U8 sec )
{
#ifndef _DEBUG_BUILD
	uint32_t	tickstart;
	bool		terr = false;
	
	IWDG_Handle.Instance->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;
	IWDG_Handle.Instance->RLR = sec * 125;
	
	/* Check pending flag, if previous update not done, return timeout */
	tickstart = HAL_GetTick();
	
	/* Wait for register to be updated */
	while( IWDG_Handle.Instance->SR != RESET ){
		if( (HAL_GetTick() - tickstart) > 48U ){
			terr = true;
		}
	}
	
	IWDG_Handle.Instance->KR = IWDG_KEY_RELOAD;
	SystemError.Peripheral.Bit.WatchDog = terr;
#endif
}


