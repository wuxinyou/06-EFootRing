/**
  ******************************************************************************
	*文件：LEDLighter.h
	*版本：2.0
	*日期：2013-06-17
	*概要：LED灯类的头文件
	*备注：
	*
  ******************************************************************************  
	*/ 
#ifndef _LEDLIGHTER_H
#define _LEDLIGHTER_H

	#include <stdlib.h>			 
	#include <stm32f10x_gpio.h>
	#include  "STM32_GpioInit.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LEDLighter
{
public:
	LEDLighter(GPIO_TypeDef *port,unsigned int Pin,bool bLevel=false);
	~LEDLighter();
// 	void setPort(GPIO_TypeDef *port);
// 	void setPin(unsigned int Pin);
// 	void initLighter();
	void setLightOn(){setLightStatus(true);}		//开灯
	void setLightOff(){setLightStatus(false);}	//关灯
	void reverseLight();	//翻转灯的状态
	void setLightStatus(bool light);
private:
	void initLighter();
	GPIO_TypeDef *lighterPort;
	unsigned int lighterPin;
	bool ledLevel;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif



