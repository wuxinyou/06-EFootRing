/**
  ******************************************************************************
	*文件：Key.cpp
	*作者：孟浩
	*版本：1.0
	*日期：2015-03-09
	*概要：Key类的头文件
	*备注：
	*
  ******************************************************************************  
	*/ 
#ifndef _IO_OUT_H
	#define _IO_OUT_H

	#include <stdlib.h>			 
	#include <stm32f10x_gpio.h>
	#include  "STM32_GpioInit.h"

	class IoOut
	{
		public:
			IoOut(GPIO_TypeDef *port=GPIOA,unsigned int pin=GPIO_Pin_0, bool bLevel=false);
			~IoOut();

			void setOn();		
			void setOff();	
			void reverseState();	//翻转Io的状态
			
		private:
			void initIo();
			GPIO_TypeDef *ioOutPort; 
			unsigned int ioOutPin;	
			bool ioOutLevel;						
	};

#endif



