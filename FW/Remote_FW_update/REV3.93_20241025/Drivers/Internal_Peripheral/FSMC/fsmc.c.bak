#define __FSMC_C__
    #include "fsmc.h"
#undef  __FSMC_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "nand_flash.h"
/* End Include */

/* Start Define */
#define FSMC_nRE_Pin			GPIO_PIN_4 		// PD4		------> FSMC_NOE	( TFT & NAND )
#define FSMC_nWE_Pin			GPIO_PIN_5 		// PD5		------> FSMC_NWE	( TFT & NAND )

#define FSMC_D00_Pin			GPIO_PIN_14		// PD14		------> FSMC_D0		( TFT & NAND )
#define FSMC_D01_Pin			GPIO_PIN_15		// PD15		------> FSMC_D1		( TFT & NAND )
#define FSMC_D02_Pin			GPIO_PIN_0 		// PD0		------> FSMC_D2		( TFT & NAND )
#define FSMC_D03_Pin			GPIO_PIN_1 		// PD1		------> FSMC_D3		( TFT & NAND ) 
#define FSMC_D04_Pin			GPIO_PIN_7 		// PE7		------> FSMC_D4		( TFT & NAND )
#define FSMC_D05_Pin			GPIO_PIN_8 		// PE8		------> FSMC_D5		( TFT & NAND )
#define FSMC_D06_Pin			GPIO_PIN_9 		// PE9		------> FSMC_D6		( TFT & NAND )
#define FSMC_D07_Pin			GPIO_PIN_10		// PE10		------> FSMC_D7		( TFT & NAND )
#define FSMC_D08_Pin			GPIO_PIN_11		// PE11		------> FSMC_D8		( TFT )
#define FSMC_D09_Pin			GPIO_PIN_12		// PE12		------> FSMC_D9		( TFT )
#define FSMC_D10_Pin			GPIO_PIN_13		// PE13		------> FSMC_D10	( TFT )
#define FSMC_D11_Pin			GPIO_PIN_14		// PE14		------> FSMC_D11	( TFT )
#define FSMC_D12_Pin			GPIO_PIN_15		// PE15		------> FSMC_D12	( TFT ) 
#define FSMC_D13_Pin			GPIO_PIN_8 		// PD8		------> FSMC_D13	( TFT )
#define FSMC_D14_Pin			GPIO_PIN_9 		// PD9		------> FSMC_D14	( TFT )
#define FSMC_D15_Pin			GPIO_PIN_10		// PD10		------> FSMC_D15	( TFT )
                                                                                
#define LCD_nCS_Pin				GPIO_PIN_7 		// PD7		------> FSMC_NE1	( TFT )
#define LCD_RS_Pin				GPIO_PIN_5		// PF5		------> FSMC_A5		( TFT )
                                                                            
#define NAND_nCE_Pin			GPIO_PIN_9 		// PG9		------> FSMC_NCE3	( NAND )
#define NAND_nBUSY_Pin			GPIO_PIN_6 		// PD6		------> FSMC_NWAIT	( NAND )
#define NAND_CLE_Pin			GPIO_PIN_11		// PD11		------> FSMC_CLE	( NAND )
#define NAND_ALE_Pin			GPIO_PIN_12		// PD12		------> FSMC_ALE	( NAND )

#define FSMC_GPIOD_ALL_PIN		( FSMC_D02_Pin | FSMC_D03_Pin | FSMC_nRE_Pin | FSMC_nWE_Pin | NAND_nBUSY_Pin | LCD_nCS_Pin | FSMC_D13_Pin | FSMC_D14_Pin | FSMC_D15_Pin | NAND_CLE_Pin | NAND_ALE_Pin | FSMC_D00_Pin | FSMC_D01_Pin )
#define FSMC_GPIOE_ALL_PIN		( FSMC_D04_Pin | FSMC_D05_Pin | FSMC_D06_Pin | FSMC_D07_Pin | FSMC_D08_Pin | FSMC_D09_Pin | FSMC_D10_Pin | FSMC_D11_Pin | FSMC_D12_Pin )
#define FSMC_GPIOF_ALL_PIN		( LCD_RS_Pin )
#define FSMC_GPIOG_ALL_PIN		( NAND_nCE_Pin )

