/**
  ******************************************************************************
	*文件：Process.h
	*作者：吴新有
	*版本：2.0
	*日期：2017-01-09
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


//#define USE_SLEEP
/*--宏定义-引脚----------------------------------------------------------*/




//	蜂鸣器
#define	PIN_BEEP			GPIO_Pin_3	
#define	PORT_BEEP			GPIOA

//	电击控制
#define	PIN_SHOCK_POW		GPIO_Pin_2
#define	PORT_SHOCK_POW			GPIOA

#define	PIN_SHOCK		GPIO_Pin_1
#define	PORT_SHOCK			GPIOA

//	指示灯
//电路上没有

// 绿灯
#define	PIN_LED_GREEN		GPIO_Pin_5	
#define	PORT_LED_GREEN		GPIOA

//	红灯
#define	PIN_LED_RED			GPIO_Pin_6	
#define	PORT_LED_RED		GPIOA
 
//数传模块使能控制 YL_EN
#define	PIN_YL_EN		GPIO_Pin_7
#define	PORT_YL_EN		GPIOA

//数传模块使能控制 YL_SET
#define	PIN_YL_SET		GPIO_Pin_8
#define	PORT_YL_SET		GPIOA

//设备损坏检测 CutDect
#define	PIN_CUT_DECT		GPIO_Pin_11
#define	PORT_CUT_DECT		GPIOA



/*--宏定义：操作命令等--------------------------------------------------------*/


/*--宏定义:AD-----------------------------------------------------------------*/
//	电压采集
#define	NUM_AD				1
#define NUM_POWER			0

//	房间号，从001开始；每个房间内的设备ID，从01开始
#define	ID_ROOM				101		//	实际房间号
#define	ID_TERMINAL			01		//	实际本体号
#define	ID_ROOM_INVALID		000		//	无效房间号
#define	ID_TERMINAL_INVALID	0x00	//	无效本体号

//	模式：定点模式/接近模式
#define	MODE_POINT			0X00
#define	MODE_APPROACH		0X01

//	按键状态：报警/电击/默认
#define	STATE_ALARM			0X01
#define	STATE_SHOCK			0X02
#define STATE_DEFAULT		0X00

#define	KEY_DEFAULT			0X00	//	默认
#define	KEY_ALARM_SHOT		0X01	//	报警键短按
#define	KEY_ALARM_LONG		0X02	//	报警键长按
#define	KEY_SHOCK_SHOT		0X03	//	电击键短按
#define	KEY_SHOCK_LONG		0X04	//	电击键长按

class Process
{
	public:
					Process();
					~Process();
		void 		runOnTime2(void);				//	运行在定时器TIM2中的函数
		void 		runOnTime3(void);				//	运行在定时器TIM3中的函数
		void 		runOnTime4(void);				//	运行在定时器TIM4中的函数
		void 		openPeriph(void);				//	打开用外设


	
	private:
		SerialPort 	*zigbeePort;

		TerminalControl *zigbeeControl;
		Timer		*t2;							//	定时器2
		Timer		*t3;							//	定时器3
		Timer		*t4;							//	定时器4
	
		IoOut		*greenLed,*redLed;				//	红绿灯
		IoOut		*beepCtrl;					     //	控制终端输出蜂鸣器
		IoOut   *shockPowCtrl;           //	控制终端输出电击
		IoOut		*shockCtrl;						   //	控制终端输出电击
		IoOut   *YL_EN_Ctrl,*YL_SET_Ctrl;  //控制YL设备的EN，和SET


	



		uint8_t		keyState;
		uint8_t		powerVoltage;
		uint8_t		controlMode;						//控制模式模式
		uint8_t		controlStatus;					//控制状态
	
		uint16_t	RoomID;						//	接收到的房间ID
		uint16_t	TerminalID;					//	接收到的终端ID
		uint16_t  watchDogTimer;    //模拟看门狗

		void		enable_SWD_disable_Jtag(void);	//	使能SWD，关闭JTAG
		void		receiveMainData(void);				//	处理主控数据
		void	    initAllData(void);				//	全部数据初始化，包括标志位
		void		resetControlData(void);			//	重置控制数据
		void		initIOs(void);					//	初始化GPIO
		void	    initADs(void);					//	初始化AD通道
		void	    initUsarts(void);				//	初始化串口
		void	    initTimers(void);				//	初始化定时器
		void		resetTerminal(void);			//	复位
		void		disposal();				//	处理数据,并执行操作


		

		void		getPowerValue(void);			//	获取电压值
		void    showPowerStatus();

		

};
	
#endif 
/*--------------------------------- End of Process.h -----------------------------*/
