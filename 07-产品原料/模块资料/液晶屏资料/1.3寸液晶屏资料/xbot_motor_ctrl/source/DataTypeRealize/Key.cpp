/**
  ******************************************************************************
	*文件：Key.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：按键类的实现
	*备注：
	*
  ******************************************************************************  
	*/ 
#include "Key.h"

/**
  * 功  能：按键类的构造函数
  * 参  数：
	* 	port：按键引脚所在的GPIO端口
	*		Pin : 按键引脚号
  * 返回值：无
  */
Key::Key(GPIO_TypeDef *port,unsigned int Pin)
{
	keyPort=port;
	keyPin=Pin;
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
	STM32_GpioOneInit(keyPin,GPIO_Speed_2MHz,GPIO_Mode_IPU,keyPort);	//浮空输入GPIO_Mode_IN_FLOATING
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
	return !GPIO_ReadInputDataBit(keyPort,keyPin);
}

