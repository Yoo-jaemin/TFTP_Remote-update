#ifndef   __HTTPD_PARSING_H__
#define   __HTTPD_PARSING_H__


#ifdef __HTTPD_PARSING_C__
	#define HTTPD_PARSING_EXT
#else
	#define HTTPD_PARSING_EXT extern
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
HTTPD_PARSING_EXT const char *Httpd_Parsing( const unsigned char *pName, const unsigned char *pData, int Len );
/* End Function */

/* Start Variable */
extern U8		iParameter;
extern U8		iAlarm;
extern U8		iTest;
/* End Variable */


#endif // __HTTPD_PARSING_H__
