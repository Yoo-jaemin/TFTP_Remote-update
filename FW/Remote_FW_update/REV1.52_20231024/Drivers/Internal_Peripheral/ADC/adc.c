#define __ADC_C__
    #include "adc.h"
#undef  __ADC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "ir.h"
#include "option.h"
/* End Include */

/* Start Define */
#define aIrSourcePwr_Pin			GPIO_PIN_0
#define ADC1_GPIO_Port				GPIOA
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC1_Init( void )
{
    ADC_ChannelConfTypeDef	sConfig = { 0 };
	
    iADC1_Handle.Instance					= ADC1;
    iADC1_Handle.Init.ScanConvMode			= ADC_SCAN_ENABLE;
	iADC1_Handle.Init.NbrOfConversion		= 2;
    iADC1_Handle.Init.ContinuousConvMode	= DISABLE;
    iADC1_Handle.Init.DiscontinuousConvMode	= DISABLE;
	iADC1_Handle.Init.NbrOfDiscConversion	= 0;
    iADC1_Handle.Init.ExternalTrigConv		= ADC_SOFTWARE_START;
    iADC1_Handle.Init.DataAlign				= ADC_DATAALIGN_RIGHT;
	
    if( HAL_ADC_Init( &iADC1_Handle ) != HAL_OK ){
        Error_Handler();
    }
	
    sConfig.Channel							= ADC_CHANNEL_0;
    sConfig.Rank							= ADC_REGULAR_RANK_1;
    sConfig.SamplingTime					= ADC_SAMPLETIME_239CYCLES_5;
	
    if( HAL_ADC_ConfigChannel( &iADC1_Handle, &sConfig ) != HAL_OK ){
        Error_Handler();
    }
	
    sConfig.Channel							= ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank							= ADC_REGULAR_RANK_2;
	
    if( HAL_ADC_ConfigChannel( &iADC1_Handle, &sConfig ) != HAL_OK ){
        Error_Handler();
    }
	
    if( HAL_ADCEx_Calibration_Start( &iADC1_Handle ) != HAL_OK ){
        Error_Handler();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_MspInit( ADC_HandleTypeDef* hadc )
{

    GPIO_InitTypeDef            GPIO_InitStruct = { 0 };
    static DMA_HandleTypeDef    DmaHandle = { 0 };
    RCC_PeriphCLKInitTypeDef    PeriphClkInit = { 0 };
	
	if( SystemOption.HardwareVersionScanFlag == true ){
		return;
	}

    if( hadc->Instance == ADC1 ){
		
        __HAL_RCC_GPIOA_CLK_ENABLE();
		
        __HAL_RCC_ADC1_CLK_ENABLE();
		
        PeriphClkInit.PeriphClockSelection	= RCC_PERIPHCLK_ADC;
        PeriphClkInit.AdcClockSelection		= RCC_ADCPCLK2_DIV2;
		
        if( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit ) != HAL_OK ){
            Error_Handler();
        }
		
        __HAL_RCC_DMA1_CLK_ENABLE();
        
        GPIO_InitStruct.Pin		= aIrSourcePwr_Pin;
        GPIO_InitStruct.Mode	= GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull	= GPIO_NOPULL;
		
        HAL_GPIO_Init( ADC1_GPIO_Port, &GPIO_InitStruct );
		
        DmaHandle.Instance                  = DMA1_Channel1;
        DmaHandle.Init.Direction            = DMA_PERIPH_TO_MEMORY;
        DmaHandle.Init.PeriphInc            = DMA_PINC_DISABLE;
        DmaHandle.Init.MemInc               = DMA_MINC_ENABLE;
        DmaHandle.Init.PeriphDataAlignment  = DMA_PDATAALIGN_HALFWORD;
        DmaHandle.Init.MemDataAlignment     = DMA_MDATAALIGN_HALFWORD;
        DmaHandle.Init.Mode                 = DMA_CIRCULAR;           
        DmaHandle.Init.Priority             = DMA_PRIORITY_HIGH;
        
        if( HAL_DMA_DeInit( &DmaHandle ) != HAL_OK ){
            Error_Handler();
        }
        
        if( HAL_DMA_Init( &DmaHandle ) != HAL_OK ){
            Error_Handler();
        }
        
        __HAL_LINKDMA( hadc, DMA_Handle, DmaHandle );
		
        HAL_NVIC_SetPriority( DMA1_Channel1_IRQn, 2, 0 );
        HAL_NVIC_EnableIRQ( DMA1_Channel1_IRQn );
		
    }
    
}      

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_MspDeInit( ADC_HandleTypeDef* hadc )
{                        
	if( SystemOption.HardwareVersionScanFlag == true ){
		return;
	}
	
    if( hadc->Instance == ADC1 ){
		
        __HAL_RCC_ADC1_FORCE_RESET();
        __HAL_RCC_ADC1_RELEASE_RESET();
		
        /**ADC1 GPIO Configuration    
        PA0-WKUP     ------> ADC1_IN0
        PA1     ------> ADC1_IN1
        PA2     ------> ADC1_IN2
        PA3     ------> ADC1_IN3 
        */
        HAL_GPIO_DeInit( GPIOA, aIrSourcePwr_Pin );
		
        if( hadc->DMA_Handle != NULL ){
            HAL_DMA_DeInit( hadc->DMA_Handle );
        }
		
        HAL_NVIC_DisableIRQ( DMA1_Channel1_IRQn );
        
        HAL_NVIC_DisableIRQ( ADC1_2_IRQn );
    }
    
}    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC1_Conversion_Start( void )
{
	if( HAL_ADC_Start_DMA( &iADC1_Handle, (uint32_t *)iADC1_Raw, 2 ) != HAL_OK ){
	    Error_Handler();
	}
}    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef *hadc )
{
	U8		Hp = IrProcess.FifoHp & IR_FIFO_MASK;
	
	if( hadc->Instance == ADC1 ){
    	IrData[Hp].SrcPwrRaw  = iADC1_Raw[0];
    	IrData[Hp].McuTempRaw = iADC1_Raw[1];
		IrData[Hp].iADC_ConversionComplete = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_ConvHalfCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( hadc->Instance == ADC1 ){
	}  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_ErrorCallback( ADC_HandleTypeDef *hadc )
{
    Error_Handler();
}


