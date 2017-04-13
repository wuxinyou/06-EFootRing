/**
  ******************************************************************************
	*文件：main.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：主程序的入口
	*备注：
	*
  ******************************************************************************  
	*/ 
	
/* 宏定义-----------------------------------------------------------------------*/ 	

//看门狗只复位一次
// #define RESET_ONE

#include "STM32_SysTick.h"
#include "MasterCtrlApp.h"
	
// #include <rt_heap.h>
//  #define HEAP_SIZE   (0x300)              //堆内存池的长度3000*4=16K STM32的RAM总大小是32K
//  int32_t  heap_zone[HEAP_SIZE] = {0};       //堆内存数据全初始化为0


#define CLI()     __set_PRIMASK(1) //关闭总中断
#include <stm32f10x.h>
#define SEI()     __set_PRIMASK(0) //打开总中断
	
//全局变量
MasterCtrlApp *myApp;
LEDLighter *LED_EXCEPTION;//
int main(void)
{ 
	//SystemInit(); //这个函数应该注释掉，因为在启动文件里面已经执行了
	CLI();
	NVIC_PriorityGroupConfig(STM32_NVIC_PriorityGroup);	//优先级分组
	SysTick_Init();																			//1ms系统时钟初始化

	SysTick_DelayMs(1000);															//延时1秒，等待设备启动,小液晶屏启动 需要1S时间
	LED_EXCEPTION=new LEDLighter(GPIOA,GPIO_Pin_2);     //程序异常指示灯
	LED_EXCEPTION->setLightOn();                          //程序运行正常时常亮
	
	//复用JTAG引脚
	#if 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable ,ENABLE);
	#endif
	
	myApp=new MasterCtrlApp();
	SEI();
	myApp->startWork();
	while(1)
	{  
	
	
	}
}



/**/
extern "C" {
	void TIM2_IRQHandler(void)
	{ 
		if (Timer::getITStatus(TIM2)!= RESET) 
		{
			Timer::clearITPendingBit(TIM2);
			Timer::stop(TIM2);
			myApp->runOnTime2();
			Timer::start(TIM2);
		}
	}
	
	void TIM3_IRQHandler(void)
	{ 
		if (Timer::getITStatus(TIM3)!= RESET) 
		{
			Timer::clearITPendingBit(TIM3);
			Timer::stop(TIM3);
			myApp->runOnTime3();
			Timer::start(TIM3);
		}
	}
	
	void TIM4_IRQHandler(void)
	{ 
		if (Timer::getITStatus(TIM4)!= RESET) 
		{
			Timer::clearITPendingBit(TIM4);
			Timer::stop(TIM4);
			myApp->runOnTime4();
			Timer::start(TIM4);
		}
	}
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	LED_EXCEPTION->setLightOff();
}

/**
  * @函数说明：  	该中断函数处理硬故障异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当硬件出现故障时,异常指示灯LED_EXCEPTION以200毫秒的频率闪烁
  */
void HardFault_Handler(void)
{
// 	__IO int32_t ReturnAddr=0x00000000; 
// 	ReturnAddr = __get_MSP();//返回堆栈指针 可以在DEBUG时看到出错的地址
// 	ReturnAddr = *(int32_t *)(ReturnAddr+32); 
	/* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		SysTick_DelayMs(50);
		LED_EXCEPTION->reverseLight();
  }
}

/**
  * @函数说明：  	该中断函数处理内存管理异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当内存管理出现故障时,异常指示灯LED_EXCEPTION以500毫秒的频率闪烁
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
	LED_EXCEPTION->setLightOff();
  while (1)
  {
		SysTick_DelayMs(500);	
		LED_EXCEPTION->reverseLight();
  }
}

/**
  * @函数说明：  	该中断函数处理总线故障异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当总线出现故障时,异常指示灯LED_EXCEPTION以1秒的频率闪烁
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
		SysTick_DelayMs(1000);	
		LED_EXCEPTION->reverseLight();
  }
}

/**
  * @函数说明：  	该中断函数处理使用故障异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当使用出现故障时,异常指示灯LED_EXCEPTION以2秒的频率闪烁
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
		SysTick_DelayMs(2000);	
		LED_EXCEPTION->reverseLight();
  }
}

/**
  * @函数说明：  	该中断函数处理SVCcall故障异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当SVCcall出现故障时,异常指示灯LED_EXCEPTION熄灭
  */
void SVC_Handler(void)
{
	LED_EXCEPTION->setLightOff();
}

/**
  * @函数说明：  	该中断函数处理调试监视故障异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当调试监视出现故障时,异常指示灯LED_EXCEPTION熄灭
  */
void DebugMon_Handler(void)
{
	LED_EXCEPTION->setLightOff();
}

/**
  * @函数说明：  	该中断函数处理PendSVC异常
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			当PendSVC出现故障时,异常指示灯LED_EXCEPTION熄灭
  */
void PendSV_Handler(void)
{
	LED_EXCEPTION->setLightOff();
}

/**
  * @函数说明：  	该中断函数处理系统1ms时钟
  * @参数：				无
  * @返回值： 		无
	*	@备注： 			该中断不受CLI()影响，开关系统总中断对其没有影响
  */
void SysTick_Handler(void)
{
	SysTick_TimingDelay_Decrement();   
}

}



