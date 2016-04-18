#include "px4.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
 unsigned short ReadAddress=0; //定义当前读取指针地址
 unsigned short WriteAddress=0;//定义当前收到数据指针地址
 unsigned short RestData=0;    //定义剩余数据个数
 unsigned char ReceivedBuf[PX4FlowBufDeep]={0};

 #define PX4_GetBuf 8
 unsigned char PX4_Buf[PX4_GetBuf];
 FLOW MyFlow;//定义结构体

 uint32_t sss=0;
 uint32_t ddd=10;
 UART_HandleTypeDef PX4_UartHandle;
 DMA_HandleTypeDef PX4DMA_Receive;
 void PX4_UART_MspInit(UART_HandleTypeDef *huart)
 {
	 	 //配置PX4  TX,RX  GPIOA_PIN9  GPIOA_10
	 	GPIO_InitTypeDef     GPIO_PX4;
	 	/* Enable the GPIO_A clock */
	  __GPIOA_CLK_ENABLE();//打开端口时钟
	 __USART1_CLK_ENABLE();//打开串口时钟
		GPIO_PX4.Pin = GPIO_PIN_9;					//GPIOA Pin9 RX
		GPIO_PX4.Mode = GPIO_MODE_AF_PP;		//作为选择功能
		GPIO_PX4.Pull = GPIO_PULLUP;				//上拉
		GPIO_PX4.Speed = GPIO_SPEED_HIGH;		//高速
		GPIO_PX4.Alternate=GPIO_AF7_USART1;	//选择功能 作为USART功能
		HAL_GPIO_Init(GPIOA, &GPIO_PX4);  
		GPIO_PX4.Pin=GPIO_PIN_10;						//配置RX口
		HAL_GPIO_Init(GPIOA, &GPIO_PX4); 
	   
	 
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
#if 0	/*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for reception process */
	__HAL_RCC_DMA2_CLK_ENABLE();		//打开DMA时钟
	  PX4DMA_Receive.Instance               = DMA2_Stream2;									//数据流2
  PX4DMA_Receive.Init.Channel             = DMA_CHANNEL_4;								//通道4
  PX4DMA_Receive.Init.Direction           = DMA_PERIPH_TO_MEMORY;					//外设到内存
  PX4DMA_Receive.Init.PeriphInc           = DMA_PINC_DISABLE;							//外设地址是否增  不增加
  PX4DMA_Receive.Init.MemInc              = DMA_MINC_ENABLE;							//内存地址是否增	 递增
  PX4DMA_Receive.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;					//外设字节对其
  PX4DMA_Receive.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;					//内存字节对其
  PX4DMA_Receive.Init.Mode                = DMA_NORMAL;										//DMA 模式 正常模式
  PX4DMA_Receive.Init.Priority            = DMA_PRIORITY_HIGH;						//优先级
//	PX4DMA_Receive.Init.FIFOMode						= DMA_FIFOMODE_DISABLE;					//是否使用FIFO   否     
//	PX4DMA_Receive.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL;  //指定FIFO的阈值水平
//	PX4DMA_Receive.Init.MemBurst 						= DMA_MBURST_SINGLE;						//指定的Burst转移配置内存传输
//	PX4DMA_Receive.Init.PeriphBurst 				= DMA_PBURST_SINGLE;						//指定的Burst转移配置外围转移
  HAL_DMA_Init(&PX4DMA_Receive);//DMA初始化
	
	//*****************************************************自配DMA模式  start******************//
	    /* Set the UART DMA transfer complete callback */
   // PX4_UartHandle->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;
		    /* Enable the DMA channel */
    //HAL_DMA_Start_IT(PX4_UartHandle->hdmarx, (uint32_t)&PX4_UartHandle->Instance->RDR, *(uint32_t*)PX4_Buf, PX4_GetBuf);
	//*****************************************************自配DMA模式  start******************//
	  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(&PX4_UartHandle, hdmarx, PX4DMA_Receive);
	//DMA 中断
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);//设置优先级
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);//使能DMA2中断
	#endif
 }
 void PX4FlowPortInit(void)
 {
	 //*******************************************************
	 //配置USART发送与接受
		__USART1_CLK_ENABLE();
		PX4_UartHandle.Instance        = USART1;

		PX4_UartHandle.Init.BaudRate   = 115200;
		PX4_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		PX4_UartHandle.Init.StopBits   = UART_STOPBITS_1;
		PX4_UartHandle.Init.Parity     = UART_PARITY_NONE;
		PX4_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
		PX4_UartHandle.Init.Mode       = UART_MODE_TX_RX;
		PX4_UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT; 
		PX4_UartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//关闭OverRun
		HAL_UART_DeInit(&PX4_UartHandle);//卸载USART1
		HAL_UART_Init(&PX4_UartHandle);//装载USART1
	 		   /* NVIC for USART, to catch the RX complete */
  
  /*##-4- Put UART peripheral in reception process ###########################*/ 
    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&PX4_UartHandle, UART_IT_RXNE);	
 }
 void SendPX4(uint8_t *date,uint16_t DateSize)
 {
	 	HAL_UART_Transmit(&PX4_UartHandle,date,DateSize,5000);
 }
 
//************由数组转换成float变量*********//
float BytesToFloat(unsigned char* ByteArry)
{
  return *((float*)ByteArry);
}

unsigned char FLOW_STATE[4];
unsigned char flow_buf[30];

void Flow_Mavlink(unsigned char data)
{
  /*
  红色的是起始标志位（stx），在v1.0版本中以“FE”作为起始标志。这个标志位在mavlink消息帧接收端进行消息解码时有用处。
  
  第二个格子代表的是灰色部分（payload，称作有效载荷，要用的数据在有效载荷里面）的字节长度（len），范围从0到255之间。在mavlink消息帧接收端可以用它和实际收到的有效载荷的长度比较，以验证有效载荷的长度是否正确。
  
  第三个格子代表的是本次消息帧的序号（seq），每次发完一个消息，这个字节的内容会加1，加到255后会从0重新开始。这个序号用于mavlink消息帧接收端计算消息丢失比例用的，相当于是信号强度。
  
  第四个格子代表了发送本条消息帧的设备的系统编号（sys），使用PIXHAWK刷PX4固件时默认的系统编号为1，用于mavlink消息帧接收端识别是哪个设备发来的消息。
  
  第五个格子代表了发送本条消息帧的设备的单元编号（comp），使用PIXHAWK刷PX4固件时默认的单元编号为50，用于mavlink消息帧接收端识别是设备的哪个单元发来的消息（暂时没什么用） 。
  
  第六个格子代表了有效载荷中消息包的编号（msg），注意它和序号是不同的，这个字节很重要，mavlink消息帧接收端要根据这个编号来确定有效载荷里到底放了什么消息包并根据编号选择对应的方式来处理有效载荷里的信息包。
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
    Flow_Mavlink(ReceivedBuf[ReadAddress++]);//读取指针
    if(ReadAddress==PX4FlowBufDeep)
      ReadAddress=0;
  }
}
void InputDate(unsigned char DateBuf[],unsigned short DataNum)
{
  int i;
       for(i=0;i<DataNum;i++)
     {
       ReceivedBuf[WriteAddress++]=DateBuf[i];//数据及地址增加
       if(WriteAddress==PX4FlowBufDeep)
         WriteAddress=0;//数据指针清零
       RestData++;//数据个数增加
     }
}
