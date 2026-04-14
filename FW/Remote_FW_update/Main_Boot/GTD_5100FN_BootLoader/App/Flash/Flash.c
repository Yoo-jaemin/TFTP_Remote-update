#define __FLASH_C__
    #include "Flash.h"
#undef  __FLASH_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "SD_Card.h"
/* End Include */

/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
/* End Struct */

/* Start Function */
static unsigned long GetSector(uint32_t Address);
static U32 GetStratSector(U8 ucSectorNo);
static unsigned long GetSectorSize(uint32_t Sector);
static U32 Flash_Read(U32 dwAdd);
/* End Function */

/* Start Variable */
/* End Variable */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Flash_Erase(U32 ulStart_add, U32 ulEnd_add)
{
  	static FLASH_EraseInitTypeDef EraseInitStruct;
	U32 ulFirst_Sector, ulNb_Sectors, ulSector_Err;
  	
	HAL_FLASH_Unlock();
	/* Get the 1st sector to erase */
  	ulFirst_Sector = GetSector(ulStart_add);
  	/* Get the number of sector to erase from 1st sector*/
  	ulNb_Sectors = GetSector(ulEnd_add + GetSectorSize(ADDR_FLASH_SECTOR_11)-1) - ulFirst_Sector + 1;
	
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
 	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  	EraseInitStruct.Sector = ulFirst_Sector;
  	EraseInitStruct.NbSectors = ulNb_Sectors;
	
	if (HAL_FLASHEx_Erase(&EraseInitStruct, (uint32_t*)&ulSector_Err) != HAL_OK)
	{
		printf("\r\n Erase Error = %x", ulSector_Err);
	 	Flash.ulEraseErr = HAL_FLASH_GetError();
		//while(1);
	}
	
	__HAL_FLASH_DATA_CACHE_DISABLE();
  	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	__HAL_FLASH_DATA_CACHE_RESET();
  	__HAL_FLASH_INSTRUCTION_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();
		
	HAL_FLASH_Lock();
	
	Flash.ucErase_complet = TRUE;
}


void FlashSector_Erase(U8 ucSector)
{
  	static FLASH_EraseInitTypeDef EraseInitStruct;
	U32 ulFirst_Sector, ulNb_Sectors, ulSector_Err;
  	
	HAL_FLASH_Unlock();
	/* Get the 1st sector to erase */
  	ulFirst_Sector = GetStratSector(ucSector);
  	/* Get the number of sector to erase from 1st sector*/
  	//ulNb_Sectors = GetSector(ulEnd_add + GetSectorSize(ADDR_FLASH_SECTOR_11)-1) - ulFirst_Sector + 1;
	ulNb_Sectors = 1;
	
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
 	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  	EraseInitStruct.Sector = ulFirst_Sector;
  	EraseInitStruct.NbSectors = ulNb_Sectors;
	
	if (HAL_FLASHEx_Erase(&EraseInitStruct, (uint32_t*)&ulSector_Err) != HAL_OK)
	{
		printf("\r\n Erase Error = %x", ulSector_Err);
	 	Flash.ulEraseErr = HAL_FLASH_GetError();
		//while(1);
	}
	
	__HAL_FLASH_DATA_CACHE_DISABLE();
  	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	__HAL_FLASH_DATA_CACHE_RESET();
  	__HAL_FLASH_INSTRUCTION_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();
		
	HAL_FLASH_Lock();
}

static U32 GetStratSector(U8 ucSectorNo)
{
  	U32 ulSector = 0;
	
  	switch(ucSectorNo)
	{
		case nSECTOR0: ulSector = ADDR_FLASH_SECTOR_0; break;
		case nSECTOR1: ulSector = ADDR_FLASH_SECTOR_1; break;
		case nSECTOR2: ulSector = ADDR_FLASH_SECTOR_2; break;
		case nSECTOR3: ulSector = ADDR_FLASH_SECTOR_3; break;
		case nSECTOR4: ulSector = ADDR_FLASH_SECTOR_4; break;
		case nSECTOR5: ulSector = ADDR_FLASH_SECTOR_5; break;
		case nSECTOR6: ulSector = ADDR_FLASH_SECTOR_6; break;
		case nSECTOR7: ulSector = ADDR_FLASH_SECTOR_7; break;
		case nSECTOR8: ulSector = ADDR_FLASH_SECTOR_8; break;
		case nSECTOR9: ulSector = ADDR_FLASH_SECTOR_9; break;
		case nSECTOR10: ulSector = ADDR_FLASH_SECTOR_10; break;
		case nSECTOR11: ulSector = ADDR_FLASH_SECTOR_11; break;
		default:
		  	ulSector = ucSectorNo;
	  		Flash.ulEraseErr = TRUE;
	  		printf("\r\n Get Sector Error");
			break;
	}
	
	return GetSector(ulSector);
} 

