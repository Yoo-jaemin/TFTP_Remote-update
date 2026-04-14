#ifndef   __I2C2_H__
#define   __I2C2_H__


#ifdef __I2C2_C__
	#define I2C2_EXT
#else
	#define I2C2_EXT extern
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
I2C2_EXT HAL_StatusTypeDef I2C2_Init( void );
I2C2_EXT bool I2C2_Write( uint16_t DevAddr, uint8_t *pData, uint16_t Size );
I2C2_EXT bool I2C2_Bulk_Write( uint16_t DevAddr, uint8_t *pData, uint16_t Size );
I2C2_EXT bool I2C2_Read( uint16_t DevAddr, uint8_t *pData, uint16_t Size );
I2C2_EXT bool I2C2_Mem_8Bit_Write( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size );
I2C2_EXT bool I2C2_Mem_8Bit_Read( uint16_t DevAddr, uint16_t MemAddr, uint8_t *pData, uint16_t Size );
/* End Function */

/* Start Variable */
I2C2_EXT I2C_HandleTypeDef		I2C2_Handle;
/* End Variable */


#endif // __I2C2_H__


