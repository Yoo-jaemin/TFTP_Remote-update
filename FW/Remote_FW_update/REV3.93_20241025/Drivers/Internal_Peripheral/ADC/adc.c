#define __ADC_C__
    #include "adc.h"
#undef  __ADC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */                                  
#define aP24N_Pin				GPIO_PIN_3 // GPIOA AD-03
#define aP24P_Pin				GPIO_PIN_4 // GPIOA AD-04
#define aTempSensor_Pin			GPIO_PIN_5 // GPIOA AD-05
#define aPsenOutput				GPIO_PIN_2 // GPIOC AD-12
#define aPsenOffset				GPIO_PIN_0 // GPIOC AD-10
#define iADC_GPIO_Port_A		GPIOA
#define iADC_GPIO_Port_C		GPIOC
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
static DMA_HandleTypeDef		iADC_DMA_Handle;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool iADC_Init( void )
{                   
	bool						err = false;
	ADC_ChannelConfTypeDef		sConfig;
	
	iADC.ConversionComplete						= false;
	iADC.NoOfConversion							= iADC_MAX;
	
	iADC.Handle.Instance						= ADC1;
	iADC.Handle.Init.ClockPrescaler				= ADC_CLOCK_SYNC_PCLK_DIV8;
	iADC.Handle.Init.Resolution					= ADC_RESOLUTION_12B;
	iADC.Handle.Init.ScanConvMode				= ENABLE;
	iADC.Handle.Init.ContinuousConvMode			= DISABLE;
	iADC.Handle.Init.DiscontinuousConvMode		= DISABLE;
  	iADC.Handle.Init.NbrOfDiscConversion		= 0;
	iADC.Handle.Init.ExternalTrigConvEdge		= ADC_EXTERNALTRIGCONVEDGE_NONE;
	iADC.Handle.Init.ExternalTrigConv			= ADC_SOFTWARE_START;
	iADC.Handle.Init.DataAlign					= ADC_DATAALIGN_RIGHT;
	iADC.Handle.Init.NbrOfConversion			= iADC.NoOfConversion;
	iADC.Handle.Init.DMAContinuousRequests		= ENABLE;
	iADC.Handle.Init.EOCSelection				= DISABLE;
	
	if( HAL_ADC_Init( &iADC.Handle ) != HAL_OK ){ err = true; }
	
	sConfig.SamplingTime	= ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset			= 0;
		
	sConfig.Channel			= ADC_CHANNEL_3;
	sConfig.Rank			= iADC_VSUP_N+1;
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
		
	sConfig.Channel			= ADC_CHANNEL_4;
	sConfig.Rank			= iADC_VSUP_P+1;
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
	
	sConfig.Channel			= ADC_CHANNEL_5;
	sConfig.Rank			= iADC_LMT_TEMP+1;   
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
	
	sConfig.Channel			= ADC_CHANNEL_12;
	sConfig.Rank			= iADC_PSEN_AMPOUT+1;   
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
	
	sConfig.Channel			= ADC_CHANNEL_10;
	sConfig.Rank			= iADC_PSEN_AMPREF+1;   
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
	
	sConfig.Channel			= ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank			= iADC_MCU_TEMP+1;  
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
	
	sConfig.Channel			= ADC_CHANNEL_VBAT;
	sConfig.Rank			= iADC_VBAT+1;   
	if( HAL_ADC_ConfigChannel( &iADC.Handle, &sConfig ) != HAL_OK ){ err = true; }
	
	iADC_BatteryScan( false );
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void iADC_BatteryScan( bool use )
{
#if 1
	if( use == true ){ ADC123_COMMON->CCR |=  ADC_CCR_VBATE; }
	else			 { ADC123_COMMON->CCR &= ~ADC_CCR_VBATE; }
#else
	bool		err = false;
	
	if( HAL_ADC_Stop_DMA( &iADC.Handle ) != HAL_OK ){ err = true; }
	
	if( use == true ){
		ADC123_COMMON->CCR |= ADC_CCR_VBATE;
		iADC.NoOfConversion = iADC_MAX;
	}
	else {
		ADC123_COMMON->CCR &= ( ~ADC_CCR_VBATE) ;
		iADC.NoOfConversion = iADC_MAX - 1;
	}
	
  	iADC.Handle.Instance->SQR1 &= ~(ADC_SQR1_L);
	iADC.Handle.Instance->SQR1 |=   ADC_SQR1( iADC.NoOfConversion );
	
	iADC.ConversionComplete = true;
	
	SystemError.Peripheral.Bit.iADC = err;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

void iADC_Conversion_Start( void )
{
	bool		err = false;
	
	if( iADC.ConversionComplete == false ){
		
		if( HAL_ADC_Start_DMA( &iADC.Handle, (uint32_t *)iADC.Raw, iADC.NoOfConversion ) != HAL_OK ){ err = true; }
	}
	
	SystemError.Peripheral.Bit.iADC = err;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_MspInit( ADC_HandleTypeDef* hadc )
{      
	GPIO_InitTypeDef		GPIO_InitStructure = { 0 };
	
	if( iADC.IsVersionScanMode == true ){
		return;
	}
	
    if( hadc->Instance == ADC1 ){
  
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO clock */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		/* ADC1 Periph clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();
		/* Enable DMA2 clock */
		__HAL_RCC_DMA2_CLK_ENABLE(); 
		
		/*##-2- Configure peripheral GPIO ##########################################*/ 
		/* ADC1 Channel4, 5, 6 GPIO pin configuration */
		GPIO_InitStructure.Pin				= ( aP24N_Pin | aP24P_Pin | aTempSensor_Pin );
		GPIO_InitStructure.Mode				= GPIO_MODE_ANALOG;
		GPIO_InitStructure.Pull				= GPIO_NOPULL;
		HAL_GPIO_Init( iADC_GPIO_Port_A, &GPIO_InitStructure );
		
		GPIO_InitStructure.Pin				= ( aPsenOutput | aPsenOffset );
		HAL_GPIO_Init( iADC_GPIO_Port_C, &GPIO_InitStructure );
		
		/*##-3- Configure the DMA streams ##########################################*/
		/* Set the parameters to be configured */
		iADC_DMA_Handle.Instance					= DMA2_Stream0;
		
		iADC_DMA_Handle.Init.Channel				= DMA_CHANNEL_0;
		iADC_DMA_Handle.Init.Direction				= DMA_PERIPH_TO_MEMORY;
		iADC_DMA_Handle.Init.PeriphInc				= DMA_PINC_DISABLE;
		iADC_DMA_Handle.Init.MemInc					= DMA_MINC_ENABLE;
		iADC_DMA_Handle.Init.PeriphDataAlignment	= DMA_PDATAALIGN_HALFWORD;
		iADC_DMA_Handle.Init.MemDataAlignment		= DMA_MDATAALIGN_HALFWORD;
		iADC_DMA_Handle.Init.Mode					= DMA_CIRCULAR;
		iADC_DMA_Handle.Init.Priority				= DMA_PRIORITY_HIGH;
		iADC_DMA_Handle.Init.FIFOMode				= DMA_FIFOMODE_DISABLE;         
		iADC_DMA_Handle.Init.FIFOThreshold			= DMA_FIFO_THRESHOLD_HALFFULL;
		iADC_DMA_Handle.Init.MemBurst				= DMA_MBURST_SINGLE;
		iADC_DMA_Handle.Init.PeriphBurst			= DMA_PBURST_SINGLE; 
		
		HAL_DMA_Init( &iADC_DMA_Handle );
		  
		/* Associate the initialized DMA handle to the the ADC handle */
		__HAL_LINKDMA( hadc, DMA_Handle, iADC_DMA_Handle );
		
		/*##-4- Configure the NVIC for DMA #########################################*/
		/* NVIC configuration for DMA transfer complete interrupt */
		HAL_NVIC_SetPriority( DMA2_Stream0_IRQn, 4, 0 );   
		HAL_NVIC_EnableIRQ( DMA2_Stream0_IRQn );
	}
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_MspDeInit( ADC_HandleTypeDef* hadc )
{     
	if( iADC.IsVersionScanMode == true ){
		return;
	}

    if( hadc->Instance == ADC1 ){
  
		/*##-1- Reset peripherals ##################################################*/
		__HAL_RCC_ADC_FORCE_RESET();
		__HAL_RCC_ADC_RELEASE_RESET();
		
		/*##-2- Disable peripherals and GPIO Clocks ################################*/
		/* De-initialize the ADC1 Channel8 GPIO pin */
		HAL_GPIO_DeInit( iADC_GPIO_Port_A, aP24N_Pin | aP24P_Pin | aTempSensor_Pin );
		HAL_GPIO_DeInit( iADC_GPIO_Port_C, aPsenOutput | aPsenOffset );
		
		/*##-3- Disable the DMA Streams ############################################*/
		/* De-Initialize the DMA Stream associate to transmission process */
		HAL_DMA_DeInit( &iADC_DMA_Handle ); 
		  
		/*##-4- Disable the NVIC for DMA ###########################################*/
		HAL_NVIC_DisableIRQ( DMA2_Stream0_IRQn );
    }
    
}     

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef *hadc )
{
	if( hadc->Instance == ADC1 ){
		iADC.ConversionComplete = true; 
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
    /* In case of ADC error, call main error handler */
    Error_Handler();
}
