#ifndef   __FSMC_H__
#define   __FSMC_H__


#ifdef __FSMC_C__
	#define FSMC_EXT
#else
	#define FSMC_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */

/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
FSMC_EXT bool FSMC_Init( void );
FSMC_EXT void FSMC_SramBank1_WriteData( uint16_t Data );
FSMC_EXT void FSMC_SramBank1_WriteReg( uint8_t Reg );
FSMC_EXT uint16_t FSMC_SramBank1_ReadData( void );
/* End Function */

/* Start Variable */ 
/* End Variable */


#endif // __FSMC_H__

