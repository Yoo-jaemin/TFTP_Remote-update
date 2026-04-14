#ifndef   __FLASH_H__
#define   __FLASH_H__

#ifdef __FLASH_C__
	#define FLASH_EXT
#else
	#define FLASH_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
/* End Include */

/* Start Define */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Start @ of user Flash area */
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_5 
/* End @ of user Flash area : sector start address + sector size -1 */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_11 //+ GetSectorSize(ADDR_FLASH_SECTOR_11) -1 

#define FLASH_USER_START_SECTOR nSECTOR5

#define FLASH_BUFF_MAX 0x100
/* End Define */

/* Start Enum */
enum 
{
  	nSECTOR0,
	nSECTOR1,
	nSECTOR2,
	nSECTOR3,
	nSECTOR4,
	nSECTOR5,
	nSECTOR6,
	nSECTOR7,
	nSECTOR8,
	nSECTOR9,
	nSECTOR10,
	nSECTOR11,
	nSECTOR_MAX,
};

enum 
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR
};

enum{
  FLASHIF_PROTECTION_NONE         = 0,
  FLASHIF_PROTECTION_PCROPENABLED = 0x1,
  FLASHIF_PROTECTION_WRPENABLED   = 0x2,
  FLASHIF_PROTECTION_RDPENABLED   = 0x4,
};
/* End Enum */

/* Start Struct */

typedef struct {
	U8	ucErase_complet;
  	U8	ucWriteErr;
  	U8	ucVerifyErr;
	U16	uBuf_cnt;
	U32	ulDownload_cnt;
	U32	ulEraseErr;
	U32	dwBuff[FLASH_BUFF_MAX];
}Flash_Handle;
/* End Struct */

/* Start Function */
FLASH_EXT void Flash_Erase(U32 ulStart_add, U32 ulEnd_add);
FLASH_EXT void FlashSector_Erase(U8 ucSector);
FLASH_EXT void Upload_Flash(char* cBuf, U16 uSize, U32 ulOffset);
FLASH_EXT void Verify_Flash(char* cBuf, U32 ulSize, U32 ulOffset);
FLASH_EXT void App_Jump(U32 ulStart_add);
/* End Function */

/* Start Variable */
FLASH_EXT Flash_Handle Flash;
/* End Variable */


#endif // __FLASH_H__