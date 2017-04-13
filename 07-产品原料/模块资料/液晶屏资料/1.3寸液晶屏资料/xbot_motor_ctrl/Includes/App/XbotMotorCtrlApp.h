/**
  ******************************************************************************
	*文件：XbotApp.h
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：控制Xbot的主类的头文件
	*备注：类成员中有很多静态成员变量，为的是回调函数使用，如果有必要的话，可以修改调用方式设为普通的成员
	*
  ******************************************************************************  
	*/ 
	
/* 定义防止递归包含的控制宏----------------------------------------------------*/
#ifndef _XBOT_MOTOR_CTRL_APP_H
#define _XBOT_MOTOR_CTRL_APP_H

/* 头文件包含 ------------------------------------------------------------------*/
#include "stm32f10x.h"
// #include "WatchDag.h"
#include "SerialPort.h"
#include "SpiPort.h"
#include "LCDdisplay.h"
#include "TerminalControl.h"
#include "Timer.h"
#include "LEDLighter.h"
#include "Key.h"
#include "STM32_SysTick.h"
#include "ADC.h" 
#include "MotorControl.h"
/* 类型定义 --------------------------------------------------------------------*/

/* 宏定义-----------------------------------------------------------------------*/ 


class XbotMotorCtrlApp
{
public:
	XbotMotorCtrlApp();
	~XbotMotorCtrlApp();
	void runOnTime2();								//运行在定时器TIM2中的函数
	void runOnTime3();								//运行在定时器TIM3中的函数
	void runOnTime4();								//运行在定时器TIM4中的函数
	void initLCD();

private:
	LCDdisplay *LCDManager;						//控制液晶屏
// 	TerminalControl *terminalManager; //和XBOT控制箱交互
  MotorControl *motor;							//机械臂电机
	SerialPort *motorPort;						//和机械臂通信的串口
// 	SerialPort *terminalPort;					//和XBOT控制箱交互的串口
	SpiPort *lcdPort;									//和LCD液晶屏通信的串口
	Timer *t1;												//定时器1
	Timer *t2;												//定时器2
	Timer *t3;												//定时器3

	LEDLighter *led;

	Key *key_M;
	Key *key_A;
	Key *key_B;
	Key *key_C;
	Key *key_D;
	int8_t keysValue;				//按键的值
	int16_t speed_v;
	int16_t speed_u;
	int16_t *maxAngle;
	int16_t *minAngle;
	int16_t maxEle;					//最大电流值
	int16_t eleValue;	 			//当前的电流值
	RingChain *ele;					//电流柱形图高度缓存 波形图数据
	int8_t interfaceIndex; 	//界面编号
	int8_t motorIndex;			//电机编号 正好对应电机的地址
	int16_t angle;					//角度值
	int8_t barValue;
	int8_t ctrlModel;				//控制模式 0 手动  1 自动
	int8_t speedModel;			//速度模式 0 V模式 1 U模式
	ctrData *fBMotorAngle;	//反馈的电机角度
	ctrData *fBMotorEle;	  //反馈的电机电流
	int8_t specialCommand;	//特殊指令的标志
	int8_t speed_direction;	//速度方向 0 停止 1 负方向 2 正方向
// 	uint8_t workMode;
// 	uint8_t debugMode;
private:
// 	bool updateLCDdata();
// 	void setKeyPic(uint8_t);
// 	void setWorkMode(uint8_t);
	void disPage_start();	
	void disPage_main(int8_t,int8_t);
  void disPage_auto(int8_t,int8_t);
	void disPage_calibration(int8_t,int8_t);
	void disPage_set(int8_t,int8_t);
	void disPage_warning();	
	void disposeMotorData();
};
#endif /* _XBOT_MOTOR_CTRL_APP_H */









