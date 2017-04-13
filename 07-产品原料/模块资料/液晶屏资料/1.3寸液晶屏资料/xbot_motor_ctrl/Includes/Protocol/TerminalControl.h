/**
  ******************************************************************************
	*文件：TerminalControl.h
	*作者：叶依顺
	*版本：2.0
	*日期：2014-01-02
	*概要：控制箱和液晶屏交互类的头文件
	*备注：
	*
  ******************************************************************************  
  */ 
/* 定义防止递归包含的控制宏----------------------------------------------------*/
#ifndef _TERMINAL_CONTROL_H
#define _TERMINAL_CONTROL_H

/* 头文件包含 ------------------------------------------------------------------*/
#include "SerialPort.h"
#include "DataChain.h"

/* 类型定义 --------------------------------------------------------------------*/

/* 宏定义-----------------------------------------------------------------------*/ 

class TerminalControl
{
public:
	TerminalControl(SerialPort *port);	//构造函数
	virtual ~TerminalControl();					//析构函数
	void updateSendBuf(uint8_t,uint8_t);//更新发送数组
	void sendcommand();									//发送数据，将sendBuf的数据发送出去
	bool reciveData();					//接收数据，将串口缓冲区的数据放进reciveBuf
	uint8_t* getReciveData(){return reciveBuf;}
private:
	SerialPort *terminalPort;		//串口号
	RingChain *reciveChin;			//数据接收链表
	uint8_t *sendBuf; 					//发送数组
	uint8_t *reciveBuf;					//接收数组
};

#endif /* _TERMINAL_CONTROL_H */


