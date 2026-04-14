#define __SPI_C__
    #include "spi.h"
#undef  __SPI_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "ADS868x.h"
#include "led.h"    
#include "ir.h"
/* End Include */

/* Start Define */
#define SPI1_SCLK_Pin               GPIO_PIN_5
#define SPI1_SDO_Pin                GPIO_PIN_6
#define SPI1_SDI_Pin                GPIO_PIN_7
#define SPI1_GPIO_Port              GPIOA
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */


/* SPI1 init function */
void SPI1_Init(void)
{
    SPI1_Handle.Instance = SPI1;
    SPI1_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI1_Handle.Init.Direction = SPI_DIRECTION_2LINES;
    SPI1_Handle.Init.DataSize = SPI_DATASIZE_16BIT;
    SPI1_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI1_Handle.Init.CLKPhase = SPI_PHASE_2EDGE;
    SPI1_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI1_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    SPI1_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI1_Handle.Init.TIMode = SPI_TIMODE_DISABLE;
    SPI1_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPI1_Handle.Init.CRCPolynomial = 10;
    
    if( HAL_SPI_Init( &SPI1_Handle ) != HAL_OK )
    {
        Error_Handler();
    }
    
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    
    /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = SPI1_SCLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPI1_GPIO_Port, &GPIO_InitStruct);
                                 
	__HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = SPI1_SDI_Pin;             
    HAL_GPIO_Init(SPI1_GPIO_Port, &GPIO_InitStruct);
                                 
	__HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = SPI1_SDO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SPI1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, SPI1_SCLK_Pin|SPI1_SDO_Pin|SPI1_SDI_Pin);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
