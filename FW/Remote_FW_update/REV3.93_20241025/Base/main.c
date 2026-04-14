/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "power_control.h"
#include "system_init.h"
#include "system_conf.h"
#include "self_test.h"
#include "task.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void SystemClock_Config( void );
static void GPIO_Init( void );
/* End Function */

/* Start Variable */
System_StateType		SystemState = SYSTEM_STATE_INIT;
System_ErrorType		SystemError = { {0}, {0}, {0, 0} };
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main( void )
{
	GPIO_Init();
	
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	
	/* Configure the system clock */
	SystemClock_Config();
	
	__enable_irq();
	
	/* System Init */
	if( System_Init() == SYSTEM_INIT_OK ){
		
    	dprintf( "\n\n ===============================================================================================\n\n" );
		dprintf( "  GTD-5100F QUAD CHANNEL NDIR + CARTRIDGE TYPE GAS DETECTOR" );
    	dprintf( "\n\n ===============================================================================================\n\n" );
		
		System_Config();
		Enter_Self_Test_Mode();
	}
	else {
		dprintf( "  System Error: Initialize\n" );
		while( 1 );
	}
	
	while( 1 )
	{
		Main_Task();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _Error_Handler( char *file, int line )
{
	printf( "  System Error: file %s on line %d\n\n", file, line );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef  USE_FULL_ASSERT

void assert_failed( uint8_t* file, uint32_t line )
{
	printf( "  Wrong parameters value: file %s on line %d\n\n", file, line );
}

#endif /* USE_FULL_ASSERT */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void SystemClock_Config( void )
{
	RCC_OscInitTypeDef		RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef		RCC_ClkInitStruct = { 0 };
	
	__HAL_RCC_PWR_CLK_ENABLE();
	
	__HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );
	
	RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState			= RCC_HSE_BYPASS;
	RCC_OscInitStruct.LSEState			= RCC_LSE_ON;
	RCC_OscInitStruct.LSIState			= RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState		= RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource		= RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM			= 25;
	RCC_OscInitStruct.PLL.PLLN			= 336;
	RCC_OscInitStruct.PLL.PLLP			= RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ			= 7;
	
	if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK ){
		Error_Handler();
	}
	
	RCC_ClkInitStruct.ClockType			= (RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider		= RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider	= RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider	= RCC_HCLK_DIV2;
	
	if( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_5 ) != HAL_OK ){
		Error_Handler();
	}
	
	/* Configure the Systick interrupt time */
	HAL_SYSTICK_Config( HAL_RCC_GetHCLKFreq() / 1000 );
	
	/* Configure the Systick */
	HAL_SYSTICK_CLKSourceConfig( SYSTICK_CLKSOURCE_HCLK );
	
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority( SysTick_IRQn, 0, 0 );
	
	/* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
	if( HAL_GetREVID() == 0x1001 ){
		
		/* Enable the Flash prefetch */
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
    
	__HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStructure.Pin		= ( GPIO_PIN_8 | GPIO_PIN_15 );
    GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull		= GPIO_PULLDOWN;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
	
    HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );
}


