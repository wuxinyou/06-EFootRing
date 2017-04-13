/**
  ******************************************************************************
	*文件：MasterCtrlApp.c
	*作者：吴新有
	*版本：1.0
	*日期：2015-08-31
	*概要：导线切割主控类的实现文件
	*备注：包含LCD显示
	*
  ******************************************************************************  
	*/ 

/* 头文件包含 ------------------------------------------------------------------*/
#include "MasterCtrlApp.h"
#ifdef __cplusplus
 extern "C" {
#endif
#include <stdlib.h>
#include <math.h>								//数学函数库
#ifdef __cplusplus
 }
#endif

/* 类型定义 --------------------------------------------------------------------*/

/* 宏定义-----------------------------------------------------------------------*/ 

#define LCD_PORT_NUM 					3
#define LCD_PORT_BAUD					115200
#define TERMINAL_PORT_NUM     1      
#define TERMINAL_PORT_BAUD    38400
 #define GREENCOLOR  0x07E0
#define BLACKCOLOR  0X00
#define REDCOLOR    0xF800
#define FONT24 0X6f            //24X24
#define FONT16 0X54   
#define FONT24CH  0X6F


#define XMasterMode   65
#define YMasterMode   22
#define XSlaveMode    205
#define YSlaveMode    22
#define XMasterVol    270
#define YMasterVol    93
#define XSlaveVol     270
#define YSlaveVol     113
#define XShowBar      185
#define YShowBar      42
#define XMasterOrder  65
#define YMasterOrder  42

#define XChannelStatus     65
#define XChannelRES        155

#define XAlarm_1     240
#define YAlarm_1     166
#define XAlarm_2     240
#define YAlarm_2     186

const uint16_t YChannel[8]={86,104,123,142,161,181,199,217};


// #define BRIDGE 0;
// #define Powder 0;
// #define Energy 0;
// const char* masterMode[3]={"搭桥","火药","蓄能"};

//显示的模式
const char* BridgeMode="搭桥";
const char* PowderMode="火药";
const char* EnergyMode="蓄能";
const char* OtherMode="    ";

//显示的状态
const char* Checking="正在自检";
const char* CheckDone="自检完成";

const char* ReadyCut="准备切割";
const char* Cuting="正在切割";
const char* CutDone="切割完成";
const char* CutError="切割异常";

const char* ReadyFire="准备点火";
const char* Firing="正在点火";
const char* FireDone="点火完成";
const char* FireError="点火异常";

const char* ReadyShort="准备断路";
const char* Shorting="正在断路";
const char* ShortDone="断路完成";
const char* ShortError="断路异常";


// const char* OtherMode="  ##  ";

// const char* Stop="短路";
// const char* ResMeasure="测量";
// const char* MeasureDone="测量完";
// const char* FireStart="点火";
// const char* FireDone="点火完";
// const char* OtherStatus="  ##  ";

const char* Alarm1="模式正常";
const char* Alarm2="模式异常";	

const char* Alarm3="通信正常";
const char* Alarm4="通信异常";
//构造函数
MasterCtrlApp::MasterCtrlApp()
{
		

	led_1=new LEDLighter(GPIOA,GPIO_Pin_1);                //程序执行指示灯
	
	ctrlFuseLed =new LEDLighter(GPIOB,GPIO_Pin_3,true);      //保险灯，高电平有效
	ctrlStartLed=new LEDLighter(GPIOA,GPIO_Pin_11,true);    //启动灯，高点平有效
	
	keyBridge=new Key(GPIOB,GPIO_Pin_15);
	keyPowder=new Key(GPIOB,GPIO_Pin_14);
	keyEnergy=new Key(GPIOB,GPIO_Pin_13);
	
	keyStart=new Key(GPIOA,GPIO_Pin_12);    //启动开关
	keyFuse =new Key(GPIOA,GPIO_Pin_15);    //保险开关
	
	terminalPort=new SerialPort(TERMINAL_PORT_NUM,TERMINAL_PORT_BAUD);					//和终端通信的串口
	terminalPort->setRS485CtrTx(GPIOA,GPIO_Pin_8);                              //设置RS485_EN
	
	lcdPort=new SerialPort(LCD_PORT_NUM,LCD_PORT_BAUD);													//和LCD液晶屏通信的串口
	
	LCDManager=new LCDdisplay(lcdPort);															//控制液晶屏
	terminalManager=new TerminalControl(terminalPort);							//和从机交互

	led_1->setLightOn();                     //只要执行就一直亮
	t1=new Timer(TIM2);
	t2=new Timer(TIM3);
	t3=new Timer(TIM4);
	
	t1->setTimeOut(1);
	t1->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM2_SubPriority);

	t2->setTimeOut(15);
	t2->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM3_SubPriority);
	
	t3->setTimeOut(60);
	t3->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM4_SubPriority);

	//以下为数据初始化
	for(uint8_t i=0;i<8;i++)
	{
		slaveStatus[i]=stop;
		slaveResister[i]=0xff;
		isActionFinish[i]=false;
	}
	
	keyStartStatus=false;
	slaveMode=otherMode;
	startKeyCount=0;
	
	isSameMode=false;            //主控端和从机模式对比
	isOnLine=false;                 //是否通信正常
	
	isLandLine=false;                  //是否重新上线
	isDropLine=true;               //是否掉线
	
// 	isCheckFinish=false;
// 	isFireFinish=false;
// 	isMeasureFinish=false;
	isAction=false;                //是否已经动作
	isAllActionFinish=true;        //一定要是true
	isSystemEnd=false;
	
	slaveProgress=stop;              //初始进度为stop
// 	slaveIsActionFinish=false;
// 	slaveIsCheckFirstFinish=false;
	
	masterVoltage=0;
	slaveVoltage =0;
	
	keyStartStatus=false;             //开关状态
	keyFuseStatus =false;
	
	keyBridgeStatus=false;             //开关状态
	keyPowderStatus=false;             //开关状态
	keyEnergyStatus=false;
	
	masterMode=otherMode;
	masterCommand=cmdStop;
	
	TEST=0;          //测试运行次数
	

	

	
}

MasterCtrlApp::~MasterCtrlApp()
{
	//从机掉电
	//ctrlTerminal->setLightOn();
	
	t1->stop();
	t2->stop();
	t3->stop();
	terminalPort->close();
	lcdPort->close();
	delete terminalPort;					//和终端（XBOT控制箱）通信的串口
	delete lcdPort;							//和LCD液晶屏通信的串口
	delete t1;												//定时器1
	delete t2;												//定时器2
	delete t3;												//定时器3
	delete led_1;
	//delete ctrlTerminal;
	delete ctrlStartLed;
	delete keyBridge;
	delete keyStart;
	delete keyPowder;
	delete keyEnergy;
}

/******************************************************************
**初始化
**Modify:吴新有 7-16
********************************************************************/
void MasterCtrlApp::startWork()
{
	ADC1_Init();
	lcdPort->open();
	SysTick_DelayMs(80);
	LCDManager->ShowPic(0);    //下载主控端欢迎界面
	terminalPort->open();
	SysTick_DelayMs(1500);
	LCDManager-> SetColor(0x07E0,0x00);

	
	recogniseMode();     //识别主控端模式
	
	LCDManager->SetColor(REDCOLOR,BLACKCOLOR);//前景色设成红色
	LCDManager->DisplayTxt_FixedMode(FONT16,XAlarm_1,YAlarm_1,Alarm4);
	LCDManager->DisplayTxt_FixedMode(FONT16,XAlarm_2,YAlarm_2,Alarm2);
	LCDManager->SetColor(GREENCOLOR,BLACKCOLOR);//前景色设成绿色
	
// //打开定时器	
	t1->start();
	t2->start();
	t3->start();

	
}

/******************************************************************
**识别主控模式
**
********************************************************************/
void MasterCtrlApp::recogniseMode()
{
		//获取模式，单次操作
  keyBridgeStatus=keyBridge->getValue();  //这里绝对不能使用按键延时
	keyPowderStatus =keyPowder->getValue();
	keyEnergyStatus=keyEnergy->getValue();	
	
		//主控模式命令识别

	if(keyPowderStatus)
	{
		masterMode=powderMode;                   //火药模式
		SysTick_DelayMs(80);
		LCDManager->ShowPic(1);    //下载 火药模式 操作界面
		SysTick_DelayMs(80);
	}
	else if(keyBridgeStatus)
	{		
		masterMode=bridgeMode;                   //搭桥断路模式
		SysTick_DelayMs(80);
		LCDManager->ShowPic(3);    //下载 搭桥模式 操作界面
		SysTick_DelayMs(80);
	}
		else
	{
		masterMode=energyMode;                   //蓄能模式
		
		SysTick_DelayMs(80);
		LCDManager->ShowPic(2);    //下载 蓄能模式 操作界面
		SysTick_DelayMs(80);
	}
	
}

/**
  * 功  能：获取主控板电压值
  * 参  数：无
  * 返回值：无
  * 备  注：	
	*/
void MasterCtrlApp::getPowSample()
{
  uint32_t  tempPowerAD;					//	主控板电池电压
	tempPowerAD	=	(uint32_t)After_filter;		//	控制箱电池电压采集
	tempPowerAD	=	(tempPowerAD*61*33>>12)/10;		//电池电压=	tempPowerAD*61*3.3/10,电池电压放大10倍
	masterVoltage = uint8_t(tempPowerAD);
}




/**
  * 功  能：获取主控板按键命令
  * 参  数：无
  * 返回值：无
  * 备  注：	
	*/
