
#include "STM32_GpioInit.h"
	 
void STM32_GpioOneInit(uint16_t GPIO_Pin,	GPIOSpeed_TypeDef GPIO_Speed,	\
				GPIOMode_TypeDef GPIO_Mode,	GPIO_TypeDef* GPIO_PORT)	
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin ;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed;
	GPIO_Init(GPIO_PORT, &GPIO_InitStructure);
}





