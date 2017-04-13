#ifndef __STM32_SYSTICK_H_
#define __STM32_SYSTICK_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f10x.h"

void SysTick_Init(void); 

void SysTick_Delay(__IO uint32_t nTime);
void SysTick_TimingDelay_Decrement(void);
void SysTick_DelayUs(unsigned int time);
void SysTick_DelayMs(unsigned int time);

#ifdef __cplusplus
 }
#endif
#endif

