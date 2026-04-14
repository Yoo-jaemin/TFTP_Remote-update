#ifndef   __I2C1_H__
#define   __I2C1_H__


#ifdef __I2C1_C__
	#define I2C1_EXT
#else
	#define I2C1_EXT extern
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
I2C1_EXT HAL_StatusTypeDef I2C1_Init( void );
I2C1_EXT bool I2C1_Mem_16Bit_Write( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size );
I2C1_EXT bool I2C1_Mem_16Bit_Read ( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size );
/* End Function */

/* Start Variable */
I2C1_EXT I2C_HandleTypeDef		I2C1_Handle;
/* End Variable */


#endif // __I2C1_H__
