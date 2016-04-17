#include "stm32f7xx_hal.h"
#include <stdio.h> 

#ifdef __GNUC__  
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf 
     set to 'Yes') calls __io_putchar() */  
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)  
#else  
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)  
#endif /* __GNUC__ */ 
  
/**
  * @brief  初始化串口
  * @param  None
  * @retval None
  */
void Init_comm(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;	  				//定义GPIO结构体
//	USART_InitTypeDef USART_InitStructure;					//定义USART结构体

//	GPIO_InitStructure.Mode = 1;
}