void MasterCtrlApp::getCommand()
{
	
	
//根据设计要求，启动开关一旦按下，便软自锁，所以这里的按键状态一旦置位便永久置位，直到重启。
	if(!keyStartStatus)
	{
		keyFuseStatus=keyFuse->getValue(5);
		keyStartStatus=keyStart->getValue(5);
	}
	
	

	
	
//判断主机模式和从机模式是否一致
	if(masterMode==slaveMode)
		isSameMode=true;
	else 
		isSameMode=false;	
	

//如果掉线 必须重置一些从机参数
	if(!isOnLine)
	{

		isSameMode=false;
	}

	
//以下开始执行操作命令
	if(isOnLine && isSameMode && !isSystemEnd)	         //通信正常和模式一致
	{
// 		if(!keyStartStatus && !isMeasureFinish)
// 		{
// 			if(!keyBridgeStatus || keyPowderStatus)
// 				masterCommand=cmdCheckFirst;                  //电阻测量
// 			else 
// 				masterCommand=cmdStop;                   //停止命令
// 		}
		//火药模式
		if(masterMode==powderMode)
		{
			if(slaveStatus[0]==stop)
			{
				masterCommand=cmdCheckFirst;
			}
			else if(!isAction  && slaveStatus[0]!=checkSecond)
			{
				if(keyStartStatus)
					masterCommand=cmdReadyAction;		
				else
					masterCommand=cmdCheckFirst;
			}
			else if(isAction && slaveStatus[0]==actionStart)
			{
				masterCommand=cmdAction; 
			}
			else if(slaveStatus[0]==actionDone)
			{
				masterCommand=cmdCheckSecond;                     //查询切割是否完成
// 				confirmActionFinish();  //注释  
			}
			else if(slaveStatus[0]==checkSecond)
			{
				confirmActionFinish();//修改，添加，解决了点火完成后出现点火异常的问题 20161010
				masterCommand=cmdStop;
				isSystemEnd=true;
			}
//	else if()
// 			else if(!isAction)
// 			{
// 				
// 			}
				
			
// 			if(keyStartStatus && !isAction)  //如果启动按下，则发送预启动命令，通知终端蜂鸣器动作。
// 			{
// 				masterCommand=cmdReadyAction;
// 			}
// 			if(slaveIsActionFinish)  //如果收到动作完成，则发送第二次测量电阻，以查询动作是否完成。
// 			{
// 				masterCommand=cmdCheckSecond;                     //查询切割是否完成
// 				confirmActionFinish();
// 			}	
// 							
// 			
		}
		//蓄能模式
		else if(masterMode==energyMode)
		{
			if(keyStartStatus && !isAction)  //如果启动按下，则发送预启动命令，通知终端蜂鸣器动作。
			{
				masterCommand=cmdReadyAction;
			}
			else if(isAction)
			{
				masterCommand=cmdAction; 
				isSystemEnd=true;
			}
			else 
			{
				masterCommand=cmdStop;
			}			
		}
		//搭桥模式
		else if(masterMode==bridgeMode)
		{
			
			if(keyStartStatus && !isAction)  //如果启动按下，则发送预启动命令，通知终端蜂鸣器动作。
			{
				masterCommand=cmdReadyAction;
			}
			else if(isAction)
			{
				masterCommand=cmdAction; 
				isSystemEnd=true;
			}
			else 
			{
				masterCommand=cmdStop;
			}
			
		}	

	}
	//系统动作结束
	else if(isSystemEnd)
	{
		if(masterMode==powderMode)
			masterCommand=cmdCheckSecond;
		
	}
	else if(isOnLine)
	{
		if(masterMode==powderMode)
			masterCommand=cmdStop;

	}	
	else
	{
		;
		//slaveMode=otherMode;   //otherMode就是不显示
	}


}
/**
  * 功  能：进度条计数
  * 参  数：无
  * 返回值：无
  * 修  改：吴新有 11-11
  * 备  注：	
	*/
void MasterCtrlApp::progressBarCount()
{
	if(isOnLine && isSameMode)	         //通信正常和模式一致
	{
			if(keyStartStatus)  //调试时用
			{
				startKeyCount++;
				if(startKeyCount>STARTKEY_MAX_TIMER)
				{
					masterCommand=cmdAction;           //发送启动命令
					isAction=true;
					startKeyCount=STARTKEY_MAX_TIMER;
				}
			}
			else
			{
				startKeyCount=0;

			}
		
	}
}
/**
  * 功  能：判断动作是否完成
  * 参  数：无
  * 返回值：无
  * 修  改：吴新有 11-17
  * 备  注：	
	*/
