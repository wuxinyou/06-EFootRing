#ifndef __GPIO_INIT_H
#define __GPIO_INIT_H	
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f10x.h"
extern void STM32_GpioOneInit(uint16_t GPIO_Pin,	GPIOSpeed_TypeDef GPIO_Speed,	GPIOMode_TypeDef GPIO_Mode,	GPIO_TypeDef* GPIO_PORT);
	 
#ifdef __cplusplus
 }
#endif
#endif




