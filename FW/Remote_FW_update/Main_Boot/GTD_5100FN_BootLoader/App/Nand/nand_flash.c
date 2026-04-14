#define __NAND_FLASH_C__
    #include "nand_flash.h"
#undef  __NAND_FLASH_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "fsmc.h"


#include "CRC.h"
#include "Display.h"
#include "gdi.h"

#include "Task.h"
#include "IO_Ctrl.h"

/* End Include */

/* Start Define */
#define NAND_FLASH_nWP_Pin			        GPIO_PIN_3
#define NAND_FLASH_nWP_Gpio_Port			GPIOD



/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */

/* End Function */
                                     
/* Start Variable */

u_Nand_Page nand;
extern s_NandFlash Nand_flash;
extern uint8_t g_Chk;
/* End Variable */                                                                                                  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Nand_Flash_Init( void )
{
	bool	err = true;
	
	if( HAL_NAND_Read_ID( &NandBank3_Handle, &NandID ) == HAL_OK ) {
		
		if( ((NandID.Maker_Id == 0xEC) && (NandID.Device_Id == 0xDC) && (NandID.Third_Id == 0x10) && (NandID.Fourth_Id == 0x95)) ||
			((NandID.Maker_Id == 0x01) && (NandID.Device_Id == 0xDC) && (NandID.Third_Id == 0x90) && (NandID.Fourth_Id == 0x95)) ){
#if 0
				dprintf( "  Manufacturer                              => SAMSUNG\n" );
				dprintf( "  Part No.                                  => K9F4G08U0F\n\n" );
				
				dprintf( "  Internal Chip Number                      => 1\n" );
				dprintf( "  Cell Type                                 => 2 Level Cell\n" );
				dprintf( "  Number of Simultaneously Programmed Pages => 2\n" );
				dprintf( "  Interleave Program Between multiple chips => Not Support\n" );
				dprintf( "  Cache Program                             => Not Support\n\n" );
				
				dprintf( "  Page Size                                 => 2,048 bytes\n" );
				dprintf( "  Redundant Area Size                       => 64 bytes / 1 page (2,048 bytes)\n" );
				dprintf( "  Block Size                                => 64 pages (2,048 * 64 = 131,072 bytes)\n" );
				dprintf( "  Plane Size                                => 2,048 blocks (131,072 * 2,048 = 268,435,456 bytes)\n" );
				dprintf( "  Plane Number                              => 2 planes (268,435,456 * 2 = 536,870,912 bytes)\n\n" );
				
				dprintf( "  Total Block Number                        => 4,096 blocks\n\n" );
				
				dprintf( "  Organization                              => x8\n\r" );
				dprintf( "  Serial Access Minimum                     => 25 nSec.\n\n" );
				
				dprintf( "  Process                                   => 1ynm\n\n" );
#endif
				NandBank3_Handle.Config.PageSize			= 2048; 	// 2,048 bytes 
				NandBank3_Handle.Config.SpareAreaSize		= 64;		// 64 bytes / 1 page (1 page = 2,048 bytes)
				NandBank3_Handle.Config.BlockSize			= 64;		// 64 pages 
				NandBank3_Handle.Config.PlaneSize			= 2048;		// 2,048 blocks 
				NandBank3_Handle.Config.PlaneNbr			= 2;		// NAND memory number of planes
				NandBank3_Handle.Config.BlockNbr			= 4096;		// NAND memory number of total blocks
				NandBank3_Handle.Config.ExtraCommandEnable	= DISABLE;	//
				
				err = false;
		} 
	}
               
        return err;
}

void Remote_Download_File_Find(void)
{
    switch (g_Chk)
    {
        case nMAIN_UPLOAD : 
            NandAddr.Page  = 0;
            NandAddr.Block = 0;
            NandAddr.Plane = 0;
            break;
        case nCT_SENSOR_UPLOAD :
            NandAddr.Page  = 0;
            NandAddr.Block = 10;
            NandAddr.Plane = 0; 
            break;
        case nIR_SENSOR_UPLOAD : 
            NandAddr.Page  = 0;
            NandAddr.Block = 12;
            NandAddr.Plane = 0;
            break;
     }
   
    do {
      
        printf("\r\n\r\n P: %d, B: %d, P: %d\r\n",NandAddr.Page, NandAddr.Block, NandAddr.Plane);
        
        if (HAL_NAND_Read_Page_8b(&NandBank3_Handle, &NandAddr, nand.byte, 1) != HAL_OK) {
          printf("\r\n Read Page Err\r\n");
          break;
        }
        
        printf(" pageByte : %d | totByte : %d \n", nand.pageByte, nand.totByte);
        
        for (uint16_t i =0; i < nand.pageByte; i++)
            printf(" %x", nand.byte[i]);
        
        if (HAL_NAND_Address_Inc(&NandBank3_Handle, &NandAddr)!= NAND_VALID_ADDRESS) {
            printf("\r\n Addr Inc Err\r\n");
            break;
        }
         
    } while(nand.pageByte == 1024);
    
    Nand_flash.bFile_chk    = TRUE;
    Nand_flash.totByte      = nand.totByte;
    Nand_flash.program_cnt  = Nand_flash.totByte / PAGE_SIZE;
    Nand_flash.program_tail = Nand_flash.totByte % PAGE_SIZE;
    if (Nand_flash.program_tail)
        Nand_flash.program_cnt++;
    printf("\r\n\r\n *** TOT Byte : %d | Page : %d | Tail : %d ***\r\n", Nand_flash.totByte, Nand_flash.program_cnt, Nand_flash.program_tail);
}


