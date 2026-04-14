#ifndef   __SPI_H__
#define   __SPI_H__

#ifdef __SPI_C__
	#define SPI_EXT
#else
	#define SPI_EXT extern
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
SPI_EXT void SPI1_Init(void);
/* End Function */

/* Start Variable */
SPI_EXT SPI_HandleTypeDef SPI1_Handle;
/* End Variable */


#endif // __SPI_H__
