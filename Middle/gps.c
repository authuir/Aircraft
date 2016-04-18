#include "GPS.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
//不同定位信息存储数组
unsigned char GPRMC[200]={"$GPRMC"}, GPVTG[200]={"$GPVTG"}, GPGGA[200]={"$GPGGA"}, GPGSA[200]={"$GPGSA"}, GPGSV[200]={"$GPGSV"}, GPGLL[200]={"$GPGLL"};
unsigned char GPRMC_P[50]={0},GPVTG_P[50]={0},GPGGA_P[50]={0},GPGSA_P[50]={0},GPGSV_P[50]={0},GPGLL_P[50]={0};//记录逗号位置
unsigned char GPRMC_P_Num=0,GPVTG_P_Num=0,GPGGA_P_Num=0,GPGSA_P_Num=0,GPGSV_P_Num=0,GPGLL_P_Num=0;//记录逗号个数
unsigned char GPRMC_Num=6,GPVTG_Num=6,GPGGA_Num=6,GPGSA_Num=6,GPGSV_Num=6,GPGLL_Num=6;
unsigned GPRMC_Flag=0,GPVTG_Flag=0,GPGGA_Flag=0,GPGSA_Flag=0,GPGSV_Flag=0,GPGLL_Flag=0;
#define  StartFlag '$' //定义起始标志
#define  PauseFlag ',' //定义逗号位
#define  EndFlag   '*' //定义结束标志

 unsigned short GPS_ReadAddress=0; //定义当前读取指针地址
 unsigned short GPS_WriteAddress=0;//定义当前收到数据指针地址
 unsigned short GPS_RestData=0;    //定义剩余数据个数
 unsigned char GPS_ReceivedBuf[GPSBufDeep]={0};
 #define GPS_GetBuf 8
 unsigned char GPS_Buf[GPS_GetBuf];
 //构建GPS信息结构体
struct GPRMC_PER GPRMC_Per;
struct GPVTG_PER GPVTG_Per;
struct GPGGA_PER GPGGA_Per;
struct GPGSA_PER GPGSA_Per;
struct GPGSV_PER GPGSV_Per;
struct GPGLL_PER GPGLL_Per;

 UART_HandleTypeDef GPS_UartHandle;
 DMA_HandleTypeDef GPSDMA_Receive;
 void GPS_UART_MspInit(UART_HandleTypeDef *huart)
 {
	 	 //配置PX4  TX,RX  GPIOA_PIN9  GPIOA_10
	 	GPIO_InitTypeDef     GPIO_GPS;
	 	/* Enable the GPIO_A clock */
	  __GPIOA_CLK_ENABLE();//打开端口时钟
	 __UART4_CLK_ENABLE();//打开串口时钟
		GPIO_GPS.Pin = GPIO_PIN_1;					//GPIOA Pin1 RX
		GPIO_GPS.Mode = GPIO_MODE_AF_PP;		//作为选择功能
		GPIO_GPS.Pull = GPIO_PULLUP;				//上拉
		GPIO_GPS.Speed = GPIO_SPEED_HIGH;		//高速
		GPIO_GPS.Alternate=GPIO_AF8_UART4;	//选择功能 作为USART功能
		HAL_GPIO_Init(GPIOA, &GPIO_GPS);  
		GPIO_GPS.Pin=GPIO_PIN_0;						//配置TX口
		HAL_GPIO_Init(GPIOA, &GPIO_GPS); 
	   
	 
		HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(UART4_IRQn);
 }
