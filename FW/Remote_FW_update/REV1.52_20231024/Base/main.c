#define __MAIN_C__
    #include "main.h"
#undef  __MAIN_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"       
#include "btimer.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim1.h"
#include "usart3.h" 
#include "usart5.h" 
#include "ads868x.h"
#include "ir.h"        
#include "led.h"        
#include "tpl0102.h"    
#include "at24c32.h"    
#include "eeprom.h"    
#include "modbus.h"    
#include "lmt84.h"  
#include "option.h" 
#include "warmup.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void SystemClock_Config( void );
/* End Function */

/* Start Variable */
SystemStateType		SystemState = SYSTEM_STATE_WARM_UP;
SystemErrorType		SystemError = { 0 }, SystemErrorBuf = { 0 };
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main( void )
{
	HAL_Init();
    
    SystemClock_Config();
	
	__enable_irq();
    
    BSP_LED_Init();
    
    UART5_Init();
    System_Option_Load();
    I2C_Init(); 
    TPL0102_Init();  
    AT24C32_Init();
    EEPROM_Init();
    ADC1_Init();
    TIM1_Init();
    USART3_Init();
    
    SPI1_Init();
    ADS868x_Init();
    Base_Timer_Init();
    IR_Init();
	MODBUS_Init();
	
#ifdef _DEBUG_BUILD
	SystemError.Bit.DebugMode =  true;
#else
	SystemError.Bit.DebugMode = false;
	MX_IWDG_Init();
#endif
    
    while( 1 )
    {
        Warm_Up_Task();
        IR_Task();
        USART3_Rx_Handler();
		EEPROM_Task();
        LED_Task();
		
#ifndef _DEBUG_BUILD
        HAL_IWDG_Refresh( &IWDG_Handle );
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void SystemClock_Config( void )
{
	RCC_OscInitTypeDef			RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef			RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef	PeriphClkInit = { 0 };
    
    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState			= RCC_HSE_BYPASS;
    RCC_OscInitStruct.HSEPredivValue	= RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState			= RCC_HSI_ON;
	RCC_OscInitStruct.LSIState			= RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState		= RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource		= RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL		= RCC_PLL_MUL9;
    
	if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK ){ Error_Handler(); }
    
    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType			= (RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource		= RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider		= RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider	= RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider	= RCC_HCLK_DIV1;
    
    if( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_2 ) != HAL_OK ){ Error_Handler(); }
	
    PeriphClkInit.PeriphClockSelection	= RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection		= RCC_ADCPCLK2_DIV6;
	
    if( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit ) != HAL_OK ){ Error_Handler(); }
	
	
    HAL_SYSTICK_Config( HAL_RCC_GetHCLKFreq() / 10000 ); // 10 KHz = 100 us
    HAL_SYSTICK_CLKSourceConfig( SYSTICK_CLKSOURCE_HCLK );
    HAL_NVIC_SetPriority( SysTick_IRQn, 0, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _Error_Handler(char *file, int line)
{
	SystemError.Bit.Driver = 1;
	printf( "\n  System Error: file %s on line %d\n\n", file, line );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
	printf( "\n  Wrong parameters value: file %s on line %d\n\n", file, line );
}

#endif /* USE_FULL_ASSERT */


