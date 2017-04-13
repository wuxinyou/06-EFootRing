/**
  ******************************************************************************
	*文件：TerminalControl.h
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：Xbot和终端交互类的头文件
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



class TerminalControl
{
public:
	TerminalControl(SerialPort *);
	virtual ~TerminalControl();
	void updateSendBuf(uint8_t * temBuf);
	void sendcommand();
	bool reciveData();                //接收数据，将串口缓冲区的内容读取到reciveBuf
	uint8_t* getReciveData(){return reciveBuf;}  //返回接收的数据

private:
	SerialPort *terminalPort;		//无线串口号	
	RingChain *reciveChin;
	uint8_t *sendBuf; 
	uint8_t *reciveBuf;
private:
	

};

#endif /* _TERMINAL_CONTROL_H */


