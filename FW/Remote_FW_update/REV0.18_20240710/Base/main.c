#define __MAIN_C__
    #include "main.h"
#undef  __MAIN_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "uart.h"
#include "flash.h"
#include "led.h"
#include "gpt.h"
#include "i2c.h"
#include "adc.h"
#include "modbus.h"
#include "option.h"
#include "version.h"
#include "measuring.h"
#include "wdt.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static void SystemClock_Config( void );
static void Variable_Init( void );
/* End Function */

/* Start Variable */
System_StateType	SystemState;
System_ErrorType	SystemError;
extern void			__vector_table;
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *  \brief Set the VTOR to point to the user firmware exception table.
 *
 *  \return None
 *
 *  \details The bootloader is already performing this step but IAR is bypassing 
 *           running the bootloader when clicking reset in the ISE. It performs a type 0
 *           reset and then sets the PC to what it thinks is the reset vector.
 *  
 *           Therefore for interrupts to work in the debugger, we need this.
 */
void __iar_init_core (void)
{
    SCB->VTOR = (uint32_t) &__vector_table;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main( void )
{
	AfeWdtGo( false );		// Disable Watchdog timer for this example
	SystemClock_Config();	// Init System clocks
	
	Variable_Init();
	
	UART_Init();
	
    dprintf( "\n\n ===============================================================================================\n\n" );
	dprintf( "  GTD-5x00(F)N CARTRIDGE" );
    dprintf( "\n\n ===============================================================================================\n\n" );
	
	FLASH_Init();
	
	System_Ver_Load();
	System_Opt_Load();
	
	LED_Init();
	GPT_Init();
	I2C_Init();
	ADC_Init();
	
	MODBUS_Init();
	Sensing_Process_Init();
	Measuring_Init();
	
#ifndef _DEBUG_BUILD
	WDT_Init();
#endif
	
	while( 1 )
	{
#if 0
		if( ADC_Handle.ConvComplete == true ){
			ADC_Handle.ConvComplete = false;
			
			dprintf(
				"  %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f"
				"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
				"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
				"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_AVDD]			) * 2,
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_DVDD]			) * 2,
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_TEMP]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_AIN0]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_AIN1]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_AIN2]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_LPTIA0_LPF]	),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VZERO0]		),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VBIAS0]		),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VCE0]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VRE0]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_LPTIA1_LPF]	),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VZERO1]		),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VBIAS1]		),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VCE1]			),
				ADC_TO_VOLT_PGA_1_5( ADC_Handle.Buf[ADMUX_VRE1]			)
			);
		}
#endif
		FLASH_Task();
		Measuring_Task();
		HS300x_Task();
		MODBUS_Task();
		
#ifndef _DEBUG_BUILD
		WDT_Refresh();
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _Error_Handler( char *file, int line )
{
	dprintf( "  System Error: file %s on line %d\n\n", file, line );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialize the Digital and AFE die clocks
// After this function:
//  - Digital die clocked via its own 26MHz oscillator.
//  - AFE die clocked by its own 16Mhz oscillator
// But, connection path enabled for AFE die clock to Digital die EXT_CLKIN path
static void SystemClock_Config( void )
{
	AfeClkSel( AFECLK_SOURCE_XTAL );
	AfeSysClkDiv( AFE_SYSCLKDIV_1 );
	
	// Internally, AFE die P2.2 is connected to Digital Die P1.10
	// This is the connection for Digital die to accept AFE die as its external clock source
	// AFE P2.2 is not bonded to an external pim 
	pADI_AGPIO2->OEN |= 4;				//  AFE_GP2.2 as an output
	DioCfgPin( pADI_GPIO1, PIN10, 2 );	// External Clock mode for Digital die P1.10
	DioIenPin( pADI_GPIO1, PIN10, 1 );	// Enable p1.10 input path
	pADI_AFECON->CLKEN1 &= 0x00FF;	//enable AFE die to output clock to Digital Die clock
	// Just to call this function to select AFE clock as digital die's EXT_CLKIN - DigClkSel(DIGCLK_SOURCE_AFE);
   
	DigClkSel( DIGCLK_SOURCE_HFOSC );
	ClkDivCfg( 1, 1 );
	
	delay_10us( 5 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Variable_Init( void )
{
	SystemState			= SYSTEM_STATE_NORMAL;
	SystemError.Word	= 0;
}