void Remote_FilePageLoad(void)
{
    printf("\r\n\r\n P: %d, B: %d, P: %d\r\n",NandAddr.Page, NandAddr.Block, NandAddr.Plane);
      
    if (HAL_NAND_Read_Page_8b(&NandBank3_Handle, &NandAddr, nand.byte, 1) != HAL_OK) {
       printf("\r\n Read Page Err\r\n");
    }

    printf(" pageByte : %d | totByte : %d \n", nand.pageByte, nand.totByte);
    
    for (uint16_t i = 0; i < nand.pageByte; i++)
        printf(" %x", nand.byte[i]); 
}



uint8_t Remote_Update_Check(void)
{
    printf("\r\n Remote_Update_Check Func! \r\n");
    
    NandAddr.Page  = 0;         
    NandAddr.Block = 14;
    NandAddr.Plane = 0;
    HAL_NAND_Read_Page_8b(&NandBank3_Handle, &NandAddr, nand.byte, 1);
    
    printf(" %x, %x, %x", nand.byte[0], nand.byte[1], nand.byte[2]);
    Nand_flash.bMain_UpdateChk_flg = nand.byte[0];
    Nand_flash.bCt_UpdateChk_flg   = nand.byte[1];
    Nand_flash.bIr_UpdateChk_flg   = nand.byte[2];
    
    if (nand.byte[0]) {
        NAND_Erase_Block(0, 14, 0);
        nand.byte[0] = false;
        nand.byte[1] = Nand_flash.bCt_UpdateChk_flg;
        nand.byte[2] = Nand_flash.bIr_UpdateChk_flg;
        NAND_Write_Page_8bit(0, 14, 0, nand.byte);  
        return 0;
        
    } else if (nand.byte[1]) {
        NAND_Erase_Block(0, 14, 0);
        nand.byte[1] = false;
        nand.byte[0] = Nand_flash.bMain_UpdateChk_flg;
        nand.byte[2] = Nand_flash.bIr_UpdateChk_flg;
        NAND_Write_Page_8bit(0, 14, 0, nand.byte); 
        return 1;
        
    } else if (nand.byte[2]) {
       NAND_Erase_Block(0, 14, 0);
       nand.byte[2] = false;
       nand.byte[0] = Nand_flash.bMain_UpdateChk_flg;
       nand.byte[1] = Nand_flash.bCt_UpdateChk_flg;
       NAND_Write_Page_8bit(0, 14, 0, nand.byte); 
       return 2;
        
    } else  return 0xff; 		
}

       
U8 NAND_Erase_Block(U16 addr_page, U16 addr_block, U16 addr_plane)
{
    NandAddr.Page  = addr_page;
    NandAddr.Block = addr_block;
    NandAddr.Plane = addr_plane;
    
    if (HAL_NAND_Erase_Block(&NandBank3_Handle, &NandAddr) == HAL_OK)
      return true;
    else        return false;
}

U8 NAND_Read_Page_8bit(U16 addr_page, U16 addr_block, U16 addr_plane, U8 *read_buff)
{
    NandAddr.Page  = addr_page;
    NandAddr.Block = addr_block;
    NandAddr.Plane = addr_plane;
    
    if (HAL_NAND_Read_Page_8b(&NandBank3_Handle, &NandAddr, read_buff, 1) == HAL_OK) 
      return true;
    else        return false;
}
	  
U8 NAND_Write_Page_8bit(U16 addr_page, U16 addr_block, U16 addr_plane, U8 *write_buff)
{
    NandAddr.Page  = addr_page;
    NandAddr.Block = addr_block;
    NandAddr.Plane = addr_plane;
    
    if (HAL_NAND_Write_Page_8b(&NandBank3_Handle, &NandAddr, write_buff, 1) == HAL_OK)
        return true;
    else        return false;  
}