#define __BSP_SD_C__
    #include "bsp_sd.h"
#undef  __BSP_SD_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
/* End Include */

/* Start Define */    
#define SD_DETECT_PIN					( GPIO_PIN_8 )
#define SD_DETECT_GPIO_PORT				( GPIOG )
/* End Define */
	
/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */ 
void BSP_SD_MspInit( SD_HandleTypeDef *hsd, void *Params );
void BSP_SD_Detect_MspInit( SD_HandleTypeDef *hsd, void *Params );
void BSP_SD_MspDeInit( SD_HandleTypeDef *hsd, void *Params );
void BSP_SD_AbortCallback( void );
void BSP_SD_WriteCpltCallback( void );
void BSP_SD_ReadCpltCallback( void );
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t BSP_SD_Init( void )
{
	uint8_t		SD_state = MSD_OK;
	
	/* uSD device interface configuration */
	BspSdHandle.Instance = SDIO;
	
	BspSdHandle.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
	BspSdHandle.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
	BspSdHandle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
	BspSdHandle.Init.BusWide             = SDIO_BUS_WIDE_1B;
	BspSdHandle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	BspSdHandle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;
	
	/* Msp SD initialization */
	BSP_SD_MspInit( &BspSdHandle, NULL );
	
	if( HAL_SD_Init( &BspSdHandle ) != HAL_OK ){
		SD_state = MSD_ERROR;
	}
	
	return  SD_state;
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @retval Returns if SD is detected or not
  */
uint8_t BSP_SD_IsDetected( void )
{
	__IO uint8_t status = SD_PRESENT;
	
	/* Check SD card detect pin */
	if( HAL_GPIO_ReadPin( SD_DETECT_GPIO_PORT, SD_DETECT_PIN ) != GPIO_PIN_RESET ) 
	{
		status = SD_NOT_PRESENT;
	}
	
	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @param  Timeout: Timeout for read operation
  * @retval SD status
  */

uint8_t BSP_SD_ReadBlocks( uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout )
{
	if( HAL_SD_ReadBlocks( &BspSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout ) != HAL_OK ){
		return MSD_ERROR;
	}
	else {
		return MSD_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: Timeout for write operation
  * @retval SD status
  */

uint8_t BSP_SD_WriteBlocks( uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout )
{
	if( HAL_SD_WriteBlocks( &BspSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout ) != HAL_OK ){
		return MSD_ERROR;
	}
	else {
		return MSD_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */

uint8_t BSP_SD_ReadBlocks_DMA( uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks )
{  
	/* Read block(s) in DMA transfer mode */
	if( HAL_SD_ReadBlocks_DMA( &BspSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks ) != HAL_OK ){
		return MSD_ERROR;
	}
	else {
		return MSD_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write 
  * @retval SD status
  */

uint8_t BSP_SD_WriteBlocks_DMA( uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks )
{ 
	/* Write block(s) in DMA transfer mode */
	if( HAL_SD_WriteBlocks_DMA( &BspSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks ) != HAL_OK ){
		return MSD_ERROR;
	}
	else {
		return MSD_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Erases the specified memory area of the given SD card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */

uint8_t BSP_SD_Erase( uint32_t StartAddr, uint32_t EndAddr )
{
	if( HAL_SD_Erase( &BspSdHandle, StartAddr, EndAddr ) != HAL_OK ){
		return MSD_ERROR;
	}
	else {
		return MSD_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  Initializes the SD MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */

__weak void BSP_SD_MspInit( SD_HandleTypeDef *hsd, void *Params )
{
	static DMA_HandleTypeDef	dmaRxHandle = { 0 };
	static DMA_HandleTypeDef	dmaTxHandle = { 0 };
	GPIO_InitTypeDef			GPIO_Init_Structure = { 0 };
	
	/* Enable SDIO clock */
	__HAL_RCC_SDIO_CLK_ENABLE();
	
	/* Enable DMA2 clocks */
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	/* Enable GPIOs clock */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	/* Common GPIO configuration */
	GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init_Structure.Pull      = GPIO_PULLUP;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Alternate = GPIO_AF12_SDIO;
	
	/* GPIOC configuration */
	GPIO_Init_Structure.Pin = ( GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 );
	HAL_GPIO_Init( GPIOC, &GPIO_Init_Structure );
	
	/* GPIOD configuration */
	GPIO_Init_Structure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init( GPIOD, &GPIO_Init_Structure );
	
	/* SD Card detect pin configuration */
	GPIO_Init_Structure.Mode      = GPIO_MODE_INPUT;
	GPIO_Init_Structure.Pull      = GPIO_PULLUP;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Pin       = SD_DETECT_PIN;
	HAL_GPIO_Init( SD_DETECT_GPIO_PORT, &GPIO_Init_Structure );
	  
	/* NVIC configuration for SDIO interrupts */
	HAL_NVIC_SetPriority( SDIO_IRQn, 0x0E, 0 );
	HAL_NVIC_EnableIRQ( SDIO_IRQn );
	  
	/* Configure DMA Rx parameters */
	dmaRxHandle.Init.Channel             = DMA_CHANNEL_4;
	dmaRxHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	dmaRxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaRxHandle.Init.MemInc              = DMA_MINC_ENABLE;
	dmaRxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaRxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	dmaRxHandle.Init.Mode                = DMA_PFCTRL;
	dmaRxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	dmaRxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	dmaRxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	dmaRxHandle.Init.MemBurst            = DMA_MBURST_INC4;
	dmaRxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
	
	dmaRxHandle.Instance = DMA2_Stream3;
	
	/* Associate the DMA handle */
	__HAL_LINKDMA( hsd, hdmarx, dmaRxHandle );
	
	/* Deinitialize the stream for new transfer */
	HAL_DMA_DeInit( &dmaRxHandle );
	
	/* Configure the DMA stream */
	HAL_DMA_Init( &dmaRxHandle );
	
	/* Configure DMA Tx parameters */
	dmaTxHandle.Init.Channel             = DMA_CHANNEL_4;
	dmaTxHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	dmaTxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaTxHandle.Init.MemInc              = DMA_MINC_ENABLE;
	dmaTxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaTxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	dmaTxHandle.Init.Mode                = DMA_PFCTRL;
	dmaTxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	dmaTxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	dmaTxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	dmaTxHandle.Init.MemBurst            = DMA_MBURST_INC4;
	dmaTxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
	
	dmaTxHandle.Instance = DMA2_Stream6;
	
	/* Associate the DMA handle */
	__HAL_LINKDMA( hsd, hdmatx, dmaTxHandle );
	
	/* Deinitialize the stream for new transfer */
	HAL_DMA_DeInit( &dmaTxHandle );
	
	/* Configure the DMA stream */
	HAL_DMA_Init( &dmaTxHandle ); 
	
	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_SetPriority( DMA2_Stream3_IRQn, 0x0F, 0 );
	HAL_NVIC_EnableIRQ( DMA2_Stream3_IRQn );
	
	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_SetPriority( DMA2_Stream6_IRQn, 0x0F, 0 );
	HAL_NVIC_EnableIRQ( DMA2_Stream6_IRQn );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t BSP_SD_GetCardState( void )
{
	return ( ( HAL_SD_GetCardState( &BspSdHandle ) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BSP_SD_GetCardInfo( HAL_SD_CardInfoTypeDef *CardInfo )
{
	/* Get SD card Information */
	HAL_SD_GetCardInfo( &BspSdHandle, CardInfo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_SD_AbortCallback( SD_HandleTypeDef *hsd )
{
	BSP_SD_AbortCallback();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_SD_TxCpltCallback( SD_HandleTypeDef *hsd )
{
	BSP_SD_WriteCpltCallback();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_SD_RxCpltCallback( SD_HandleTypeDef *hsd )
{
	BSP_SD_ReadCpltCallback();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__weak void BSP_SD_AbortCallback( void )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__weak void BSP_SD_WriteCpltCallback( void )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__weak void BSP_SD_ReadCpltCallback( void )
{
	
}
