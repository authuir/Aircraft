#include "logs.h"
#include "system.h"

#include "ff_gen_drv.h"
#include "sd_diskio.h"


#define TEST_FATFS 1

bool is_fs_inited = false;

uint8_t TxBuffer[] = "TEST\n";
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */

FRESULT res;                                          /* FatFs function common result code */
uint32_t byteswritten, bytesread;                     /* File write/read counts */
uint8_t wtext[] = "This is STM32 working with FatFs\n\r"; /* File write buffer */
uint8_t rtext[100];                                   /* File read buffer */


void CMD_Log(char * data,uint32_t len)
{
	if (is_fs_inited == false)
		Error_Handler();		

	if(f_open(&MyFile, "Log.TXT", FA_OPEN_EXISTING | FA_WRITE) != FR_OK)
	{
		Error_Handler();
	} else {
		byteswritten = f_size(&MyFile);
		f_lseek(&MyFile, byteswritten);
		res = f_write(&MyFile, (void *)data, len, (void *)&byteswritten);
		
				byteswritten = f_size(&MyFile);
		f_lseek(&MyFile, byteswritten);
		res = f_write(&MyFile, (void *)data, len, (void *)&byteswritten);
		
				byteswritten = f_size(&MyFile);
		f_lseek(&MyFile, byteswritten);
		res = f_write(&MyFile, (void *)data, len, (void *)&byteswritten);
		
				byteswritten = f_size(&MyFile);
		f_lseek(&MyFile, byteswritten);
		res = f_write(&MyFile, (void *)data, len, (void *)&byteswritten);
		
				byteswritten = f_size(&MyFile);
		f_lseek(&MyFile, byteswritten);
		res = f_write(&MyFile, (void *)data, len, (void *)&byteswritten);
		
		if((byteswritten == 0) || (res != FR_OK))
		{
			Error_Handler();
		} else {
			f_close(&MyFile);
		}
	}	
}

void FAT_TEST()
{
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
	{
		if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
		{
			Error_Handler();
		} else {
			/*if(f_mkfs((TCHAR const*)SDPath, 0, 0) != FR_OK)
			{
				Error_Handler();	//ºÏ≤È «∑Ò≤Â»ÎSDø®
			} else {
				if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
				{
					Error_Handler();
				} else {
					res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);
					if((byteswritten == 0) || (res != FR_OK))
					{
						Error_Handler();
					} else {
						f_close(&MyFile);
					}
				}
			}*/
		}
	}
	//FATFS_UnLinkDriver(SDPath);
	is_fs_inited = true;
}
