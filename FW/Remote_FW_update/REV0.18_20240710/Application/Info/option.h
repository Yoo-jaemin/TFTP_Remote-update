#ifndef   __OPTION_H__
#define   __OPTION_H__


#ifdef __OPTION_C__
	#define OPTION_EXT
#else
	#define OPTION_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	MODEL_PROTO,
	MODEL_5000N,
	MODEL_5100FN,
	MODEL_5200N,
	MODEL_5200FN,
	MODEL_5200N_DUAL,
	MODEL_5200FN_DUAL
}Model_Type;
/* End Enum */

/* Start Struct */
typedef union {
    U8  Byte;
    struct{
        Model_Type	Model		: 4;
        U8			Reserved	: 4;
    }Item;
}System_OptionType;
/* End Struct */

/* Start Function */
OPTION_EXT void System_Opt_Load( void );
/* End Function */

/* Start Variable */
OPTION_EXT System_OptionType		SystemOption;
/* End Variable */


#endif // __OPTION_H__
