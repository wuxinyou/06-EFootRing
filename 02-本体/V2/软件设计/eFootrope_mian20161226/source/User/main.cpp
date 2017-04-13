
	
/* 宏定义-----------------------------------------------------------------------*/ 	

#include "Process.h"

#define CLI()     __set_PRIMASK(1) 							//	关闭总中断
#define SEI()     __set_PRIMASK(0) 							//	打开总中断

#define	LED_SYS_PORT	GPIOA								//	系统指示灯端口
#define	LED_SYS_PIN		GPIO_Pin_8							//	系统指示灯引脚号
//全局变量
Process *process;
IoOut *ledExcp;

int main(void)
{ 
	CLI();
	NVIC_PriorityGroupConfig(STM32_NVIC_PriorityGroup);		//	中断优先级分组
	SysTick_Init();											//	SysTick初始化

//	ledExcp = new IoOut(LED_SYS_PORT,LED_SYS_PIN);
//	ledExcp->setOn();
	SysTick_DelayMs(200);
	process = new Process();
	process->openPeriph();
	SEI();
//	ledE/tOff();

	while(1)
	{  
//		调试代码
//		process->testLed->reverseState();
//		SysTick_DelayMs(500);
//		ledExcp->reverseState();
	}
}

extern "C" {
	void TIM2_IRQHandler(void)
	{ 
		if (Timer::getITStatus(TIM2)!= RESET) 
		{
			Timer::clearITPendingBit(TIM2);
			Timer::stop(TIM2);
				process->runOnTime2();
			Timer::start(TIM2);
		}
	}
	void TIM3_IRQHandler(void)
	{ 
		if (Timer::getITStatus(TIM3)!= RESET) 
		{
			Timer::clearITPendingBit(TIM3);
			Timer::stop(TIM3);
				process->runOnTime3();
			Timer::start(TIM3);
		}
	}
	void TIM4_IRQHandler(void)
	{ 
		if (Timer::getITStatus(TIM4)!= RESET) 
		{
			Timer::clearITPendingBit(TIM4);
			Timer::stop(TIM4);
				process->runOnTime4();
			Timer::start(TIM4);
		}
	}

	/**
	  * @函数说明：  	该中断函数处理NMI异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当NMI出现故障时,异常指示灯ledExcp关闭
	  */
	void NMI_Handler(void)
	{
//		ledExcp->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理硬故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当硬件出现故障时,异常指示灯ledExcp以200毫秒的频率闪烁
	  */
	void HardFault_Handler(void)
	{
		/* 当产生硬件错误异常时进入死循环 */
	  while (1)
	  {
			SysTick_DelayMs(100);
//			ledExcp->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理内存管理异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当内存管理出现故障时,异常指示灯ledExcp以500毫秒的频率闪烁
	  */
	void MemManage_Handler(void)
	{
	  /* 当产生内存管理异常时进入死循环 */
		ledExcp->setOff();
	  while (1)
	  {
			SysTick_DelayMs(500);	
//			ledExcp->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理总线故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当总线出现故障时,异常指示灯ledExcp以1秒的频率闪烁
	  */
	void BusFault_Handler(void)
	{
	  /* 当产生总线异常时进入死循环 */
	  while (1)
	  {
			SysTick_DelayMs(1000);	
//			ledExcp->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理使用故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当使用出现故障时,异常指示灯ledExcp以2秒的频率闪烁
	  */
	void UsageFault_Handler(void)
	{
	  /* 当产生使用错误异常时进入死循环 */
	  while (1)
	  {
			SysTick_DelayMs(2000);	
//			ledExcp->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理SVCcall故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当SVCcall出现故障时,异常指示灯ledExcp熄灭
	  */
	void SVC_Handler(void)
	{
//		ledExcp->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理调试监视故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当调试监视出现故障时,异常指示灯ledExcp熄灭
	  */
	void DebugMon_Handler(void)
	{
//		ledExcp->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理PendSVC异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当PendSVC出现故障时,异常指示灯ledExcp熄灭
	  */
	void PendSV_Handler(void)
	{
//		ledExcp->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理系统1ms时钟
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			该中断不受CLI()影响，开关系统总中断对其没有影响
	  */
	void SysTick_Handler(void)
	{
		  
	}

}



