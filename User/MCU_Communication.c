#include "stm32f7xx.h"
#include "stm32f7xx_hal.h" 
#include "MCU_Communication.h"
 
UART_HandleTypeDef MCU_UartHandle;
DMA_HandleTypeDef MCU_DMA_Receive;
	
	#define BufDeep 8
uint8_t ReceiveBuf[BufDeep]={0};
 void MCU_UART_MspInit(UART_HandleTypeDef *huart)
 {
	 	 //����PX4  TX,RX  GPIOA_PIN9  GPIOA_10
	 	GPIO_InitTypeDef     GPIO_MCU;
	 	/* Enable the GPIO_A clock */
	  __GPIOB_CLK_ENABLE();//�򿪶˿�ʱ��
	 __USART3_CLK_ENABLE();//�򿪴���ʱ��
		GPIO_MCU.Pin = GPIO_PIN_10;					//GPIOB
		GPIO_MCU.Mode = GPIO_MODE_AF_PP;		//��Ϊѡ����
		GPIO_MCU.Pull = GPIO_PULLUP;				//����
		GPIO_MCU.Speed = GPIO_SPEED_HIGH;		//����
		GPIO_MCU.Alternate=GPIO_AF7_USART3;	//ѡ���� ��ΪUSART����
		HAL_GPIO_Init(GPIOB, &GPIO_MCU);  
		GPIO_MCU.Pin=GPIO_PIN_11;						//����TX��
		HAL_GPIO_Init(GPIOB, &GPIO_MCU); 
	   
		HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
#if 0	/*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for reception process */
	__HAL_RCC_DMA1_CLK_ENABLE();		//��DMAʱ��
	  MCU_DMA_Receive.Instance               = DMA1_Stream1;									//������2
  MCU_DMA_Receive.Init.Channel             = DMA_CHANNEL_4;								//ͨ��4
  MCU_DMA_Receive.Init.Direction           = DMA_PERIPH_TO_MEMORY;					//���赽�ڴ�
  MCU_DMA_Receive.Init.PeriphInc           = DMA_PINC_DISABLE;							//�����ַ�Ƿ���  ������
  MCU_DMA_Receive.Init.MemInc              = DMA_MINC_ENABLE;							//�ڴ��ַ�Ƿ���	 ����
  MCU_DMA_Receive.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;					//�����ֽڶ���
  MCU_DMA_Receive.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;					//�ڴ��ֽڶ���
  MCU_DMA_Receive.Init.Mode                = DMA_CIRCULAR;										//DMA ģʽ ����ģʽ
  MCU_DMA_Receive.Init.Priority            = DMA_PRIORITY_HIGH;						//���ȼ�
//	PX4DMA_Receive.Init.FIFOMode						= DMA_FIFOMODE_DISABLE;					//�Ƿ�ʹ��FIFO   ��     
//	PX4DMA_Receive.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL;  //ָ��FIFO����ֵˮƽ
//	PX4DMA_Receive.Init.MemBurst 						= DMA_MBURST_SINGLE;						//ָ����Burstת�������ڴ洫��
//	PX4DMA_Receive.Init.PeriphBurst 				= DMA_PBURST_SINGLE;						//ָ����Burstת��������Χת��
  HAL_DMA_Init(&MCU_DMA_Receive);//DMA��ʼ��
	
	//*****************************************************����DMAģʽ  start******************//
	    /* Set the UART DMA transfer complete callback */
   // PX4_UartHandle->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;
		    /* Enable the DMA channel */
    //HAL_DMA_Start_IT(PX4_UartHandle->hdmarx, (uint32_t)&PX4_UartHandle->Instance->RDR, *(uint32_t*)PX4_Buf, PX4_GetBuf);
	//*****************************************************����DMAģʽ  start******************//
	  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(&MCU_UartHandle, hdmarx, MCU_DMA_Receive);
	//DMA �ж�
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);//�������ȼ�
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);//ʹ��DMA2�ж�
#else 
	  /*##-4- Put UART peripheral in reception process ###########################*/ 
    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&MCU_UartHandle, UART_IT_RXNE);	//�򿪽����ж�
#endif
 }
void MCUPortInit(void)
{
  
	 //*******************************************************
	 //����USART���������
		__USART3_CLK_ENABLE();
		MCU_UartHandle.Instance        = USART3;
		MCU_UartHandle.Init.BaudRate   = 115200;
		MCU_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		MCU_UartHandle.Init.StopBits   = UART_STOPBITS_1;
		MCU_UartHandle.Init.Parity     = UART_PARITY_NONE;
		MCU_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
		MCU_UartHandle.Init.Mode       = UART_MODE_TX_RX;
		MCU_UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT; 
		MCU_UartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//�ر�OverRun
		HAL_UART_DeInit(&MCU_UartHandle);//ж��USART3
		HAL_UART_Init(&MCU_UartHandle);//װ��USART3

}

