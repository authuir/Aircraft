#include <stdio.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "system.h"
#include "wifi.h"
#include "ble.h"
#include "px4.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "gps.h"
#include "pwm.h"

#define TEST_FATFS 0

extern __IO int mscnt;
uint8_t TxBuffer[] = "TEST\n";
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
uint8_t aRxBuffer22[8]={0,0,0,0,0,0,0,0};
#define RXBUFFERSIZE22 8
__IO ITStatus UartReady = RESET;

void FAT_TEST(void);
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

union 
{
	float a;
	uint8_t b[4];
}floattt;

int main()
{
	double pul = 0.8;
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */
	MPU_Config();
	CPU_CACHE_Enable();
	
	//晶振没起振，可能有问题
	SystemClock_Config();
	
	HAL_Init();
	Init_LED();
	LED_ALL_ON;
	
	Init_PWM();
	
	while (1)
	{
		if (pul > 1) 
			pul = 0;
		pul = pul + 0.01;
		PWM_CHG(M1,pul);
		PWM_CHG(M2,1-pul);
		PWM_CHG(M3,pul);
		PWM_CHG(M4,1-pul);
		HAL_Delay(50);
	}

	PX4FlowPortInit();
	GPSPortInit();
	Init_BLE();
	Init_WIFI();
	WifiSendCom_Start();
	WifiSendCom_Reset();//复位指令
	while(1)
	{
//		int i;
//		uint8_t send_nem[]={0x08,0x00,0x02,0x03};
//		Wifi_send(send_nem,4);
	}
	return 0;
} 

void FAT_TEST()
{
#if TEST_FATFS
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
#endif
}
