#ifndef   __BSP_SD_H__
#define   __BSP_SD_H__


#ifdef __BSP_SD_C__
	#define BSP_SD_EXT
#else
	#define BSP_SD_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
/* End Include */

/* Start Define */
#define BSP_SD_CardInfo						HAL_SD_CardInfoTypeDef
#define MSD_OK								((uint8_t)0x00)
#define MSD_ERROR							((uint8_t)0x01) 
#define SD_TRANSFER_OK						((uint8_t)0x00)
#define SD_TRANSFER_BUSY					((uint8_t)0x01)

#define SD_DATATIMEOUT						((uint32_t)100000000)

#define SD_PRESENT               			((uint8_t)0x01)
#define SD_NOT_PRESENT           			((uint8_t)0x00)
   
/* DMA definitions for SD DMA transfer */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
BSP_SD_EXT uint8_t BSP_SD_Init( void );
BSP_SD_EXT uint8_t BSP_SD_IsDetected( void );
BSP_SD_EXT uint8_t BSP_SD_ReadBlocks( uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout );
BSP_SD_EXT uint8_t BSP_SD_WriteBlocks( uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout );
BSP_SD_EXT uint8_t BSP_SD_ReadBlocks_DMA( uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks );
BSP_SD_EXT uint8_t BSP_SD_WriteBlocks_DMA( uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks );
BSP_SD_EXT uint8_t BSP_SD_Erase( uint32_t StartAddr, uint32_t EndAddr );
BSP_SD_EXT uint8_t BSP_SD_GetCardState( void );
BSP_SD_EXT void BSP_SD_GetCardInfo( HAL_SD_CardInfoTypeDef *CardInfo );
BSP_SD_EXT uint8_t BSP_SD_IsDetected( void );

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
#if 0
BSP_SD_EXT void BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params);
BSP_SD_EXT void BSP_SD_Detect_MspInit(SD_HandleTypeDef *hsd, void *Params);
BSP_SD_EXT void BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params);
BSP_SD_EXT void BSP_SD_AbortCallback(void);
BSP_SD_EXT void BSP_SD_WriteCpltCallback(void);
BSP_SD_EXT void BSP_SD_ReadCpltCallback(void);
#endif
/* End Function */

/* Start Variable */
BSP_SD_EXT SD_HandleTypeDef		BspSdHandle;
/* End Variable */


#endif // __BSP_SD_H__


