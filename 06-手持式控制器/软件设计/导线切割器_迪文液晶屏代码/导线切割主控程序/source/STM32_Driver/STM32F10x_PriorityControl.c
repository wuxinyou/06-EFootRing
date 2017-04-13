#include "STM32F10x_PriorityControl.h"

void setSystemPriorityGroup()
{
		NVIC_PriorityGroupConfig(STM32_NVIC_PriorityGroup);
}
/**
  * 功  能：配置中断的优先级
  * 参  数：
	* NVIC_IRQChannel：中断号
	* NVIC_IRQChannelPreemptionPriority：抢占优先级
	* NVIC_IRQChannelSubPriority：响应优先级
  * 返回值：无
  */
void setIRQPriority(uint8_t NVIC_IRQChannel,uint8_t NVIC_IRQChannelPreemptionPriority,uint8_t NVIC_IRQChannelSubPriority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel 	= NVIC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
// 	NVIC_Init(&NVIC_InitStructure);
}




