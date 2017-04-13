/**
  ******************************************************************************
	*文件：Process.cpp
	*作者：吴新有
	*版本：1.0
	*日期：2016-12-17
	*概要：
	*备注：
	*
  ******************************************************************************  
	*/ 
/* 头文件包含 ------------------------------------------------------------------*/
#include "Process.h"
#include "bmp.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdlib.h>
#ifdef __cplusplus
 }
#endif
 
/* 类的实现--------------------------------------------------------------------*/

 
 
/**
  * @brief  构造函数
  * @param  None
  * @retval None
  */
 
 
  Process::Process()
{
	enable_SWD_disable_Jtag();											//	关闭JTAG打开SWD	
	initGPIO();															//	初始化指示灯
	initADs();															//	初始化AD通道
	initUsarts();														//	初始化串口
	initTimers();														//	初始化定时器	
	initAllData();														//	初始化所有数据
	
	//初始化操作

	YL_En->setOn();													//
	YL_Set->setOn();
	
	powEn->setOn();
	

	
	

	
//<开机后马上处理按键数据>

	getKeyValue();													//	获取AD按键值
	dealKeyValue();														  //	处理按键



	updataToZigbee();
	
	SysTick_DelayMs(50);
//</开机后马上发送数据>

}
/**
  * @brief  TIM2定时器中断函数,,用于接收和处理按键
  * @param  None
  * @retval None
	* @定时：10ms
  */
void Process::runOnTime2(void)
{
	getADKeyValue();
	dealADKeyValue();
	
}

/**
  * @brief  TIM3定时器中断函数,用于发送串口数据
  * @param  None
  * @retval None
	* @定时：80ms
  */

void Process::runOnTime3(void)
{	
	
	if(controlStatusFlag)
	{
		updataToZigbee();
	}
	
	
  //sendCheckSignal();
	
	//showOtherSymbol();
}

/**
  * @brief  TIM4定时器中断函数，用于刷新显示控制状态，200ms
  * @param  None
  * @retval None
	* @定时：200ms
  */

void Process::runOnTime4(void)
{


	//获取电量值
	//	getPowerValue();
	//showPowerSymbol();
	


//自由模式关灯，定时关电源
	if(isFreeMode)
	{
	//	ledMode->setOff();
		//定时10S后，关断电源

		watchDogTimer++;
		if(watchDogTimer>20)
		{
			watchDogTimer=0;
			powEn->setOff();
		}
		else
		{
			powEn->setOn();
		}
	}
	else
	{
		//ledMode->setOn();
	}
	
	
#ifdef USE_SLEEP

		//模拟看门狗休眠
		watchDogTimer++;
		if(watchDogTimer>200)
		{
			watchDogTimer=0;
		
			SysTick_DelayMs(100);
			PWR_EnterSTANDBYMode();  //进入待机
			
		
		}
		else if(watchDogTimer<2)
		{
			//唤醒后立马需要的初始化
		}
		
	
#endif
	
}



/**
  * @brief  初始化IO
  * @param  None
  * @retval None
  */
void Process::initGPIO()
{	

	//按键实例化



	ledKey        = new IoOut(LED_KEY_PORT,LED_KEY_PIN);//低电平有效
	ledPow        = new IoOut(LED_POW_PORT,LED_POW_PIN);
	
	powEn         = new IoOut(POW_EN_PORT,POW_EN_PIN,true);//g高电平有效
	
	YL_En         = new IoOut(YL_EN_PORT,YL_EN_PIN);
	YL_Set        = new IoOut(YL_SET_PORT,YL_SET_PIN);   //低电平有效
	
	//zigbeeReset = new IoOut(ZIGBEE_RESET_PORT,ZIGBEE_RESET_PIN);
	

	

}


/**
  * @brief  初始化AD
  * @param  None
  * @retval None
  */
void Process::initADs()
{
	ADC1_DMA_Init();
}

/**
  * @brief  初始化定时器
  * @param  None
  * @retval None
  */
void Process::initTimers()
{
	t2=new Timer (TIM2);
	t2->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM2_SubPriority);
	t2->setTimeOut(TIM2_DELAY_TIME);		// 修改宏定义以改变延时

	t3=new Timer (TIM3);
	t3->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM3_SubPriority);
	t3->setTimeOut(TIM3_DELAY_TIME);		// 修改宏定义以改变延时
	
	t4=new Timer (TIM4);
	t4->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM4_SubPriority);
	t4->setTimeOut(TIM4_DELAY_TIME);		// 修改宏定义以改变延时
}

