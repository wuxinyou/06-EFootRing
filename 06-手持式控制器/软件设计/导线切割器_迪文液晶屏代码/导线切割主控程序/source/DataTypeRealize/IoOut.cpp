/**
  ******************************************************************************
	*文件：IoOut.cpp
	*作者：孟浩
	*版本：1.0
	*日期：2015-03-09
	*概要：IoOut类的实现
	*备注：
	*
  ******************************************************************************  
	*/ 
	
#include "IoOut.h"
/**
  * 功  能：IoOut构造函数
  * 参  数：port		=	[GPIOA(默认),GPIOB,GPIOC,GPIOD,GPIOB,GPIOE,GPIOF,GPIOG]
						Pin			=	[GPIO_Pin_0(默认)~GPIO_Pin_15]
						bLevel	=	[false(默认)，true]
  * 返回值：无
  */
IoOut::IoOut(GPIO_TypeDef *port,unsigned int pin, bool bLevel)
{
	ioOutPort=port;
	ioOutPin=pin;
	ioOutLevel = bLevel;
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
	initIo();
}


IoOut::~IoOut()
{
// 	delete ioOutPort;
}

/**
  * 功  能：初始化Io的端口
  */
void IoOut::initIo()
{
	STM32_GpioOneInit(ioOutPin,GPIO_Speed_2MHz,GPIO_Mode_Out_PP,ioOutPort);	//推挽输出
}

/**
  * 功  能：翻转Io的状态
  */
void IoOut::reverseState()	
{
	 GPIO_WriteBit(ioOutPort, ioOutPin, (BitAction)(1 - GPIO_ReadOutputDataBit(ioOutPort, ioOutPin)));
}

void IoOut::setOn()
{
	if(ioOutLevel)
		GPIO_SetBits(ioOutPort, ioOutPin);
	else
		GPIO_ResetBits(ioOutPort, ioOutPin);
}	

void IoOut::setOff()
{
	if(ioOutLevel)
		GPIO_ResetBits(ioOutPort, ioOutPin);
	else
		GPIO_SetBits(ioOutPort, ioOutPin);
}	

