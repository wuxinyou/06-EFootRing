/**
  ******************************************************************************
	*文件：MasterCtrlApp.h
	*作者：吴新有
	*版本：1.0
	*日期：2015-08-31
	*概要：导线切割主控类
	*备注：
	*
  ******************************************************************************  
	*/ 
	
/* 定义防止递归包含的控制宏----------------------------------------------------*/
#ifndef _MASTER_CTRL_APP_H
#define _MASTER_CTRL_APP_H

/* 头文件包含 ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "SerialPort.h"
#include "LCDdisplay.h"
#include "TerminalControl.h"
#include "Timer.h"
#include "LEDLighter.h"
#include "Key.h"
#include "ADC.h"
#include "STM32_SysTick.h"

/* 类型定义 --------------------------------------------------------------------*/
typedef enum 
{
	otherMode=0,
	bridgeMode,
	powderMode,
	energyMode
	
}Mode;

typedef enum 
{
	stop=0,
	checkFirst,
	checkFirstDone,
	actionStart,
	actionDone,
	checkSecond,
	checkSecondDone
	
}SlaveStatus;

typedef enum
{
	cmdStop=0,
	cmdCheckFirst=1,
	cmdCheckSecond=2,
	cmdReadyAction=3,
	cmdAction=4,
	cmdOther=5
	
}MasterCommand;



/* 宏定义-----------------------------------------------------------------------*/ 
#define MAX_DROP_FRAME  1000       //通讯最大丢失帧数
#define STARTKEY_MAX_TIMER    20.0 //启动按键延迟

/* 类定义-----------------------------------------------------------------------*/
class MasterCtrlApp
{
public:
	MasterCtrlApp();
	~MasterCtrlApp();

 	void runOnTime2();												//运行在定时器TIM2中的函数
	void runOnTime3();												//运行在定时器TIM3中的函数
	void runOnTime4();												//运行在定时器TIM4中的函数
	void startWork();
	
private:
	LCDdisplay *LCDManager;										//控制液晶屏
	TerminalControl *terminalManager; 				//和XBOT控制箱交互
	SerialPort *terminalPort;									//和终端（XBOT控制箱）通信的串口
	SerialPort *lcdPort;											//和LCD液晶屏通信的串口
	Timer *t1;																//定时器1
	Timer *t2;																//定时器2
	Timer *t3;															  //定时器3

	LEDLighter *led_1;
	LEDLighter *led_2;
	LEDLighter *ctrlFuseLed;        //保险开关灯
	LEDLighter *ctrlStartLed;        //启动开关灯

	
	Key *keyStart;                   //启动开关
	Key *keyFuse;									   //保险开关
	Key *keyPowder;                  //火药模式开关
	Key *keyBridge;                  //搭桥模式开关
	Key *keyEnergy;                  //蓄能模式开关
	
	bool keyStartStatus;             //开关状态
	bool keyFuseStatus;

	bool keyBridgeStatus;            //开关状态
	bool keyPowderStatus;            //开关状态
	bool keyEnergyStatus;            //蓄能模式开关状态

	
	uint8_t masterVoltage;
	
	Mode masterMode;
	MasterCommand masterCommand;            //主控端控制指令
// 	uint8_t sendCommand[2];         //最终发送从机的指令
	
	Mode slaveMode;                //从机模式
	uint16_t slaveVoltage;            //从机电压
	SlaveStatus slaveStatus[8];           //从机8路状态
	SlaveStatus slaveProgress;            //记录从机的执行进度
	uint8_t slaveResister[8];        //从机8路电阻值

	bool isSameMode;                 //主控端和从机模式对比
	bool isOnLine;                   //是否通信正常
	
// 	bool isMeasureFinish;            //是否测量完成
// 	bool isFireFinish;               //是否点火完成
// 	bool isCheckFinish;              //是否查询完成
// 	bool slaveIsActionFinish;
// 	bool slaveIsCheckFirstFinish;
	
	bool isAction;
	bool isActionFinish[8];
	bool isAllActionFinish;
	bool isSystemEnd;
	
	bool isLandLine;
	bool isDropLine;
	
	int startKeyCount;               //启动按键计时	
	unsigned int TEST;                //仅仅用于测试



private:
	void recogniseMode();                          //识别主控模式
  void getPowSample();                           //获取主控电压值
  void getCommand();                             //获取主控命令
	void getSlaveData();                           //获取从机数据
	void showKeyLed();                             //显示开关灯
	void updateLCDdata();                          //更新LCD数据
	void dealSlaveData();
	void updateSlavedata();                        //更新发送从机数据
	void progressBarCount();                       //进度条计数
	void confirmActionFinish();                    //确认动作完成
	

//以下是LCD的显示函数
	void showMode();
	void showVoltage();
	void showSlaveStatus();
	void showSlaveStatus1();
	void showSlaveResister();
	void showMasterCommand();
	void showProgressBar();
	void showAlarmInfo();
	void showSignalIndicator();
	void showLCD();
	void signLandLine();
	void signDropLine();    //标记掉线

//字符串转换
	void double2str(double num, char *str,int n,int size);
	void int2str(int num, char *str,int size);
	void uint162str_voltage(uint16_t num,char *str,int size);
	void uint82str_res(uint8_t num,char *str ,int size);
};
#endif /* _MASTER_CTRL_APP_H */









