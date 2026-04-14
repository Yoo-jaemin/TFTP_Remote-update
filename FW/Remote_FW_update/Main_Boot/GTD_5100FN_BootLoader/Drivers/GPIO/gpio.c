#define __GPIO_C__
    #include "gpio.h"
#undef  __GPIO_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

bool GPIO_Init(void)
{
  	bool bErr_flag = FALSE;
	
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
	  	
	// Sensor Module USART Port Select
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	IR_UPLOAD_ENABLE();
	
  	/* LED
	PE4 -- LED_FUNC
	PE5 -- LED_UP
	PF2 -- LED_RST
	PF3 -- LED_DOWN
	*/

    // PF4 -- SHDN VSEN (Sensor Power Control)
    
  	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	
		
	LED_FUNC_OFF(); LED_RST_OFF(); LED_UP_OFF(); LED_DOWN_OFF();
    SHDN_VSEN_OFF();
            
    // PG7 -- Charge LED Open Drain
    GPIO_InitStruct.Pin = GPIO_PIN_7;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	
    
    CHARGE_LED_OFF();
    
	/*TACT Switch
	PE0 -- TACT_RST
	PE1 -- TACT_DOWN
	PE2 -- TACT_UP
	PE3 -- TACT_FUNC*/
	
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    // PG6 -- Portable Model Check
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	return bErr_flag;
}

void GPIO_DeInit(void)
{
  	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);
	__HAL_RCC_GPIOB_CLK_DISABLE();
	
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_3);
	__HAL_RCC_GPIOE_CLK_DISABLE();
	
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_2);
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3);
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_4);
	__HAL_RCC_GPIOF_CLK_DISABLE();
	
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_13);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_14);
    __HAL_RCC_GPIOG_CLK_DISABLE();
}