void Upload_Flash(char* cBuf, U16 uSize, U32 ulOffset)
{
  	U16 uAdd_cnt, uData_cnt;
	U32	ulStart_add;
	
	Flash.uBuf_cnt = 0;
	uData_cnt = 0;
	uAdd_cnt = 0;
	ulStart_add = FLASH_USER_START_ADDR + ulOffset;
	
	memset(Flash.dwBuff, 0xFF, PAGE_SIZE);
	
	while(uData_cnt < uSize){
	  	switch(uData_cnt & 3)
		{
			case 0: 
			  	Flash.dwBuff[Flash.uBuf_cnt] &= 0xFFFFFF00;
				Flash.dwBuff[Flash.uBuf_cnt] |= (U32) *(cBuf + uData_cnt); break;
			case 1: 
			  	Flash.dwBuff[Flash.uBuf_cnt] &= 0xFFFF00FF;
			  	Flash.dwBuff[Flash.uBuf_cnt] |= (U32) *(cBuf + uData_cnt) << 8; break;
			case 2: 
			  	Flash.dwBuff[Flash.uBuf_cnt] &= 0xFF00FFFF;
			  	Flash.dwBuff[Flash.uBuf_cnt] |= (U32) *(cBuf + uData_cnt) << 16; break;
			case 3: 
			  	Flash.dwBuff[Flash.uBuf_cnt] &= 0x00FFFFFF;
			  	Flash.dwBuff[Flash.uBuf_cnt] |= (U32) *(cBuf + uData_cnt) << 24; break;
		}
		uData_cnt ++;
		if((uData_cnt & 3) == 0) Flash.uBuf_cnt++;
	}
	
	uData_cnt = 0;
	HAL_FLASH_Unlock();
	
	if(uSize < PAGE_SIZE){
		while(uData_cnt < Flash.uBuf_cnt+1){
	  		if(HAL_ERROR == (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ulStart_add + uAdd_cnt, Flash.dwBuff[uData_cnt]))){
				Flash.ucWriteErr = TRUE;
	 		}
			uAdd_cnt += 4;
			uData_cnt ++;
		}
	}
	else {
	  	while(uData_cnt < Flash.uBuf_cnt){
	  		if(HAL_ERROR == (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ulStart_add + uAdd_cnt, Flash.dwBuff[uData_cnt]))){
				Flash.ucWriteErr = TRUE;
	 		}
			uAdd_cnt += 4;
			uData_cnt ++;
		}
	}
	
	HAL_FLASH_Lock();
}

void Verify_Flash(char* cFilebuf, U32 ulSize, U32 ulOffset)
{
  	int	iErrFlag;
	U32	ulStart_add; //ulRead_data, ulFile_data;
	
	ulStart_add = FLASH_USER_START_ADDR + ulOffset;
	
	HAL_FLASH_Unlock();
	
	iErrFlag = memcmp(cFilebuf, (const void*)ulStart_add, ulSize);
	
	if(iErrFlag != FALSE) {
		Flash.ucVerifyErr = TRUE;
		printf("\r\n Page:%d : Errcode:%d", (ulOffset/PAGE_SIZE), iErrFlag);
	}
}

static U32 Flash_Read(U32 dwAdd)
{
  	return *(__IO U32 *) dwAdd;
}


static unsigned long GetSector(uint32_t Address)
{
  	uint32_t sector = 0;
  
  	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  	{
	    sector = FLASH_SECTOR_0;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  	{
	    sector = FLASH_SECTOR_1;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  	{
	    sector = FLASH_SECTOR_2;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  	{
	    sector = FLASH_SECTOR_3;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  	{
	    sector = FLASH_SECTOR_4;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  	{
	    sector = FLASH_SECTOR_5;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  	{
	    sector = FLASH_SECTOR_6;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  	{
	    sector = FLASH_SECTOR_7;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  	{
	    sector = FLASH_SECTOR_8;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  	{
	    sector = FLASH_SECTOR_9;  
  	}
  	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  	{
	    sector = FLASH_SECTOR_10;  
  	}
  	else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
  	{
	    sector = FLASH_SECTOR_11;  
  	}
	
  	return sector;
}

static unsigned long GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}