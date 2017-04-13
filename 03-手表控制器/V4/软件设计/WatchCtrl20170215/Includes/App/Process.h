/**
  ******************************************************************************
	*文件：Process.h
	*作者：吴新有
	*版本：1.0
	*日期：2016-12-17
	*概要：
	*备注：
	*
  ******************************************************************************  
	*/ 
	
/* 定义防止递归包含的控制宏----------------------------------------------------*/
#ifndef _PROCESS_H
#define _PROCESS_H

	/* 头文件包含 ------------------------------------------------------------------*/
	#include "DataTypeConvertion.h"
	#include "stm32f10x.h"
	//#include "Protocol.h"
	#include "TerminalControl.h"
	#include "SerialPort.h"
	#include "Timer.h"
	#include "IoOut.h"
	#include "Key.h"
	#include "STM32_SysTick.h"
	#include "STM32_ADC.h" 
	#include "stdint.h"
	
	#include "oled.h"
	#include "sqStack.h"
	
//#define USE_SLEEP	
//#define TEST
/* 引脚定义--------------------------------------------------------------------*/


#define	USE_PERIPH
#ifdef	USE_PERIPH

	
	//<按键 引脚定义>



//控制引脚
//YL_EN引脚定义

// 	#define YL_EN_PORT                GPIOA
// 	#define YL_EN_PIN                 GPIO_Pin_12
// 	#define YL_SET_PORT                GPIOA
// 	#define YL_SET_PIN                 GPIO_Pin_11	
// 	
// 	#define POW_EN_PORT                GPIOA
// 	#define POW_EN_PIN                 GPIO_Pin_1

//LED引脚
	#define LED_POW1_PORT              GPIOA
	#define LED_POW1_PIN               GPIO_Pin_0	
	
	#define LED_POW2_PORT              GPIOA
	#define LED_POW2_PIN               GPIO_Pin_7


	
	//</按键 引脚定义>
	

	
//</引脚端口定义>
	
//<AD 相关定义>
	/* AD顺序定义--------------------------------------------------------------------*/


	#define	AD_NUM_KEY1		   	0									//	按键1
	#define	AD_NUM_KEY2		   	1									//	按键2


//</AD 相关定义>

#endif
	//定时器时间
	#define	TIM2_DELAY_TIME		10
	#define	TIM3_DELAY_TIME		80
	#define	TIM4_DELAY_TIME		200

	/* 静态变量声明 --------------------------------------------------------------------*/
	


	/* 类的声明 --------------------------------------------------------------------*/
	class Process
	{
		public:
						Process();
						~Process();
			void 		runOnTime2(void);							//	运行在定时器TIM2中的函数
			void 		runOnTime3(void);							//	运行在定时器TIM3中的函数
			void 		runOnTime4(void);							//	运行在定时器TIM4中的函数
		void 		openPeriph(void);							//	打开用外设		
		
		private:
		

			
			u8 *sendData;
			u8 *receiveData;

		


			Timer		         *t2,*t3,*t4;								//	定时器2,定时器3,定时器4			
			SerialPort       *zigbeePort;
			TerminalControl  *zigbeeControl;

			//<按键实例声明>


		
		
			//IO
			IoOut   *ledPow1,*ledPow2;
// 			IoOut   *powEn;
// 			IoOut   *YL_En;
// 			IoOut   *YL_Set;


			u8 controlModeFlag;    //标识控制模式
			u8 controlStatusFlag;  //标识控制状态
			u8 isFreeMode;        
			
			u8 powerValue;   //电压值
			//AD按键值
			u8 ADKey1Value;
			u8 ADKey2Value;
		//	u8 ADKey3Value;
			
			bool isShocking;       //正在电击
			bool isAlarming;        //正在警报
			
			int watchDogTimer;
					
	
			
private:
			void	    initAllData(void);							//	全部数据初始化，包括标志位
			void			initGPIO(void);							  	//	初始化IO
			void	    initADs(void);								//	初始化AD通道
			void	    initUsarts(void);							//	初始化串口
			void	    initTimers(void);							//	初始化定时器		

			
			void    getPowerValue();
			void	  getKeyValue(void);							//	获取按键值
			void    getADKeyValue();                //	获取AD按键值

			void		dealKeyValue(void);							//	处理按键控制数据
			void    dealADKeyValue();


			void    updataToZigbee();         //更新发送数据
			void    sendCheckSignal();        //发送检测信号，用于发送FF FF AA
		
			

			void		enable_SWD_disable_Jtag(void);				//	使能SWD，关闭JTAG
			void		testWithUsart(void);						//	通过串口测试数据
	};
	
#endif 
/*--------------------------------- End of Process.h -----------------------------*/
