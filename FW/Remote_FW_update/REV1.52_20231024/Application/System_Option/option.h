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
#include "ir.h"
/* End Include */

/* Start Define */
#define TP_TMR_Pin						GPIO_PIN_15
#define TP_USART_Pin					GPIO_PIN_14
#define TP_IR_SENSING_Pin				GPIO_PIN_13
#define TP_IR_TASK_Pin					GPIO_PIN_12 
#define TP_GPIO_Port					GPIOB
#define TP_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()

#ifdef _DEBUG_BUILD
#define TP_TMR_H()						HAL_GPIO_WritePin( TP_GPIO_Port, TP_TMR_Pin, GPIO_PIN_SET )
#define TP_TMR_L()						HAL_GPIO_WritePin( TP_GPIO_Port, TP_TMR_Pin, GPIO_PIN_RESET )
#define TP_TMR_T()						HAL_GPIO_TogglePin( TP_GPIO_Port, TP_TMR_Pin )
#define TP_USART_H()					HAL_GPIO_WritePin( TP_GPIO_Port, TP_USART_Pin, GPIO_PIN_SET )
#define TP_USART_L()					HAL_GPIO_WritePin( TP_GPIO_Port, TP_USART_Pin, GPIO_PIN_RESET )
#define TP_IR_SENSING_H()				HAL_GPIO_WritePin( TP_GPIO_Port, TP_IR_SENSING_Pin, GPIO_PIN_SET )
#define TP_IR_SENSING_L()				HAL_GPIO_WritePin( TP_GPIO_Port, TP_IR_SENSING_Pin, GPIO_PIN_RESET )
#define TP_IR_SENSING_T()				HAL_GPIO_TogglePin( TP_GPIO_Port, TP_IR_SENSING_Pin )
#define TP_IR_TASK_H()					HAL_GPIO_WritePin( TP_GPIO_Port, TP_IR_TASK_Pin, GPIO_PIN_SET )
#define TP_IR_TASK_L()					HAL_GPIO_WritePin( TP_GPIO_Port, TP_IR_TASK_Pin, GPIO_PIN_RESET )
#else
#define TP_TMR_H()
#define TP_TMR_L()
#define TP_TMR_T()
#define TP_USART_H()
#define TP_USART_L()
#define TP_IR_SENSING_H()
#define TP_IR_SENSING_L()
#define TP_IR_SENSING_T()
#define TP_IR_TASK_H()
#define TP_IR_TASK_L()
#endif
/* End Define */

/* Start Enum */
enum {
	SENSOR_TYPE_0, // LMM-274-X025
	SENSOR_TYPE_1, // LMM-274-X027
	SENSOR_TYPE_2, // LIM-272-CH
	SENSOR_TYPE_3, // LIM-272-DH
	SENSOR_TYPE_4, // LIM-272-IH
	SENSOR_TYPE_5, // LIM-272-X018
	SENSOR_TYPE_6, // LIM-272-X021
	SENSOR_TYPE_7, // LIM-272-X054
	SENSOR_TYPE_8, // LIM-272-X040
	SENSOR_TYPE_9, // LIM-272-X044
	SENSOR_TYPE_A, // LMM-274-X033
	SENSOR_TYPE_B, // LMM-274-X040
	SENSOR_TYPE_C, // LMM-274-X042
	SENSOR_TYPE_D, // LMM-274-X044
	SENSOR_TYPE_E, // LIM-272-X127
	SENSOR_TYPE_F, // RESERVED
};
/* End Enum */

/* Start Struct */
typedef struct {
	U8					SenType;
	U16					DetType;
	U8					Channels;
	float				HardwereVersion;
	bool				HardwareVersionScanFlag;
}System_OptionType;
/* End Struct */

/* Start Function */
OPTION_EXT void System_Option_Load( void );
/* End Function */

/* Start Variable */
OPTION_EXT System_OptionType		SystemOption;

/* End Variable */


#endif // __OPTION_H__