void MasterCtrlApp::confirmActionFinish()
{
	//static uint16_t temslaveResister[8];
	
	if(masterCommand==cmdCheckSecond)
	{
		for(int i=0;i<8;i++)
		{
			if(slaveResister[i]>200)
				isActionFinish[i]=true;	
			
			if(!isActionFinish[i])
				isAllActionFinish=false;
		}
		
		
	}
}

/**
  * 功  能：获取从机数据
  * 参  数：无
  * 返回值：无
  * 修  改：吴新有 11-11
  * 备  注：	
	*/
void MasterCtrlApp::getSlaveData()
{
	static int count=0;
	if(terminalManager->reciveData())
	{
		uint8_t *temReciveData=terminalManager->getReciveData();
		
		slaveMode=Mode(temReciveData[2]);                       //保存终端模式
		slaveVoltage=temReciveData[3] + (temReciveData[4]<<8);  //保存终端电压
		
		//保存终端状态
		for(uint8_t i=0;i<8;i++)
		{
			slaveStatus[i]=SlaveStatus(temReciveData[i+5]);
		}
		//保存终端电阻
		for(uint8_t i=0;i<8;i++)
		{
			slaveResister[i] = temReciveData[i+13];
		}
		isOnLine=true;	
		count=0;
	}
	else
	{
		if(count<MAX_DROP_FRAME) 
			count++;
		else                 //连续1000次没有收到则为通信故障
			isOnLine=false;
	}
}
/*
  * 功  能:处理接收到的从机数据
  * 参  数：无
  * 返回值：无
  * 备  注：	错误状态注释
  *         本函数已经废弃不用了  2016 10 09
	*/


void MasterCtrlApp::dealSlaveData()
{
// 	if(slaveStatus[0]==checkFirst)
// 	{ ;   }

// 	if(slaveStatus[0]==checkFirstDone)
// 	{
// 		slaveIsCheckFirstFinish=true;
// 	}
// 	if(slaveStatus[0]==actionDone)
// 	{
// 		slaveIsActionFinish=true;
// 	}

// 	if(slaveStatus[0]==checkSecondDone)
// 	{
// 		isCheckFinish=true;
// 	}

	slaveProgress=slaveStatus[0];
	
	
	
	
		
}

/**
  * 功  能：显示开关LED
  * 参  数：无
  * 返回值：无
  * 备  注：	错误状态注释
	*/
void MasterCtrlApp::showKeyLed()
{
	if(keyStartStatus)
		ctrlStartLed->setLightOn();
	else
		ctrlStartLed->setLightOff();
	
	if(keyFuseStatus)
		ctrlFuseLed->setLightOn();
	else
		ctrlFuseLed->setLightOff();	
		
}

/**
  * 功  能：更新发送的数据
  * 参  数：无
  * 返回值：无
  * 备  注：	错误状态注释
	*/

void MasterCtrlApp::updateSlavedata()
{
	terminalManager->updateSendBuf(masterMode,masterCommand);
	
	//test data
	//terminalManager->updateSendBuf(TEST,isDropLine);
	terminalManager->sendcommand();
}



/**************************************************************************************
**以下是LCD显示功能函数**
***************************************************************************************/

/**
  * 功  能：显示主控端和从机模式
  * 参  数：无
  * 返回值：无
  * 
  */
void MasterCtrlApp::showMode()
{
		//显示主控端模式
	static Mode temMasterMode=otherMode;
	static Mode temSlaveMode=otherMode;
	static bool showModeTemIsOnLine=false;
	
	if(temMasterMode!=masterMode)
	{
		temMasterMode=masterMode;
		if(masterMode==bridgeMode)
			LCDManager->DisplayTxt_FixedMode(FONT16,XMasterMode,YMasterMode,BridgeMode);//masterModeBridge
		else if(masterMode==powderMode)
			LCDManager->DisplayTxt_FixedMode(FONT16,XMasterMode,YMasterMode,PowderMode);
		else
			LCDManager->DisplayTxt_FixedMode(FONT16,XMasterMode,YMasterMode,EnergyMode);	
	}
	
		//显示从机模式
	
	//掉线或上线重置参数值
	if(showModeTemIsOnLine!=isOnLine)
	{
		showModeTemIsOnLine=isOnLine;
		temSlaveMode=otherMode;
	}	
	
	//显示
	if(temSlaveMode!=slaveMode)
	{
		temSlaveMode=slaveMode;
		if(!isOnLine)
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveMode,YSlaveMode,"        ");
		else if(slaveMode==0x01)
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveMode,YSlaveMode,BridgeMode);
		else if(slaveMode==0x02)
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveMode,YSlaveMode,PowderMode);
		else if(slaveMode==0x03)
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveMode,YSlaveMode,EnergyMode);
		else
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveMode,YSlaveMode,OtherMode);	
	}
}
/**
  * 功  能：显示主控端命令
  * 参  数：无
  * 返回值：无
	* 修  改：分模式显示
  */
