#include "px4.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
 unsigned short ReadAddress=0; //���嵱ǰ��ȡָ���ַ
 unsigned short WriteAddress=0;//���嵱ǰ�յ�����ָ���ַ
 unsigned short RestData=0;    //����ʣ�����ݸ���
 unsigned char ReceivedBuf[PX4FlowBufDeep]={0};

 #define PX4_GetBuf 8
 unsigned char PX4_Buf[PX4_GetBuf];
 FLOW MyFlow;//����ṹ��

 uint32_t sss=0;
 uint32_t ddd=10;
 UART_HandleTypeDef PX4_UartHandle;
 DMA_HandleTypeDef PX4DMA_Receive;
 void PX4_UART_MspInit(UART_HandleTypeDef *huart)
 {
	 	 //����PX4  TX,RX  GPIOA_PIN9  GPIOA_10
	 	GPIO_InitTypeDef     GPIO_PX4;
	 	/* Enable the GPIO_A clock */
	  __GPIOA_CLK_ENABLE();//�򿪶˿�ʱ��
	 __USART1_CLK_ENABLE();//�򿪴���ʱ��
		GPIO_PX4.Pin = GPIO_PIN_9;					//GPIOA Pin9 RX
		GPIO_PX4.Mode = GPIO_MODE_AF_PP;		//��Ϊѡ����
		GPIO_PX4.Pull = GPIO_PULLUP;				//����
		GPIO_PX4.Speed = GPIO_SPEED_HIGH;		//����
		GPIO_PX4.Alternate=GPIO_AF7_USART1;	//ѡ���� ��ΪUSART����
		HAL_GPIO_Init(GPIOA, &GPIO_PX4);  
		GPIO_PX4.Pin=GPIO_PIN_10;						//����RX��
		HAL_GPIO_Init(GPIOA, &GPIO_PX4); 
	   
	 
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
#if 0	/*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for reception process */
	__HAL_RCC_DMA2_CLK_ENABLE();		//��DMAʱ��
	  PX4DMA_Receive.Instance               = DMA2_Stream2;									//������2
  PX4DMA_Receive.Init.Channel             = DMA_CHANNEL_4;								//ͨ��4
  PX4DMA_Receive.Init.Direction           = DMA_PERIPH_TO_MEMORY;					//���赽�ڴ�
  PX4DMA_Receive.Init.PeriphInc           = DMA_PINC_DISABLE;							//�����ַ�Ƿ���  ������
  PX4DMA_Receive.Init.MemInc              = DMA_MINC_ENABLE;							//�ڴ��ַ�Ƿ���	 ����
  PX4DMA_Receive.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;					//�����ֽڶ���
  PX4DMA_Receive.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;					//�ڴ��ֽڶ���
  PX4DMA_Receive.Init.Mode                = DMA_NORMAL;										//DMA ģʽ ����ģʽ
  PX4DMA_Receive.Init.Priority            = DMA_PRIORITY_HIGH;						//���ȼ�
//	PX4DMA_Receive.Init.FIFOMode						= DMA_FIFOMODE_DISABLE;					//�Ƿ�ʹ��FIFO   ��     
//	PX4DMA_Receive.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL;  //ָ��FIFO����ֵˮƽ
//	PX4DMA_Receive.Init.MemBurst 						= DMA_MBURST_SINGLE;						//ָ����Burstת�������ڴ洫��
//	PX4DMA_Receive.Init.PeriphBurst 				= DMA_PBURST_SINGLE;						//ָ����Burstת��������Χת��
  HAL_DMA_Init(&PX4DMA_Receive);//DMA��ʼ��
	
	//*****************************************************����DMAģʽ  start******************//
	    /* Set the UART DMA transfer complete callback */
   // PX4_UartHandle->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;
		    /* Enable the DMA channel */
    //HAL_DMA_Start_IT(PX4_UartHandle->hdmarx, (uint32_t)&PX4_UartHandle->Instance->RDR, *(uint32_t*)PX4_Buf, PX4_GetBuf);
	//*****************************************************����DMAģʽ  start******************//
	  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(&PX4_UartHandle, hdmarx, PX4DMA_Receive);
	//DMA �ж�
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);//�������ȼ�
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);//ʹ��DMA2�ж�
	#endif
 }
 void PX4FlowPortInit(void)
 {
	 //*******************************************************
	 //����USART���������
		__USART1_CLK_ENABLE();
		PX4_UartHandle.Instance        = USART1;

		PX4_UartHandle.Init.BaudRate   = 115200;
		PX4_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		PX4_UartHandle.Init.StopBits   = UART_STOPBITS_1;
		PX4_UartHandle.Init.Parity     = UART_PARITY_NONE;
		PX4_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
		PX4_UartHandle.Init.Mode       = UART_MODE_TX_RX;
		PX4_UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT; 
		PX4_UartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//�ر�OverRun
		HAL_UART_DeInit(&PX4_UartHandle);//ж��USART1
		HAL_UART_Init(&PX4_UartHandle);//װ��USART1
	 		   /* NVIC for USART, to catch the RX complete */
  
  /*##-4- Put UART peripheral in reception process ###########################*/ 
    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&PX4_UartHandle, UART_IT_RXNE);	
 }
 void SendPX4(uint8_t *date,uint16_t DateSize)
 {
	 	HAL_UART_Transmit(&PX4_UartHandle,date,DateSize,5000);
 }
 
//************������ת����float����*********//
float BytesToFloat(unsigned char* ByteArry)
{
  return *((float*)ByteArry);
}

unsigned char FLOW_STATE[4];
unsigned char flow_buf[30];

