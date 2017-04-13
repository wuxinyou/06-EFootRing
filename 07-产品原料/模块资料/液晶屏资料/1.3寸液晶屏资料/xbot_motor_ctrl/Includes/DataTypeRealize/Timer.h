/**
  ******************************************************************************
	*文件：Timer.h
	*版本：2.0
	*日期：2013-06-17
	*概要：定时器类的头文件
	*备注：
  ******************************************************************************  
	*/ 
#ifndef _TIMER_H
#define _TIMER_H

#include <stdlib.h>			 
#include "stm32f10x.h"
#include "STM32F10x_PriorityControl.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Timer
{
public:
	Timer(TIM_TypeDef *tim=TIM2);
	~Timer();
	void setTimeOut(int);					//定时时间长度 单位MS
	void start();
	void stop();
	static void start(TIM_TypeDef*);
	static void stop(TIM_TypeDef*);
	static ITStatus getITStatus(TIM_TypeDef*);
	static void clearITPendingBit(TIM_TypeDef*);
	void setPriority(uint8_t,uint8_t);						//配置优先级

private:
	int Tim_IRQn;								//中断号
	TIM_TypeDef* TimX;       		//定时器寄存器
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif



