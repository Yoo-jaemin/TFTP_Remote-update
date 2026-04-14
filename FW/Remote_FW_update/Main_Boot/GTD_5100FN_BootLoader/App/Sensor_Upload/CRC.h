#ifndef   __CRC_H__
#define   __CRC_H__

#ifdef __CRC_C__
	#define CRC_EXT
#else
	#define CRC_EXT extern
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
typedef struct{
  	CRC_HandleTypeDef 	Hal_CRC;
  	__IO uint32_t 		uwCRCValue;
  	U32					dwData[MAX_BUFF];
  	U16 				uData_size;
}CRC_HandleType;
/* End Struct */

/* Start Function */
CRC_EXT U8 CRC_Init(void);
CRC_EXT U32	CRC_Calculate(U8 *pBuff, U16 dwLen);
CRC_EXT void CRC_DeInit(void);
/* End Function */

/* Start Variable */
CRC_EXT CRC_HandleType CRC_Handle;
/* End Variable */


#endif // __CRC_H__