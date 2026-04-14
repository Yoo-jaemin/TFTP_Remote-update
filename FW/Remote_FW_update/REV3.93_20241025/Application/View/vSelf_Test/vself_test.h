#ifndef   __VSELF_TEST_H__
#define   __VSELF_TEST_H__


#ifdef __VSELF_TEST_C__
	#define VSELF_TEST_EXT
#else
	#define VSELF_TEST_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "self_test.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
VSELF_TEST_EXT void vSelf_Test( SelfTest_HandleType *pTest );
/* End Function */

/* Start Variable */ 
/* End Variable */



#endif // __VSELF_TEST_H__
