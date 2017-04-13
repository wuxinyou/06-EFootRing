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

#include "Process.h"


#define CLI()     __set_PRIMASK(1) 							//	关闭总中断
#define SEI()     __set_PRIMASK(0) 							//	打开总中断

#define	__LEDSYS_PORT	GPIOA								//	系统指示灯端口
#define	__LEDSYS_PIN	GPIO_Pin_7							//	系统指示灯引脚号
//全局变量
Process *process;
IoOut *ledSys;

extern  vu32 TimingDelay ;
ErrorStatus HSEStartUpStatus;

void SYSCLKconfig_STOP(void);
void RCC_Configuration(void);
void EXTI_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void RTC_Configuration (void);
void SysTick_Configuration(void);    
void Delay(vu32 nCount);

 /*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)  /*初始时钟*/
{   
       RCC_DeInit();                                              /* RCC system reset(for debug purpose) */
       RCC_HSEConfig(RCC_HSE_ON);                            /* Enable HSE */
       HSEStartUpStatus = RCC_WaitForHSEStartUp();            /* Wait till HSE is ready */

  if(HSEStartUpStatus == SUCCESS)
  {
   //  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);            /* Enable Prefetch Buffer */
   //  FLASH_SetLatency(FLASH_Latency_2);             /* Flash 2 wait state */ 
     RCC_HCLKConfig(RCC_SYSCLK_Div1);             /* HCLK = SYSCLK */
     RCC_PCLK2Config(RCC_HCLK_Div1);         /* PCLK2 = HCLK */
     RCC_PCLK1Config(RCC_HCLK_Div2);                                /* PCLK1 = HCLK/2 */
     RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);          /* PLLCLK = 8MHz * 9 = 72 MHz */
     RCC_PLLCmd(ENABLE);                                          /* Enable PLL */

  RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|    /*总线APB2周围时钟设置*/
                                 RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO ,
            ENABLE 
                               );

     while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)                     /* Wait till PLL is ready */
     {

     }
     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                           /* Select PLL as system clock source */
     while(RCC_GetSYSCLKSource() != 0x08)                                /* Wait till PLL is used as system clock source */
     {
     }
  }
}


/*******************************************************************************
* Function Name  : 从停机模式下唤醒之后： 配置系统时钟允许HSE，和 pll 作为系统时钟。
* Description    : Inserts a delay time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  SYSCLKconfig_STOP(void)
  {
    RCC_HSEConfig(RCC_HSE_ON); /*HSES使能*/  
    HSEStartUpStatus = RCC_WaitForHSEStartUp(); /*等待*/
    if(HSEStartUpStatus == SUCCESS) 
       { 
      RCC_PLLCmd(ENABLE);/*使能*/
         while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)== RESET); /*等待PLL有效*/      
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/*将PLL作为系统时钟*/
   while(RCC_GetSYSCLKSource() != 0x08);/*等待*/
     } 
  }

/*******************************************************************************
* Function Name  : GPIO_Configuration();
* Description    : GPIO SET
* Input          : nCount: None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11|GPIO_Pin_12 | GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//GPIO_InitStructure指向了GPIO_InitTypeDef ，GPIO_InitTypeDef结构体包含了基本信息

    /* Configure Key Button GPIO Pin as input floating (Key Button EXTI Line) */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : GPIO_Configuration();
* Description    : GPIO SET
* Input          : nCount: None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  /* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_ClearITPendingBit(EXTI_Line0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

/*配置EXTI_Line17（RTC_Alarm）为上升沿触发*/
//   EXTI_ClearITPendingBit(EXTI_Line17);
//   EXTI_InitStructure.EXTI_Line = EXTI_Line17 ;
//   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//   EXTI_Init(&EXTI_InitStructure);

}


/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : PTC_clclk 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration (void)
  {
  /*配置RTC时钟源*/
  /* 允许访问BKP*/
 PWR_BackupAccessCmd(ENABLE); 
 /*复位备份域*/
// BKP_DeInit();
 /*允许LSE*/
 RCC_LSEConfig(RCC_LSE_ON); 
 /*等待LSE有效*/
 while (RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET) ;
 /*选择LSE做为RTC时钟 */
 RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
 /*允许RTC时钟*/
 RCC_RTCCLKCmd(ENABLE);

 /*配置RTC*/
 /*等待RTC APB同步*/
 RTC_WaitForSynchro();

 /*预分频值为1s*/

 RTC_SetPrescaler(32767);

 /*等待最后一条写指令完成*/

 RTC_WaitForLastTask();

 /*允许RTC报警中断*/
 RTC_ITConfig(RTC_IT_ALR, ENABLE);

 /*等待最后一条写指令完成*/
 RTC_WaitForLastTask(); 
  }

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
 {  
       NVIC_InitTypeDef NVIC_InitStruct; 
    /*设置中断向量基址为0x8000000*/
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStruct.NVIC_IRQChannel= RTCAlarm_IRQn ;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd= ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel= EXTI0_IRQn ;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 0;
    NVIC_Init(&NVIC_InitStruct);

 }

