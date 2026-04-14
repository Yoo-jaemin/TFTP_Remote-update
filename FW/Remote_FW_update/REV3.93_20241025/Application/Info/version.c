#define __VERSION_C__
    #include "version.h"
#undef  __VERSION_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "adc.h"
#include "option.h"
/* End Include */

/* Start Define */
#define aHardware_Version_Check_Pin								( GPIO_PIN_0 )
#define aHardware_Version_Check_GPIO_Port						( GPIOA )
#define aHardware_Version_Check_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()
#define aHardware_Version_Check_ADC1_CLK_ENABLE()				__HAL_RCC_ADC1_CLK_ENABLE()
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

bool System_Ver_Load( void )
{                                               
	static ADC_HandleTypeDef	sHandle = { 0 };
	ADC_ChannelConfTypeDef		sConfig = { 0 };
    GPIO_InitTypeDef            GPIO_InitStructure = { 0 };
	
    float           			fData;
	U16		        			i;
	bool	        			err = false;
	
	
	// ADC Initialize
	sHandle.Instance						= ADC1;
	sHandle.Init.ClockPrescaler				= ADC_CLOCK_SYNC_PCLK_DIV8;
	sHandle.Init.Resolution					= ADC_RESOLUTION_12B;
	sHandle.Init.ScanConvMode				= DISABLE;
	sHandle.Init.ContinuousConvMode			= DISABLE;
	sHandle.Init.DiscontinuousConvMode		= DISABLE;
  	sHandle.Init.NbrOfDiscConversion		= 0;
	sHandle.Init.ExternalTrigConvEdge		= ADC_EXTERNALTRIGCONVEDGE_NONE;
	sHandle.Init.ExternalTrigConv			= ADC_SOFTWARE_START;
	sHandle.Init.DataAlign					= ADC_DATAALIGN_RIGHT;
	sHandle.Init.NbrOfConversion			= 1;
	sHandle.Init.DMAContinuousRequests		= DISABLE;
	sHandle.Init.EOCSelection				= DISABLE;
	
	aHardware_Version_Check_GPIO_CLK_ENABLE();
	aHardware_Version_Check_ADC1_CLK_ENABLE(); 
	
	// ADC GPIO Initialize
	GPIO_InitStructure.Pin					= aHardware_Version_Check_Pin;
	GPIO_InitStructure.Mode					= GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull					= GPIO_NOPULL;
	
	HAL_GPIO_Init( aHardware_Version_Check_GPIO_Port, &GPIO_InitStructure );
	
	iADC.IsVersionScanMode = true;
	
	sConfig.Channel							= ADC_CHANNEL_0;
	sConfig.Rank							= 1;
	sConfig.SamplingTime					= ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset							= 0;
	
	if( HAL_ADC_ConfigChannel( &sHandle, &sConfig ) != HAL_OK ){
        Error_Handler();
	}
	
	for( fData=0, i=0; i<100; i++ ){
		
		HAL_ADC_Start( &sHandle );
		
		if( HAL_ADC_PollForConversion( &sHandle, 1000 ) == HAL_OK ){
			fData += HAL_ADC_GetValue( &sHandle );
		}
		else {
			err = true;
			break;
		}
	}
	
	if( HAL_ADC_DeInit( &sHandle ) != HAL_OK ){ err = true; }
    
    fData = fData / 100 * iADC_LSB;
    
    for( i=0; i<15; i++ ){
        if( fData < 200.f * i + 200.f ){ break; }
    }
	
	HardwareVersion = i * 0.1f;
		
	iADC.IsVersionScanMode = false;
	
	HAL_GPIO_DeInit( aHardware_Version_Check_GPIO_Port, aHardware_Version_Check_Pin );
	
	return err;
}


