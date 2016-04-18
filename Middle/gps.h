#ifndef __GPS__H__
#define __GPS__H__
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_usart.h"
#define GPSBufDeep 1000

extern unsigned short GPS_ReadAddress; //���嵱ǰ��ȡָ���ַ
extern unsigned short GPS_WriteAddress;//���嵱ǰ�յ�����ָ���ַ
extern unsigned short GPS_RestData;    //����ʣ�����ݸ���

//************************************************
//�������ƣ�void GPS_Resolver(unsigned char NewData)
//����������½��ܵ������ݣ���˳������
//�������ܣ�����GPS��Ϣָ��
//�������ز�������
//************************************************
void GPS_Resolver(unsigned char NewData);
void RefreshGPS(void);//����GPS����
void GPS_InputDate(unsigned char DateBuf[],unsigned short DataNum);//GPS����������
//��Ϣ����
void GPS_GPRMC(void);
void GPS_GPVTG(void);
void GPS_GPGGA(void);
void GPS_GPGSA(void);
void GPS_GPGSV(void);
void GPS_GPGLL(void);
//����ṹ�庯��
extern UART_HandleTypeDef GPS_UartHandle;
int ToInt(unsigned char InputTable[],int StartP,int TableLength);
char ToChar(unsigned char InputTable[],int StartP,int TableLength);
float ToFloat(unsigned char InputTable[],int StartP,int TableLength);
void GPSPortInit(void);//GPS�˿ڳ�ʼ��
//����GPS����
//*********************GPRMC*********************//  ��ʾ�Ƽ���λ��Ϣ
struct GPRMC_PER
{
	int 						 UTC_Timer;  //UTCʱ��
	unsigned char 	 SetValid;//��λ״̬
	float 					 		Latitude;//γ��
	unsigned char 	 HalfEarth_La;//γ�Ȱ���  N  S
	float 	     	   		Longitude;//����
	unsigned char 	 HalfEarth_Lo;//���Ȱ��� E  W
	float 					 FaceV;//�����ٶ�  000.0~999.9
	float						 FaceH;//���溽�� 000.0-359.9 ���汱��Ϊ�ο���׼
	int   					 UTC_Date;//UTC���� ddmmyy  ������
	float 					 Declination;//��ƫ��  000.0-180.0
	unsigned char		 Declination_H;//��ƫ�Ƿ���  E  W
	unsigned char 	 ModeAPP;//ģʽָʾ  A=������λ��D=��֣�E=���㣬N=������Ч
};
extern struct GPRMC_PER GPRMC_Per;
//*********************GPVTG*********************//  �����ٶ���Ϣ
//�����ٶ���Ϣ
struct GPVTG_PER
{
	int 					HeadFromN;//������Ϊ�ο���׼�ĵ��溽��  (000-359��)
	int 					HeadFromM;//�Դű�Ϊ�ο���׼�ĵ��溽��  (000-359��)
	float				  FaceV1;//�����ٶ�  ��000.0-999.9��
	float 				FaceV2;//�����ٶȣ�0000.0-1851.8����/Сʱ��
	unsigned char	ModeAPP;//ģʽָʾ  A=������λ��D=��֣�E=���㣬N=������Ч
};
extern struct GPVTG_PER GPVTG_Per;

//*********************GPGGA*********************// GPS��λ��Ϣ
struct GPGGA_PER
{
		int 	 				 UTC_Timer;  //UTCʱ��
		float  				 Latitude;//γ��
		unsigned char	 HalfEarth_La;//γ�Ȱ���  N  S
		float  				 Longitude;//����
		unsigned char  HalfEarth_Lo;//���Ȱ��� E  W
		unsigned char	 GPS_State;//��λ״̬  0=δ��λ��1=�ǲ�ֶ�λ��2=��ֶ�λ
		int						 UsingNum;//����ʹ�õ�������  00-12
		float 				 HDOP;//ˮƽ��ȷ������  0.5-99.9
		float 				 High;//���θ߶�  -9999.9~9999.9��
		float 				 HighEarth;//���ˮ׼��߶�-9999.9~9999.9��
		int		 				 TimerOfD;//���ʱ�� �����һ�ν��յ�����źſ�ʼ���������ǲ�ֶ�λ����Ϊ��
		int 	 			   D_num;//��ֻ�վ���  0000-1023
};
extern struct GPGGA_PER GPGGA_Per;

//*********************GPGSA*********************// ��ǰ������Ϣ
struct GPGSA_PER
{
		unsigned char MODE;  //ģʽ M=�ֶ�  A=�Զ�
		unsigned char SetMODE;//1=δ��λ  2=2D��λ 3=3D��λ
		int						UsingForSet;//�������ڶ�λ�����Ǻţ�01-23��
		float					PDOP;//PDOP�ۺ�λ�þ������� ��0.5-99.9��
		float				  HDOP;//HDOPˮƽ�������� ��0.5-99.9��
		float				  VDOP;//VDOP��ֱ�������ӣ�0.5-99.9��
};
extern struct GPGSA_PER GPGSA_Per;
//*********************GPGSV*********************// �ɼ�������
struct GPGSV_PER
{
		int ALL_Num;//GSV�������
		int ThisNum;//����GSV���
		int ValidNum;//�ɼ����ǵ�������00-12��
		int SerialNum;//���Ǳ仯  01-32
		int Elevation;//�������� 00-90
		int Azimuth;//���Ƿ�λ�� 000-359
		int SNR;//�����  00-99dB
};
extern struct GPGSV_PER GPGSV_Per;
//*********************GPGLL*********************// ��λ������Ϣ
struct GPGLL_PER
{
		float 	      Latitude;//γ��
		unsigned char HalfEarth_La;//γ�Ȱ���  N  S
		float         Longitude;//����
		unsigned char HalfEarth_Lo;//���Ȱ��� E  W
		int						UTC_Timer;  //UTCʱ��
		unsigned char SetValid;//��λ״̬
		unsigned char ModeAPP;//ģʽָʾ  A=������λ��D=��֣�E=���㣬N=������Ч
};


#endif