/*******************************************************************************
* Function Name  : SysTick_Configuration
* Description    : 配置1ms时钟
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Configuration(void)
{

}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(vu32 nTime)
 { 
   /*允许SysTick计数器*/

 }
 
 
 void StopModeTest(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	/* Configure PA.00 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Enable AFIO clock */
	
	/* Connect EXTI0 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
// 	wait_ms( 1000 );
// 	printf( "EnterStopMode\r\n");
// 	wait_ms( 1000 );

// 	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
// 	printf( "ExitStopMode\r\n");
}

/*----------------------------------------内部RC时钟配置--------------------------------------*/
void InnerRCC_Configuration(void)
{
  RCC_DeInit();//将外设RCC寄存器重设为缺省值
 
	RCC_HSEConfig(RCC_HSE_OFF);//关闭外部晶振
  RCC_HSICmd(ENABLE);//使能HSI  
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){}//等待HSI使能成功

  if(1)
  {
    //FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    //FLASH_SetLatency(FLASH_Latency_2);
   
    RCC_HCLKConfig(RCC_SYSCLK_Div1);   
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    
    //设置 PLL 时钟源及倍频数
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);                
    RCC_PLLCmd(ENABLE);//使能PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}//等待PLL初始化成功
 
    //将PLL设置为系统时钟源
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  
    while(RCC_GetSYSCLKSource() != 0x08){}//等待设置PLL为时钟源成功
  }
}
	
int main(void)
{ 

		CLI();
	//InnerRCC_Configuration();
//	SysTick_Init();											//	SysTick初始化
	

	

	NVIC_PriorityGroupConfig(STM32_NVIC_PriorityGroup);		//	中断优先级分组
	SysTick_Init();											//	SysTick初始化
	SysTick_DelayMs(1500);								
	//ledSys = new IoOut(__LEDSYS_PORT,__LEDSYS_PIN);
//	ledSys->setOn();
	
	/*	*/
	process = new Process();
	process->startWork();
	
	
	//<唤醒相关测试>

/*
	RCC_Configuration();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE );
//  GPIO_Configuration();
  EXTI_Configuration();

  NVIC_Configuration();

	PWR_WakeUpPinCmd(ENABLE);
	*/
		//</唤醒相关测试>

	SEI();


	while(1)
	{  	

		
		
	}
}


/**/
extern "C" {
	
	
	
	void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
		
	
		SystemInit();
  }
}


		
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
	  *	@备注： 			当NMI出现故障时,异常指示灯ledSys关闭
	  */
	void NMI_Handler(void)
	{
		ledSys->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理硬故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当硬件出现故障时,异常指示灯ledSys以200毫秒的频率闪烁
	  */
	void HardFault_Handler(void)
	{
		/* 当产生硬件错误异常时进入死循环 */
	  while (1)
	  {
			SysTick_DelayMs(100);
			ledSys->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理内存管理异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当内存管理出现故障时,异常指示灯ledSys以500毫秒的频率闪烁
	  */
	void MemManage_Handler(void)
	{
	  /* 当产生内存管理异常时进入死循环 */
		ledSys->setOff();
	  while (1)
	  {
			SysTick_DelayMs(500);	
			ledSys->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理总线故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当总线出现故障时,异常指示灯ledSys以1秒的频率闪烁
	  */
	void BusFault_Handler(void)
	{
	  /* 当产生总线异常时进入死循环 */
	  while (1)
	  {
			SysTick_DelayMs(1000);	
			ledSys->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理使用故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当使用出现故障时,异常指示灯ledSys以2秒的频率闪烁
	  */
	void UsageFault_Handler(void)
	{
	  /* 当产生使用错误异常时进入死循环 */
	  while (1)
	  {
			SysTick_DelayMs(2000);	
			ledSys->reverseState();
	  }
	}

	/**
	  * @函数说明：  	该中断函数处理SVCcall故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当SVCcall出现故障时,异常指示灯ledSys熄灭
	  */
	void SVC_Handler(void)
	{
		ledSys->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理调试监视故障异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当调试监视出现故障时,异常指示灯ledSys熄灭
	  */
	void DebugMon_Handler(void)
	{
		ledSys->setOff();
	}

	/**
	  * @函数说明：  	该中断函数处理PendSVC异常
	  * @参数：			无
	  * @返回值： 		无
	  *	@备注： 			当PendSVC出现故障时,异常指示灯ledSys熄灭
	  */
	void PendSV_Handler(void)
	{
		ledSys->setOff();
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
	
	
	/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
	
void RTCAlarm_IRQHandler(void)
{
   if(RTC_GetITStatus(RTC_IT_ALR)!=RESET)
   {
   
 GPIOB->ODR=0Xfff;
    EXTI_ClearITPendingBit(EXTI_Line17); 
 if(PWR_GetFlagStatus(PWR_FLAG_WU)!=RESET) 
   {
    PWR_ClearFlag(PWR_FLAG_WU);
   }
  /*等待最后一条命令写完成*/
  RTC_WaitForLastTask();
  /*清楚RTC报警挂起*/
  EXTI_ClearITPendingBit(RTC_IT_ALR);
     RTC_WaitForLastTask();
   }
}
	
}



