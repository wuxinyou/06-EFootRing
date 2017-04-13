/**
  ******************************************************************************
	*文件：Process.cpp
	*作者：孟浩
	*版本：2.0
	*日期：2015-03
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

#define TIM2_PERIOD_MS	5
#define TIM3_PERIOD_MS	10
#define TIM4_PERIOD_MS	100
/* 类的实现---------------------------------------------------------------------*/

 Process::Process()
{
	enable_SWD_disable_Jtag();								//	关闭JTAG打开SWD	
	
	initIOs();												//	初始化IO
	initADs();												//	初始化AD通道
	initUsarts();											//	初始化串口
	initTimers();											//	初始化定时器
	
	SysTick_DelayMs(1000);	
	testLed->setOn();
//	shockCtr->setOn();
	//beepCtr->setOn();
	initAllData();											//  初始化所有数据
	SysTick_DelayMs(100);
//	shockCtr->setOff();
	testLed->setOff();
	beepCtr->setOff();
}

void Process::openPeriph(void)
{
	SysTick_DelayMs(10);									//  延时 10ms
	com1st->open();    										//  开启通信端口1
	
	t2->start();											//  开启定时器2
	t3->start();											//  开启定时器3
	t4->start();											//  开启定时器4

}

/**
  * @brief  TIM2定时器中断函数，处理
  * @param  None
  * @retval None
  */
void Process::runOnTime2(void)
{
	getPowerValue();
	getKeyValue();
}

/**
  * @brief  TIM3定时器中断函数,更新电阻值与电源电压
  * @param  None
  * @retval None
  */
#define	OVER_TIME	1000
void Process::runOnTime3(void)
{
	static uint16_t	overTimerCount=0;
	if(protocol->com1st_Receive())							//  接收串口数据
	{
		overTimerCount=0;									//	超时计数复位
		testLed->reverseState();
		dealMainData();										//	处理数据
//		protocol->com1st_Send();							//	com1st发送数据
	}
	else
	{
		overTimerCount++;									//	超时计数
		if(overTimerCount>=OVER_TIME)
		{
	//		beepCtr->setOn();								//	蜂鸣器报警
			resetTerminal();
			overTimerCount=0;
		}
	}

}

/**
  * @brief  TIM4定时器中断函数
  * @param  None
  * @retval None
  */
void Process::runOnTime4(void)
{

}


/**
  * @brief  处理主控数据
  * @param  None
  * @retval None
  */
void Process::dealMainData(void)
{
	uint8_t* dataFromMain;								//	保存接收自主控的数据
	uint16_t roomID1,roomID2,roomID3;
	uint8_t	terminalID1,terminalID2;
	
	dataFromMain = protocol->com1st_GetData();			//	获取串口数据
	
	revMode		=	dataFromMain[3]&0x03;				//	获取模式
	revKeyState	=	(dataFromMain[3]>>2)&0x03;			//	获取按键状态
	roomID1		=	dataFromMain[4];					//	获取房间号最高位
	roomID2		=	dataFromMain[5];					//	获取房间号中间位
	roomID3		=	dataFromMain[6];					//	获取房间号最低位
	terminalID1	=	dataFromMain[7];					//	获取本体号最高位
	terminalID2	=	dataFromMain[8];					//	获取本体号最低位
	
	revRoomID 		=	roomID1*100 + roomID2*10 + roomID3;	//	计算接收到的房间号
	revTerminalID	=	terminalID1*10 + terminalID2;		//	计算接收到的本体号
	
	disposal(revMode);
	

	
}