void MasterCtrlApp::showMasterCommand()
{
	//显示主控端命令
	static MasterCommand temmasterCommand=cmdOther;
	if(temmasterCommand==cmdStop)    //不知道说明原因不能初始化为cmdOther
		temmasterCommand=cmdOther;
	
	//掉线或上线重置参数值
	//Add code  here

	//这里的判断是为了通讯掉线而设置的判断，如果没有掉线重连，以下代码只需要运行一次即可
	if(temmasterCommand!=masterCommand ) 
	{
		temmasterCommand=masterCommand;	
		
		//火药模式
		if(masterMode==powderMode)
		{
			if(masterCommand==cmdStop)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,Checking);
			else if(masterCommand==cmdCheckFirst)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,CheckDone);
			else if(masterCommand==cmdReadyAction)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,ReadyFire);
			else if(masterCommand==cmdAction)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,Firing);
			else if(isAllActionFinish)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,FireDone);
			else
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,FireError);				
				
		}
		//蓄能模式
		else if(masterMode==energyMode)
		{
			if(!isOnLine)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,Checking);
			else if(masterCommand==cmdStop)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,CheckDone);		
			else if(masterCommand==cmdReadyAction)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,ReadyCut);
			else if(masterCommand==cmdAction)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,CutDone);			

		}
		//搭桥模式
		else if(masterMode==bridgeMode)											
		{
			if(!isOnLine)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,Checking);
			else if(masterCommand==cmdStop)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,CheckDone);		
			else if(masterCommand==cmdReadyAction)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,ReadyShort);
			else if(masterCommand==cmdAction)
				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,ShortDone);
// 			else if(isAllActionFinish)
// 				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,ShortDone);		
// 			else 
// 				LCDManager->DisplayTxt_FixedMode(FONT16,XMasterOrder,YMasterOrder,ShortError);
		}
			

	}
		
}
/**
  * 功  能：显示执行动作进度条
  * 参  数：无
  * 返回值：无
  */
void MasterCtrlApp::showProgressBar()
{
	//显示进度条
	static int temstartKeyCount=-1;
	int startKeyCount_1; 
	
	progressBarCount();
	
	if(temstartKeyCount!=startKeyCount)
	{
		temstartKeyCount=startKeyCount;
		startKeyCount_1=int((120.0/STARTKEY_MAX_TIMER)*double(startKeyCount));    //进度条与最大计数值比例放大
		if(startKeyCount_1<=144)
		{
			if(startKeyCount_1>100)
			{
				LCDManager->SetColor(REDCOLOR,BLACKCOLOR);//前景色设成红色
				LCDManager->ShowBar(XShowBar,YShowBar,startKeyCount_1);
				LCDManager->SetColor(GREENCOLOR,BLACKCOLOR);//前景色设成绿色	
			}
			else
			{
				LCDManager->ShowBar(XShowBar,YShowBar,startKeyCount_1);
			}
		}
	}
// 	else if(temstartKeyCount==-1)
// 		LCDManager->ShowBar(XShowBar,YShowBar,0);\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

}
/**
  * 功  能：显示主控端电压
  * 参  数：无
  * 返回值：无
  */
void MasterCtrlApp::showVoltage()
{

	char TempChar[5] = {0};
	static uint8_t temMasterVol = 0xff;  
	static uint16_t temSlaveVol = 0xffff;
	static bool showVoltageTemIsOnLine=false;


//显示主控端电压
	if(temMasterVol>masterVoltage+1 || temMasterVol<masterVoltage-1)      //施密特消抖处理
	{
		temMasterVol=masterVoltage;
 		
		uint162str_voltage(temMasterVol,(char *)TempChar,5);
		
		if(temMasterVol<74 )
		{
			LCDManager->SetColor(0xF800,0x00);//前景色设成红色
			LCDManager->DisplayTxt_FixedMode(FONT16,XMasterVol,YMasterVol,(char *)TempChar);
			LCDManager->SetColor(0x07E0,0x00);//前景色设成绿色
		}
		else
		{
			LCDManager->DisplayTxt_FixedMode(FONT16,XMasterVol,YMasterVol,(char *)TempChar);
		}
	}	
	
//显示从机电压
	
//掉线或上线重置参数值
	if(showVoltageTemIsOnLine!=isOnLine)
	{
		showVoltageTemIsOnLine=isOnLine;
		temSlaveVol=0xffff;
	}	
	
	//显示
	if(temSlaveVol>slaveVoltage+1 || temSlaveVol<slaveVoltage-1)      //施密特消抖处理
	{
		temSlaveVol=slaveVoltage;
 		
		uint162str_voltage(temSlaveVol,(char *)TempChar,5);
		
		if(!isOnLine)
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveVol,YSlaveVol,"     ");
		else if(temSlaveVol<111 )
		{
			LCDManager->SetColor(0xF800,0x00);//前景色设成红色
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveVol,YSlaveVol,(char *)TempChar);
			LCDManager->SetColor(0x07E0,0x00);//前景色设成绿色
		}
		else
		{
			LCDManager->DisplayTxt_FixedMode(FONT16,XSlaveVol,YSlaveVol,(char *)TempChar);
		}
	}

	
	
}
/**
  * 功  能：显示从机8通道电阻
  * 参  数：无
  * 返回值：无
  */
