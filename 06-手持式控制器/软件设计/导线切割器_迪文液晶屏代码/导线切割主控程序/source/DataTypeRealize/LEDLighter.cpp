/**
  ******************************************************************************
	*文件：LEDLighter.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：LED灯类的实现
	*备注：
	*
  ******************************************************************************  
	*/ 
	
#include "LEDLighter.h"
LEDLighter::LEDLighter(GPIO_TypeDef *port,unsigned int Pin,bool bLevel)
{
	lighterPort=port;
	lighterPin=Pin;
	ledLevel=bLevel;
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
	initLighter();
}

LEDLighter::~LEDLighter()
{
// 	delete lighterPort;
}

/**
  * 功  能：初始化灯的端口
  * 参  数：无
  * 返回值：无
  */
void LEDLighter::initLighter()
{
	STM32_GpioOneInit(lighterPin,GPIO_Speed_2MHz,GPIO_Mode_Out_PP,lighterPort);	//推挽输出
}

/**
  * 功  能：翻转等的亮灭状态
  * 参  数：无
  * 返回值：无
  */
void LEDLighter::reverseLight()	//翻转灯的状态
{
	 GPIO_WriteBit(lighterPort, lighterPin, (BitAction)(1 - GPIO_ReadOutputDataBit(lighterPort, lighterPin)));
}

/**
  * 功  能：设置灯的亮灭
  * 参  数：
	*			true:点亮
	*			false:熄灭
  * 返回值：无
  */
void LEDLighter::setLightStatus(bool light)
{
	if(!ledLevel)
	{
		if(light)
			GPIO_ResetBits(lighterPort, lighterPin);
		else
			GPIO_SetBits(lighterPort, lighterPin);
	}
	else
	{
		if(!light)
			GPIO_ResetBits(lighterPort, lighterPin);
		else
			GPIO_SetBits(lighterPort, lighterPin);
	}
		
}