#define	KEY_PRESS_LONG_TIME	100
void Process::disposal(uint8_t mode)
{
	static	uint8_t lastMode=MODE_APPROACH;
	static	uint16_t keyCount=0;
	static	uint8_t	preKeyState=KEY_DEFAULT;
	
	
	if(lastMode!=mode)	//	前后两次模式不一致则复位
	{
		keyState=KEY_DEFAULT;
		resetTerminal();
		lastMode=mode;
	}
	else				//	前后两次模式一致，执行操作
	{
		//<接近模式>
		if(MODE_APPROACH==mode)	//	接近模式
		{
		//	if(preKeyState!=revKeyState) //单次触发
		//	{
				if(revKeyState==STATE_ALARM)
				{
					keyState = KEY_ALARM_SHOT;
					
				}
				else if(revKeyState==STATE_SHOCK)
				{
					keyState = KEY_SHOCK_SHOT;
				}
				else
				{
					keyState = KEY_DEFAULT;
				}
				preKeyState=revKeyState;
				keyCount=0;
		//	}
			/*
			else	//	if(preKeyState==revKeyState)   
			{
				keyCount++;      //如果keycount没有置零操作，则会一直执行？
				if(keyCount>=KEY_PRESS_LONG_TIME)
				{
					if(preKeyState==STATE_ALARM)
					{
						keyState = KEY_ALARM_LONG;
					}
					else if(preKeyState==STATE_SHOCK)
					{
						keyState = KEY_SHOCK_LONG;
					}
				}
			}
			*/
			operate();
		}
		//</接近模式>
		//<定点模式>
		else if(MODE_POINT==mode)	//	定点模式
		{
			if(ID_ROOM==revRoomID&&ID_TERMINAL==revTerminalID)	//	ID一致
			{
				if(preKeyState!=revKeyState)
				{
					if(preKeyState==STATE_ALARM)
					{
						keyState = KEY_ALARM_SHOT;
						
					}
					else if(preKeyState==STATE_SHOCK)
					{
						keyState = KEY_SHOCK_SHOT;
					}
					preKeyState=revKeyState;
					keyCount=0;
				}
				/*
				else	//	if(preKeyState==revKeyState)
				{
					keyCount++;
					if(keyCount>=KEY_PRESS_LONG_TIME)
					{
						if(preKeyState==STATE_ALARM)
						{
							keyState = KEY_ALARM_LONG;
						}
						else if(preKeyState==STATE_SHOCK)
						{
							keyState = KEY_SHOCK_LONG;
						}
					}
				}
				*/
				operate();
			}
			else
			{}
		}
		//</定点模式>
		else
		{}
		
	}
}

void Process::resetTerminal(void)
{
	beepCtr->setOff();
	shockCtr->setOff();
}

#define	BEEP_10SEC	3000	//	蜂鸣器10s
#define	SHOCK_3s	3000	//	电击器3s
void Process::operate( void)
{
	static uint16_t operateAlarmCount=0;
	static uint16_t operateShockCount=0;
	static uint16_t operateShockLongCount=0;
	static bool	operateAlarmOnce=false;
	static bool operateShockOnce=false;
	
	//警告
	if(keyState==KEY_ALARM_SHOT)
	{		


			operateAlarmCount++;
			if(operateAlarmCount<=BEEP_10SEC/TIM3_PERIOD_MS)
			{
				beepCtr->setOn();
			}
			else
			{
				beepCtr->setOff();
			}
			
	}
	//电击
	else if(keyState==KEY_SHOCK_SHOT)
	{

			operateShockCount++;
			if(operateShockCount<=SHOCK_3s/TIM3_PERIOD_MS)
			{
				shockCtr->setOn();
			}
			else
			{
				shockCtr->setOff();
			}
	}
	else
	{
		beepCtr->setOff();
		shockCtr->setOff();
		operateShockCount=0;
	}
	/*
	else if(keyState==KEY_ALARM_LONG)
	{
		operateAlarmOnce=false;
		operateShockOnce=false;
		operateShockLongCount=0;
		operateAlarmCount=0;
		operateShockCount=0;
		beepCtr->setOn();
		shockCtr->setOff();
	}
	*/
	
	//电击
	/*
	else if(keyState==KEY_SHOCK_SHOT)
	{
		operateAlarmOnce=false;
		operateShockLongCount=0;
		operateAlarmCount=0;
		operateShockCount++;
		beepCtr->setOff();
		if(!operateShockOnce)
		{
			if(operateShockCount<=SHOCK_3s/TIM3_PERIOD_MS)
			{
				shockCtr->setOn();
				operateShockOnce=false;
			}
			else
			{
				operateShockCount=0;
				operateShockOnce=true;
			}
		}
		
	}
	else if(keyState==KEY_SHOCK_LONG)
	{
		operateAlarmOnce=false;
		operateShockOnce=false;
		operateAlarmCount=0;
		operateShockCount=0;
		beepCtr->setOff();
		if(operateShockLongCount<SHOCK_3s/TIM3_PERIOD_MS)
		{
			shockCtr->setOn();
		}
		else if(operateShockLongCount<(2*SHOCK_3s/TIM3_PERIOD_MS))
		{
			shockCtr->setOff();
		}
		else if(operateShockLongCount>=(2*SHOCK_3s/TIM3_PERIOD_MS))
		{
			operateShockLongCount=0;
		}
		operateShockLongCount++;
	}
	else
	{
	}
	*/
}