void Flow_Mavlink(unsigned char data)
{
  /*
  ��ɫ������ʼ��־λ��stx������v1.0�汾���ԡ�FE����Ϊ��ʼ��־�������־λ��mavlink��Ϣ֡���ն˽�����Ϣ����ʱ���ô���
  
  �ڶ������Ӵ�����ǻ�ɫ���֣�payload��������Ч�غɣ�Ҫ�õ���������Ч�غ����棩���ֽڳ��ȣ�len������Χ��0��255֮�䡣��mavlink��Ϣ֡���ն˿���������ʵ���յ�����Ч�غɵĳ��ȱȽϣ�����֤��Ч�غɵĳ����Ƿ���ȷ��
  
  ���������Ӵ�����Ǳ�����Ϣ֡����ţ�seq����ÿ�η���һ����Ϣ������ֽڵ����ݻ��1���ӵ�255����0���¿�ʼ������������mavlink��Ϣ֡���ն˼�����Ϣ��ʧ�����õģ��൱�����ź�ǿ�ȡ�
  
  ���ĸ����Ӵ����˷��ͱ�����Ϣ֡���豸��ϵͳ��ţ�sys����ʹ��PIXHAWKˢPX4�̼�ʱĬ�ϵ�ϵͳ���Ϊ1������mavlink��Ϣ֡���ն�ʶ�����ĸ��豸��������Ϣ��
  
  ��������Ӵ����˷��ͱ�����Ϣ֡���豸�ĵ�Ԫ��ţ�comp����ʹ��PIXHAWKˢPX4�̼�ʱĬ�ϵĵ�Ԫ���Ϊ50������mavlink��Ϣ֡���ն�ʶ�����豸���ĸ���Ԫ��������Ϣ����ʱûʲô�ã� ��
  
  ���������Ӵ�������Ч�غ�����Ϣ���ı�ţ�msg����ע����������ǲ�ͬ�ģ�����ֽں���Ҫ��mavlink��Ϣ֡���ն�Ҫ������������ȷ����Ч�غ��ﵽ�׷���ʲô��Ϣ�������ݱ��ѡ���Ӧ�ķ�ʽ��������Ч�غ������Ϣ����
  26*/                 
  
  static unsigned char s_flow=0,data_cnt=0;
  unsigned char cnt_offset=0;        
  unsigned char get_one_fame=0;
  char floattobyte[4];
  switch(s_flow)
  {
  case 0: if(data==0xFE)
    s_flow=1;
    break;
  case 1: if(data==0x1A)
  { s_flow=2;}
  else
    s_flow=0;
    break;
  case 2:
    if(data_cnt<4)
    {s_flow=2; FLOW_STATE[data_cnt++]=data;}
    else
    {data_cnt=0;s_flow=3;flow_buf[data_cnt++]=data;}
    break;
  case 3:
    if(FLOW_STATE[3]==100)
    {
      if(data_cnt<26)
      {s_flow=3; flow_buf[data_cnt++]=data;}
      else
      {data_cnt=0;s_flow=4;}
      if(data_cnt<26) break;
    }
    else
    {
      data_cnt=0;s_flow=0;break;
    }
  case 4:get_one_fame=1;s_flow=0;data_cnt=0;break;
  default:s_flow=0;data_cnt=0;break;
  }//--end of s_uart
  
  if(get_one_fame)
  {
    MyFlow.time_sec=(((unsigned long)flow_buf[7])<<64)|((unsigned long)flow_buf[6]<<56)|((unsigned long)flow_buf[5]<<48)|((unsigned long)flow_buf[4]<<40)
      |((unsigned long)flow_buf[3]<<32)|((unsigned long)flow_buf[2]<<16)|((unsigned long)flow_buf[1]<<8)|((unsigned long)flow_buf[0]);
    floattobyte[0]=flow_buf[8];
    floattobyte[1]=flow_buf[9];
    floattobyte[2]=flow_buf[10];
    floattobyte[3]=flow_buf[11];
    MyFlow.flow_comp_x =BytesToFloat((unsigned char *)floattobyte);
    floattobyte[0]=flow_buf[12];
    floattobyte[1]=flow_buf[13];
    floattobyte[2]=flow_buf[14];
    floattobyte[3]=flow_buf[15];
    MyFlow.flow_comp_y =BytesToFloat((unsigned char *)floattobyte);
    floattobyte[0]=flow_buf[16];
    floattobyte[1]=flow_buf[17];
    floattobyte[2]=flow_buf[18];
    floattobyte[3]=flow_buf[19];
    MyFlow.hight=BytesToFloat((unsigned char *)floattobyte);//ground_distance        float        Ground distance in m. Positive value: distance known. Negative value: Unknown distance     
    MyFlow.flow_x=(short)((flow_buf[20])|(flow_buf[21]<<8));
    MyFlow.flow_y=(short)((flow_buf[22])|(flow_buf[23]<<8));
    MyFlow.id=flow_buf[24];
    MyFlow.quality=flow_buf[25]; //Optical flow quality / confidence. 0: bad, 255: maximum quality
  }
}
void RefreshFlow()
{
  for(;RestData>0;RestData--)
  {
    Flow_Mavlink(ReceivedBuf[ReadAddress++]);//��ȡָ��
    if(ReadAddress==PX4FlowBufDeep)
      ReadAddress=0;
  }
}
void InputDate(unsigned char DateBuf[],unsigned short DataNum)
{
  int i;
       for(i=0;i<DataNum;i++)
     {
       ReceivedBuf[WriteAddress++]=DateBuf[i];//���ݼ���ַ����
       if(WriteAddress==PX4FlowBufDeep)
         WriteAddress=0;//����ָ������
       RestData++;//���ݸ�������
     }
}