HAL_StatusTypeDef MCU_send(uint8_t * TxBuffer, uint32_t TxLen)
{
	if(HAL_UART_Transmit(&MCU_UartHandle, (uint8_t*)TxBuffer, (uint32_t)TxLen, 5000)!= HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

static void  MCU_One( uint8_t data)
{
	HAL_UART_Transmit(&MCU_UartHandle, (uint8_t*)(&data), 1, 5000);
}

struct NewStruct
{
	union
	{
		uint32_t sendsimes;
		uint8_t  sendtimesBuf[4];
	}SendTimes;
	union
	{
		float height;
		uint8_t  heightBuf[4];
	}Height;
	union
	{
		float longitude;
		uint8_t  longitudeBuf[4];
	}Longitude;
	union
	{
		float latitude;
		uint8_t  latitudeBuf[4];
	}Latitude;
	union
	{
		uint32_t yeartime;
		uint8_t  yeartimeBuf[4];
	}YearTime;
	union
	{
		uint32_t nowtime;
		uint8_t  nowtimeBuf[4];
	}NowTime;
	union
	{
		float expeule0;
		uint8_t  expeule0Buf[4];
	}ExpEule0;
	union
	{
		float expeule1;
		uint8_t  expeule1Buf[4];
	}ExpEule1;
	union
	{
		float expeule2;
		uint8_t  expeule2Buf[4];
	}ExpEule2;
};
struct NewStruct MyStruct;

//MCU֮��ͨ��Э��
//�������߶ȷ��ͣ�����ŷ���ǣ�GPS��Ϣ��
#define MCU_Address			0x55	//������ַ
#define Comm_State			0x10	//״̬����
#define Comm_SendTimes	0x11	//���ʹ���
#define Comm_Height			0x12	//�߶�����
#define Comm_Longitude	0x13	//����
#define Comm_Latitude		0x14	//ά��
#define Comm_YearTime		0x15	//ʱ��������
#define Comm_NowTime		0x16	//ʱ��ʱ����
#define Comm_ExpEule0		0x17	//����ŷ����0
#define Comm_ExpEule1		0x18	//����ŷ����1
#define Comm_ExpEule2		0x19	//����ŷ����2
#define Comm_Open			0x20
#define Comm_Close 			0x21
#define Comm_Calibration	0x22	//У׼����

void MCU_CalibrationSend()
{
	MCU_One(MCU_Address);MCU_One(Comm_Calibration);MCU_One(MCU_Address^Comm_Calibration);
}

void MCU_AccordSend(float X,float Y,float Z,float T)
{
	uint8_t SendI=0;
//*************��Struct��ֵ************************************
	MyStruct.SendTimes.sendsimes++;
	MyStruct.Height.height=T;
	
	MyStruct.Longitude.longitude=222.222;
	MyStruct.Latitude.latitude=333.3333;
	MyStruct.YearTime.yeartime=20160424;
	MyStruct.NowTime.nowtime=164033;
	
	MyStruct.ExpEule0.expeule0=Y;
	MyStruct.ExpEule1.expeule1=Z;
	MyStruct.ExpEule2.expeule2=X;
//*************************************************
	//MCU_One(MCU_Address);MCU_One(Comm_State);MCU_send(MyStruct.Height.heightBuf,4);MCU_One(MCU_Address^Comm_State);//״̬
	MCU_One(MCU_Address);MCU_One(Comm_SendTimes);MCU_send(MyStruct.SendTimes.sendtimesBuf,4);MCU_One(MCU_Address^Comm_SendTimes);//���ʹ���
	MCU_One(MCU_Address);MCU_One(Comm_Height);MCU_send(MyStruct.Height.heightBuf,4);MCU_One(MCU_Address^Comm_Height);//�߶�����
	//MCU_One(MCU_Address);MCU_One(Comm_Longitude);MCU_send(MyStruct.Longitude.longitudeBuf,4);MCU_One(MCU_Address^Comm_Longitude);//�߶�����
	//MCU_One(MCU_Address);MCU_One(Comm_Latitude);MCU_send(MyStruct.Latitude.latitudeBuf,4);MCU_One(MCU_Address^Comm_Latitude);//�߶�����
	//MCU_One(MCU_Address);MCU_One(Comm_YearTime);MCU_send(MyStruct.YearTime.yeartimeBuf,4);MCU_One(MCU_Address^Comm_YearTime);//�߶�����
	//MCU_One(MCU_Address);MCU_One(Comm_NowTime);MCU_send(MyStruct.NowTime.nowtimeBuf,4);MCU_One(MCU_Address^Comm_NowTime);//�߶�����
	MCU_One(MCU_Address);MCU_One(Comm_ExpEule0);MCU_send(MyStruct.ExpEule0.expeule0Buf,4);MCU_One(MCU_Address^Comm_ExpEule0);//�߶�����
	MCU_One(MCU_Address);MCU_One(Comm_ExpEule1);MCU_send(MyStruct.ExpEule1.expeule1Buf,4);MCU_One(MCU_Address^Comm_ExpEule1);//�߶�����
	MCU_One(MCU_Address);MCU_One(Comm_ExpEule2);MCU_send(MyStruct.ExpEule2.expeule2Buf,4);MCU_One(MCU_Address^Comm_ExpEule2);//�߶�����
}

void MCU_Open(void)
{
	MCU_One(MCU_Address);MCU_One(Comm_Open);MCU_One(MCU_Address^Comm_Open);
}

void MCU_Close(void)
{
	MCU_One(MCU_Address);MCU_One(Comm_Close);MCU_One(MCU_Address^Comm_Close);
}
