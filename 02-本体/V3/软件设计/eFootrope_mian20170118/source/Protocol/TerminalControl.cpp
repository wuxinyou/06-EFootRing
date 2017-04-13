/**
  ******************************************************************************
	*文件：TerminalControl.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*修改：吴新有  20161226
	*概要：用电子脚扣的zigbee通讯
	*备注：
  ******************************************************************************  
	*/ 


/* 头文件包含 ------------------------------------------------------------------*/
#include "TerminalControl.h"

/* 类型定义 --------------------------------------------------------------------*/

/* 宏定义-----------------------------------------------------------------------*/ 
//数据帧长度宏
#define SEND_BUF_LEN 						12
#define SEND_DATA_LEN           6
#define RECIVE_BUF_LEN 					12


//帧头定义
#define SEND_FRAME_HEADER 			0XA5
#define RECEIVE_FRAME_HEADER    0XA4
//帧头定义
#define ROBOT_ADDR 						  0X01



//构造函数
TerminalControl::TerminalControl(SerialPort * p1)
{
	terminalPort=p1; 
	
	reciveChin=new RingChain(RECIVE_BUF_LEN);
	sendBuf=new uint8_t[SEND_BUF_LEN];
	reciveBuf=new uint8_t[RECIVE_BUF_LEN];
	
	for(int i=0;i<SEND_BUF_LEN;i++)
	{
		sendBuf[i]=0;
	}
	for(int i=0;i<RECIVE_BUF_LEN;i++)
	{
		reciveBuf[i]=0;
	}	
	
	sendBuf[0]=SEND_FRAME_HEADER;
	sendBuf[1]=ROBOT_ADDR;
	sendBuf[2]=0x80;
	
}


//析构函数
TerminalControl::~TerminalControl()
{
	delete reciveChin;
	delete [] sendBuf;
	delete [] reciveBuf;
}

/**
  * 功  能：发送数据到控制终端
  * 参  数：无
  * 返回值：无
	* 备  注: 
  */
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
		if(reciveChin->getData()==RECEIVE_FRAME_HEADER)//检测帧头	
		{	
			checkSum+=RECEIVE_FRAME_HEADER;
			reciveChin->next();
			if((reciveChin->getData())==ROBOT_ADDR)       // 
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
void TerminalControl::updateSendBuf(uint8_t * temBuf)
{
	uint8_t checksum=0;
	
	for(int i=0;i<SEND_DATA_LEN;i++)
	{
		sendBuf[i+3]=temBuf[i];
	}
	
	//计算校验和
	for(int i=0;i<SEND_BUF_LEN-1;i++)
	{
		checksum+=sendBuf[i];
	}
	sendBuf[SEND_BUF_LEN-1] = checksum;
}









