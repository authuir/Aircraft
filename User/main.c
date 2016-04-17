#include <stdio.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "system.h"
#include "wifi.h"
#include "ble.h"

#include "ff_gen_drv.h"
#include "sd_diskio.h"

extern __IO int mscnt;
uint8_t TxBuffer[] = "TEST\n";

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */

static void Error_Handler(void)
{
	/* Turn LED1 on */
	LED_11_ON;
	while(1)
	{
		LED_11_OFF;
		HAL_Delay(200);
		LED_11_ON;
		HAL_Delay(200);
	}
}

int main()
{
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */
	CPU_CACHE_Enable();
	
	//晶振没起振，可能有问题
	SystemClock_Config();
	
	HAL_Init();
	Init_LED();
	LED_ALL_ON;
	//Init_BLE();
	//Init_WIFI();
	
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
	{
		if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
		{
			Error_Handler();
		} else {
			if(f_mkfs((TCHAR const*)SDPath, 0, 0) != FR_OK)
			{
				Error_Handler();	//检查是否插入SD卡
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
			}
		}
	}
	
	FATFS_UnLinkDriver(SDPath);

	while(1)
	{
		if (mscnt%1000>500)
			LED_00_ON;
		else
			LED_00_OFF;
		
		if (mscnt%500>250)
			LED_11_ON;
		else
			LED_11_OFF;
		
		if (mscnt%2000>1000)
			LED_22_ON;
		else
			LED_22_OFF;
		
		if (BLE_receive(TxBuffer, 5) == HAL_OK)
			BLE_send(TxBuffer, 5);
	}
	return 0;
} 