/**
  * @brief  初始化usart
  * @param  None
  * @retval None
  */
void Process::initUsarts()
{

	zigbeePort = new SerialPort(1,9600);	
	zigbeePort->open();

	zigbeeControl = new TerminalControl(zigbeePort);
}

/**
  * @brief  初始化全部数据
  * @param  None
  * @retval None
  */	
void Process::initAllData(void)
{
	

	
	sendData =new u8[6];
	for(int i=0;i<6;i++)
	{
		sendData[i]=0;
	}

	

	
	controlModeFlag=1;           //默认是接近控制模式
	controlStatusFlag=0;         //默认是报警
	
	isFreeMode=1;                //默认是自由模式
	
	isShocking=false;
	isAlarming=false;

	watchDogTimer=0;
	
	powerValue=0;
	ADKey1Value=0;
	ADKey2Value=0;
	ADKey3Value=0;

	
}


	 
/**
  * @brief  开启用户外设
  * @param  None
  * @retval None
  */
void Process::openPeriph()
{
	SysTick_DelayMs(10);												//	延时 10ms

	t2->start();														//	开启定时器2
	t3->start();														//	开启定时器3
	t4->start();														//	开启定时器4
}

//////////////////////////////////////////////end of Init/////////////////////////////////////////////////////////


/**
  * @brief  计算电源电压
  * @param  None
  * @retval None
  */
void Process::getPowerValue(void)
{
	uint32_t tempValue=0;

	tempValue  = AD_Filter[AD_NUM_POWER]*33*2>>12;

	powerValue=(uint8_t)tempValue;



}


/**
  * @brief  获取按键值
  * @param  None
  * @retval None
  * @author 吴新有
  */
void Process::getKeyValue()
{	
//没有GPIO按键	
	
}

/**
  * @brief  获取AD按键值
  * @param  None
  * @retval None
  * @author 吴新有
  */
void Process::getADKeyValue()
{	
	uint32_t tempValue=0;

	ADKey1Value  = AD_Filter[AD_NUM_KEY1]*33*3>>12;
	//ADKey2Value  = AD_Filter[AD_NUM_KEY2]*33>>12;
	ADKey3Value  = AD_Filter[AD_NUM_KEY3]*33*3>>12;

	
}

/**
  * @brief  处理AD按键值
  * @param  None
  * @retval None
  * @author 吴新有
  */
void Process::dealADKeyValue()
{	
	//按键1 ---电量查询
	if(ADKey3Value>=37)
	{				
		ledPow->setOn();
		watchDogTimer=3;  //有按键按下时，喂狗
	}
	else
	{
		ledPow->setOff();
	}

	
	

	//按键3 ---电击  按下和弹起时执行一次
// 	static uint8_t preADKey3Value=0;
// 	if(preADKey3Value!=ADKey3Value)
// 	{
// 		preADKey3Value=ADKey2Value;
		if(ADKey1Value>5)
		{
// 			updataToZigbee();  //按下后立即发送数据1次，然后打开发送数据定时器
// 			t3->start();
			controlStatusFlag=2;
		//	ledKey->setOn();
		}
		else	
		{
			controlStatusFlag=0;
		//	ledKey->setOff();
// 			t3->stop();          //弹起按键后关闭发送数据定时器
		}
// 	}

	//有按键按下时，喂狗
		if(ADKey1Value>5)
	{				
		watchDogTimer=3;  //有按键按下时，喂狗
	}
 
	

}

/**
  * @brief  获取控制数据
  * @param  None
  * @retval None	
  */
