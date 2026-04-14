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
#define NAND_PAGE_SIZE          2048                // 2,048 bytes 
#define NAND_BLOCK_SIZE         131072              // 2048 * 64 Page  
#define NAND_PLANE_SIZE         268435456           // 2048 Block * 131072
#define NAND_PLANE_NUMBER       2                   // 2 Plene * 268435456
#define NAND_TOTAL_BLOCK_NUMBER 4096                // Block
#define NAND_REDUNDANT_SIZE     64                  // Bytes    

#define NAND_PAGE_NUMBER        64

#define TFTP_BLOCK_SIZE         512     // byte



/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
#if 0

typedef union {
    uint8_t   byte[NAND_PAGE_SIZE];
    
    struct {
      uint8_t   block[TFTP_BLOCK_SIZE];
      uint32_t  blk_len;
      uint32_t  tot_byte;
      uint32_t  crc;
      uint8_t   dummy[ NAND_PAGE_SIZE - TFTP_BLOCK_SIZE - (3 * sizeof(uint32_t)) ];
    };
    
} u_Nand_Page;

#else

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


#endif


typedef struct {
  bool bMain_Remote_upload;
  bool bFile_chk;
  
  bool bMain_UpdateChk_flg;
  bool bCt_UpdateChk_flg;
  bool bIr_UpdateChk_flg;
  
  uint32_t totByte;
  uint32_t program_cnt;
  uint32_t program_tail;
  
  
  
} s_NandFlash;


/* End Struct */

/* Start Function */
NAND_FLASH_EXT bool Nand_Flash_Init( void );

uint8_t Remote_Update_Check(void);


U8 NAND_Erase_Block(U16 addr_page, U16 addr_block, U16 addr_plane);
U8 NAND_Read_Page_8bit(U16 addr_page, U16 addr_block, U16 addr_plane, U8 *read_Buff);
U8 NAND_Write_Page_8bit(U16 addr_page, U16 addr_block, U16 addr_plane, U8 *write_buff);



void Remote_Download_File_Find(void);
void Remote_FilePageLoad(void);



/* End Function */

/* Start Variable */                             
NAND_FLASH_EXT NAND_HandleTypeDef		NandBank3_Handle;
NAND_FLASH_EXT NAND_IDTypeDef			NandID;
NAND_FLASH_EXT NAND_AddressTypeDef              NandAddr;

/* End Variable */


#endif // __NAND_H__

