#ifndef   __SYSTEM_INIT_H__
#define   __SYSTEM_INIT_H__


#ifdef __SYSTEM_INIT_C__
	#define SYSTEM_INIT_EXT
#else
	#define SYSTEM_INIT_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h" 
/* End Include */

/* Start Define */ 
/* End Define */

/* Start Enum */
typedef enum{
	/* 00 */ SYSTEM_INIT_DEBUG_USART,
	/* 01 */ SYSTEM_INIT_OPT_LOAD,
	/* 02 */ SYSTEM_INIT_I2C,
	/* 03 */ SYSTEM_INIT_EEPROM,
	/* 04 */ SYSTEM_INIT_POWER_CONTROL,
	/* 05 */ SYSTEM_INIT_mA_OUTPUT,
	/* 06 */ SYSTEM_INIT_RELAY,
	/* 07 */ SYSTEM_INIT_FSMC,
	/* 08 */ SYSTEM_INIT_GDI,
	/* 09 */ SYSTEM_INIT_BOR,
	/* 10 */ SYSTEM_INIT_NAND,
	/* 11 */ SYSTEM_INIT_VER_LOAD,
	/* 12 */ SYSTEM_INIT_USART,
	/* 13 */ SYSTEM_INIT_RS485,
	/* 14 */ SYSTEM_INIT_POE,
	/* 15 */ SYSTEM_INIT_ADC,
	/* 16 */ SYSTEM_INIT_RTC,
	/* 17 */ SYSTEM_INIT_TIM,
	/* 18 */ SYSTEM_INIT_SD,
	/* 19 */ SYSTEM_INIT_KEY,
	/* 20 */ SYSTEM_INIT_LED,
	/* 21 */ SYSTEM_INIT_PSEN_DPM,
	/* 22 */ SYSTEM_INIT_FLOW_CONTROL,
	/* 23 */ SYSTEM_INIT_BUZZER,
	/* 24 */ SYSTEM_INIT_SENSING,
	/* 25 */ SYSTEM_INIT_TASK,
	/* 26 */ SYSTEM_INIT_OK
}System_InitType;
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
SYSTEM_INIT_EXT System_InitType System_Init( void );
/* End Function */

/* Start Variable */                                   
/* End Variable */


#endif // __SYSTEM_INIT_H__
