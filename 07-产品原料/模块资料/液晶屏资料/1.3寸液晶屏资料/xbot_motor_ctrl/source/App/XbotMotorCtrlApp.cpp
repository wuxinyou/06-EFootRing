/**
  ******************************************************************************
	*文件：XbotDisPlayApp.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：控制Xbot的主类的实现文件
	*备注：
	*
  ******************************************************************************  
	*/ 

/* 头文件包含 ------------------------------------------------------------------*/
#include "XbotMotorCtrlApp.h"
#ifdef __cplusplus
 extern "C" {
#endif
#include <stdlib.h>
// #include <math.h>								//数学函数库
#ifdef __cplusplus
 }
#endif

/* 类型定义 --------------------------------------------------------------------*/

/* 宏定义-----------------------------------------------------------------------*/ 

//绝对值宏
#define ABS(X)			((X)>0?(X):(-(X)))
// //二者绝对值谁大谁小宏
// #define MAX(X,Y)		((ABS(X))>(ABS(Y))?(ABS(X)):(ABS(Y)))
// #define MIN(X,Y)		((ABS(X))<(ABS(Y))?(ABS(X)):(ABS(Y)))

// #define LCD_PORT_NUM 					2
// #define LCD_PORT_BAUD					19200

#define MOTOR_PORT_NUM     3
#define MOTOR_PORT_BAUD    38400

#define 	BIG_ARM_ROTATE_DDR		0x00 //大臂旋转
#define 	BIG_ARM_PITCH_ADDR		0x01 //大臂俯仰
#define 	FORE_ARM_PITCH_DDR		0x02 //小臂俯仰
#define 	WRIST_PITCH_DDR				0x03 //手腕俯仰
#define 	WRIST_ROTATE_DDR			0x04 //手腕旋转
#define 	PAN_TILT_PITCH_DDR  	0x05 //云台俯仰
#define 	PAN_TILT_ROTATE_DDR 	0x06 //云台旋转
#define 	FINGER_MEDIATE_DDR 		0x07 //手爪夹持
 
#define KM 		0
#define KA 		1
#define KB 		2
#define KC 		3
#define KD 		4
#define KABD 	5
#define KNULL -1

#define KEYINDEX_MAX 3
#define KEYINDEX_ABD_MAX 150

#define DEBUF_NULL					0X00//无调试
#define DEBUF_CANCEL				0X8A//取消限位
#define DEBUF_RESET					0X8B//重设零位

const char *motorChar={"☆大臂旋转☆☆大臂俯仰☆☆小臂俯仰☆☆手腕俯仰☆☆手腕旋转☆☆云台俯仰☆☆云台旋转☆☆手爪夹持☆"};
const char *motorChar_1={"1.DBXZ(MJ)2.XBFY(MJ)3.SWFY(MJ)4.SWFY(MJ)5.SWXZ(MJ)6.YTFY(MJ)7.YTXZ(MJ)8.SZJC (MJ)"};

//构造函数
XbotMotorCtrlApp::XbotMotorCtrlApp()
{
	
	led=new LEDLighter(GPIOA,GPIO_Pin_10);
	led->setLightOn();
	lcdPort=new SpiPort(1);
	lcdPort->open();
	
	LCDManager=new LCDdisplay(lcdPort);															//控制液晶屏
  disPage_start();
	SysTick_DelayMs(1000);

	keysValue=-1;
	speed_v=0;
	speed_u=0;
	maxEle=5000;				//最大电流值
	eleValue=0;	 	//当前的电流值
	
	interfaceIndex=0; //界面编号
	motorIndex=0;			//电机编号
	angle=0;					//角度值
	barValue=0;				//角度进度条的值
	ctrlModel=0;				//控制模式 0 手动  1 自动
	speedModel=0;			//速度模式 0 V模式 1 U模式
	speed_direction=0;//速度方向
	
	specialCommand=0;	//特殊指令标志 0正常的速度指令，1，取消限位 2，零位重置
	maxAngle=new int16_t[8];
	minAngle=new int16_t[8];
	maxAngle[0]=27000;minAngle[0]=-27000;
	maxAngle[1]=150;  minAngle[1]=-18000;
	maxAngle[2]=18000;minAngle[2]=700;
	maxAngle[3]=18000;minAngle[3]=300;
	maxAngle[4]=27000;minAngle[4]=-27000;
	maxAngle[5]=7300; minAngle[5]=0;
	maxAngle[6]=-3000;minAngle[6]=18000;
	maxAngle[7]=27000;minAngle[7]=-27000;
	
	
	
	ele=new RingChain(100,0x28);					//电流缓存 波形图数据,初始化电流高度为0（H=40-0x28）
	
	
	fBMotorAngle=new ctrData();
	fBMotorEle=new ctrData();
	ADC1_Init();

	key_M=new Key(GPIOB,GPIO_Pin_7);
	key_A=new Key(GPIOB,GPIO_Pin_6);
	key_B=new Key(GPIOB,GPIO_Pin_5);
	key_C=new Key(GPIOB,GPIO_Pin_1);
	key_D=new Key(GPIOB,GPIO_Pin_0);


	motorPort=new SerialPort(MOTOR_PORT_NUM,MOTOR_PORT_BAUD);
	motorPort->setRS485CtrTx(GPIOB,GPIO_Pin_9);
	motor=new MotorControl(motorPort);		//和XBOT交互
	motor->setAddr(motorIndex);		//初始化设置大臂旋转的地址
	t1=new Timer(TIM2);
	t2=new Timer(TIM3);
	t3=new Timer(TIM4);
	
	t1->setTimeOut(20); //按键检测以及界面显示定时器
	t1->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM2_SubPriority);

	t2->setTimeOut(1); //处理数据的定时器
	t2->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM3_SubPriority);
	
	t3->setTimeOut(10);//数据发送的定时器
	t3->setPriority(STM32_NVIC_TIM2_PrePriority,STM32_NVIC_TIM4_SubPriority);
	disPage_main(-2,-1);
}

