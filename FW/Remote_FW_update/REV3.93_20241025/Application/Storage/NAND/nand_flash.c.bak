#define __NAND_FLASH_C__
    #include "nand_flash.h"
#undef  __NAND_FLASH_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "fsmc.h"
/* End Include */

/* Start Define */
#define NAND_FLASH_nWP_Pin					GPIO_PIN_3
#define NAND_FLASH_nWP_Gpio_Port			GPIOD
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

bool Nand_Flash_Init( void )
{
	bool	err = true;
	
	if( HAL_NAND_Read_ID( &NandBank3_Handle, &NandID ) == HAL_OK ){
		
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


