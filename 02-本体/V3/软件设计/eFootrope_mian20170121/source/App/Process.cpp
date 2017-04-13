/**
  ******************************************************************************
	*文件：Process.cpp
	*作者：吴新有
	*版本：2.0
	*日期：2017-01-09
	*概要：
	*备注：
	*
  ******************************************************************************  
	*/ 

/* 头文件包含 --------------------------------------------------------------------*/
#include "Process.h"
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdlib.h>
#ifdef __cplusplus
 }
#endif

#define TIM2_PERIOD_MS	100
#define TIM3_PERIOD_MS	30
#define TIM4_PERIOD_MS	100
/* 类的实现---------------------------------------------------------------------*/

 Process::Process()
{
	enable_SWD_disable_Jtag();								//	关闭JTAG打开SWD	
	
	initIOs();												//	初始化IO
	initADs();												//	初始化AD通道
	initUsarts();											//	初始化串口
	initTimers();											//	初始化定时器
	
	initAllData();											//  初始化所有数据

	  
	YL_EN_Ctrl->setOn();  //YL模块全速工作
	
	
	//<唤醒后马上接收数据，并处理>
	runOnTime3();
	SysTick_DelayMs(5);   //等待YL模块唤醒
	runOnTime3();
	
	//</唤醒后马上接收数据，并处理>
	//YL_SET_Ctrl->setOn();

	//beepCtrl->setOn();
	
}

void Process::openPeriph(void)
{
	SysTick_DelayMs(10);									//  延时 10ms
	zigbeePort->open();    										//  开启通信端口1
	
		//  收到数据后再 开启定时器2
	//因为没接到数据时
//	t2->start();											//  开启定时器2
	t3->start();											//  开启定时器3
	t4->start();											//  开启定时器4

}

/**
  * @brief  TIM2定时器中断函数，处理远距离报警
  * @param  None
  * @retval None
	* @定时时间：100ms
  */
void Process::runOnTime2(void)
{
#ifdef USE_DISTANCE_ALARM
	//static int beepOnTime=0;
	//距离判断一定是在通讯状态下，才能判断，所以只有在接到数据时，才打开定时器2
	//远距离
	if(distance==1)
	{
		if(!isFreeMode)
		{
			beepCtrl->setOn();
			redLed->setOn();
		}
		
//watchDogTimer=3;   //远距离，一直喂狗
		
// 		beepOnTime++;
// 		
// 		//50S后，不再进行远离检测，也就会进入休眠。
// 		if(beepOnTime>500)    
// 		{
// 			distance=0;
// 		}
		
		
	}
	//近距离
	else if(controlStatus!=1 && !isDecCut)   //没有收到警报控制,没断线
	{
		//beepOnTime=0;
		beepCtrl->setOff();
	}
#endif
}

/**
  * @brief  TIM3定时器中断函数,接收串口数据
  * @param  None
  * @retval None
	* @定时时间：30ms
  */
#define	OVER_TIME	30
void Process::runOnTime3(void)
{
	static uint16_t	overTimerCount=0;
	// 接收串口数据
	if(zigbeeControl->reciveData())							
	{
	//  收到数据后再 开启定时器2
	//因为没接到数据时
		t2->start();											
		
		overTimerCount=0;									//	超时计数复位
		watchDogTimer=3;                  //收到数据，就喂狗

		receiveMainData();							 //	接收数据
		disposal();     								 //处理数据
		
		distance=0;  //收到数据后，重置距离为0

	}
	//接收超时
	else
	{
		
		
		overTimerCount++;									//	没有收到数据电击超时
		if(overTimerCount>=OVER_TIME*4)   //超长时间没有收到数据后，停止报警
		{
			overTimerCount=0;
			beepCtrl->setOff();
			
		}
		//用于 没有收到数据后，控制电击时间
		else if(overTimerCount>=OVER_TIME)
		{		
			overTimerCount=0;
			shockCtrl->setOff();   //停止电击				
			
			resetControlData();   //没收到数据后，需要复位控制指令

		}
		//用于 清除连续按键电击的时间控制
		else if(overTimerCount>=OVER_TIME/5)
		{
			shockTime=0;
		}
		
		//脱离控制器范围
		if(overTimerCount>20)
		{
			distance=1;
		}
	}

}

/**
  * @brief  TIM4定时器中断函数,定时休眠，处理断线报警，显示电压值
  * @param  None
  * @retval None
	* @定时时间：100ms
  */
void Process::runOnTime4(void)
{
	
	getPowerValue();
	showPowerLed();
	
	detectDecCut();
	
	
#ifdef USE_SLEEP
	watchDogTimer++;
	if(watchDogTimer>50)
	{
		watchDogTimer=0;
//add you code 休眠之前执行一些操作
		SysTick_DelayMs(50);
		PWR_EnterSTANDBYMode();
//	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);   //进入睡眠
		
	}
	else if(watchDogTimer<2)
	{
		//唤醒后的初始化
	}
	
#endif
}



/**
  * @brief  初始化按键
  * @param  None
  * @retval None
  */
void Process::initIOs(void)
{

	greenLed		=	new IoOut(PORT_LED_GREEN,PIN_LED_GREEN);
	redLed			=	new	IoOut(PORT_LED_RED,PIN_LED_RED);
	
	beepCtrl		=	new	IoOut(PORT_BEEP,PIN_BEEP,true);
	shockPowCtrl= new IoOut(PORT_SHOCK_POW,PIN_SHOCK_POW);
	shockCtrl		=	new IoOut(PORT_SHOCK,PIN_SHOCK);
	
	
	YL_EN_Ctrl  = new IoOut(PORT_YL_EN,PIN_YL_EN,true);
//	YL_SET_Ctrl = new IoOut(PORT_YL_SET,PIN_YL_SET,true);
	
	


}

/**
  * @brief  初始化AD
  * @param  None
  * @retval None
  */
void Process::initADs(void)
{
	ADC1_DMA_Init();
}

/**
  * @brief  初始化定时器
  * @param  None
  * @retval None
  */
void Process::initTimers(void)
{
	t2=new Timer (TIM2);
	t2->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM2_SubPriority);
	t2->setTimeOut(TIM2_PERIOD_MS);

	t3=new Timer (TIM3);
	t3->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM3_SubPriority);
	t3->setTimeOut(TIM3_PERIOD_MS);

	t4=new Timer (TIM4);
	t4->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM4_SubPriority);
	t4->setTimeOut(TIM4_PERIOD_MS);
}

/**
  * @brief  初始化usart
  * @param  None
  * @retval None
  */
void Process::initUsarts(void)
{
	zigbeePort = new SerialPort(1,9600);
	zigbeeControl = new TerminalControl(zigbeePort);
	


}

/**
  * @brief  初始化全部数据
  * @param  None
  * @retval None
  */
void Process::initAllData(void)
{
	powerVoltage	=	0;
	decCutADValue =0;
	isDecCut=false;
	
	controlMode 		=	MODE_APPROACH;					//	默认为接近模式
	controlStatus 	=	STATE_DEFAULT;					//	默认为无按键按下
	isFreeMode      =1;                       //默认为自由模式
	
	RoomID		=	ID_ROOM_INVALID;				//	默认ID为无效
	TerminalID	=	ID_TERMINAL_INVALID;			//	默认ID为无效
	
	watchDogTimer=0;
	shockTime =0;
	
	distance =0;
	
	
	

}


/**
  * @brief  处理主控数据
  * @param  None
  * @retval None
  */
void Process::receiveMainData(void)
{
	uint8_t* dataFromMain;								//	保存接收自主控的数据
	uint16_t roomID1,roomID2,roomID3;
	uint16_t	terminalID1,terminalID2;
	
	dataFromMain = zigbeeControl->getReciveData();			//获取串口收到的数据
	
	//<just for test 2017 01 11 > 
// 	if(dataFromMain[2]==0x80)
// 	{
// 		greenLed->setOn();
// 		redLed->setOff();
// 			
// 	}
	//</just for test>
	
	
	controlMode		=	dataFromMain[3]&0x03;				//	获取控制模式
	controlStatus	=	(dataFromMain[3]>>2)&0x03;			//	获取控制状态
	isFreeMode    = (dataFromMain[3]>>4)&0x01;			//	获取控制状态
	
	
	roomID1		=	dataFromMain[4];					//	获取房间号最高位
	roomID2		=	dataFromMain[5];					//	获取房间号中间位
	roomID3		=	dataFromMain[6];					//	获取房间号最低位
	terminalID1	=	dataFromMain[7];					//	获取本体号最高位
	terminalID2	=	dataFromMain[8];					//	获取本体号最低位
	
	RoomID 		  =	roomID1*100 + roomID2*10 + roomID3;	//	计算接收到的房间号
	TerminalID	=	terminalID1*10 + terminalID2;		//	计算接收到的本体号
	

	

	
}



/**
  * @brief  处理控制数据，并驱动执行器
  * @param  None
  * @retval None
  */