XbotMotorCtrlApp::~XbotMotorCtrlApp()
{
	t1->stop();
	t2->stop();
	t3->stop();
	motorPort->close();
	lcdPort->close();
	delete motorPort;					//和终端（XBOT控制箱）通信的串口
	delete lcdPort;							//和LCD液晶屏通信的串口
	delete t1;												//定时器1
	delete t2;												//定时器2
	delete t3;												//定时器3
	delete led;
	delete key_M;
	delete key_A;
	delete key_B;
	delete key_C;
	delete key_D;
}
void XbotMotorCtrlApp::initLCD()
{
	motorPort->open();
//打开定时器
	t1->start();
	t2->start();
	t3->start();
}

//开机欢迎界面
void XbotMotorCtrlApp::disPage_start()
{
	LCDManager->clearScreen(0X00);//清屏
	LCDManager->writeHanziLine(0,4,"☆☆★欢迎光临★",8,true,0,0x00);
	LCDManager->writeHanziLine(2,0,"☆排爆机器人磨机系统☆",11,true,0,0x00);
	LCDManager-> writeMediumCharLine(5,10,"checking motor...",17,true,false);
	
// 	LCDManager->writeMediumCharLine(7,10,"A.check again",21,true,false);			//显示中号字符串
}

void XbotMotorCtrlApp::disPage_main(int8_t key,int8_t backKey)
{
	static bool firstDis=false;
// 	static int8_t len=0;
	if(interfaceIndex!=0)
		return;
	if(!firstDis)//第一次显示
	{
		firstDis=true;
		LCDManager->clearScreen(0X00);//清屏
		speed_direction=0;
		LCDManager->writeHanzi(0,18,"《",false,0,0);
		LCDManager->writeHanziLine(0,32,motorChar,6,true,0,0x00,motorIndex*6);
		LCDManager->writeHanzi(0,106,"》",false,0,0);
		LCDManager->writeLargeChar_1(2,20,'A',true);
		LCDManager->writeHanziLine(2,31,"进入磨机模式",6,true,1,0x80);
		LCDManager->writeLargeChar_1(4,20,'B',true);
		LCDManager->writeHanziLine(4,31,"进入校零模式",6,true,1,0x80);
		LCDManager->writeLargeChar_1(6,20,'C',true);
		LCDManager->writeHanziLine(6,31,"角度区间设置",6,true,1,0x80);
		ctrlModel=0;//手动
		//停止电机转动
		motor->resetSpeed();
		motor->setPWMSpeed(0);
		return;
	}
	switch(keysValue)
	{
		case -1://按键复位
			if(backKey==0)
			{
				LCDManager->writeHanzi(0,18,"《",false,0,0);
				motorIndex++;
				if(motorIndex>7)
					motorIndex=0;
				motor->setAddr(motorIndex);
				LCDManager->writeHanziLine(0,32,motorChar,6,true,0,0x00,motorIndex*6);
			}
			else if(backKey==1)
			{
				LCDManager->writeHanzi(0,106,"》",false,0,0);
				motorIndex--;
				if(motorIndex<0)
					motorIndex=7;
				motor->setAddr(motorIndex);
				LCDManager->writeHanziLine(0,32,motorChar,6,true,0,0x00,motorIndex*6);
			}
			else if(backKey==2)
			{
				LCDManager->writeHanziLine(2,31,"进入磨机模式",6,true,1,0x80);
				interfaceIndex=1;
				firstDis=false;
			}
			else if(backKey==3)
			{
				LCDManager->writeHanziLine(4,31,"进入校零模式",6,true,1,0x80);
				interfaceIndex=2;
				firstDis=false;
			}
			else if(backKey==4)
			{
				LCDManager->writeHanziLine(6,31,"角度区间设置",6,true,1,0x80);
				interfaceIndex=3;
				firstDis=false;
			}
			break;
		case 0:	//《键按下
			if(backKey!=0)
				LCDManager->writeHanzi(0,18,"《",true,0,0);
			break;
		case 1:	//》键按下
			if(backKey!=1)
				LCDManager->writeHanzi(0,106,"》",true,0,0);
			break;
		case 2:	//A键按下
			if(backKey!=2)
				LCDManager->writeHanziLine(2,31,"进入磨机模式",6,false,1,0x80);
			break;
		case 3://B键按下
			if(backKey!=3)
				LCDManager->writeHanziLine(4,31,"进入校零模式",6,false,1,0x80);
			break;
		case 4://C键按下
			LCDManager->writeHanziLine(6,31,"角度区间设置",6,false,1,0x80);
			break;
		case 5://3键同时按下 进入隐藏界面
			break;
	}
}

