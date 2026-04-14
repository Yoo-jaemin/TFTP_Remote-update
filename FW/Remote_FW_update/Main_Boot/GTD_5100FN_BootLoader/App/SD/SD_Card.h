#ifndef   __SD_CARD_H__
#define   __SD_CARD_H__


#ifdef __SD_CARD_C__
	#define SD_CARD_EXT
#else
	#define SD_CARD_EXT extern
#endif

/* Start abbreviation */
/* End abbreviation */

/* Start Include */
#include "common.h"
#include "ff.h"
/* End Include */

/* Start Define */
#define PAGE_SIZE	(unsigned short)0x400
#define MAIN_EXTENSION	"GTD"
#define CT_EXTENSION	"CT"
#define IR_EXTENSION	"IR"

#define _ROF_FILE_NAME_SIZE		12
#define _FILE_ROMINFO_MAXNUM	50 //(_FILE_HEADERSIZE_WORD/ _FILE_INFOSIZE_WORD)

/* End Define */

/* Start Enum */
/* End Enum */

/* Start Struct */
typedef union{
  	struct {
	  	bool	bMount_err		:1;
		bool	bInsert_err		:1;
		bool	bFileFind_err	:1;
		bool	bFileOpen_err	:1;
		bool	bFileRead_err	:1;
		bool	bInit_err		:1;
		bool	bEmpty			:1;
		bool	bComplet		:1;
	}Bit;
	U8	All;
}Flag;

typedef struct {
  	Flag 	Check;
	U8		ucRet;
	char* 	pExtension;
  	char  	cPath[4];
	U16 	uProgram_cnt;
	U16		uData_cnt;
	U16		uPage_cnt;
	U16		uProgram_tail;
	FATFS	FatFS;
	FILINFO Info;
	FIL 	File;
}File_Handle;


/* End Struct */

/* Start Function */
SD_CARD_EXT U8 SDIO_SD_Init(void);
SD_CARD_EXT void SDIO_SD_DeInit(void);
SD_CARD_EXT void SD_Mount(void);
SD_CARD_EXT void Download_File_Find(void);
SD_CARD_EXT void put_rc(FRESULT rc);
SD_CARD_EXT void FilePageLoad(char* fstr, char*buf, U32 size, U32 offset);
/* End Function */

/* Start Variable */
SD_CARD_EXT SD_HandleTypeDef hsd;
SD_CARD_EXT File_Handle SD;
/* End Variable */


#endif // __SD_CARD_H__