void GPSPortInit(void)
{
  
	 //*******************************************************
	 //配置USART发送与接受
		__UART4_CLK_ENABLE();
		GPS_UartHandle.Instance        = UART4;
		GPS_UartHandle.Init.BaudRate   = 115200;
		GPS_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		GPS_UartHandle.Init.StopBits   = UART_STOPBITS_1;
		GPS_UartHandle.Init.Parity     = UART_PARITY_NONE;
		GPS_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
		GPS_UartHandle.Init.Mode       = UART_MODE_TX_RX;
		GPS_UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT; 
		GPS_UartHandle.AdvancedInit.OverrunDisable=UART_ADVFEATURE_OVERRUN_DISABLE;//关闭OverRun
		HAL_UART_DeInit(&GPS_UartHandle);//卸载Uart4
		HAL_UART_Init(&GPS_UartHandle);//装载Uart4
	 		   /* NVIC for USART, to catch the RX complete */
  
  /*##-4- Put UART peripheral in reception process ###########################*/ 
    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&GPS_UartHandle, UART_IT_RXNE);	//打开接收中断
}
//************************************************
//函数名称：void GPS_Resolver(unsigned char NewData)
//输入参数：新接受到的数据，按顺序输入
//函数功能：解析GPS信息指令
//函数返回参数：无
//************************************************
void GPS_Resolver(unsigned char NewData)
{
  static char IsStart=0;
  switch(IsStart) //准备解析不同类别信息
  {
  case 0: if(NewData==StartFlag) IsStart=1;break;//表示读到起始标志位，将判断定位类别
  case 1: if(NewData=='G') IsStart=2;else IsStart=0;break;     
  case 2: if(NewData=='P') IsStart=3;else IsStart=0;break;
  case 3: 
    {
      if(NewData=='R') IsStart=4;
      else if(NewData=='V') IsStart=5;
      else if(NewData=='G') IsStart=6;
      else IsStart=0;break;
    }
  case 4:
    {
      if(NewData=='M') IsStart=7; else IsStart=0; break;
    }
  case 5:
    {
      if(NewData=='T') IsStart=8; else IsStart=0; break;
    }
  case 6:
    {
      if(NewData=='G') IsStart=9;
      else if(NewData=='S') IsStart=10;
      else if(NewData=='L') IsStart=11;
      else IsStart=0;break;
    }
  case 7:if(NewData=='C') IsStart=12; else IsStart=0; break;
  case 8:if(NewData=='G') IsStart=13; else IsStart=0; break;
  case 9:if(NewData=='A') IsStart=14; else IsStart=0; break;
  case 10:
    {
      if(NewData=='A') IsStart=15; 
      else if(NewData=='V') IsStart=16;
      else IsStart=16;break;
    }
  case 11:if(NewData=='L') IsStart=17; else IsStart=0; break;
  //以上解析完毕，下面的代码是向各数组赋值并记录
  case 12://GPRMC信息
    {
      if(NewData!=EndFlag)
      {
        GPRMC[GPRMC_Num++]=NewData;
        if(NewData==PauseFlag)
          GPRMC_P[GPRMC_P_Num++]=GPRMC_Num-1;
      }
      else
      {
        GPRMC[GPRMC_Num++]=EndFlag;
        GPRMC_Flag=1;//表示接受完毕
        IsStart=0;//重新接受标志
      }break;
    }
  case 13://GPVTG信息
    {
      if(NewData!=EndFlag)
      {
        GPVTG[GPVTG_Num++]=NewData;
        if(NewData==PauseFlag)
          GPVTG_P[GPVTG_P_Num++]=GPVTG_Num-1;
      }
      else
      {
        GPVTG[GPVTG_Num++]=EndFlag;
        GPVTG_Flag=1;//表示接受完毕
        IsStart=0;//重新接受标志
      }break;
    }
  case 14://GPGGA信息
    {
      if(NewData!=EndFlag)
      {
        GPGGA[GPGGA_Num++]=NewData;
        if(NewData==PauseFlag)
          GPGGA_P[GPGGA_P_Num++]=GPGGA_Num-1;
      }
      else
      {
        GPGGA[GPGGA_Num++]=EndFlag;
        GPGGA_Flag=1;//表示接受完毕
        IsStart=0;//重新接受标志
      }break;
    }
  case 15://GPGSA信息
    {
      if(NewData!=EndFlag)
      {
        GPGSA[GPGSA_Num++]=NewData;
        if(NewData==PauseFlag)
          GPGSA_P[GPGSA_P_Num++]=GPGSA_Num-1;
      }
      else
      {
        GPGSA[GPGSA_Num++]=EndFlag;
        GPGSA_Flag=1;//表示接受完毕
        IsStart=0;//重新接受标志
      }break;
    }
  case 16://GPGSV信息
    {
      if(NewData!=EndFlag)
      {
        GPGSV[GPGSV_Num++]=NewData;
        if(NewData==PauseFlag)
          GPGSV_P[GPGSV_P_Num++]=GPGSV_Num-1;
      }
      else
      {
        GPGSV[GPGSV_Num++]=EndFlag;
        GPGSV_Flag=1;//表示接受完毕
        IsStart=0;//重新接受标志
      }break;
    }
  case 17://GPGLL信息
    {
      if(NewData!=EndFlag)
      {
        GPGLL[GPGLL_Num++]=NewData;
        if(NewData==PauseFlag)
          GPGLL_P[GPGLL_P_Num++]=GPGLL_Num-1;
      }
      else
      {
        GPGLL[GPGLL_Num++]=EndFlag;
        GPGLL_Flag=1;//表示接受完毕
        IsStart=0;//重新接受标志
      }break;
    }
    default:IsStart=0;GPRMC_Num=6;GPVTG_Num=6;GPGGA_Num=6;GPGSA_Num=6;GPGSV_Num=6;GPGLL_Num=6;//清零
            GPRMC_P_Num=0,GPVTG_P_Num=0,GPGGA_P_Num=0,GPGSA_P_Num=0,GPGSV_P_Num=0,GPGLL_P_Num=0;//清零
    break;
  }
  GPS_GPRMC();
  GPS_GPVTG();
  GPS_GPGGA();
  GPS_GPGSA();
  GPS_GPGSV();
  GPS_GPGLL();
}
void RefreshGPS()
{
  for(;GPS_RestData>0;GPS_RestData--)
  {
    GPS_Resolver(GPS_ReceivedBuf[GPS_ReadAddress++]);//读取指针
    if(GPS_ReadAddress==GPSBufDeep)
      GPS_ReadAddress=0;
  }
}
void GPS_InputDate(unsigned char DateBuf[],unsigned short DataNum)
{
  int i;
       for(i=0;i<DataNum;i++)
     {
       GPS_ReceivedBuf[GPS_WriteAddress++]=DateBuf[i];//数据及地址增加
       if(GPS_WriteAddress==GPSBufDeep)
         GPS_WriteAddress=0;//数据指针清零
       GPS_RestData++;//数据个数增加
     }
}
void GPS_GPRMC(void)
{
  if(GPRMC_Flag==1)//说明数据有效，开始解析
  {
		if(GPRMC_P_Num==12)
		{
    //*******************start code here***********//
    GPRMC_Per.UTC_Timer = ToInt(GPRMC,GPRMC_P[0],GPRMC_P[1]-GPRMC_P[0]-1);
    GPRMC_Per.SetValid =ToChar(GPRMC,GPRMC_P[1],GPRMC_P[2]-GPRMC_P[1]-1);
    GPRMC_Per.Latitude = ToFloat(GPRMC,GPRMC_P[2],GPRMC_P[3]-GPRMC_P[2]-1);
    GPRMC_Per.HalfEarth_La =ToChar(GPRMC,GPRMC_P[3],GPRMC_P[4]-GPRMC_P[3]-1);
    GPRMC_Per.Longitude = ToFloat(GPRMC,GPRMC_P[4],GPRMC_P[5]-GPRMC_P[4]-1);
    GPRMC_Per.HalfEarth_Lo =ToChar(GPRMC,GPRMC_P[5],GPRMC_P[6]-GPRMC_P[5]-1);
    GPRMC_Per.FaceV = ToFloat(GPRMC,GPRMC_P[6],GPRMC_P[7]-GPRMC_P[6]-1);
    GPRMC_Per.FaceH = ToFloat(GPRMC,GPRMC_P[7],GPRMC_P[8]-GPRMC_P[7]-1);
    GPRMC_Per.UTC_Date = ToInt(GPRMC,GPRMC_P[8],GPRMC_P[9]-GPRMC_P[8]-1);
    GPRMC_Per.Declination =ToFloat(GPRMC,GPRMC_P[9],GPRMC_P[10]-GPRMC_P[9]-1);
    GPRMC_Per.FaceH = ToChar(GPRMC,GPRMC_P[10],GPRMC_P[11]-GPRMC_P[10]-1);
    GPRMC_Per.ModeAPP =ToChar(GPRMC,GPRMC_P[11],GPRMC_P[12]-GPRMC_P[11]-1);
    //**********************end code***************//
		}
    GPRMC_Num=6;GPRMC_P_Num=0;
  }
  GPRMC_Flag=0;//清除标志
}
void GPS_GPVTG(void)
{
  if(GPVTG_Flag==1)//说明数据有效，开始解析
  {
		if(GPVTG_P_Num==9)
		{
    //*******************start code here***********//
    GPVTG_Per.HeadFromN =ToInt(GPVTG,GPVTG_P[0],GPVTG_P[1]-GPVTG_P[0]-1);
    GPVTG_Per.HeadFromM = ToInt(GPVTG,GPVTG_P[2],GPVTG_P[3]-GPVTG_P[2]-1);
    GPVTG_Per.FaceV1 = ToFloat(GPVTG,GPVTG_P[4],GPVTG_P[5]-GPVTG_P[4]-1);
    GPVTG_Per.FaceV2 = ToFloat(GPVTG,GPVTG_P[6],GPVTG_P[7]-GPVTG_P[6]-1);;
    GPVTG_Per.ModeAPP = ToChar(GPVTG,GPVTG_P[8],GPVTG_P[9]-GPVTG_P[8]-1);
    //**********************end code***************//
		}
    GPVTG_Num=6;GPVTG_P_Num=0;
  }
  GPVTG_Flag=0;//清除标志
}
float Latitude2=0;
void GPS_GPGGA(void)
{
  if(GPGGA_Flag==1)//说明数据有效，开始解析
  {
		if(GPGGA_P_Num==14)
		{
    //*******************start code here***********//
    GPGGA_Per.UTC_Timer=ToInt(GPGGA,GPGGA_P[0],GPGGA_P[1]-GPGGA_P[0]-1);
		GPGGA_Per.Latitude = ToFloat(GPGGA,GPGGA_P[1], GPGGA_P[2]-GPGGA_P[1]-1);
    GPGGA_Per.HalfEarth_La = ToChar(GPGGA,GPGGA_P[2],GPGGA_P[3]-GPGGA_P[2]-1);
    GPGGA_Per.Longitude =ToFloat(GPGGA,GPGGA_P[3], GPGGA_P[4]-GPGGA_P[3]-1);;
    GPGGA_Per.HalfEarth_Lo =ToChar(GPGGA,GPGGA_P[4],GPGGA_P[5]-GPGGA_P[4]-1);
    GPGGA_Per.GPS_State = ToChar(GPGGA,GPGGA_P[5],GPGGA_P[6]-GPGGA_P[5]-1);
    GPGGA_Per.UsingNum = ToInt(GPGGA,GPGGA_P[6], GPGGA_P[7]-GPGGA_P[6]-1);
    GPGGA_Per.HDOP = ToFloat(GPGGA,GPGGA_P[7], GPGGA_P[8]-GPGGA_P[7]-1);
    GPGGA_Per.High = ToFloat(GPGGA,GPGGA_P[8], GPGGA_P[9]-GPGGA_P[8]-1);
    GPGGA_Per.HighEarth =ToFloat(GPGGA,GPGGA_P[10], GPGGA_P[11]-GPGGA_P[10]-1);
    GPGGA_Per.TimerOfD = ToInt(GPGGA,GPGGA_P[12], GPGGA_P[13]-GPGGA_P[12]-1);
    GPGGA_Per.D_num = ToInt(GPGGA,GPGGA_P[13], GPGGA_P[14]-GPGGA_P[13]-1);
		}
    
    //**********************end code***************//
    GPGGA_Num=6;GPGGA_P_Num=0;
  }
  GPGGA_Flag=0;//清除标志
}
void GPS_GPGSA(void)
{
  if(GPGSA_Flag==1)//说明数据有效，开始解析
  {
    //*******************start code here***********//

    //**********************end code***************//
    GPGSA_Num=6;GPGSA_P_Num=0;
  }
  GPGSA_Flag=0;//清除标志
}
void GPS_GPGSV(void)
{
  if(GPGSV_Flag==1)//说明数据有效，开始解析
  {
    //*******************start code here***********//
//    GPGSV_Per.ALL_Num = SubToLong(GetAll,StartPlace+PlaceOfPause[0] + 1, PlaceOfPause[1] - PlaceOfPause[0] - 1);
//    GPGSV_Per.ThisNum = SubToLong(GetAll,StartPlace+PlaceOfPause[1] + 1, PlaceOfPause[2] - PlaceOfPause[1] - 1);
//    GPGSV_Per.ValidNum = SubToLong(GetAll,StartPlace+PlaceOfPause[2] + 1, PlaceOfPause[3] - PlaceOfPause[2] - 1);
//    GPGSV_Per.SerialNum = SubToLong(GetAll,StartPlace+PlaceOfPause[3] + 1, PlaceOfPause[4] - PlaceOfPause[3] - 1);
//    GPGSV_Per.Elevation = SubToLong(GetAll,StartPlace+PlaceOfPause[4] + 1, PlaceOfPause[5] - PlaceOfPause[4] - 1);
//    GPGSV_Per.Azimuth = SubToLong(GetAll,StartPlace+PlaceOfPause[5] + 1, PlaceOfPause[6] - PlaceOfPause[5] - 1);
//    GPGSV_Per.SNR =SubToLong(GetAll,StartPlace+PlaceOfPause[6] + 1, PlaceOfPause[7] - PlaceOfPause[6] - 1);
    //**********************end code***************//
    GPGSV_Num=6;GPGSV_P_Num=0;
  }
  GPGSV_Flag=0;//清除标志
}
void GPS_GPGLL(void)
{
  if(GPGLL_Flag==1)//说明数据有效，开始解析
  {
		if(GPGLL_P_Num==7)
		{
    //*******************start code here***********//
    GPGLL_Per.Latitude = ToFloat(GPGLL,GPGLL_P[0], GPGLL_P[1]-GPGLL_P[0]-1);
    GPGLL_Per.HalfEarth_La = ToChar(GPGLL,GPGLL_P[1], GPGLL_P[2]-GPGLL_P[1]-1);
    GPGLL_Per.Longitude = ToFloat(GPGLL,GPGLL_P[2], GPGLL_P[3]-GPGLL_P[2]-1);
    GPGLL_Per.HalfEarth_Lo = ToChar(GPGLL,GPGLL_P[3], GPGLL_P[4]-GPGLL_P[3]-1);
    GPGLL_Per.UTC_Timer = ToInt(GPGLL,GPGLL_P[4], GPGLL_P[5]-GPGLL_P[4]-1);
    GPGLL_Per.SetValid = ToChar(GPGLL,GPGLL_P[5], GPGLL_P[6]-GPGLL_P[5]-1);
    GPGLL_Per.ModeAPP = ToChar(GPGLL,GPGLL_P[6], GPGLL_P[7]-GPGLL_P[6]-1);
    //**********************end code***************//
		}
			GPGLL_Num=6;GPGLL_P_Num=0;
  }
  GPGLL_Flag=0;//清除标志
}


