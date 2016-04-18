#ifndef __PX4_H__
#define __PX4_H__
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_usart.h"
#define PX4FlowBufDeep      100  //定义缓冲区深度


extern unsigned short ReadAddress; //定义当前读取指针地址
extern unsigned short WriteAddress;//定义当前收到数据指针地址
extern unsigned short RestData;    //定义剩余数据个数

extern UART_HandleTypeDef PX4_UartHandle;
extern DMA_HandleTypeDef PX4DMA_Receive;
 void SendPX4(uint8_t *date,uint16_t DateSize);
 void PX4FlowPortInit(void);
//**************Port Init*************//



//*********************************************************************
extern unsigned char ReceivedBuf[PX4FlowBufDeep];
typedef struct
{
        unsigned long int  time_sec;
        unsigned char   id;
        short flow_x;
        short flow_y;
        float flow_comp_x;//Flow in m in x-sensor direction, angular-speed compensated
        float flow_comp_y;
        unsigned char quality; //Optical flow quality / confidence. 0: bad, 255: maximum quality
        float hight;//ground_distance        float        Ground distance in m. Positive value: distance known. Negative value: Unknown distance       
}FLOW;

extern FLOW MyFlow;//定义结构体
void PX4FlowPortInit(void);
float BytesToFloat(unsigned char* ByteArry);//由数组转换成float
void Flow_Mavlink(unsigned char data);//解析数据
void RefreshFlow(void);//更新参数
void InputDate(unsigned char DateBuf[],unsigned short DataNum);//输入接受到的数据

#endif
