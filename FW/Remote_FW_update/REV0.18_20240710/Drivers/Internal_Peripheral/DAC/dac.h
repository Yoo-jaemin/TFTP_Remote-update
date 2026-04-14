#ifndef   __DAC_H__
#define   __DAC_H__


#ifdef __DAC_C__
	#define DAC_EXT
#else
	#define DAC_EXT extern
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
typedef struct {
	U16		VBIAS; // mV
	U16		VZERO; // mV
}LPDAC_ConfType;
/* End Struct */

/* Start Function */
DAC_EXT void LPDAC_Config( U8 Channel, LPDAC_ConfType *pDac );
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __DAC_H__
