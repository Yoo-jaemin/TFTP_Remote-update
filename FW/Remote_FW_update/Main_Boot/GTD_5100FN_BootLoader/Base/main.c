/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "Task.h"
#include "fsmc.h"
#include "gdi.h"
#include "gpio.h"
#include "SD_Card.h"
#include "usart.h"
#include "CRC.h"

#include "IO_Ctrl.h"
#include "nand_flash.h"
#include "Sensor_Upload.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t g_Chk;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
typedef union{
  	struct {
	  	bool	bFSMC_Err		:1;
		bool	bSD_Err			:1;
		bool	bGDI_Err		:1;
		bool	bUSART_Err		:1;
		bool	bVer_Err		:1;
		bool	bCRC_Err		:1;
		bool	bGPIO_Err		:1;
		bool	bNAND_Err /*bEmpty7*/   :1;
	}Bit;
	U8	All;
}Init_Flag;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void HAL_SYSTICK_Callback(void)
{	
  	SysTick_Task();
}

void SysDeInit(void)
{	
  	CRC_DeInit();
	USART_DeInit();
	SDIO_SD_DeInit();
	GDI_DeInit();
	FSMC_DeInit();
	GPIO_DeInit();
    
	HAL_Delay(5);
	__disable_irq();
}


int main(void)
{
	static Init_Flag Init;
	
  	HAL_Init();
	
	SystemClock_Config();
	
	Init.All = FALSE;
	
	Init.Bit.bGPIO_Err = GPIO_Init();
	Init.Bit.bUSART_Err = USART_Init();
	Init.Bit.bCRC_Err = CRC_Init();
	HAL_Delay(10); // 9ms 부터 IR Sensor 응답 가능
        
        Init.Bit.bFSMC_Err = FSMC_Init();
	Init.Bit.bSD_Err  = SDIO_SD_Init();
	Init.Bit.bGDI_Err = GDI_Init();
        Init.Bit.bNAND_Err = Nand_Flash_Init();

        g_Chk = Remote_Update_Check();
        //g_Chk = 0x02;
        if (g_Chk == nMAIN_UPLOAD || g_Chk == nCT_SENSOR_UPLOAD || g_Chk == nIR_SENSOR_UPLOAD) 
        {
            Remote_SysConfig();
            Remote_Upload_Set(g_Chk);   
            Rx_Handler();
            
            while(1) 
            {
                Remote_Upload_Task(g_Chk);
            }
            
        } else printf("\r\n ==== Remote_Update fail ! ==== \r\n"); 
 
        BootKey_Check(Init.All); //Key Check
		
	printf( "\r\n GTD-5100FN Boot Loader " );
	printf( "\r\n MAIN F/W VERSION: %.1f ",  FirmwareVersion);
	if(Init.All != FALSE) printf( "\r\n Initialize Error - 0x%x ",Init.All);
	 
   	SysConfig();
    
	while (1)
  	{
            Boot_Task();
	}
	
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	
  	// Configure the main internal regulator output voltage 
  	
  	__HAL_RCC_PWR_CLK_ENABLE();
  	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  	// Initializes the CPU, AHB and APB busses clocks
	RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState			= RCC_HSE_BYPASS;
	RCC_OscInitStruct.LSIState			= RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState		= RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource		= RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM			= 25;
	RCC_OscInitStruct.PLL.PLLN			= 336;
	RCC_OscInitStruct.PLL.PLLP			= RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ			= 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  	{
	    Error_Handler();
  	}
  	
  	// Initializes the CPU, AHB and APB busses clocks 
	
  	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              	|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	
  	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  	{
	    Error_Handler();
  	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


void _Error_Handler(char *file, int line)
{	
    printf("\n  System Error: file %s on line %d\n\n", file, line );
}

/*void Error_Handler(void)
{
  USER CODE BEGIN Error_Handler_Debug 
  User can add his own implementation to report the HAL error return state 
	
  USER CODE END Error_Handler_Debug 
}*/

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