// #define	BEEP_10SEC	500	//	蜂鸣器3s
// #define	SHOCK_3s	500	//	电击器2s
void Process::disposal()
{
	static	uint8_t lastMode=MODE_APPROACH;
	static  uint8_t preControlStatus=0;

	static uint16_t operateAlarmCount=0;

	
	
	if(lastMode!=controlMode)	//切换了控制模式
	{
		lastMode=controlMode;		
		resetTerminal();
		
		operateAlarmCount=0; //将计数清零
			
	}
	//没有切换控制模式
	else				
	{
		//<接近模式>
		if(MODE_APPROACH==controlMode)	//	接近模式
		{
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////以下代码复用3次///////////////////////////////////////////////
		//一次按键只操作二次，按下一次，弹起一次
		//取消按键单次操作的原因：松开按键后，收不到数据，所以就不会处理数据。
		//	if(preControlStatus!=controlStatus)
			{
				preControlStatus=controlStatus;
				if(controlStatus==STATE_ALARM)  //警告
				{
					beepCtrl->setOn();
					redLed->setOn();
						
				}
				else if(controlStatus==STATE_SHOCK)//电击
				{
				//		shockTime=0;    //按下电击，开始计时电击时间
								//定时关闭电击，放置长期电击
					shockTime++;
					if(shockTime>6)
					{
						//shockTime会在resetControlData里复位
						shockPowCtrl->setOff();
						shockCtrl->setOff();				
					}
					else 
					{
						
						shockPowCtrl->setOn();
						shockCtrl->setOn();
						redLed->setOn();
					}

				}
				//如果没断线，没脱离视野范围，没有控制动作信号
				else  
				{
					if(!isDecCut && (distance!=0) ) 
					{
						beepCtrl->setOff();
						redLed->setOff();
					}
						
						shockPowCtrl->setOff();
						shockCtrl->setOff();
						
				}
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////		
			
		//</接近模式>
		}
/*
		//<定点模式>
		if(MODE_POINT==controlMode)	//	接近模式
		{
			//本体号为空
			if(TerminalID==ID_TERMINAL_INVALID)    
			{
				if(RoomID==ID_ROOM)
				{
					if(controlStatus==STATE_ALARM)  //警告
					{
						operateAlarmCount++;
					 if(operateAlarmCount<=BEEP_10SEC/TIM3_PERIOD_MS)
					 {
							beepCtrl->setOn();

					 }
					 else
					 {
							beepCtrl->setOff();
					
					 }
							
					}
					else if(controlStatus==STATE_SHOCK)//电击
					{
						
						operateShockCount++;
						if(operateShockCount<=SHOCK_3s/TIM3_PERIOD_MS)
						{
							shockCtrl->setOn();
					
						}
						else
						{
							shockCtrl->setOff();
						
						}	
					}
					else  //默认模式，即不动作
					{
						operateAlarmCount=0; //将计数清零
						operateShockCount=0;	
						//关闭执行器
						beepCtrl->setOff();
						shockCtrl->setOff();	
		
						
					}
				}
			}
			//本体号不为空
			else if(TerminalID==ID_TERMINAL && RoomID==ID_ROOM)  //本体号和房间号都对应时
			{
				if(controlStatus==STATE_ALARM)  //警告
				{
					operateAlarmCount++;
				 if(operateAlarmCount<=BEEP_10SEC/TIM3_PERIOD_MS)
				 {
						beepCtrl->setOn();
				
				 }
				 else
				 {
						beepCtrl->setOff();
					
				 }
						
				}
				else if(controlStatus==STATE_SHOCK)//电击
				{
					
					operateShockCount++;
					if(operateShockCount<=SHOCK_3s/TIM3_PERIOD_MS)
					{
						shockCtrl->setOn();
						
					}
					else
					{
						shockCtrl->setOff();
						
					}	
				}
				else  //默认模式，即不动作
				{
					operateAlarmCount=0; //将计数清零
					operateShockCount=0;	
					//关闭执行器
					beepCtrl->setOff();
					shockCtrl->setOff();	

					
				}				

			}
			
		}
			
*/
		//</定点模式>
	

	}
	
}

void Process::resetTerminal(void)
{
	beepCtrl->setOff();
	shockCtrl->setOff();
}



/**
  * @brief  计算电源电压，放大10倍
  * @param  None
  * @retval None
  */
void Process::getPowerValue(void)
{
	
	if(controlStatus!=STATE_SHOCK)
	{
		powerVoltage  = AD_Filter[AD_NUM_POWER]*33*3>>12;
	}
	

}


/**
  * @brief  显示电量状态
  * @param  None
  * @retval None
  */
void Process::showPowerLed()
{
	if(powerVoltage>=37)
	{
		//redLed->setOff();
		greenLed->setOn();
	}
	else
	{
	//	redLed->setOn();
		greenLed->setOff();
	}
}

void Process::detectDecCut()
{
	//电击的时候不检测断线报警
	if(controlStatus!=STATE_SHOCK)
	{
		decCutADValue  = AD_Filter[AD_NUM_DECCUT]*33>>12;//
	}
	
	
	//线断报警
	if(decCutADValue>15)
	{
		isDecCut=true;
		beepCtrl->setOn();
		redLed->setOn();
		
		watchDogTimer=3;   //断线了，一直喂狗
	}
	else if(controlStatus!=1)
	{
		isDecCut=false;
		beepCtrl->setOff();
		redLed->setOff();
	}
	
}

/**
  * @brief  重置化全部数据
  * @param  None
  * @retval None
  */
void Process::resetControlData(void)
{
	controlStatus=0;
	
}

/**
  * @brief  使能SWD，关闭JTAG
  * @param  None
  * @retval None
  */
void Process::enable_SWD_disable_Jtag(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//	使能AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//	关闭JTAG，引脚作为GPIO用
}
/*--------------------------------- End of Process.cpp -----------------------------*/






