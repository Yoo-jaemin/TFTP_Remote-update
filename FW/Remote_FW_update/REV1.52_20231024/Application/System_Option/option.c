#define __OPTION_C__
    #include "option.h"
#undef  __OPTION_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "adc.h"
#include "option.h" 
#include "version.h"
/* End Include */

/* Start Define */
#define SenType1_Pin											( GPIO_PIN_0 )
#define SenType2_Pin											( GPIO_PIN_1 )
#define SenType4_Pin											( GPIO_PIN_2 )
#define SenType8_Pin											( GPIO_PIN_3 )
#define DetType1_Pin											( GPIO_PIN_4 )
#define DetType2_Pin											( GPIO_PIN_5 )
#define SystemOption_GPIO_Port									( GPIOC )
#define SystemOption_GPIO_CLK_ENABLE()							__HAL_RCC_GPIOC_CLK_ENABLE()

#define aHardware_Version_Check_Pin								( GPIO_PIN_3 )
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

void System_Option_Load( void )
{                                  
	static ADC_HandleTypeDef	sHandle = { 0 };
	ADC_ChannelConfTypeDef		sConfig = { 0 };
	GPIO_InitTypeDef			GPIO_InitStructure = { 0 };
    float           			fData;
	U16		        			i, cnt;
	U8		        			dat, buf, cop;
	
    dprintf( "\n\n ====================================================================================================================================\n\n" );
    dprintf( "  GSA-970 QUAD CHANNEL NDIR SENSOR MODULE" );
    dprintf( "\n\n ====================================================================================================================================\n\n" );
              
	SystemOption.HardwareVersionScanFlag = true;
	
	aHardware_Version_Check_GPIO_CLK_ENABLE();
	aHardware_Version_Check_ADC1_CLK_ENABLE();
	
	sHandle.Instance					= ADC1;
	sHandle.Init.ScanConvMode			= ADC_SCAN_DISABLE;
	sHandle.Init.NbrOfConversion		= 1;
	sHandle.Init.ContinuousConvMode		= DISABLE;
	sHandle.Init.DiscontinuousConvMode	= DISABLE;
	sHandle.Init.NbrOfDiscConversion	= 0;
	sHandle.Init.ExternalTrigConv		= ADC_SOFTWARE_START;
	sHandle.Init.DataAlign				= ADC_DATAALIGN_RIGHT;
	    
	GPIO_InitStructure.Pin				= aHardware_Version_Check_Pin;
	GPIO_InitStructure.Mode				= GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull				= GPIO_NOPULL;
	GPIO_InitStructure.Speed			= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init( aHardware_Version_Check_GPIO_Port, &GPIO_InitStructure );
	
	if( HAL_ADC_Init( &sHandle ) != HAL_OK ){ SystemError.Bit.Init = true; }
	
	sConfig.Channel						= ADC_CHANNEL_3;
	sConfig.Rank						= ADC_REGULAR_RANK_1;
	sConfig.SamplingTime				= ADC_SAMPLETIME_239CYCLES_5;
	
	if( HAL_ADC_ConfigChannel( &sHandle, &sConfig )	!= HAL_OK ){ SystemError.Bit.Init = true; }
	if( HAL_ADCEx_Calibration_Start( &sHandle )		!= HAL_OK ){ SystemError.Bit.Init = true; }
	
	
	for( fData=0, i=0; i<100; i++ ){
		
		HAL_ADC_Start( &sHandle );
		
		if( HAL_ADC_PollForConversion( &sHandle, 1000 ) == HAL_OK ){
			fData += HAL_ADC_GetValue( &sHandle );
		}
		else {
			SystemError.Bit.Init = true;
			break;
		}
	}
	
	if( HAL_ADC_DeInit( &sHandle ) != HAL_OK ){ SystemError.Bit.Init = true; }
    
    fData = fData / 100 * iADC_LSB;
    
    for( i=0; i<13; i++ ){
		
        if( fData < 250.f * i + 375.f ){
            SystemOption.HardwereVersion = i * 0.1f;
            break;
        }
    }
	
	SystemOption.HardwareVersionScanFlag = false;
	
	HAL_GPIO_DeInit( aHardware_Version_Check_GPIO_Port, aHardware_Version_Check_Pin );
	
    dprintf( "  H/W VERSION: %3.1f, %5.3f[V]\n", SystemOption.HardwereVersion, fData*0.001f  );
    dprintf( "  F/W VERSION: %4.2f\n", FirmwareVersion );
	
	
	/* Option Load */
	
	SystemOption_GPIO_CLK_ENABLE();
	
	GPIO_InitStructure.Pin		= (SenType1_Pin | SenType2_Pin | SenType4_Pin | SenType8_Pin | DetType1_Pin | DetType2_Pin);
	GPIO_InitStructure.Mode		= GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull		= GPIO_PULLUP;
	GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init( SystemOption_GPIO_Port, &GPIO_InitStructure );
	
	TP_GPIO_CLK_ENABLE();
	
	GPIO_InitStructure.Pin		= (TP_TMR_Pin | TP_USART_Pin |TP_IR_SENSING_Pin | TP_IR_TASK_Pin);
	GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull		= GPIO_NOPULL;
	GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init( TP_GPIO_Port, &GPIO_InitStructure );
	
	
	for( i=0, cnt=0, dat=0, buf=0, cop=0; i<0xFFFF; i++ ){
		
		HAL_Delay( 10 );
		
		dat = 0;
		
        if( HAL_GPIO_ReadPin( SystemOption_GPIO_Port, SenType1_Pin ) == GPIO_PIN_RESET ){ dat |= 0x01; }
        if( HAL_GPIO_ReadPin( SystemOption_GPIO_Port, SenType2_Pin ) == GPIO_PIN_RESET ){ dat |= 0x02; }
        if( HAL_GPIO_ReadPin( SystemOption_GPIO_Port, SenType4_Pin ) == GPIO_PIN_RESET ){ dat |= 0x04; }
        if( HAL_GPIO_ReadPin( SystemOption_GPIO_Port, SenType8_Pin ) == GPIO_PIN_RESET ){ dat |= 0x08; }
        if( HAL_GPIO_ReadPin( SystemOption_GPIO_Port, DetType1_Pin ) == GPIO_PIN_RESET ){ dat |= 0x10; }
        if( HAL_GPIO_ReadPin( SystemOption_GPIO_Port, DetType2_Pin ) == GPIO_PIN_RESET ){ dat |= 0x20; }
		
		cop = dat ^ buf;
		
		if( cop ){
			cnt = 0;
		}
		else {
			cnt++;
			if( cnt == 50 ){
                SystemOption.SenType = dat & 15;
				switch( dat >> 4 & 3 ){
					case 0: SystemOption.DetType = 970; break;
					case 1: SystemOption.DetType = 972; break;
					case 2: SystemOption.DetType = 974; break;
					case 3: SystemOption.DetType = 976; break;
				}
				break;
			}
		}
        
		buf = dat;
	}
    dprintf( "  GSA-%3u SENSOR TYPE: %X\n", SystemOption.DetType, SystemOption.SenType );
	switch( SystemOption.DetType ){
		case 970:
			switch( SystemOption.SenType ){
				case SENSOR_TYPE_0	: SystemOption.Channels = 4; break; // LMM-274-X025
				case SENSOR_TYPE_1	: SystemOption.Channels = 4; break; // LMM-274-X027
				case SENSOR_TYPE_2	: SystemOption.Channels = 2; break; // LIM-272-CH
				case SENSOR_TYPE_3	: SystemOption.Channels = 2; break; // LIM-272-DH
				case SENSOR_TYPE_4	: SystemOption.Channels = 2; break; // LIM-272-IH
				case SENSOR_TYPE_5	: SystemOption.Channels = 2; break; // LIM-272-X018
				case SENSOR_TYPE_6	: SystemOption.Channels = 2; break; // LIM-272-X021
				case SENSOR_TYPE_7	: SystemOption.Channels = 2; break; // LIM-272-X054
				case SENSOR_TYPE_8	: SystemOption.Channels = 2; break; // LIM-272-X040
				case SENSOR_TYPE_9	: SystemOption.Channels = 2; break; // LIM-272-X044
				case SENSOR_TYPE_A	: SystemOption.Channels = 4; break; // LMM-274-X033
				case SENSOR_TYPE_B	: SystemOption.Channels = 4; break; // LMM-274-X040
				case SENSOR_TYPE_C	: SystemOption.Channels = 4; break; // LMM-274-X042
				case SENSOR_TYPE_D	: SystemOption.Channels = 4; break; // LMM-274-X044
				case SENSOR_TYPE_E	: SystemOption.Channels = 2; break; // LIM-272-X127
				default				: SystemOption.Channels = 0; break; // RESERVED
			}
			break;
		
		case 972:
			switch( SystemOption.SenType ){
				case SENSOR_TYPE_0	: SystemOption.Channels = 4; break; // LMM-274-X025
				case SENSOR_TYPE_1	: SystemOption.Channels = 4; break; // LMM-274-X027
				default				: SystemOption.Channels = 0; break; // RESERVED
			}
			break;
			
		default:
			SystemOption.Channels = 0;
			break;
	}
    
    dprintf( "  FILTER CHANNELS: %u\n\n", SystemOption.Channels );
}


