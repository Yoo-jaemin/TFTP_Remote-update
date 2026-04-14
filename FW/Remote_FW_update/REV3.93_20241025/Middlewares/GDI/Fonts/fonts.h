#ifndef   __FONTS_H__
#define   __FONTS_H__


/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define LINE(x)			( (x) * ( ((sFONT *)GDI_GetFont())->Height ) )
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	const uint8_t		*pTable;
	uint16_t			Width;
	uint16_t			Height;
}sFONT;
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */
                      
extern sFONT Fimg07x12;
extern sFONT Font05x08;
extern sFONT Font07x12;
extern sFONT Font07x24;
extern sFONT Font11x16;
extern sFONT Font29x46;

#endif /* __FONTS_H */


