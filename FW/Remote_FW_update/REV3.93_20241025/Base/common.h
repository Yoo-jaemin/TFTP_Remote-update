#ifndef   __COMMMON_H__
#define   __COMMMON_H__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <intrinsics.h>
                 
/* Start abbreviation */
typedef unsigned char				U8;
typedef	signed char					S8;
typedef	unsigned short				U16;
typedef	signed short				S16;
typedef	unsigned long				U32;
typedef	signed long					S32;
typedef	unsigned long long			U64;
typedef	signed long long			S64;

typedef unsigned char				bool;
/* End abbreviation */
                     
/* Start Include */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx.h"
#include "main.h"
/* End Include */

/* Start Define */
#define _DEBUG_BUILD

#define MHZ							*1000000l
#define KHZ							*1000l
#define HZ							*1l

#define abs32(x)					(x = (x > 0) ? x : (-1)*x)
#define SWAP16(A)					((((A << 8 ) & 0xFF00)) | ((A >> 8)& 0x00FF))
#define SWAP32(A)					((((A<<24)&0xFF000000)) | (((A<<8)&0x00FF0000)) | (((A>>8)&0x0000FF00)) | (((A>>24)&0x000000FF)))
#define HI_BYTE(x)					((U8)( x >> 8 & 0xFF ))
#define LO_BYTE(x)					((U8)( x      & 0xFF ))
#define MSB(x)						( (x >> 8) & (0xFF) )
#define LSB(x)						( (x >> 0) & (0xFF) )

#define	NTOHS(x)					(((x<<8) & 0xFF00) | ((x>>8) & 0x00FF))
#define	HTONS(x)					NTOHS(x)
#define	NTOHL(x)					(((x<<24) & 0xFF000000L) | ((x<<8) & 0x00FF0000L) | ((x>>8) & 0x0000FF00L) | ((x>>24) & 0x000000FFL))
#define	HTONL(x)					NTOHL(x)

#define BIT0						0x01
#define BIT1						0x02
#define BIT2						0x04
#define BIT3						0x08
#define BIT4						0x10
#define BIT5						0x20
#define BIT6						0x40
#define BIT7						0x80

#define BIT00						0x0001
#define BIT01						0x0002
#define BIT02						0x0004
#define BIT03						0x0008
#define BIT04						0x0010
#define BIT05						0x0020
#define BIT06						0x0040
#define BIT07						0x0080
#define BIT08						0x0100
#define BIT09						0x0200
#define BIT10						0x0400
#define BIT11						0x0800
#define BIT12						0x1000
#define BIT13						0x2000
#define BIT14						0x4000
#define BIT15						0x8000
            
#ifndef true  
  #define true						1
#endif

#ifndef false  
  #define false						0
#endif

#ifndef TRUE  
  #define TRUE						1
#endif

#ifndef FALSE  
  #define FALSE						0
#endif

#ifndef NULL
  #define NULL						(void *)0
#endif

#define MCU_MAX_PRIORITIES			0x00
#define MCU_MIN_PRIORITIES			0x0F
#define e(func) e##func
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union {
	float		f;
    struct{
		U16		l: 16;
        U16		h: 16;
    }w;
}ufloat;

typedef union {
	U16			w;
    struct{
		U8		l: 8;
        U8		h: 8;
    }b;
}uuword;

typedef union {
	S16			w;
    struct{
		U8		l: 8;
        U8		h: 8;
    }b;
}usword;

typedef union {
	U32			l;
    struct{
		U8		_0: 8;
        U8		_1: 8;
        U8		_2: 8;
        U8		_3: 8;
    }b;
}uulong;

typedef union {
	U8			by;
    struct{
		bool	_0: 1;
        bool	_1: 1;
        bool	_2: 1;
        bool	_3: 1;
        bool	_4: 1;
        bool	_5: 1;
        bool	_6: 1;
        bool	_7: 1;
    }bi;
}uubyte;
/* End Struct */

/* Start Function */ 
#ifdef _DEBUG_BUILD
	#define dprintf( ... )			printf( __VA_ARGS__ )
#else                                                    
	#define dprintf( ... )
#endif
/* End Function */

/* Start Variable */
/* End Variable */


#endif // __COMMMON_H__
