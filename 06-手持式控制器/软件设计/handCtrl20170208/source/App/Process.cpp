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
 
 
 const u8 Coordinate_X[11]={192,66,80,94,108,122,136,150,164,178,220};  //数码显示液晶屏X坐标数组
 const u8 Coordinate_P[11]={66,80,94,108,122,136,150,164,178,220};  //数码显示液晶屏X坐标数组
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
	

	ledPow->setOn();
	
	


}
/**
  * @brief  TIM2定时器中断函数,,用于接收和处理按键
  * @param  None
  * @retval None
	* @定时：20ms
  */
void Process::runOnTime2(void)
{
	static int index=0;
	if(index>100)
	{
		//showControlMode();
		showPower();
		
		ledPow->reverseState();
		index=0;
		//showControlMode();
	}
	
	showControlMode();
	showPower();
	index++;
	

	getKeyValue();													//	获取AD按键值
	dealKeyValue();														  //	处理按键




}

/**
  * @brief  TIM3定时器中断函数,发送串口控制数据
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

}

/**
  * @brief  TIM4定时器中断函数，用于刷新显示控制状态，200ms
  * @param  None
  * @retval None
	* @定时：200ms
  */

void Process::runOnTime4(void)
{
	//static int sleepTimer=0;
	
	//正常使用是打开
#ifndef TEST
	//showControlStatus();
#endif

	//获取电量值
//	getPowerValue();
	//showPowerSymbol();
	
#ifdef USE_SLEEP
	//模拟看门狗休眠
	watchDogTimer++;
	if(watchDogTimer>200)
	{
		watchDogTimer=0;
		OLED_Display_Off();//关闭液晶屏显示
		SysTick_DelayMs(100);
		PWR_EnterSTANDBYMode();  //进入待机
		
	
	//	PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);   //进入停机
		//SysTick_DelayMs(10);
	//	SYSCLKconfig_STOP();		
	}
	else if(watchDogTimer<2)
	{
		//唤醒后的初始化
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
	keyNum1    = new Key(KEYNUM1_PORT,KEYNUM1_PIN);    //数字按键1
	keyNum2    = new Key(KEYNUM2_PORT,KEYNUM2_PIN);     //数字按键2
	keyNum3    = new Key(KEYNUM3_PORT,KEYNUM3_PIN);    //数字按键3
	keyNum4    = new Key(KEYNUM4_PORT,KEYNUM4_PIN);    //数字按键4
	keyNum5    = new Key(KEYNUM5_PORT,KEYNUM5_PIN);    //数字按键5
	keyNum6    = new Key(KEYNUM6_PORT,KEYNUM6_PIN);    //数字按键6
	keyNum7    = new Key(KEYNUM7_PORT,KEYNUM7_PIN);    //数字按键7
	keyNum8    = new Key(KEYNUM8_PORT,KEYNUM8_PIN);    //数字按键8
	keyNum9    = new Key(KEYNUM9_PORT,KEYNUM9_PIN);    //数字按键9
	keyNum0    = new Key(KEYNUM0_PORT,KEYNUM0_PIN);    //数字按键0

	
	keyDel     = new Key(KEYDEL_PORT,KEYDEL_PIN);      //退格键
	keyMode    = new Key(KEYMODE_PORT,KEYMODE_PIN);   //模式按键
	keyShock   = new Key(KEYSHOCK_PORT,KEYSHOCK_PIN);
	keyAlarm   = new Key(KEYALARM_PORT,KEYALARM_PIN);
	
	ledPow       = new IoOut(LED_POW_PORT,LED_POW_PIN);
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

	zigbeePort = new SerialPort(3,9600);	
	zigbeePort->open();
	zigbeeControl = new TerminalControl(zigbeePort);
	
	lcdPort    = new SerialPort(1,38400);
	lcdPort->open();
	LCDControl    = new LCDdisplay(lcdPort);

	
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
	controlData=0;
	
	sqStack=new SqStack();
	
	controlModeFlag=1;           //默认是接近控制模式
	controlStatusFlag=0;         //默认是无操作
	isFreeMode=1;                //默认是自由模式
	
	isShocking=false;
	isAlarming=false;

	watchDogTimer=0;
	powerValue=0;

	
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

void Process::startWork()
{
	showStartPic();
	showMainWin();
	
	
	//openPeriph();
	
	
	
	openPeriph();
	SysTick_DelayMs(10);												//	延时 10ms
	
	//showControlMode();
	
}
//////////////////////////////////////////////end of Init/////////////////////////////////////////////////////////


/**
  * @brief  计算电源电压
  * @param  None
  * @retval None
  */
void Process::getPowerValue(void)
{
	powerValue  = AD_Filter[NUM_POWER]*33*11>>12;

}


/**
  * @brief  获取按键值
  * @param  None
  * @retval None
  * @author 吴新有
  */
void Process::getKeyValue()
{

	
	keyNum1->getValue(); //1
	keyNum2->getValue(); //2
	keyNum3->getValue();
	keyNum4->getValue();
	keyNum5->getValue();
	keyNum6->getValue();
	keyNum7->getValue();
	keyNum8->getValue();
	keyNum9->getValue();
	keyNum0->getValue(); //0
	
	keyDel->getValue();
	keyMode->getValue();
	keyShock->getValue();
	keyAlarm->getValue();
	

	
	
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
 	static 	bool	preNumdKey[14]		={true};


	//如果是定点控制，则使能数字按键
	if(0==controlModeFlag)
	{		
		//按一下Num1键
		if (preNumdKey[1] != keyNum1->FinalState)					//	
		{
			preNumdKey[1]=keyNum1->FinalState;
			if (keyNum1->FinalState==false)
			{
				if(!sqStack->IsFull())
				{
					sqStack->Push(1);			
					showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
				}

				
			}

		}
		
			//按一下Num2键
		if (preNumdKey[2] != keyNum2->FinalState)					//	
		{
			preNumdKey[2]=keyNum2->FinalState;
			if (keyNum2->FinalState==false)
			{
				
				if(!sqStack->IsFull())
				{
				sqStack->Push(2);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
		
				//按一下Num3键
		if (preNumdKey[3] != keyNum3->FinalState)					//	
		{
			preNumdKey[3]=keyNum3->FinalState;
			if (keyNum3->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(3);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}

		
				
			}

		}
					//按一下Num4键
		if (preNumdKey[4] != keyNum4->FinalState)					//	
		{
			preNumdKey[4]=keyNum4->FinalState;
			if (keyNum4->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(4);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
						//按一下Num5键
		if (preNumdKey[5] != keyNum5->FinalState)					//	
		{
			preNumdKey[5]=keyNum5->FinalState;
			if (keyNum5->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(5);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
		
						//按一下Num6键
		if (preNumdKey[6] != keyNum6->FinalState)					//	
		{
			preNumdKey[6]=keyNum6->FinalState;
			if (keyNum6->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(6);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
		
						//按一下Num7键
		if (preNumdKey[7] != keyNum7->FinalState)					//	
		{
			preNumdKey[7]=keyNum7->FinalState;
			if (keyNum7->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(7);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
		
						//按一下Num8键
		if (preNumdKey[8] != keyNum8->FinalState)					//	
		{
			preNumdKey[8]=keyNum8->FinalState;
			if (keyNum8->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(8);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
		
						//按一下Num9键
		if (preNumdKey[9] != keyNum9->FinalState)					//	
		{
			preNumdKey[9]=keyNum9->FinalState;
			if (keyNum9->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(9);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}


				
			}

		}
						//按一下Num0键
		if (preNumdKey[0] != keyNum0->FinalState)					//	
		{
			preNumdKey[0]=keyNum0->FinalState;
			if (keyNum0->FinalState==false)
			{
				
				if(!sqStack->IsFull())       //非满
				{
				sqStack->Push(0);
				showNumber(sqStack->GetTop(),sqStack->GetTopElem() );
					
				}

				
			}

		}
	}
		//按一下DEL键
 	if (preNumdKey[10] != keyDel->FinalState)					//	
 	{
 		preNumdKey[10]=keyDel->FinalState;
		if (keyDel->FinalState==false)
		{
			if(!sqStack->IsEmpty())                                       //非空
			{
				showNumber(sqStack->GetTop(),10 );		//显示空白背景，代替清除		
				sqStack->Pop();

			}
			
		}

 	}
	
			//按一下MODE键
 	if (preNumdKey[11] != keyMode->FinalState)					//	
 	{
 		preNumdKey[11]=keyMode->FinalState;
		if (keyMode->FinalState==false)
		{
			controlModeFlag=1-controlModeFlag;         //实现1和0的切换
			
			showControlMode();     //
		
			//终止控制状态动作
			controlStatusFlag=0;

			
		}

 	}
	
	
				//按一下SHOCK键
 	if (preNumdKey[12] != keyShock->FinalState)					
 	{
 		preNumdKey[12]=keyShock->FinalState;
		if (keyShock->FinalState==false)
		{
			controlStatusFlag=2;         //电击
			
		}
		else
		{
			controlStatusFlag=0;         //没有操作
		}
		showControlStatus();

 	}
	//按一下ALARM键
	else if (preNumdKey[13] != keyAlarm->FinalState)					//	
 	{
 		preNumdKey[13]=keyAlarm->FinalState;
		if (keyAlarm->FinalState==false)
		{
			controlStatusFlag=1;         //警报		
			
		}
				else
		{
			controlStatusFlag=0;         //没有操作
		}
		showControlStatus();


 	}

	
	//喂狗
	static int keyActionFlag=0;
	if(!keyMode->FinalState || !keyAlarm->FinalState || !keyShock->FinalState || !keyDel->FinalState
		 || !keyNum1->FinalState || !keyNum2->FinalState || !keyNum3->FinalState || !keyNum4->FinalState
		 || !keyNum5->FinalState || !keyNum6->FinalState || !keyNum7->FinalState || !keyNum8->FinalState
	   || !keyNum9->FinalState || !keyNum0->FinalState)
	{
		watchDogTimer=3;//最好大于3，避免多余的刷屏显示
	
/*		
		//有按键后立即发送数据 2107-01-13
		//为了防止按下按键就一直发送数据，这里只允许发送2帧，第一帧用于唤醒，第二帧是有效数据
		if(keyActionFlag==0 || keyActionFlag==1)
		{
			
			//按键发送数据之前先关闭定时器发送数据
			t3->stop();
		//	updataToZigbee();
			
			keyActionFlag++;
			
			//第二次发送完按键数据后，打开定时器3，定时发送数据
			if(keyActionFlag==1)
			{
				t3->start();
			}
			
				
		}
		
	*/
	}
	//正常松开状态
	else
	{
		keyActionFlag=0;
	}


}


bool Process::anyKeyDown()
{
	//任意键按下
	if(!keyMode->FinalState || !keyAlarm->FinalState || !keyShock->FinalState || !keyDel->FinalState
		 || !keyNum1->FinalState || !keyNum2->FinalState || !keyNum3->FinalState || !keyNum4->FinalState
		 || !keyNum5->FinalState || !keyNum6->FinalState || !keyNum7->FinalState || !keyNum8->FinalState
	   || !keyNum9->FinalState || !keyNum0->FinalState)
	{
		
		return true;
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////LCD显示//////////////////////////////////////////////


/**
  * @brief  显示开机画面
  * @param  None
  * @retval None
  * @author:吴新有
  */
void Process::showStartPic()
{
	SysTick_DelayMs(80);
	LCDControl->ShowPic(0);    //下载主控端欢迎界面
	SysTick_DelayMs(1000);
}
/**
  * @brief  显示主控界面
  * @param  None
  * @retval None
  * @author:吴新有
  */
void Process::showMainWin()
{
	SysTick_DelayMs(80);
	LCDControl->ShowPic(1);    //下载操作界面
	SysTick_DelayMs(80);
}
/**
  * @brief  显示输入编码窗口
  * @param  None
  * @retval None
  * @author:吴新有
  */
void Process::showInputWin(bool flag)
{
	if(flag)
	{
		LCDControl->CutPicAndShow(2,120,119,188,55,66,141);    //显示输入编码框
	}
	else
	{
		LCDControl->CutPicAndShow(2,120,3,188,55,66,141);    //显示背景色
	}
	
}

/**
  * @brief  显示编码
  * @param  None
  * @retval pisiton:显示的第pisiton个数码
						num：   显示的数码
						如：按下的第position个数字是num
  * @author:吴新有
  
  */
void Process::showNumber(int position,int num)
{

	if(num>9)
	{
		LCDControl->CutPicAndShow(3,Coordinate_X[10],95,14,24,Coordinate_P[position],141);    //显示空白背景
	}
	else
	{
		LCDControl->CutPicAndShow(3,Coordinate_X[num],95,14,24,Coordinate_P[position],141);    //显示输入编码
	}
	
	
	
}

/**
  * @brief  显示电量图标
  * @param  None
  * @retval None
  * @author:吴新有
  */
void Process::showPower()
{
	getPowerValue();
	static u8 prePowerValue=0;
	if(prePowerValue!=powerValue)
	{
		prePowerValue=powerValue;
	//显示电量图标
		if(powerValue>120)
			LCDControl->CutPicAndShow(2,10,15,60,30,240,102);    //显示100
		else if(powerValue>115)
			LCDControl->CutPicAndShow(2,10,57,60,30,240,102);    //显示80
		else if(powerValue>113)
			LCDControl->CutPicAndShow(2,10,102,60,30,240,102);    //显示60
		else if(powerValue>111)
			LCDControl->CutPicAndShow(2,10,147,60,30,240,102);    //显示40
		else 
			LCDControl->CutPicAndShow(2,10,191,60,30,240,102);    //显示20
			
	}
	
	
}
/**
  * @brief  显示控制模式
  * @param  None
  * @retval None
  * @author:吴新有
  */
void Process::showControlMode()
{
	static u8 preControlModeFlag=0xff;
	
	if(preControlModeFlag!=controlModeFlag)
	{
		preControlModeFlag=controlModeFlag;
	//接近控制
		if(1==controlModeFlag)
		{
			LCDControl->CutPicAndShow(2,219,61,60,30,240,59);    //显示 接近控制
			showInputWin(false);
		}
		//定点控制
		else if(0==controlModeFlag)
		{
			LCDControl->CutPicAndShow(2,119,61,60,30,240,59);    //显示 定点控制
			showInputWin(true);
		}
		//清空栈
		sqStack->ClearStack();
		
	}
	
	
}

/**
  * @brief  显示控制模式
  * @param  None
  * @retval None
  * @author:吴新有
  */

void Process::showControlStatus()
{
	if(2==controlStatusFlag)     //正在电击
	{

		//显示电击
		LCDControl->CutPicAndShow(2,137,203,40,30,251,203);    //显示 电击

	}
	else if(1==controlStatusFlag) //正在警报
	{

		 //显示警报
		LCDControl->CutPicAndShow(2,209,203,40,30,251,203);    //显示 警报

	}
	else
	{
		//没有动作，显示空白
		LCDControl->CutPicAndShow(2,260,203,40,30,251,203);    //显示 空白
	}
	
}


////////////////////////////////////////////////////LCD显示完///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
  * @brief  更新发送数据
  * @param  None
  * @retval None
  * @author:吴新有
  */

void Process::updataToZigbee()
{
	//更新数据
	
	sendData[0]=(isFreeMode<<4) +(controlStatusFlag <<2)+controlModeFlag;
	
	//如果有控制动作时，则不更新本体ID号，避免在动作时，误改变了本体ID号，然后动作会随ID号转移。
	//所以在动作时，改变本体ID号是无效的。
	//if(0==controlStatusFlag)
	{
		for(int i=0;i<=sqStack->GetTop();i++)
		{
			sendData[i+1] = sqStack->st.data[i];
		}
		for(int i=sqStack->GetTop()+1;i<5;i++)
		{
			sendData[i+1] = 0;
		}
	}
	

	
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


/*******************************************************************************
* Function Name  : 从停机模式下唤醒之后： 配置系统时钟允许HSE，和 pll 作为系统时钟。
* Description    : Inserts a delay time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  Process::SYSCLKconfig_STOP(void)
{
		ErrorStatus HSEStartUpStatus;
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
/*--------------------------------- End of Process.cpp -----------------------------*/
