#define __BOR_C__
    #include "bor.h"
#undef  __BOR_C__

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
static FLASH_OBProgramInitTypeDef		BorInit;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BOR_Init( void )
{
	bool	err = true;
	
	HAL_FLASHEx_OBGetConfig( &BorInit );
	
	if( BorInit.BORLevel != OB_BOR_LEVEL1 ){
		BorInit.BORLevel  = OB_BOR_LEVEL1;
		
		BorInit.OptionType = OPTIONBYTE_BOR;
		
		if( HAL_FLASH_OB_Unlock() == HAL_OK ){
			if( HAL_FLASHEx_OBProgram( &BorInit ) == HAL_OK ){
				if( HAL_FLASH_OB_Launch() == HAL_OK ){
					if( HAL_FLASH_OB_Lock() == HAL_OK ){
						err = false;
					}
				}
			}
		}
	}
	else {
		err = false;
	}
	
	return err;
}