//************************************************
//函数名称：int ToInt(unsigned char InputTable[],int StartP,int TableLength)
//输入参数：（信息数组，起始点逗号位置，长度）
//函数功能：取对应位置所有数字，方便安排后面操作32位
//函数返回参数：int
//************************************************

int ToInt(unsigned char InputTable[],int StartP,int TableLength)
{
  int i,UpOrDown=1;//计数以及正负
  int ReturnValue=0;
  for(i=0;i<TableLength;i++)
  {
    if((InputTable[StartP+i+1]>='0')&&(InputTable[StartP+i+1]<='9')) 
      ReturnValue=ReturnValue*10+InputTable[StartP+i+1]-'0';
    if((i==0)&&(InputTable[StartP+i+1]=='-'))  UpOrDown=-1; 
  }
  return ReturnValue*UpOrDown;
}
//************************************************
//函数名称：int ToFloat(unsigned char InputTable[],int StartP,int TableLength)
//输入参数：（信息数组，起始点逗号位置，长度）
//函数功能：取对应位置所有数字，方便安排后面操作32位
//函数返回参数：float
//************************************************

float ToFloat(unsigned char InputTable[],int StartP,int TableLength)
{
  int i,UpOrDown=1,D_Place=0;//计数以及正负
  float ReturnValue=0;int Fenmu=1;
  for(i=0;i<TableLength;i++)
  {
    if((InputTable[StartP+i+1]>='0')&&(InputTable[StartP+i+1]<='9'))
		{
      ReturnValue=ReturnValue*10+InputTable[StartP+i+1]-'0';
			if(D_Place==1) Fenmu*=10;
		}
		else if(InputTable[StartP+i+1]=='.') D_Place=1;
    if((i==0)&&(InputTable[StartP+i+1]=='-'))  UpOrDown=-1; 
  }
  return ReturnValue*UpOrDown/Fenmu;
}
//************************************************
//函数名称：char ToChar(unsigned char InputTable[],int StartP,int TableLength)
//输入参数：（信息数组，起始点逗号位置，长度）
//函数功能：返回一个字符，表示状态，获取状态参数时用到
//函数返回参数：char
//************************************************
#define ERRChar 0XFF
char ToChar(unsigned char InputTable[],int StartP,int TableLength)
{
  if(TableLength!=0)
  {
    return InputTable[StartP+1];
  }
  else return ERRChar;
}
