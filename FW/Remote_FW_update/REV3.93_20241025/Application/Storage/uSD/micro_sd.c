#define __MICRO_SD_C__
    #include "micro_sd.h"
#undef  __MICRO_SD_C__

/* Start abbreviation */
/* End abbreviation */

/* Start Include */ 
/* End Include */

/* Start Define */
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

void MicroSD_Insertion_Check( void )
{
	uint8_t		port = BSP_SD_IsDetected();
	uint8_t		comp = uSD_Handle.Scan.Buf ^ port;
	
	uSD_Handle.Scan.Buf = port;
	
	if( comp ){
		uSD_Handle.Scan.Cnt = 0;
	}
	else {
		if( uSD_Handle.Scan.Cnt != 0xFF ){
			uSD_Handle.Scan.Cnt++;
		}
		if( uSD_Handle.Scan.Cnt == 0xFE ){
			uSD_Handle.Scan.Cnt++;
			
			uSD_Handle.Scan.IsPresent = port;
			uSD_Handle.Scan.IsChanged = 250;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uSD_Config( void )
{
	uint16_t	i	= 0xFFFF;
	
	while( i-- ){ MicroSD_Insertion_Check(); }
	
	if( uSD_Handle.Scan.IsPresent == SD_PRESENT ){
		/* Enable wide operation */
		if( HAL_SD_ConfigWideBusOperation( &BspSdHandle, SDIO_BUS_WIDE_4B ) == HAL_OK ){
			
			if( FATFS_LinkDriver( &SD_Driver, uSD_Handle.Path ) == 0 ){
				
				/*##-2- Register the file system object to the FatFs module ##############*/
				uSD_Handle.FatFsResult = f_mount( &uSD_Handle.FatFs, (TCHAR const*)uSD_Handle.Path, 0 );
			}
		}
	}
	
#if 0
	FRESULT		res;												/* FatFs function common result code */
	uint32_t	byteswritten, bytesread;							/* File write/read counts */
	uint8_t		wtext[] = "This is STM32 working with FatFs";		/* File write buffer */
	uint8_t		rtext[100];											/* File read buffer */
	
	/*##-1- Link the micro SD disk I/O driver ##################################*/
	if( FATFS_LinkDriver( &SD_Driver, SDPath ) == 0 ){
		
		/*##-2- Register the file system object to the FatFs module ##############*/
		if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK){
			/* FatFs Initialization Error */
			Error_Handler();
		}
		else {
			/*##-3- Create a FAT file system (format) on the logical drive #########*/
			/* WARNING: Formatting the uSD card will delete all content on the device */
			res = f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, buffer, sizeof(buffer));
			if( res != FR_OK ){
				/* FatFs Format Error */
				Error_Handler();
			}
			else {       
				/*##-4- Create and Open a new text file object with write access #####*/
				if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
				  /* 'STM32.TXT' file Open for write Error */
				  Error_Handler();
				}
				else {
					/*##-5- Write data to the text file ################################*/
					res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);
					
					if((byteswritten == 0) || (res != FR_OK)){
						/* 'STM32.TXT' file Write or EOF Error */
						Error_Handler();
					}
					else {
						/*##-6- Close the open text file #################################*/
						f_close(&MyFile);
						
						/*##-7- Open the text file object with read access ###############*/
						if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK){
							/* 'STM32.TXT' file Open for read Error */
							Error_Handler();
						}
						else {
							/*##-8- Read data from the text file ###########################*/
							res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);
							
							if((bytesread == 0) || (res != FR_OK)){
								/* 'STM32.TXT' file Read or EOF Error */
								Error_Handler();
							}
							else {
								/*##-9- Close the open text file #############################*/
								f_close(&MyFile);
								
								/*##-10- Compare read data with the expected data ############*/
								if((bytesread != byteswritten)){                
									/* Read data is different from the expected data */
									Error_Handler();
								}
								else {
									/* Success of the demo: no error occurrence */
									dprintf( "  SD Success!\n" );
									
								}
							}
						}
					}
				}
			}
		}
	}
	
	/*##-11- Unlink the RAM disk I/O driver ####################################*/
	FATFS_UnLinkDriver( SDPath );
#endif
}


