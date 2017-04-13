/**
  ******************************************************************************
	*文件：TerminalControl.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：控制箱和液晶屏交互的协议类实现
  ******************************************************************************  
	*/ 


/* 头文件包含 ------------------------------------------------------------------*/
#include "TerminalControl.h"

/* 类型定义 --------------------------------------------------------------------*/

/* 宏定义-----------------------------------------------------------------------*/ 
//数据帧长度宏
#define SEND_BUF_LEN 13
#define RECIVE_BUF_LEN 13

//帧头定义
#define FRAME_HEADER_SEND 0XA5
#define FRAME_COMMAND_SEND 0X00

#define FRAME_HEADER_RECIVE 0XA5
// #define FRAME_COMMAND_RECIVE 0X00
//构造函数
TerminalControl::TerminalControl(SerialPort * port)
{
	terminalPort=port;
	reciveChin=new RingChain(RECIVE_BUF_LEN);
	sendBuf=new uint8_t[SEND_BUF_LEN];
	reciveBuf=new uint8_t[RECIVE_BUF_LEN];
	sendBuf[0]=FRAME_HEADER_SEND;
	sendBuf[1]=FRAME_COMMAND_SEND;
	for(int i=2;i<SEND_BUF_LEN;i++)
	{
		sendBuf[i]=0;
	}
	for(int i=0;i<RECIVE_BUF_LEN;i++)
	{
		reciveBuf[i]=0;
	}
}
//析构函数
TerminalControl::~TerminalControl()
{
	delete reciveChin;
	delete [] sendBuf;
	delete [] reciveBuf;
}
void TerminalControl::sendcommand()
{
		terminalPort->write(sendBuf,SEND_BUF_LEN);
}

/**
  * 功  能：接收控制终端的数据
  * 参  数：接收数据的串口
  * 返回值：
			true:成功接收一帧
			false:没有接收到一帧数据
  */
bool TerminalControl::reciveData()
{
	uint8_t checkSum=0;
	bool readFlag=false;
  //接收串行数据
	while (terminalPort->readAllow()&&(readFlag==false))
	{
		reciveChin->setData(terminalPort->getChar());
		reciveChin->next();
		if(reciveChin->getData()==FRAME_HEADER_RECIVE)//检测帧头	
		{	
			checkSum+=FRAME_HEADER_RECIVE;
			reciveChin->next();
			if(reciveChin->getData()==0x00||reciveChin->getData()==0x01||reciveChin->getData()==0x02||reciveChin->getData()==0x03)//方向位正确
			{
				for(int i=0;i<RECIVE_BUF_LEN-2;i++)
				{
				   checkSum+=reciveChin->getData();
				   reciveChin->next();
				}
				if(checkSum==reciveChin->getData())//校验和正确
				{
					//将数据存入数组
					for(int i=0;i<RECIVE_BUF_LEN;i++)
					{
					   reciveChin->next();
					   reciveBuf[i]=reciveChin->getData();
					}
					readFlag=true;
				}
				reciveChin->next();
			}
			else
			{
				 reciveChin->previous();//跳至上个节点	
			}			
		 	checkSum=0;
		}	
	}
	return 	readFlag;
}

/**
  * 功  能：更新发送的数组
  * 参  数：更新的数据
  * 返回值：无
  */
void TerminalControl::updateSendBuf(uint8_t data1,uint8_t data2)
{
	sendBuf[2]=data1;
	sendBuf[3]=data2;
	sendBuf[12]=0;
	//计算校验和
	for(int i=0;i<12;i++)
	{
		sendBuf[12]+=sendBuf[i];
	}
}


