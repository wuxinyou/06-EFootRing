/**
  ******************************************************************************
	*文件：SpiPort.h
	*作者：叶依顺
	*版本：2.0
	*日期：2014-01-02
	*概要：SpiPort类的头文件
	*
  ******************************************************************************  
	*/

#ifndef _SPIPORT_H
#define _SPIPORT_H
#include <stdlib.h>	
#include <stdint.h>	
#include "stm32f10x.h"
#include "STM32_GpioInit.h"
#include "STM32F10x_PriorityControl.h"

class SpiPort
{
	public:
		SpiPort(uint8_t);
		~SpiPort();
	public:
		void putData(uint8_t);
		void open();							  //打开spi
		void close();								//关闭spi
		void setPriority(uint8_t,uint8_t);
	private:
		void spiConfig();//spi初始化
		void spiGpioInit();															 //spi端口配置											
	private:
		int port;										//spi端口号
		int SPI_IRQn;								//中断号
		SPI_TypeDef *SPIX;
};

#endif

