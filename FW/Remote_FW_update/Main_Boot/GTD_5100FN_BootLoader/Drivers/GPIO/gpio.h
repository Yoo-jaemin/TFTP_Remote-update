#ifndef   __GPIO_H__
#define   __GPIO_H__


#ifdef __GPIO_C__
	#define GPIO_EXT
#else
	#define GPIO_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
//LED Control  
#define LED_FUNC_ON()   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4 , GPIO_PIN_RESET)
#define LED_FUNC_OFF()  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4 , GPIO_PIN_SET)
#define LED_UP_ON()     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5 , GPIO_PIN_RESET)
#define LED_UP_OFF()    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5 , GPIO_PIN_SET)
#define LED_RST_ON()    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2 , GPIO_PIN_RESET)
#define LED_RST_OFF()   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2 , GPIO_PIN_SET)
#define LED_DOWN_ON()   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3 , GPIO_PIN_RESET)
#define LED_DOWN_OFF()  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3 , GPIO_PIN_SET)

//Tact Swtich
#define TECT_FUNC_READ()    HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)
#define TECT_RST_READ()     HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2)
#define TECT_UP_READ()      HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0)
#define TECT_DOWN_READ()    HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1)

//Cartridge & IR-Cavity Select
#define CT_UPLOAD_ENABLE()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14 , GPIO_PIN_SET)
#define IR_UPLOAD_ENABLE()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14 , GPIO_PIN_RESET)

//Portable Model Control
#define PORTABLE_MODEL_CHECK()  HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6)
#define CHARGE_LED_OFF()        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7 , GPIO_PIN_SET)
#define CHARGE_LED_ON()         HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7 , GPIO_PIN_RESET)
#define SHDN_VSEN_OFF()         HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4 , GPIO_PIN_RESET)
#define SHDN_VSEN_ON()          HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4 , GPIO_PIN_SET)


/* End Define */

/* Start Enum */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
GPIO_EXT bool GPIO_Init(void);
GPIO_EXT void GPIO_DeInit(void);
/* End Variable */


#endif // __GPIO_H__