#define	POWER_OFF_COUNT_DOWN			10							//	关机计时,5s以后自动关机
#define	POWER_OFF_HARD_WARE_COUNT_DOWN	50							//	强制关机计时
void Process::dealKeyValue()
{
//  	static 	bool	preNumdKey[14]		={true};


// 		
// 	//按一下Num1键
//  	if (preNumdKey[1] != keyNum1->FinalState)					//	
//  	{
//  		preNumdKey[1]=keyNum1->FinalState;
// 		if (keyNum1->FinalState==false)
// 		{
// 			if(!sqStack->IsFull())
// 			{
// 				sqStack->Push('1');			
// 			
// 				OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 			}
// 			Led->reverseState();
// 			
// 		}

//  	}
// 	
// 		//按一下Num2键
//  	if (preNumdKey[2] != keyNum2->FinalState)					//	
//  	{
//  		preNumdKey[2]=keyNum2->FinalState;
// 		if (keyNum2->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())
// 			{
// 			sqStack->Push('2');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 	
// 			//按一下Num3键
//  	if (preNumdKey[3] != keyNum3->FinalState)					//	
//  	{
//  		preNumdKey[3]=keyNum3->FinalState;
// 		if (keyNum3->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('3');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 				//按一下Num4键
//  	if (preNumdKey[4] != keyNum4->FinalState)					//	
//  	{
//  		preNumdKey[4]=keyNum4->FinalState;
// 		if (keyNum4->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('4');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 					//按一下Num5键
//  	if (preNumdKey[5] != keyNum5->FinalState)					//	
//  	{
//  		preNumdKey[5]=keyNum5->FinalState;
// 		if (keyNum5->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('5');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 	
// 					//按一下Num6键
//  	if (preNumdKey[6] != keyNum6->FinalState)					//	
//  	{
//  		preNumdKey[6]=keyNum6->FinalState;
// 		if (keyNum6->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('6');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 	
// 					//按一下Num7键
//  	if (preNumdKey[7] != keyNum7->FinalState)					//	
//  	{
//  		preNumdKey[7]=keyNum7->FinalState;
// 		if (keyNum7->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('7');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 	
// 					//按一下Num8键
//  	if (preNumdKey[8] != keyNum8->FinalState)					//	
//  	{
//  		preNumdKey[8]=keyNum8->FinalState;
// 		if (keyNum8->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('8');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 	
// 					//按一下Num9键
//  	if (preNumdKey[9] != keyNum9->FinalState)					//	
//  	{
//  		preNumdKey[9]=keyNum9->FinalState;
// 		if (keyNum9->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('9');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 					//按一下Num0键
//  	if (preNumdKey[0] != keyNum0->FinalState)					//	
//  	{
//  		preNumdKey[0]=keyNum0->FinalState;
// 		if (keyNum0->FinalState==false)
// 		{
// 			
// 			if(!sqStack->IsFull())       //非满
// 			{
// 			sqStack->Push('0');
// 			OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,sqStack->GetTopElem(),16);
// 				
// 			}

// 			Led->reverseState();
// 			
// 		}

//  	}
// 		//按一下DEL键
//  	if (preNumdKey[10] != keyDel->FinalState)					//	
//  	{
//  		preNumdKey[10]=keyDel->FinalState;
// 		if (keyDel->FinalState==false)
// 		{
// 			if(!sqStack->IsEmpty())                                       //非空
// 			{
// 				OLED_ShowChar(Coordinate_X[sqStack->GetTop()],0,' ',16);		//显示空格，代替清除		
// 				sqStack->Pop();

// 			}
// 			
// 		}

//  	}
// 	
// 			//按一下MODE键
// //  	if (preNumdKey[11] != keyMode->FinalState)					//	
// //  	{
// //  		preNumdKey[11]=keyMode->FinalState;
// // 		if (keyMode->FinalState==false)
// // 		{
// // 			controlModeFlag=1-controlModeFlag;         //实现1和0的切换
// // 			
// // 			showControlMode();     //
// // 		
// // 			//终止控制状态动作
// // 			controlStatusFlag=0;

// // 			
// // 		}

// //  	}
// 	
// 	
// 				//按一下SHOCK键
//  	if (preNumdKey[12] != keyShock->FinalState)					
//  	{
//  		preNumdKey[12]=keyShock->FinalState;
// 		if (keyShock->FinalState==false)
// 		{
// 			controlStatusFlag=2;         //电击

// 		//	showControlStatus();
// 			
// 		}

//  	}
// 	
// 	
// 					//按一下ALARM键
//  	if (preNumdKey[13] != keyAlarm->FinalState)					//	
//  	{
//  		preNumdKey[13]=keyAlarm->FinalState;
// 		if (keyAlarm->FinalState==false)
// 		{
// 			controlStatusFlag=1;         //警报

// 		//	showControlStatus();
// 			
// 		}

//  	}
// 	
// 	//喂狗
// 	static int keyActionFlag=0;
// 	if(!keyMode->FinalState || !keyAlarm->FinalState || !keyShock->FinalState || !keyDel->FinalState
// 		 || !keyNum1->FinalState || !keyNum2->FinalState || !keyNum3->FinalState || !keyNum4->FinalState
// 		 || !keyNum5->FinalState || !keyNum6->FinalState || !keyNum7->FinalState || !keyNum8->FinalState
// 	   || !keyNum9->FinalState || !keyNum0->FinalState)
// 	{
// 		watchDogTimer=3;//最好大于3，避免多余的刷屏显示
// 		
// 		//有按键后立即发送数据 2107-01-13
// 		//为了防止按下按键就一直发送数据，这里只允许发送2帧，第一帧用于唤醒，第二帧是有效数据
// 		if(keyActionFlag==0 || keyActionFlag==1)
// 		{
// 			
// 			//按键发送数据之前先关闭定时器发送数据
// 			t3->stop();
// 			updataToZigbee();
// 			
// 			keyActionFlag++;
// 			
// 			//第二次发送完按键数据后，打开定时器3，定时发送数据
// 			if(keyActionFlag==1)
// 			{
// 				t3->start();
// 			}
// 			
// 				
// 		}
// 		
// 		
// 	}
// 	//正常松开状态
// 	else
// 	{
// 		keyActionFlag=0;
// 	}


}