void MasterCtrlApp::showSlaveResister()
{
	char *strSlaveRES;
	static uint8_t temslaveResister[8]={0};//={0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
	static bool showSlaveResisterTemIsOnLine=false;
	

	//掉线或上线重置参数值
	if(showSlaveResisterTemIsOnLine!=isOnLine)
	{
		showSlaveResisterTemIsOnLine=isOnLine;
		for(int i=0;i<8;i++)
		{
				temslaveResister[i]=0;
		}
	}
	
	//显示参数值
	for(int i=0;i<8;i++)
	{

		if((temslaveResister[i]!=slaveResister[i]) && (masterMode!=bridgeMode))
		{
			temslaveResister[i]=slaveResister[i];
			if(!isOnLine)
			{
				LCDManager->CutPicAndShow(10,0,40,30,15,XChannelRES,YChannel[i]);    //显示背景黑
			}
			else if(slaveResister[i]==0xff)
				LCDManager->CutPicAndShow(10,0,40,30,15,XChannelRES,YChannel[i]);    //显示背景黑
				//strSlaveRES="    ";
			else 
				//strSlaveRES="have";
			//int2str((int)slaveResister[i],strSlaveRES,4);
			{
			uint82str_res(slaveResister[i],strSlaveRES,3);
			
			LCDManager->DisplayTxt_FixedMode(FONT16,XChannelRES,YChannel[i],strSlaveRES);		
			}
		}
		
	}
}
/*
   功  能：显示从机8通道状态
	 说  明：通过从机的数据来显示8通道状态
   参  数：无
   返回值：无
	 修  改：不同的模式显示的状态不一样，但是收到的终端状态值是固定的。
	 说明： 该函数已经被放弃使用
  */