#define FSMC_SramBank1_Base		( (uint32_t)( 0x60000000 | 0x00000040 ) )
#define FSMC_SramBank1			( (LCD_CONTROLLER_TypeDef *)FSMC_SramBank1_Base )
/* End Define */
                                 
/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct{
  __IO uint16_t		REG;
  __IO uint16_t		RAM;
}LCD_CONTROLLER_TypeDef;
/* End Struct */

/* Start Function */         
static void GPIO_Init( void );
static HAL_StatusTypeDef SramBank1_Init( void );
static HAL_StatusTypeDef NandBank3_Init( void );
/* End Function */

/* Start Variable */
static SRAM_HandleTypeDef		SramBank1_Handle; // ILI9225G
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool FSMC_Init( void )
{
	bool	err = false;
	
	__HAL_RCC_FSMC_CLK_ENABLE();
	
	GPIO_Init();
	
	if( SramBank1_Init() != HAL_OK ){ err = true; }
	if( NandBank3_Init() != HAL_OK ){ err = true; }
	
	return err;
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FSMC_SramBank1_WriteData( uint16_t Data ) 
{
	/* Write 16-bit Reg */
	//FSMC_SramBank1->RAM = Data;
	*((U16*)0x60000040) = Data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FSMC_SramBank1_WriteReg( uint8_t Reg )
{
	/* Write 16-bit Index, then write register */
	//FSMC_SramBank1->REG = Reg;
	*((U16*)0x60000000) = Reg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t FSMC_SramBank1_ReadData( void )
{
	//return FSMC_SramBank1->RAM;
	return *((U16*)0x60000040);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void GPIO_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure = { 0 };  
	                           
	/** FSMC GPIO Configuration  
	PF5    ------> FSMC_A5
	PE7    ------> FSMC_D4
	PE8    ------> FSMC_D5
	PE9    ------> FSMC_D6
	PE10   ------> FSMC_D7
	PE11   ------> FSMC_D8
	PE12   ------> FSMC_D9
	PE13   ------> FSMC_D10
	PE14   ------> FSMC_D11
	PE15   ------> FSMC_D12
	PD8    ------> FSMC_D13
	PD9    ------> FSMC_D14
	PD10   ------> FSMC_D15
	PD11   ------> FSMC_CLE
	PD12   ------> FSMC_ALE
	PD14   ------> FSMC_D0
	PD15   ------> FSMC_D1
	PD0    ------> FSMC_D2
	PD1    ------> FSMC_D3
	PD4    ------> FSMC_NOE
	PD5    ------> FSMC_NWE
	PD6    ------> FSMC_NWAIT
	PD7    ------> FSMC_NE1
	PG9    ------> FSMC_NCE3
	*/
	                              
	__HAL_RCC_GPIOD_CLK_ENABLE(); 
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE(); 
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	GPIO_InitStructure.Pin 			= FSMC_GPIOD_ALL_PIN;
	GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull			= GPIO_NOPULL;
	GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate	= GPIO_AF12_FSMC;
	
	HAL_GPIO_Init( GPIOD, &GPIO_InitStructure );
	
	GPIO_InitStructure.Pin			= FSMC_GPIOE_ALL_PIN;
	GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull			= GPIO_NOPULL;
	GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate	= GPIO_AF12_FSMC;
	
	HAL_GPIO_Init( GPIOE, &GPIO_InitStructure );
	
	GPIO_InitStructure.Pin			= FSMC_GPIOF_ALL_PIN;
	GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull			= GPIO_NOPULL;
	GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate	= GPIO_AF12_FSMC;
	
	HAL_GPIO_Init( GPIOF, &GPIO_InitStructure );
	
	GPIO_InitStructure.Pin			= FSMC_GPIOG_ALL_PIN;
	GPIO_InitStructure.Mode			= GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull			= GPIO_NOPULL;
	GPIO_InitStructure.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate	= GPIO_AF12_FSMC;
	
	HAL_GPIO_Init( GPIOG, &GPIO_InitStructure );
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HAL_StatusTypeDef SramBank1_Init( void )
{
	FSMC_NORSRAM_TimingTypeDef	Timing;
	
	
	SramBank1_Handle.Instance					= FSMC_NORSRAM_DEVICE;
	SramBank1_Handle.Extended					= FSMC_NORSRAM_EXTENDED_DEVICE;
	
	SramBank1_Handle.Init.NSBank				= FSMC_NORSRAM_BANK1;
	SramBank1_Handle.Init.DataAddressMux		= FSMC_DATA_ADDRESS_MUX_DISABLE;
	SramBank1_Handle.Init.MemoryType			= FSMC_MEMORY_TYPE_SRAM;
	SramBank1_Handle.Init.MemoryDataWidth		= FSMC_NORSRAM_MEM_BUS_WIDTH_16;
	SramBank1_Handle.Init.BurstAccessMode		= FSMC_BURST_ACCESS_MODE_DISABLE;
	SramBank1_Handle.Init.WaitSignalPolarity	= FSMC_WAIT_SIGNAL_POLARITY_LOW;
	SramBank1_Handle.Init.WrapMode				= FSMC_WRAP_MODE_DISABLE;
	SramBank1_Handle.Init.WaitSignalActive		= FSMC_WAIT_TIMING_BEFORE_WS;
	SramBank1_Handle.Init.WriteOperation		= FSMC_WRITE_OPERATION_ENABLE;
	SramBank1_Handle.Init.WaitSignal			= FSMC_WAIT_SIGNAL_DISABLE;
	SramBank1_Handle.Init.ExtendedMode			= FSMC_EXTENDED_MODE_DISABLE;
	SramBank1_Handle.Init.AsynchronousWait		= FSMC_ASYNCHRONOUS_WAIT_DISABLE;
	SramBank1_Handle.Init.WriteBurst			= FSMC_WRITE_BURST_DISABLE;
	SramBank1_Handle.Init.PageSize				= FSMC_PAGE_SIZE_NONE;
	
	Timing.AddressSetupTime						= 5;//10;
	Timing.AddressHoldTime						= 2;//5;
	Timing.DataSetupTime						= 5;//10;
	Timing.BusTurnAroundDuration				= 0;//0;
	Timing.CLKDivision							= 2;
	Timing.DataLatency							= 2;
	Timing.AccessMode							= FSMC_ACCESS_MODE_A;
	
	return HAL_SRAM_Init( &SramBank1_Handle, &Timing, NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HAL_StatusTypeDef NandBank3_Init( void )
{
	FSMC_NAND_PCC_TimingTypeDef					ComSpaceTiming;
	FSMC_NAND_PCC_TimingTypeDef					AttSpaceTiming;
	
	
	NandBank3_Handle.Instance					= FSMC_NAND_DEVICE;
	
	NandBank3_Handle.Init.NandBank				= FSMC_NAND_BANK3;
	NandBank3_Handle.Init.Waitfeature			= FSMC_NAND_PCC_WAIT_FEATURE_ENABLE;
	NandBank3_Handle.Init.MemoryDataWidth		= FSMC_NAND_PCC_MEM_BUS_WIDTH_16;
	NandBank3_Handle.Init.EccComputation		= FSMC_NAND_ECC_DISABLE;
	NandBank3_Handle.Init.ECCPageSize			= FSMC_NAND_ECC_PAGE_SIZE_2048BYTE;
	NandBank3_Handle.Init.TCLRSetupTime			= 0;
	NandBank3_Handle.Init.TARSetupTime			= 0;
#if 0
	NandBank3_Handle.Config.PageSize			= 2048;
	NandBank3_Handle.Config.SpareAreaSize		= 64;
	NandBank3_Handle.Config.BlockSize			= 64;
	NandBank3_Handle.Config.PlaneSize			= 2048;
	NandBank3_Handle.Config.PlaneNbr			= 2;
	NandBank3_Handle.Config.BlockNbr			= 4096;
	NandBank3_Handle.Config.ExtraCommandEnable	= DISABLE;
#endif
	ComSpaceTiming.SetupTime					= 4;
	ComSpaceTiming.WaitSetupTime				= 6;
	ComSpaceTiming.HoldSetupTime				= 5;
	ComSpaceTiming.HiZSetupTime					= 4;
	
	AttSpaceTiming.SetupTime					= 4;
	AttSpaceTiming.WaitSetupTime				= 6;
	AttSpaceTiming.HoldSetupTime				= 5;
	AttSpaceTiming.HiZSetupTime					= 4;
	
	return HAL_NAND_Init( &NandBank3_Handle, &ComSpaceTiming, &AttSpaceTiming );
}
