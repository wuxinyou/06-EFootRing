#ifndef __STM32_ADC_H
#define __STM32_ADC_H	
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"


#define STM32_ADC_CHANNEL_N	1		   
#define STM32_ADC_CACHEL_M	50
 

// extern int16_t AD_Value[10][2];   //AD采样值
 extern int16_t After_filter;   //AD滤波后，对外此数据是唯一接口，可直接用


extern void ADC1_Init(void);
extern void FilteAdcVaule(void);
extern int16_t getADCValue(void);

#ifdef __cplusplus
 }
#endif

#endif 