void MasterCtrlApp::showSlaveStatus()
{
	char *strSlaveStatus;
	static uint8_t temslaveStatus[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

	if(masterMode==powderMode)
	{
		for(int i=0;i<8;i++)
	{
		if(temslaveStatus[i]!=slaveStatus[i])
		{
			temslaveStatus[i]=slaveStatus[i];
				if(slaveStatus[i]==checkFirst)
					strSlaveStatus=(char*)CheckDone;
				else if(slaveStatus[i]==checkFirstDone)
					strSlaveStatus=(char*)CheckDone;
				else if(slaveStatus[i]==actionStart)
					strSlaveStatus=(char*)Firing;
				else if(slaveStatus[i]==checkSecondDone)
					strSlaveStatus=(char*)FireDone;
				else if(slaveStatus[i]==stop)
					strSlaveStatus=(char*)Checking;
				else	strSlaveStatus=(char*)Checking;
				
			LCDManager->DisplayTxt_FixedMode(FONT16,XChannelStatus,YChannel[i],strSlaveStatus);
// 			return;
		}
	}
	}
	else if(masterMode==energyMode)
	{
		
	}
	else if(masterMode==bridgeMode)
	{
		
	}
	
}
/*
   功  能：显示从机8通道状态
	 说  明：通过主控命令来显示8通道状态
   参  数：无
   返回值：无
	 修  改：不同的模式显示的状态不一样，但是收到的终端状态值是固定的。
  */
void MasterCtrlApp::showSlaveStatus1()
{
	char *strSlaveStatus;
	static MasterCommand temMasterCommand1=cmdOther;	
	static bool showSlaveStatusTemIsOnLine=false;
	
	//掉线或上线重置参数值
	if(showSlaveStatusTemIsOnLine!=isOnLine)
	{
		showSlaveStatusTemIsOnLine=isOnLine;
		temMasterCommand1=cmdOther;
		TEST++;
	}
	
	
	if(temMasterCommand1!=masterCommand )
	{
		temMasterCommand1=masterCommand;

		if(masterMode==powderMode)       //火药模式  
		{
			for(int i=0;i<8;i++)
			{
				if(!isOnLine)
					strSlaveStatus="        ";
				else if(masterCommand==cmdStop)
					strSlaveStatus=(char*)Checking;
				else if(masterCommand==cmdCheckFirst)
					strSlaveStatus=(char*)CheckDone;
				else if(masterCommand==cmdReadyAction)
					strSlaveStatus=(char*)ReadyFire;
				else if(masterCommand==cmdAction)
					strSlaveStatus=(char*)Firing;
				else if(isActionFinish[i])
					strSlaveStatus=(char*)FireDone;
				else 
					strSlaveStatus=(char*)FireError;
				
				LCDManager->DisplayTxt_FixedMode(FONT16,XChannelStatus,YChannel[i],strSlaveStatus);			

			}
		}
		else if(masterMode==energyMode)    //储能模式  只有2通道
		{
			for(int i=0;i<2;i++)
			{
				if(!isOnLine)
					strSlaveStatus="        ";
				else if((masterCommand==cmdStop) && isSameMode)  //开机并且模式相同
					strSlaveStatus=(char*)CheckDone;
				else if(masterCommand==cmdReadyAction)
					strSlaveStatus=(char*)ReadyCut;
				else if(masterCommand==cmdAction)
					strSlaveStatus=(char*)CutDone;
				
// 				else if(isActionFinish[i])
// 					strSlaveStatus=(char*)CutDone;
// 				else 
// 					strSlaveStatus=(char*)CutError;
				
				LCDManager->DisplayTxt_FixedMode(FONT16,XChannelStatus+40,YChannel[i],strSlaveStatus);			

			}
		
		}
		else if(masterMode==bridgeMode)   //搭桥模式
		{
			for(int i=0;i<8;i++)
			{
				if(!isOnLine)
					strSlaveStatus="        ";
				else if((masterCommand==cmdStop) && isSameMode) //开机并且模式相同
					strSlaveStatus=(char*)CheckDone;
				else if(masterCommand==cmdReadyAction)
					strSlaveStatus=(char*)ReadyShort;
				else if(masterCommand==cmdAction)
					strSlaveStatus=(char*)ShortDone;
				
// 				else if(isActionFinish[i])
// 					strSlaveStatus=(char*)CutDone;
// 				else 
// 					strSlaveStatus=(char*)CutError;
				
				LCDManager->DisplayTxt_FixedMode(FONT16,XChannelStatus+40,YChannel[i],strSlaveStatus);			

			}
		
		}
	
	}

}



/**
  * 功  能：显示报警信息
  * 参  数：无
  * 返回值：无
  */
void MasterCtrlApp::showAlarmInfo()
{
	//显示通讯报警信息
	static bool temIsOnLine=false;
	static bool temIsSameMode=false;
	if(temIsOnLine!=isOnLine)
	{
		temIsOnLine=isOnLine;	
		if(isOnLine)
		{
			LCDManager->DisplayTxt_FixedMode(FONT16,XAlarm_1,YAlarm_1,Alarm3);	
		}
		else
		{
			LCDManager->SetColor(REDCOLOR,BLACKCOLOR);//前景色设成红色
			LCDManager->DisplayTxt_FixedMode(FONT16,XAlarm_1,YAlarm_1,Alarm4);
			LCDManager->SetColor(GREENCOLOR,BLACKCOLOR);//前景色设成绿色				
		}
	}
	//显示模式报警信息
	if(temIsSameMode!=isSameMode)
	{
		temIsSameMode=isSameMode;
		if(isSameMode)
		{
			LCDManager->DisplayTxt_FixedMode(FONT16,XAlarm_2,YAlarm_2,Alarm1);	
		}
		else
		{
			LCDManager->SetColor(REDCOLOR,BLACKCOLOR);//前景色设成红色
			LCDManager->DisplayTxt_FixedMode(FONT16,XAlarm_2,YAlarm_2,Alarm2);
			LCDManager->SetColor(GREENCOLOR,BLACKCOLOR);//前景色设成绿色				
		}
	}
}
/**
  * 功  能：显示信号指示灯
  * 参  数：无
  * 返回值：无
  * 
  */
void MasterCtrlApp::showSignalIndicator()
{
	static uint16_t reversalIndicatorCount=0;
	
	if(isOnLine)
	{
		reversalIndicatorCount++;
		if(reversalIndicatorCount<=1)
			LCDManager->CutPicAndShow(10,80,2,15,15,300,1);    //显示灰色灯
		else 
			LCDManager->CutPicAndShow(10,60,20,15,15,300,1);    //显示绿色灯
		
		if(reversalIndicatorCount>=2)
			reversalIndicatorCount=0;
	}
	else
	{
		LCDManager->CutPicAndShow(10,100,20,15,15,300,1);      //显示红色灯
	}

}
/**
  * 功  能：显示所有LCD信息
  * 参  数：无
  * 返回值：无
  * 
  */
void MasterCtrlApp::showLCD()
{
	static uint8_t index=0;   //用于分次显示
	if(index==0)
	{
		showMode();
		showMasterCommand();
		showProgressBar();
		showAlarmInfo();
		showVoltage();
		showSignalIndicator();
		index++;
	}
	else if(index==1)
	{
		//if(isOnLine)
		//如果是搭桥模式则显示电阻，否则不显示
		if(masterMode==powderMode)
			showSlaveResister();
		index++;
	}
	else if(index==2)
	{
		//if(isOnLine)
			showSlaveStatus1();
		
			signDropLine();
			signLandLine();
		
		index=0;
	}
}

//////////////////////////////////////////显示LCD结束///////////////////////////////////////////////////////////////////


/**
  * 功  能:重新上线标志
  * 参  数：无
  * 返回值：无
  * 使用说明：一定要保证本函数的调用频率是最低的
  */
void MasterCtrlApp::signLandLine()
{
	static bool temIsLandLine=true;
  isLandLine=false;
	if(temIsLandLine!=isOnLine)
	{
		temIsLandLine=isOnLine;
		if(isOnLine)   //重新上线	
		{
			isLandLine=true;
		}
	}
	
}

void MasterCtrlApp::signDropLine()
{
	static bool temIsDropLine=true;
	isDropLine=false;
	if(temIsDropLine!= (!isOnLine))
	{
		temIsDropLine=(!isOnLine);
		if(!isOnLine)
		{
			isDropLine=true;
		}
		
	}

}

/**
  * 功  能：运行在定时器2中的函数,主要作用是检测数据
  * 参  数：无
  * 返回值：无
  * 
  */

void MasterCtrlApp::runOnTime2()
{
	getSlaveData();
	
}
/**
  * 功  能：运行在定时器3中的函数,主要作用是检测数据
  * 参  数：无
  * 返回值：无
  * 
  */
void MasterCtrlApp::runOnTime3()
{
	getPowSample();	
	getCommand();
}


void MasterCtrlApp::runOnTime4()
{
//	dealSlaveData();
	updateSlavedata();//发送到从机
	showLCD();//液晶屏显示
	//clearLCD();
	showKeyLed(); 
}





/**-------------------------------------------------------------------------------
  * 功  能：将数字转换为字符串
  * 参  数：num
  * 返回值：
  *-----------------------------------------------------------------------------*/
void MasterCtrlApp::double2str(double num, char *str,int n,int size)
{
	char buf[10] = "";
	int i = 0;
	int len = 0;
	int temSize=size;
	int temp = num*pow(10.0, n);
	temp = temp < 0 ? -temp: temp;
	if (str == NULL)
	{
		return;
	}
	
	do
	{
		buf[i++] = (temp % 10) + '0';
		if(i==n)
			buf[i++] ='.';
		temp = temp / 10;
	}
	while(temp!=0||i<n+1);
	if(i==n+1)
		buf[i++] ='0';
	 len = num < 0 ? ++i: i;
	 if(temSize<len)
		 temSize=len;
	 for(int k=len;k<temSize;k++)
	 {
			str[k]=' ';//使用空格占位
	 }
	 str[temSize] = 0;//最高位加上/0
	 while(1)
	 {
			 i--;
			 if (buf[len-i-1] ==0)
			 {
					 break;
			 }
			 str[i] = buf[len-i-1];
	 }
	 if (i == 0 )
	 {
			 str[i] = '-';
	 }
}

void MasterCtrlApp::int2str(int num, char *str,int size)
{
	int temSize=size;
	char buf[10] = "";
	int i = 0;
	int len = 0;
	int temp = num < 0 ? -num: num;
	 if (str == NULL)
	 {
			 return;
	 }
	 do
	 {
			 buf[i++] = (temp % 10) + '0';
			 temp = temp / 10;
	 }while(temp);
	 len = num < 0 ? ++i: i;
	 if(temSize<len)
		 temSize=len;
	 for(int k=len;k<temSize;k++)
	 {
			str[k]=' ';//使用空格占位
	 }
	 str[temSize] = 0;
	 while(1)
	 {
			 i--;
			 if (buf[len-i-1] ==0)
			 {
					 break;
			 }
			 str[i] = buf[len-i-1];
	 }
	 if (i == 0 )
	 {
			 str[i] = '-';
	 }
}


void MasterCtrlApp::uint162str_voltage(uint16_t num,char *str,int size)
{
	
	uint8_t i=0;
	
	if(num==0)
		str[i++] = '0';
	else
	{
				 if(num>=100)
						str[i++] = num/100+'0';  
				 str[i++] = (num%100)/10+'0';
				 str[i++] = '.';
				 str[i++] = num%10 + '0';
	 }
	 str[i++] =  'V';

	while(i<size)
		str[i++] = ' ';
	str[i]=0;	


}

void MasterCtrlApp::uint82str_res(uint8_t num,char *str,int size)
{
	uint8_t i=0;
	
	if(num==0)
		str[i++] = '0';
	else
	{
				 if(num>=100)
						str[i++] = num/100+'0';  
				 if(num>=10)
				 str[i++] = (num%100)/10+'0';			
				 
				 str[i++] = num%10 + '0';
	 }
	
	while(i<size)
		str[i++] = ' ';
	str[i]=0;	
}