/**
  * @brief  计算电源电压，放大10倍
  * @param  None
  * @retval None
  */
void Process::getPowerValue(void)
{
	uint32_t tempValue=0;

	adValue[NUM_POWER] = AD_Filter[NUM_POWER];
	tempValue  = adValue[NUM_POWER]*33*3>>12;
//	powerValue.u16_data = (uint16_t)tempValue;
	powerVoltage=(uint8_t)tempValue;
}

/**
  * @brief  设置com1s_t需要发送的数据
  * @param  None
  * @retval None
  */
void Process::setCom1Data(void)
{
//	com1st_DataToSend[0] = currentMode;
//	com1st_DataToSend[1]= powerValue.u8_data[0];
//	com1st_DataToSend[2]= powerValue.u8_data[1];
	

}


void Process::getKeyValue(void)
{
	for(uint8_t index=0;index<NUM_KEY;index++) 					//  读取所有按键信息
			keys[index]->getValue();							//  获取按键信息
}


/**
  * @brief  初始化按键
  * @param  None
  * @retval None
  */
void Process::initIOs(void)
{
	testLed			=	new IoOut(PORT_LED_TEST,PIN_LED_TEST);
	greenLed		=	new IoOut(PORT_LED_GREEN,PIN_LED_GREEN);
	redLed			=	new	IoOut(PORT_LED_RED,PIN_LED_RED);
	
	beepCtr			=	new	IoOut(PORT_BEEP,PIN_BEEP,true);
	shockCtr		=	new IoOut(PORT_SHOCK,PIN_SHOCK,true);
	
	keys[NUM_KEY_DETECT]	= new Key(PORT_KEY_DETECT, PIN_KEY_DETECT);	
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
	com1st = new SerialPort(1,38400);

	protocol = new Protocol(com1st);
}

/**
  * @brief  初始化全部数据
  * @param  None
  * @retval None
  */
void Process::initAllData(void)
{
	powerVoltage	=	0;
	revMode 		=	MODE_APPROACH;					//	默认为接近模式
	revKeyState 	=	STATE_DEFAULT;					//	默认为无按键按下
	revRoomID		=	ID_ROOM_INVALID;				//	默认ID为无效
	revTerminalID	=	ID_TERMINAL_INVALID;			//	默认ID为无效
	
	keyState		=	KEY_DEFAULT;
	
	setCom1Data();										//	设置需要发送的数据
	protocol->com1st_UpdateSendBuf(com1st_DataToSend);	//	更新需要发送的数据
	
}

/**
  * @brief  重置化全部数据
  * @param  None
  * @retval None
  */
void Process::resetControlData(void)
{
	
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