void XbotMotorCtrlApp::disPage_auto(int8_t key,int8_t backKey)
{
	static bool firstDis=false;

	int8_t len=0;


	if(interfaceIndex!=1)
		return;
	if(!firstDis)//第一次显示
	{
		firstDis=true;
		LCDManager->clearScreen(0X00);//清屏
		LCDManager->writeMediumCharLine(0,2,motorChar_1,10,true,false,motorIndex*10);			//显示中号字符串

		LCDManager->writeMediumCharLine(1,26,"<",1,false,false);			//显示中号字符串
		LCDManager->writeMediumCharLine(1,125,">",1,false,false);			//显示中号字符串
		LCDManager->setXY(2,28);
		LCDManager->sendData(0xff);
		LCDManager->setXY(3,28);
		LCDManager->sendData(0xff);
		LCDManager->setXY(4,28);
		LCDManager->sendData(0xff);
		LCDManager->setXY(5,28);
		LCDManager->sendData(0xff);
		LCDManager->setXY(6,28);
		LCDManager->sendData(0xff);
		LCDManager->disMediumNum(6,1,0,true,false);				//电流最小值
		if(speedModel==0)
			LCDManager->writeMediumCharLine(0,94,"V",1,true,false);			//显示中号字符串
		else
			LCDManager->writeMediumCharLine(0,94,"U",1,true,false);			//显示中号字符串
		if(ctrlModel==0)
		{
			LCDManager->writeMediumCharLine(0,66,"HAND",4,true,false);											//显示中号字符串
			LCDManager->writeMediumCharLine(7,3,"A.start B.U/V C.back",20,true,false);			//显示中号字符串
		}
		else
		{
			LCDManager->writeMediumCharLine(0,66,"AUTO",4,true,false);											//显示中号字符串
			LCDManager->writeMediumCharLine(7,3,"A.stop  B.U/V C.back",20,true,false);			//显示中号字符串
		}
		for(int i=0;i<100;i++)
		{
			ele->setData(0x28);
			ele->next();
		}
		
		LCDManager->disMediumNum(2,1,maxEle,true,false);				//显示电流最大值
		
		len=LCDManager->disMediumNum(1,1,angle,true,false); 			//显示角度值
		if(len<4)
		{
			LCDManager->setXY(1,1+len*6);
			for(int i=0;i<4-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		LCDManager->displayProgressBar(1,32,barValue);						//显示角度的进度条
		
		if(speedModel==0)
			len=LCDManager->disMediumNum(0,100,speed_v,true,false);
		else
			len=LCDManager->disMediumNum(0,100,speed_u,true,false);
		if(len<5)
		{
			LCDManager->setXY(0,100+len*6);
			for(int i=0;i<5-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		len=LCDManager->disMediumNum(4,1,eleValue,true,false);
		if(len<4)
		{
			LCDManager->setXY(4,1+len*6);
			for(int i=0;i<4-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		motor->resetSpeed();
		motor->setPWMSpeed(0);
		return;
	}
	switch(keysValue)
	{
		case -1://按键复位
			if(backKey==0)
			{
				LCDManager->writeMediumCharLine(1,26,"<",1,false,false);			//显示中号字符串
				speed_direction=0;
			}
			else if(backKey==1)
			{
				LCDManager->writeMediumCharLine(1,125,">",1,false,false);			//显示中号字符串
				speed_direction=0;
			}
			else if(backKey==2)
			{
				if(ctrlModel==0)
				{
					ctrlModel=1;
					LCDManager->writeMediumCharLine(0,66,"AUTO",4,true,false);											//显示中号字符串
					LCDManager->writeMediumCharLine(7,3,"A.stop  B.U/V C.back",20,true,false);			//显示中号字符串
				}
				else
				{
					ctrlModel=0;
					LCDManager->writeMediumCharLine(0,66,"HAND",4,true,false);											//显示中号字符串
					LCDManager->writeMediumCharLine(7,3,"A.start B.U/V C.back",20,true,false);			//显示中号字符串
				}
			}
			else if(backKey==3)
			{
				if(speedModel==0)
				{
					speedModel=1;
					LCDManager->writeMediumCharLine(0,94,"U",1,true,false);			//显示中号字符串
				}
				else
				{
					speedModel=0;
					LCDManager->writeMediumCharLine(0,94,"V",1,true,false);			//显示中号字符串
				}
			}
			else if(backKey==4)
			{
				interfaceIndex=0;
				firstDis=false;
			}
			break;
		case 0:	//《键按下
			if(backKey!=0&&ctrlModel==0)
				LCDManager->writeMediumCharLine(1,26,"<",1,true,false);			//显示中号字符串
				if(ctrlModel==0)
					speed_direction=1;
			break;
		case 1:	//》键按下
			if(backKey!=1&&ctrlModel==0)
				LCDManager->writeMediumCharLine(1,125,">",1,true,false);			//显示中号字符串
				if(ctrlModel==0)
					speed_direction=2;
			break;
// 		case 2:	//A键按下

// 			break;
// 		case 3://B键按下
// 			if(speedModel==0)
// 				LCDManager->writeMediumCharLine(0,94,"V",1,false,false);			//显示中号字符串
// 			else
// 				LCDManager->writeMediumCharLine(0,94,"U",1,false,false);			//显示中号字符串
// 			break;
// 		case 4://C键按下

// 			break;
		case 5://3键同时按下 进入隐藏界面
			break;
	}
	
	LCDManager->disMediumNum(2,1,maxEle,true,false);				//显示电流最大值
	
	len=LCDManager->disMediumNum(1,1,angle,true,false); 			//显示角度值
	if(len<4)
	{
		LCDManager->setXY(1,1+len*6);
		for(int i=0;i<4-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
	LCDManager->displayProgressBar(1,32,barValue);						//显示角度的进度条

//速度值显示
	if(speedModel==0)
		len=LCDManager->disMediumNum(0,100,speed_v,true,false);
	else
		len=LCDManager->disMediumNum(0,100,speed_u,true,false);
	if(len<5)
	{
		LCDManager->setXY(0,100+len*6);
		for(int i=0;i<5-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
//电流值显示
	len=LCDManager->disMediumNum(4,1,eleValue,true,false);
	if(len<4)
	{
		LCDManager->setXY(4,1+len*6);
		for(int i=0;i<4-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
// 	backEle=ele->getData();
// 	ele->setData(40-eleValue/100);
// 	for(int8_t i=0;i<100;i++)
// 	{
// 		temPosY=29+i;
// 		if(i==0)
// 			lastEle=backEle;
// 		else
// 			lastEle=ele->getData();
// 		ele->next();//数据指针指向下一个
// 		currentEle=ele->getData();
// 		if(lastEle>currentEle)//上一帧的高度小于当前帧
// 		{
// 			if(currentEle<9&&currentEle>0)
// 			{
// 				LCDManager->setXY(2,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-1));
// 				if(lastEle>32)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(6,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 				else if(lastEle>24)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 				else if(lastEle>16)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 				else if(lastEle>8)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 			}
// 			else if(currentEle<17)
// 			{
// 				LCDManager->setXY(3,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-9));
// 				if(lastEle>32)//上一帧的高度
// 				{
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(6,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 				else if(lastEle>24)//上一帧的高度
// 				{
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 				else if(lastEle>16)//上一帧的高度
// 				{
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 			}
// 			else if(currentEle<25)
// 			{
// 				LCDManager->setXY(4,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-17));
// 				if(lastEle>32)//上一帧的高度
// 				{
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0xff);
// 					LCDManager->setXY(6,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 				else if(lastEle>24)//上一帧的高度
// 				{
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 			}
// 			else if(currentEle<33)
// 			{
// 				LCDManager->setXY(5,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-25));
// 				if(lastEle>32)//上一帧的高度没有覆盖最底层
// 				{
// 					LCDManager->setXY(6,temPosY);
// 					LCDManager->sendData(0xff);
// 				}
// 			}
// 			else if(currentEle<41)
// 			{
// 				LCDManager->setXY(6,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-33));
// 			}
// 		}
// 		else if(lastEle<currentEle)//上一帧的高度大于当前帧
// 		{
// 			if(currentEle<9&&currentEle>0)
// 			{
// 				LCDManager->setXY(2,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-1));
// 			}
// 			else if(currentEle<17)
// 			{
// 				LCDManager->setXY(3,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-9));
// 				if(lastEle<9)//上一帧的高度
// 				{
// 					LCDManager->setXY(2,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 			}
// 			else if(currentEle<25)
// 			{
// 				LCDManager->setXY(4,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-17));
// 				if(lastEle<9)//上一帧的高度
// 				{
// 					LCDManager->setXY(2,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 				else if(lastEle<17)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 			}
// 			else if(currentEle<33)
// 			{
// 				LCDManager->setXY(5,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-25));
// 				if(lastEle<9)//上一帧的高度
// 				{
// 					LCDManager->setXY(2,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 				else if(lastEle<17)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 				else if(lastEle<25)//上一帧的高度
// 				{
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 			}
// 			else if(currentEle<41)
// 			{
// 				LCDManager->setXY(6,temPosY);
// 				LCDManager->sendData(0xff<<(currentEle-33));
// 				if(lastEle<9)//上一帧的高度
// 				{
// 					LCDManager->setXY(2,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 				else if(lastEle<17)//上一帧的高度
// 				{
// 					LCDManager->setXY(3,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 				else if(lastEle<25)//上一帧的高度
// 				{
// 					LCDManager->setXY(4,temPosY);
// 					LCDManager->sendData(0x00);
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 				else if(lastEle<33)//上一帧的高度
// 				{
// 					LCDManager->setXY(5,temPosY);
// 					LCDManager->sendData(0x00);
// 				}
// 			}
// 		}
// 	}
// 	ele->next();

}
void XbotMotorCtrlApp::disPage_set(int8_t key,int8_t backKey)
{
	static bool firstDis=false;
	int8_t len=0;
	if(interfaceIndex!=2)
		return;
	if(!firstDis)//第一次显示
	{
		firstDis=true;
		LCDManager->clearScreen(0X00);//清屏
		LCDManager->writeHanziLine(0,16,motorChar,6,true,0,0x00,motorIndex*6);	//显示标题 16+6*12
		//显示速度模式
		if(speedModel==0)
			LCDManager->writeLargeChar(0,94,'V',true,false,false);			//显示大号字符
		else
			LCDManager->writeLargeChar(0,94,'U',true,false,false);			//显示大号字符
		//显示速度
		LCDManager->writeMediumCharLine(3,2,"speed:",6,true,false);//显示中号字符串
		if(speedModel==0)
			len=LCDManager->disMediumNum(3,38,speed_v,true,false);
		else
			len=LCDManager->disMediumNum(3,38,speed_u,true,false);
		if(len<5)
		{
			LCDManager->setXY(3,38+len*6);
			for(int i=0;i<5-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		//显示角度
		LCDManager->writeMediumCharLine(3,68,"angle:",6,true,false);//显示中号字符串
		len=LCDManager->disMediumNum(3,104,angle,true,false);
		if(len<4)
		{
			LCDManager->setXY(3,104+len*6);
			for(int i=0;i<4-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		//显示角度进度条
		LCDManager->writeMediumCharLine(5,26,"<",1,false,false);			//显示中号字符串
		LCDManager->writeMediumCharLine(5,125,">",1,false,false);			//显示中号字符串
		LCDManager->displayProgressBar(5,32,barValue);										//显示角度的进度条
		//显示菜单
		LCDManager->writeMediumCharLine(7,3,"A.cancel B.set C.back",21,true,false);			//显示中号字符串
		return;
	}
	//检测按键
	switch(keysValue)
	{
		case -1://按键复位
			if(backKey==0)
			{
// 				LCDManager->writeMediumCharLine(5,26,"<",1,false,false);			//显示中号字符串
// 				speed_direction=0;
			}
			else if(backKey==1)
			{
				specialCommand=3;
			}
			else if(backKey==2)//A键弹起
			{
				specialCommand=1;
			}
			else if(backKey==3)
			{
				specialCommand=2;
			}
			else if(backKey==4)
			{
				interfaceIndex=0;
				firstDis=false;
			}
			break;
		case 0:	//《键按下
// 			if(backKey!=0&&ctrlModel==0)
// 				LCDManager->writeMediumCharLine(5,26,"<",1,true,false);			//显示中号字符串
// 			if(ctrlModel==0)
// 				speed_direction=1;
			break;
		case 1:	//》键按下
// 			if(backKey!=1&&ctrlModel==0)
// 				LCDManager->writeMediumCharLine(5,125,">",1,true,false);			//显示中号字符串
// 			if(ctrlModel==0)
// 				speed_direction=2;
			break;
		case 2:	//A键按下

			break;
		case 3://B键按下
			
			break;
		case 4://C键按下

			break;
		case 5://3键同时按下 进入隐藏界面
			break;
	}

	if(speedModel==0)
		len=LCDManager->disMediumNum(3,38,speed_v,true,false);
	else
		len=LCDManager->disMediumNum(3,38,speed_u,true,false);
	if(len<5)
	{
		LCDManager->setXY(3,38+len*6);
		for(int i=0;i<5-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
	//显示角度
	len=LCDManager->disMediumNum(3,104,angle,true,false);
	if(len<4)
	{
		LCDManager->setXY(3,104+len*6);
		for(int i=0;i<4-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
	//显示角度进度条
	LCDManager->displayProgressBar(5,32,barValue);										//显示角度的进度条
}

void XbotMotorCtrlApp::disPage_calibration(int8_t key,int8_t backKey)
{
	static bool firstDis=false;
	int8_t len=0;
	if(interfaceIndex!=3)
		return;
	if(!firstDis)//第一次显示
	{
		firstDis=true;
		LCDManager->clearScreen(0X00);//清屏
		LCDManager->writeHanziLine(0,16,motorChar,6,true,0,0x00,motorIndex*6);	//显示标题 16+6*12
		//显示速度模式
		if(speedModel==0)
			LCDManager->writeLargeChar(0,94,'V',true,false,false);			//显示大号字符
		else
			LCDManager->writeLargeChar(0,94,'U',true,false,false);			//显示大号字符
		//显示速度
		LCDManager->writeMediumCharLine(3,2,"speed:",6,true,false);//显示中号字符串
		if(speedModel==0)
			len=LCDManager->disMediumNum(3,38,speed_v,true,false);
		else
			len=LCDManager->disMediumNum(3,38,speed_u,true,false);
		if(len<5)
		{
			LCDManager->setXY(3,38+len*6);
			for(int i=0;i<5-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		//显示角度
		LCDManager->writeMediumCharLine(3,68,"angle:",6,true,false);//显示中号字符串
		len=LCDManager->disMediumNum(3,104,angle,true,false);
		if(len<4)
		{
			LCDManager->setXY(3,104+len*6);
			for(int i=0;i<4-len;i++)
			{
				LCDManager->writeMediumChar(' ',true,false);
			}
		}
		//显示角度进度条
		LCDManager->writeMediumCharLine(5,26,"<",1,false,false);			//显示中号字符串
		LCDManager->writeMediumCharLine(5,125,">",1,false,false);			//显示中号字符串
		LCDManager->displayProgressBar(5,32,barValue);										//显示角度的进度条
		//显示菜单
		LCDManager->writeMediumCharLine(7,12,"A.max B.min C.back",18,true,false);			//显示中号字符串
		return;
	}
	//检测按键
	switch(keysValue)
	{
		case -1://按键复位
			if(backKey==0)
			{
				LCDManager->writeMediumCharLine(5,26,"<",1,false,false);			//显示中号字符串
				speed_direction=0;
			}
			else if(backKey==1)
			{
				LCDManager->writeMediumCharLine(5,125,">",1,false,false);			//显示中号字符串
				speed_direction=0;
			}
			else if(backKey==2)
			{

			}
			else if(backKey==3)
			{

			}
			else if(backKey==4)
			{
				interfaceIndex=0;
				firstDis=false;
			}
			break;
		case 0:	//《键按下
			if(backKey!=0&&ctrlModel==0)
				LCDManager->writeMediumCharLine(5,26,"<",1,true,false);			//显示中号字符串
			if(ctrlModel==0)
				speed_direction=1;
			break;
		case 1:	//》键按下
			if(backKey!=1&&ctrlModel==0)
				LCDManager->writeMediumCharLine(5,125,">",1,true,false);			//显示中号字符串
			if(ctrlModel==0)
				speed_direction=2;
			break;
		case 2:	//A键按下

			break;
		case 3://B键按下
			
			break;
		case 4://C键按下

			break;
		case 5://3键同时按下 进入隐藏界面
			break;
	}

	if(speedModel==0)
		len=LCDManager->disMediumNum(3,38,speed_v,true,false);
	else
		len=LCDManager->disMediumNum(3,38,speed_u,true,false);
	if(len<5)
	{
		LCDManager->setXY(3,38+len*6);
		for(int i=0;i<5-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
	//显示角度
	len=LCDManager->disMediumNum(3,104,angle,true,false);
	if(len<4)
	{
		LCDManager->setXY(3,104+len*6);
		for(int i=0;i<4-len;i++)
		{
			LCDManager->writeMediumChar(' ',true,false);
		}
	}
	//显示角度进度条
	LCDManager->displayProgressBar(5,32,barValue);										//显示角度的进度条

}


void XbotMotorCtrlApp::disPage_warning()
{
	LCDManager->clearScreen(0X00);//清屏
	LCDManager->writeHanziLine(0,4,"☆☆☆☆警告☆☆☆☆",10,false,0,0x00);
}

/**
  * 功  能：运行在定时器3中的函数，处理数据
  * 参  数：无
  * 返回值：无
  * 备  注：
  */
void XbotMotorCtrlApp::disposeMotorData()
{
	uint8_t* temData=BaseControl::getReciveData(MOTOR_PORT_NUM-1);//获取串口接收的数据
	if(temData[2]==0x41||temData[2]==0x68||temData[2]==0x43)//反馈的是由带回传的速度设定指令而触发的角度反馈 0x41 0x68 0x43
	{
		fBMotorAngle->DATA_8[1]=temData[3];			//高八位
		fBMotorAngle->DATA_8[0]=temData[4];			//低八位
	}
	else if(temData[2]==0x5b)//电流缓存
	{
		fBMotorEle->DATA_8[1]=temData[3];			//高八位
		fBMotorEle->DATA_8[0]=temData[4];			//低八位
	}
}
void XbotMotorCtrlApp::runOnTime2()
{
	static uint16_t keyIndex = 0;
// 	static int8_t bacdKeysValue=-1;
	static bool backKaym=0;
	static bool backKaya=0;
	static bool backKayb=0;
	static bool backKayc=0;
	static bool bacdKayd=0;

	bool kaym=key_M->getValue();
	bool kaya=key_A->getValue();
	bool kayb=key_B->getValue();
	bool kayc=key_C->getValue();
	bool kayd=key_D->getValue();
	
	if(kaym!=backKaym)
	{
		keyIndex=0;
		backKaym=kaym;
	}
	if(kaya!=backKaya)
	{
		keyIndex=0;
		backKaya=kaya;
	}
	if(kayb!=backKayb)
	{
		keyIndex=0;
		backKayb=kayb;
	}
	if(kayc!=backKayc)
	{
		keyIndex=0;
		backKayc=kayc;
	}
	if(kayd!=bacdKayd)
	{
		keyIndex=0;
		bacdKayd=kayd;
	}
	
	if(kaym&&!kaya&&!kayb&&!kayc&&!kayd)
	{
		if(keyIndex>KEYINDEX_MAX)
		{
			keysValue=KM;
		}
		else
			keyIndex++;
	}
	else 	if(!kaym&&kaya&&!kayb&&!kayc&&!kayd)
	{
		if(keyIndex>KEYINDEX_MAX)
		{
			keysValue=KA;
		}
		else
			keyIndex++;
	}
	else 	if(!kaym&&!kaya&&kayb&&!kayc&&!kayd)
	{
		if(keyIndex>KEYINDEX_MAX)
		{
			keysValue=KB;
		}
		else
			keyIndex++;
	}
	else 	if(!kaym&&!kaya&&!kayb&&kayc&&!kayd)
	{
		if(keyIndex>KEYINDEX_MAX)
		{
			keysValue=KC;
		}
		else
			keyIndex++;
	}
	else 	if(!kaym&&!kaya&&!kayb&&!kayc&&kayd)
	{
		if(keyIndex>KEYINDEX_MAX)
		{
			keysValue=KD;
		}
		else
			keyIndex++;
	}
	else 	if(!kaym&&kaya&&kayb&&!kayc&&kayd)
	{
		if(keyIndex>KEYINDEX_ABD_MAX)
		{
			keysValue=KABD;
		}
		else
			keyIndex++;
	}
	else
	{
		keyIndex=0;
		keysValue=-1;
	}

// 	int8_t currentkeysValue=keysValue;//取按键值
// 	disPage_main(currentkeysValue,bacdKeysValue);
// 	disPage_auto(currentkeysValue,bacdKeysValue);
// 	disPage_set(currentkeysValue,bacdKeysValue);
// 	disPage_calibration(currentkeysValue,bacdKeysValue);
// 	if(bacdKeysValue!=currentkeysValue)
// 	{
// 		bacdKeysValue=currentkeysValue;
// 	}
}
/**
  * 功  能：运行在定时器3中的函数
  * 参  数：无
  * 返回值：无
  * 
  */
const int MAXSPEED[8]={5,5,5,5,9,9,9,2};

void XbotMotorCtrlApp::runOnTime3()
{
	
	disPage_warning();
	//赋值发送的数据 主要是速度值
	static int8_t bacdKeysValue=-1;
	int8_t currentkeysValue=keysValue;//取按键值
	int8_t backEle=0;
	int8_t temPosY=0;
	int8_t lastEle=0;
	int8_t currentEle=0;
	uint8_t* temData=NULL;
	static bool runToMaxAngle=false;
	int32_t speed=0;
	motor->resetSpeed();
	if(BaseControl::reciveData(motorPort))		  //接收并处理机械臂及其他设备反馈的数据
	{
// 		disposeMotorData();												//赋值到中间变量
		temData=BaseControl::getReciveData(MOTOR_PORT_NUM-1);//获取串口接收的数据
		if(temData[2]==0x41||temData[2]==0x68||temData[2]==0x43)//反馈的是由带回传的速度设定指令而触发的角度反馈 0x41 0x68 0x43
		{
			fBMotorAngle->DATA_8[1]=temData[3];			//高八位
			fBMotorAngle->DATA_8[0]=temData[4];			//低八位
		}
		else if(temData[2]==0x5b)//电流缓存
		{
			fBMotorEle->DATA_8[1]=temData[3];			//高八位
			fBMotorEle->DATA_8[0]=temData[4];			//低八位
			eleValue=ABS(fBMotorEle->DATA_16);//电流取绝对值
			if(eleValue>4999)
				eleValue=4999;
			if(interfaceIndex==1)
			{
				backEle=ele->getData();
				ele->setData(40-eleValue/125);
				for(int8_t i=0;i<100;i++)
				{
					temPosY=29+i;
					if(i==0)
						lastEle=backEle;
					else
						lastEle=ele->getData();
					ele->next();//数据指针指向下一个
					currentEle=ele->getData();
					if(lastEle>currentEle)//上一帧的高度小于当前帧
					{
						if(currentEle<9&&currentEle>0)
						{
							LCDManager->setXY(2,temPosY);
							LCDManager->sendData(0xff<<(currentEle-1));
							if(lastEle>32)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(6,temPosY);
								LCDManager->sendData(0xff);
							}
							else if(lastEle>24)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0xff);
							}
							else if(lastEle>16)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0xff);
							}
							else if(lastEle>8)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0xff);
							}
						}
						else if(currentEle<17)
						{
							LCDManager->setXY(3,temPosY);
							LCDManager->sendData(0xff<<(currentEle-9));
							if(lastEle>32)//上一帧的高度
							{
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(6,temPosY);
								LCDManager->sendData(0xff);
							}
							else if(lastEle>24)//上一帧的高度
							{
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0xff);
							}
							else if(lastEle>16)//上一帧的高度
							{
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0xff);
							}
						}
						else if(currentEle<25)
						{
							LCDManager->setXY(4,temPosY);
							LCDManager->sendData(0xff<<(currentEle-17));
							if(lastEle>32)//上一帧的高度
							{
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0xff);
								LCDManager->setXY(6,temPosY);
								LCDManager->sendData(0xff);
							}
							else if(lastEle>24)//上一帧的高度
							{
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0xff);
							}
						}
						else if(currentEle<33)
						{
							LCDManager->setXY(5,temPosY);
							LCDManager->sendData(0xff<<(currentEle-25));
							if(lastEle>32)//上一帧的高度没有覆盖最底层
							{
								LCDManager->setXY(6,temPosY);
								LCDManager->sendData(0xff);
							}
						}
						else if(currentEle<41)
						{
							LCDManager->setXY(6,temPosY);
							LCDManager->sendData(0xff<<(currentEle-33));
						}
					}
					else if(lastEle<currentEle)//上一帧的高度大于当前帧
					{
						if(currentEle<9&&currentEle>0)
						{
							LCDManager->setXY(2,temPosY);
							LCDManager->sendData(0xff<<(currentEle-1));
						}
						else if(currentEle<17)
						{
							LCDManager->setXY(3,temPosY);
							LCDManager->sendData(0xff<<(currentEle-9));
							if(lastEle<9)//上一帧的高度
							{
								LCDManager->setXY(2,temPosY);
								LCDManager->sendData(0x00);
							}
						}
						else if(currentEle<25)
						{
							LCDManager->setXY(4,temPosY);
							LCDManager->sendData(0xff<<(currentEle-17));
							if(lastEle<9)//上一帧的高度
							{
								LCDManager->setXY(2,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0x00);
							}
							else if(lastEle<17)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0x00);
							}
						}
						else if(currentEle<33)
						{
							LCDManager->setXY(5,temPosY);
							LCDManager->sendData(0xff<<(currentEle-25));
							if(lastEle<9)//上一帧的高度
							{
								LCDManager->setXY(2,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0x00);
							}
							else if(lastEle<17)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0x00);
							}
							else if(lastEle<25)//上一帧的高度
							{
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0x00);
							}
						}
						else if(currentEle<41)
						{
							LCDManager->setXY(6,temPosY);
							LCDManager->sendData(0xff<<(currentEle-33));
							if(lastEle<9)//上一帧的高度
							{
								LCDManager->setXY(2,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0x00);
							}
							else if(lastEle<17)//上一帧的高度
							{
								LCDManager->setXY(3,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0x00);
							}
							else if(lastEle<25)//上一帧的高度
							{
								LCDManager->setXY(4,temPosY);
								LCDManager->sendData(0x00);
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0x00);
							}
							else if(lastEle<33)//上一帧的高度
							{
								LCDManager->setXY(5,temPosY);
								LCDManager->sendData(0x00);
							}
						}
					}
				}
				ele->next();
			}
		}
	}
	speed=getADCValue();												//速度由电位器给出
	angle=fBMotorAngle->DATA_16/100;								//角度赋值
	barValue=((int32_t)(fBMotorAngle->DATA_16-minAngle[motorIndex])*90/(int32_t)(maxAngle[motorIndex]-minAngle[motorIndex]))-45;


	speed_v=speed*MAXSPEED[motorIndex]/4;//0-5000 speed*5/4 * 9/5
	speed_u=speed*8;	//0-32000

	if((fBMotorAngle->DATA_16>maxAngle[motorIndex]-500||fBMotorAngle->DATA_16<minAngle[motorIndex]+500)&&ctrlModel==1)
	{
		speed_v=speed_v/5;
		speed_u=speed_u/5;
	}
	
	if(fBMotorAngle->DATA_16>maxAngle[motorIndex]-200&&runToMaxAngle==true)
	{
		runToMaxAngle=false;
	}
	if(fBMotorAngle->DATA_16<minAngle[motorIndex]+200&&runToMaxAngle==false)
	{
		runToMaxAngle=true;
	}
	if(ctrlModel==0)//手动模式
	{
		if(speed_direction==0)
		{
			motor->setPWMSpeed(0);
		}
		else if(speed_direction==1)
		{
			if(speedModel==0)//v
			{
				motor->setSpeed(-speed_v);
			}
			else 
			{
				motor->setPWMSpeed(-speed_u);
			}
		}
		else if(speed_direction==2)
		{
			if(speedModel==0)//v
			{
				motor->setSpeed(speed_v);
			}
			else 
			{
				motor->setPWMSpeed(speed_u);
			}
		}
	}
	else//自动模式
	{
		if(runToMaxAngle)
		{
			if(speedModel==0)//v
			{
				motor->setSpeed(speed_v);
			}
			else 
			{
				motor->setPWMSpeed(speed_u);
			}
		}
		else
		{
			if(speedModel==0)//v
			{
				motor->setSpeed(-speed_v);
			}
			else 
			{
				motor->setPWMSpeed(-speed_u);
			}
		}
	}

	disPage_main(currentkeysValue,bacdKeysValue);
	disPage_auto(currentkeysValue,bacdKeysValue);
	disPage_set(currentkeysValue,bacdKeysValue);
	disPage_calibration(currentkeysValue,bacdKeysValue);
	if(bacdKeysValue!=currentkeysValue)
	{
		bacdKeysValue=currentkeysValue;
	}	
	
	
	
	
	
	motor->	updateUspeedTime();									//更新U0指令的时间
}
/**
  * 功  能：运行在定时器4中的函数 10ms
  * 参  数：无
  * 返回值：无
  * 
  */
void XbotMotorCtrlApp::runOnTime4()
{
	static bool flag=false;
	if(specialCommand!=0)
	{
		if(specialCommand==1)//取消限位
		{
			motor->sendAngleCommand(0x02);//限位失效指令
		}
		else
		if(specialCommand==2)//置零位
		{
			motor->sendAngleCommand(0x06);//设置零位
		}
		else if(specialCommand==2)//置零位
		{
			motor->sendAngleCommand(0x07);//保存角度
		}
		specialCommand=0;			//复位特殊指令
		return;
	}
	led->reverseLight();
	if(flag)
	{
		motor->sendSpeedCommand();
		flag=false;
	}
	else
	{
		motor->sendCurrentCommand(0x0b);//检测电流
		flag=true;
	}
}





