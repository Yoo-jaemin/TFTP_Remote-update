#ifndef   __MICRO_SD_H__
#define   __MICRO_SD_H__


#ifdef __MICRO_SD_C__
	#define MICRO_SD_EXT
#else
	#define MICRO_SD_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
#include "common.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
/* End Include */
  
/* Start Define */
/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef struct {
	uint8_t						Cnt;
	uint8_t						Buf;
	uint8_t						IsPresent;
	uint8_t						IsChanged;
}uSD_ScanType;

typedef struct {
	uSD_ScanType				Scan;
	FATFS						FatFs;			/* File system object for SD card logical drive */
	FRESULT						FatFsResult;
	char						Path[4];		/* SD card logical drive path */
}uSD_HandleType;
/* End Struct */

/* Start Function */
MICRO_SD_EXT void MicroSD_Insertion_Check( void );
MICRO_SD_EXT void uSD_Config( void );
/* End Function */

/* Start Variable */
MICRO_SD_EXT uSD_HandleType		uSD_Handle;
/* End Variable */


#endif // __SD_CARD_H__

