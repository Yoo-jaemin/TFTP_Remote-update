#ifndef   __NAND_FLASH_H__
#define   __NAND_FLASH_H__


#ifdef __NAND_FLASH_C__
	#define NAND_FLASH_EXT
#else
	#define NAND_FLASH_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"  
/* End Include */
  
/* Start Define */
#define NAND_PAGE_NUMBER                64
#define NAND_TOTAL_BLOCK_NUMBER         4096   // Block
#define NAND_PLANE_NUMBER               2                  

#define NAND_PAGE_SIZE                  2048   // 2,048 bytes 

#define TFTP_BLOCK_SIZE                 512    // byte              
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */

typedef union {
    uint8_t byte[NAND_PAGE_SIZE];    
    
      struct {
          uint8_t OddBlock[TFTP_BLOCK_SIZE];
          uint8_t EvenBlock[TFTP_BLOCK_SIZE];
          uint32_t pageByte;
          uint32_t totByte;
          uint8_t dummy[ NAND_PAGE_SIZE - (TFTP_BLOCK_SIZE*2) - (2*sizeof(uint32_t)) ];
      };
} u_Nand_Page;


/* End Struct */

/* Start Function */
NAND_FLASH_EXT bool Nand_Flash_Init( void );

void Remote_Update_Check(bool mainFlag, bool CtFlag, bool IrFlag);

U8 NAND_Erase_Block(U16 addr_page, U16 addr_block, U16 addr_plane);
U8 NAND_Read_Page_8bit(U16 addr_page, U16 addr_block, U16 addr_plane, U8 *read_Buff);
U8 NAND_Write_Page_8bit(U16 addr_page, U16 addr_block, U16 addr_plane, U8 *write_buff);
/* End Function */


/* Start Variable */                             
NAND_FLASH_EXT NAND_HandleTypeDef		NandBank3_Handle;
NAND_FLASH_EXT NAND_IDTypeDef			NandID;
NAND_FLASH_EXT NAND_AddressTypeDef              NandAddr;
/* End Variable */


#endif // __NAND_H__

