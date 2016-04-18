#ifndef __GPS__H__
#define __GPS__H__
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_usart.h"
#define GPSBufDeep 1000

extern unsigned short GPS_ReadAddress; //定义当前读取指针地址
extern unsigned short GPS_WriteAddress;//定义当前收到数据指针地址
extern unsigned short GPS_RestData;    //定义剩余数据个数

//************************************************
//函数名称：void GPS_Resolver(unsigned char NewData)
//输入参数：新接受到的数据，按顺序输入
//函数功能：解析GPS信息指令
//函数返回参数：无
//************************************************
void GPS_Resolver(unsigned char NewData);
void RefreshGPS(void);//更新GPS数据
void GPS_InputDate(unsigned char DateBuf[],unsigned short DataNum);//GPS数据流输入
//信息解码
void GPS_GPRMC(void);
void GPS_GPVTG(void);
void GPS_GPGGA(void);
void GPS_GPGSA(void);
void GPS_GPGSV(void);
void GPS_GPGLL(void);
//构造结构体函数
extern UART_HandleTypeDef GPS_UartHandle;
int ToInt(unsigned char InputTable[],int StartP,int TableLength);
char ToChar(unsigned char InputTable[],int StartP,int TableLength);
float ToFloat(unsigned char InputTable[],int StartP,int TableLength);
void GPSPortInit(void);//GPS端口初始化
//定义GPS参数
//*********************GPRMC*********************//  表示推荐定位信息
struct GPRMC_PER
{
	int 						 UTC_Timer;  //UTC时间
	unsigned char 	 SetValid;//定位状态
	float 					 		Latitude;//纬度
	unsigned char 	 HalfEarth_La;//纬度半球  N  S
	float 	     	   		Longitude;//经度
	unsigned char 	 HalfEarth_Lo;//经度半球 E  W
	float 					 FaceV;//地面速度  000.0~999.9
	float						 FaceH;//地面航向 000.0-359.9 以真北方为参考基准
	int   					 UTC_Date;//UTC日期 ddmmyy  年月日
	float 					 Declination;//磁偏角  000.0-180.0
	unsigned char		 Declination_H;//磁偏角方向  E  W
	unsigned char 	 ModeAPP;//模式指示  A=自主定位，D=差分，E=估算，N=数据无效
};
extern struct GPRMC_PER GPRMC_Per;
//*********************GPVTG*********************//  地面速度信息
//地面速度信息
struct GPVTG_PER
{
	int 					HeadFromN;//以正北为参考基准的地面航向  (000-359度)
	int 					HeadFromM;//以磁北为参考基准的地面航向  (000-359度)
	float				  FaceV1;//地面速度  （000.0-999.9）
	float 				FaceV2;//地面速度（0000.0-1851.8公里/小时）
	unsigned char	ModeAPP;//模式指示  A=自主定位，D=差分，E=估算，N=数据无效
};
extern struct GPVTG_PER GPVTG_Per;

//*********************GPGGA*********************// GPS定位信息
struct GPGGA_PER
{
		int 	 				 UTC_Timer;  //UTC时间
		float  				 Latitude;//纬度
		unsigned char	 HalfEarth_La;//纬度半球  N  S
		float  				 Longitude;//经度
		unsigned char  HalfEarth_Lo;//经度半球 E  W
		unsigned char	 GPS_State;//定位状态  0=未定位，1=非差分定位，2=差分定位
		int						 UsingNum;//正在使用的卫星数  00-12
		float 				 HDOP;//水平精确度因子  0.5-99.9
		float 				 High;//海拔高度  -9999.9~9999.9米
		float 				 HighEarth;//大地水准面高度-9999.9~9999.9米
		int		 				 TimerOfD;//差分时间 从最近一次接收到差分信号开始的秒数，非差分定位此项为空
		int 	 			   D_num;//差分基站标号  0000-1023
};
extern struct GPGGA_PER GPGGA_Per;

//*********************GPGSA*********************// 当前卫星信息
struct GPGSA_PER
{
		unsigned char MODE;  //模式 M=手动  A=自动
		unsigned char SetMODE;//1=未定位  2=2D定位 3=3D定位
		int						UsingForSet;//正在用于定位的卫星号（01-23）
		float					PDOP;//PDOP综合位置精度因子 （0.5-99.9）
		float				  HDOP;//HDOP水平精度因子 （0.5-99.9）
		float				  VDOP;//VDOP垂直精度因子（0.5-99.9）
};
extern struct GPGSA_PER GPGSA_Per;
//*********************GPGSV*********************// 可见卫星数
struct GPGSV_PER
{
		int ALL_Num;//GSV语句总数
		int ThisNum;//本句GSV编号
		int ValidNum;//可见卫星的总数（00-12）
		int SerialNum;//卫星变化  01-32
		int Elevation;//卫星仰角 00-90
		int Azimuth;//卫星方位角 000-359
		int SNR;//信噪比  00-99dB
};
extern struct GPGSV_PER GPGSV_Per;
//*********************GPGLL*********************// 定位地理信息
struct GPGLL_PER
{
		float 	      Latitude;//纬度
		unsigned char HalfEarth_La;//纬度半球  N  S
		float         Longitude;//经度
		unsigned char HalfEarth_Lo;//经度半球 E  W
		int						UTC_Timer;  //UTC时间
		unsigned char SetValid;//定位状态
		unsigned char ModeAPP;//模式指示  A=自主定位，D=差分，E=估算，N=数据无效
};


#endif
