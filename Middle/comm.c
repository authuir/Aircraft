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
  * @brief  ��ʼ������
  * @param  None
  * @retval None
  */
void Init_comm(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;	  				//����GPIO�ṹ��
//	USART_InitTypeDef USART_InitStructure;					//����USART�ṹ��

//	GPIO_InitStructure.Mode = 1;
}
