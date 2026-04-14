#define __I2C_C__
    #include "i2c.h"
#undef  __I2C_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "i2c1.h"
#include "i2c2.h"
#include "option.h"
#include "bq2579x.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
typedef enum {
	I2C_PORT_EEPROM,
	I2C_PORT_mA_DAC_PSEN_DPM_USB_PD_CHARGER
}I2C_PortType;
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
bool I2C_Init( void )
{                       
	bool	err = false;
	
	if( I2C1_Init() != HAL_OK ){ err = true; }
	if( I2C2_Init() != HAL_OK ){ err = true; }
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_I2C_MspInit( I2C_HandleTypeDef* pI2C )
{
	GPIO_TypeDef		*GpioPort;
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	I2C_PortType		I2C_Port;
	
	if		( pI2C->Instance == I2C1 ){ I2C_Port = I2C_PORT_EEPROM;							}
	else if	( pI2C->Instance == I2C2 ){ I2C_Port = I2C_PORT_mA_DAC_PSEN_DPM_USB_PD_CHARGER;	}
	                         
	/* I2C GPIO Configuration  
		PB6		------> I2C1_SCL
		PB7		------> I2C1_SDA
		PF1		------> I2C2_SCL    
		PF0		------> I2C2_SDA  
	*/
	GPIO_InitStructure.Mode		= GPIO_MODE_AF_OD;
    GPIO_InitStructure.Pull		= GPIO_NOPULL;
    GPIO_InitStructure.Speed	= GPIO_SPEED_FREQ_VERY_HIGH;
	
	switch( I2C_Port ){
		case I2C_PORT_EEPROM:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			GpioPort = GPIOB;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_6 | GPIO_PIN_7 );
			GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
			HAL_GPIO_Init( GpioPort, &GPIO_InitStructure );
			__HAL_RCC_I2C1_CLK_ENABLE();
			break;
			
		case I2C_PORT_mA_DAC_PSEN_DPM_USB_PD_CHARGER:
			__HAL_RCC_GPIOF_CLK_ENABLE();
			GpioPort = GPIOF;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_0 | GPIO_PIN_1 );
			GPIO_InitStructure.Alternate = GPIO_AF4_I2C2;
			HAL_GPIO_Init( GpioPort, &GPIO_InitStructure );
			__HAL_RCC_I2C2_CLK_ENABLE();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_I2C_MspDeInit( I2C_HandleTypeDef* pI2C )
{
	GPIO_TypeDef		*GpioPort;
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };
	I2C_PortType		I2C_Port;
	
	if		( pI2C->Instance == I2C1 ){ I2C_Port = I2C_PORT_EEPROM;							}
	else if	( pI2C->Instance == I2C2 ){ I2C_Port = I2C_PORT_mA_DAC_PSEN_DPM_USB_PD_CHARGER;	}
	
	/* I2C GPIO Configuration  
		PB6		------> I2C1_SCL
		PB7		------> I2C1_SDA
		PF1		------> I2C2_SCL    
		PF0		------> I2C2_SDA  
	*/
	switch( I2C_Port ){
		case I2C_PORT_EEPROM:
			__HAL_RCC_I2C1_CLK_DISABLE();
			GpioPort = GPIOB;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_6 | GPIO_PIN_7 );
			break;
			
		case I2C_PORT_mA_DAC_PSEN_DPM_USB_PD_CHARGER:  
    		__HAL_RCC_I2C2_CLK_DISABLE();
			GpioPort = GPIOF;
	  		GPIO_InitStructure.Pin = ( GPIO_PIN_0 | GPIO_PIN_1 );
			break;
	}
	
	HAL_GPIO_DeInit( GpioPort, GPIO_InitStructure.Pin );
}

