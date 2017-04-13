/**
  ******************************************************************************
	*文件：Key.cpp
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：按键类的实现
	*备注：
	*
  ******************************************************************************  
	*/ 
#include "Key.h"
// typedef enum
// { GPIO_Mode_AIN = 0x0,						//模拟输入
//   GPIO_Mode_IN_FLOATING = 0x04,  //浮空输入
//   GPIO_Mode_IPD = 0x28,					//带下拉输入
//   GPIO_Mode_IPU = 0x48,					//带上拉输入
//   GPIO_Mode_Out_OD = 0x14,				//开漏输出
//   GPIO_Mode_Out_PP = 0x10,				//推挽输出
//   GPIO_Mode_AF_OD = 0x1C,				//复用功能的开漏输出
//   GPIO_Mode_AF_PP = 0x18					//复用功能的推挽输出
// }GPIOMode_TypeDef;
/**
  * 功  能：按键类的构造函数
  * 参  数：
	* 	port：按键引脚所在的GPIO端口
	*		Pin : 按键引脚号
  * 返回值：无
  */
//Key::Key(GPIO_TypeDef *port,unsigned int Pin)
Key::Key(GPIO_TypeDef *port,unsigned int Pin, bool bLevel)
{
	keyPort=port;
	keyPin=Pin;
	keyLevel=bLevel;
	
	if(port==GPIOA)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if(port==GPIOB)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	}
	else if(port==GPIOC)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	}
	else if(port==GPIOD)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	}
	else if(port==GPIOE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	}
	else if(port==GPIOF)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	}
	else if(port==GPIOG)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	}
	initKey();
}

Key::Key()
{
}


/**
  * 功  能：按键类的析构函数
  * 参  数：无
  * 返回值：无
  */
Key::~Key()
{
	keyPort=NULL;
}

/**
  * 功  能：初始化灯的端口
  * 参  数：无
  * 返回值：无
  */
void Key::initKey()
{
	STM32_GpioOneInit(keyPin,GPIO_Speed_2MHz,GPIO_Mode_IPU,keyPort);	//上拉输入
	number=0;
	value=false;
}

/**
  * 功  能：返回按键值
  * 参  数：无
  * 返回值：
	* true:按键按下
	* false:按键没有按下，或者弹起了
  */
bool Key::getValue()
{
	if(keyLevel)
		return GPIO_ReadInputDataBit(keyPort,keyPin);
	else
		return !GPIO_ReadInputDataBit(keyPort,keyPin);
}
/**
  * 功  能：返回按键值，带防抖
  * 参  数：无
  * 返回值：
	* true:按键按下
	* false:按键没有按下，或者弹起了
  */
bool Key::getValue(int maxNumber)
{
	bool temValue= getValue();
	if(temValue!=value)
	{
		if(number>maxNumber)
		{
			value=!value;
			number=0;
		}
		else
		{
			number++;
		}
	}
	return value;
}