/**
  * @brief  显示控制模式
  * @param  None
  * @retval None
  * @author:吴新有
  */
// #define SHOCKTIME  5
// #define ALARMTIME  5
// void Process::showControlStatus()
// {
// #ifdef USE_OLED
// 	
// 		static int shockTimeCount=0,alarmTimeCount=0;
// 	if(2==controlStatusFlag)     //正在电击
// 	{
// 		alarmTimeCount=0;           //必须重置警报计时
// 		shockTimeCount++;
// 		if(shockTimeCount>SHOCKTIME)
// 		{
// 			shockTimeCount=0;
// 			controlStatusFlag=0;
// 		}
// 		
// 		//显示电击
// 		OLED_ShowEFootCHinese(96,2,6);
// 		OLED_ShowEFootCHinese(112,2,7);
// 	}
// 	else if(1==controlStatusFlag) //正在警报
// 	{
// 		shockTimeCount=0;           //必须重置电击计时
// 		alarmTimeCount++;
// 		if(alarmTimeCount>ALARMTIME)
// 		{
// 			alarmTimeCount=0;
// 			controlStatusFlag=0;
// 		}
// 		 //显示警报
// 	
// 		OLED_ShowEFootCHinese(96,2,8);
// 		OLED_ShowEFootCHinese(112,2,9);
// 	}
// 	else
// 	{
// 		OLED_ShowString(96,2,noAction,16);  //没有动作，显示空白
// 	}

// #endif
// 	
// }

	/**
  * @brief  更新发送数据
  * @param  None
  * @retval None
  * @author:吴新有
  */

void Process::updataToZigbee()
{
	//更新数据
	sendData[0]=(isFreeMode<<4)+(controlStatusFlag <<2)+controlModeFlag;
	
	//如果有控制动作时，则不更新本体ID号，避免在动作时，误改变了本体ID号，然后动作会随ID号转移。
	//所以在动作时，改变本体ID号是无效的。

	
	//发送数据
	zigbeeControl->updateSendBuf(sendData);
	zigbeeControl->sendcommand();
	
}



	/**
  * @brief  更新发送数据
  * @param  None
  * @retval None
  * @author:吴新有 2017-01-14
  */
void Process::sendCheckSignal()
{
	static char counter=0;
	counter++;
	if(counter>100)
		counter=0;
	
		zigbeePort->putChar(0XFF);
		zigbeePort->putChar(0XFF);
		zigbeePort->putChar(counter);		
}

/**
  * @brief  使能SWD，关闭JTAG
  * @param  None
  * @retval None
  */
void Process::enable_SWD_disable_Jtag()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//	使能AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//	关闭JTAG，引脚作为GPIO用
}






/**
  * @brief  调试代码，串口输出
  * @param  None
  * @retval None
  */
void Process::testWithUsart(void)
{
	uint8_t aa;
	aa=AD_Filter[1]>>4;													//	调试代码	

//	testLed->reverseState();
}

/*--------------------------------- End of Process.cpp -----------------------------*/
