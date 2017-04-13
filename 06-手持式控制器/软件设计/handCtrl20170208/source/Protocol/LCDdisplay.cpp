/**
  ******************************************************************************
	*文件：LCDdisplay.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：和小液晶屏交互的协议，实现小液晶屏的显示
	*备注：
  ******************************************************************************  
	*/ 
	/* 头文件包含 ------------------------------------------------------------------*/
	
#include "LCDdisplay.h"




/**
  * 功  能：LCDdisplay类的构造函数
  * 参  数：
	* port：通信的串口号
  * 返回值：无
  */
LCDdisplay::LCDdisplay(SerialPort *port)
{
	lcddisPort=port;
	//初始化发送参数的结构体
	lcdParameter=new LCDParameter();
	lcdParameter->head=0xaa;
	lcdParameter->command=0x00;
	lcdParameter->end[0]=0xcc;
	lcdParameter->end[1]=0x33;
	lcdParameter->end[2]=0xc3;
	lcdParameter->end[3]=0x3c;
	lcdParameter->len=0x00;

	for(int i=0;i<64;i++)
	{
		lcdParameter->buffer[i]=0x00;
	}
	//Speed=0;
}

/**
  * 功  能：LCDdisplay类的析构函数
  * 参  数：无
  * 返回值：无
  */
LCDdisplay::~LCDdisplay()
{
	lcddisPort=NULL;
	delete lcdParameter;

}


/*************************************************************************************************
**以下是LCD各种功能的驱动程序
**
**************************************************************************************************/

void LCDdisplay::sendCommand()
{
	while(!(lcddisPort->putChar(lcdParameter->head)));				//发送头
	while(!(lcddisPort->putChar(lcdParameter->command)));			//发送命令字
	lcddisPort->write(lcdParameter->buffer,lcdParameter->len);//发送命令内容
	lcddisPort->write(lcdParameter->end,4);										//发送帧尾
}

void LCDdisplay::SetSpace(uint8_t clum,uint8_t row) 								//设置字间距
{
	lcdParameter->command=0x41;
  lcdParameter->buffer[0]=clum;
  lcdParameter->buffer[1]=row;
  lcdParameter->len=2;
	sendCommand();
}
void LCDdisplay::SetColor(uint16_t fcolor,uint16_t bcolor)					//设置调色板
{
  lcdParameter->command = 0x40;
  lcdParameter->buffer[0]=fcolor>>8;
  lcdParameter->buffer[1]=fcolor;
  lcdParameter->buffer[2]=bcolor>>8;
  lcdParameter->buffer[3]=bcolor;
  lcdParameter->len=4;
  sendCommand();
}
void LCDdisplay::GetColor(uint8_t mode,uint16_t x,uint16_t y)										//取指定位置颜色到调色板
{
	if(mode==0)
		lcdParameter->command = 0x42;
	else
		lcdParameter->command = 0x43;
  lcdParameter->buffer[0]=x>>8;
  lcdParameter->buffer[1]=x;
  lcdParameter->buffer[2]=y>>8;
  lcdParameter->buffer[3]=y;
  lcdParameter->len=4;
  sendCommand();
}

void LCDdisplay::backlightOn()												//打开背光
{
  lcdParameter->command = 0x5f;
  lcdParameter->len=0;
  sendCommand();
}

void LCDdisplay::backlightOff()												//关闭背光
{
  lcdParameter->command = 0x5e;
  lcdParameter->len=0;
  sendCommand();
}

void LCDdisplay::PWMLight(uint8_t pwm)															//设置背光亮度
{
	lcdParameter->command = 0x5f;
  lcdParameter->buffer[0]=pwm;
  lcdParameter->len = 0x01;
  sendCommand();
}
void LCDdisplay::ClearScreen(void)																	//以背景色清屏		
{
  lcdParameter->command = 0x52;
  lcdParameter->len=0;
  sendCommand();
}
void LCDdisplay::DisplayTxt_FixedMode(uint8_t size,uint16_t x,uint16_t y,uint8_t *pdata,uint8_t Len)//标准字库显示
{
	uint8_t i=0;
	lcdParameter->command   = size;
	lcdParameter->buffer[i++] = x>>8;
  lcdParameter->buffer[i++] = x;
  
  lcdParameter->buffer[i++] = y>>8;
  lcdParameter->buffer[i++] = y;
  while(Len--)
  { 
		lcdParameter->buffer[i++]=*pdata;
		pdata++;
	}
	lcdParameter->len = i;
  sendCommand();
}

void LCDdisplay::DisplayTxt_FixedMode(uint8_t size,uint16_t x,uint16_t y,const char *pdata)//标准字库显示
{
	uint8_t i=0;
	lcdParameter->command   = size;
	lcdParameter->buffer[i++] = x>>8;
  lcdParameter->buffer[i++] = x;
  
  lcdParameter->buffer[i++] = y>>8;
  lcdParameter->buffer[i++] = y;
  while(*pdata)
  { 
		lcdParameter->buffer[i++]=*pdata;
		pdata++;
	}
	lcdParameter->len = i;
  sendCommand();
}

const uint8_t locktxt[7]={0x20,0x81,0x01,0xFF,0xE0,0x00,0x00}; 
void LCDdisplay::DisplayTxt(uint16_t x,uint16_t y,uint8_t *pdata,uint8_t Len)//选择字库显示	
{
	uint8_t i=0;
	lcdParameter->command = 0x98;		//命令
  lcdParameter->buffer[0] = x>>8;
  lcdParameter->buffer[1] = x;
  
  lcdParameter->buffer[2] = y>>8;
  lcdParameter->buffer[3] = y;
  
  for(i=4;i<sizeof(locktxt)+4;i++)
  { 
    lcdParameter->buffer[i]=locktxt[i-4];
  }
   
  while(Len--)
  { 
    lcdParameter->buffer[i++] = *pdata;
    pdata++;
  }
 
  lcdParameter->len = i;
  sendCommand();
}

/**********************************************************
(Left,Top,Width,Height)
***********************************************************/
void LCDdisplay::CutPicAndShow(uint8_t Pic_ID ,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y)//剪切图标显示
{
  lcdParameter->command = 0x71;
  lcdParameter->buffer[0] = Pic_ID;
  
  lcdParameter->buffer[1] = Xs>>8;
  lcdParameter->buffer[2] = Xs;
  
  lcdParameter->buffer[3] = Ys>>8;
  lcdParameter->buffer[4] = Ys;
  
  lcdParameter->buffer[5] = (Xe+Xs)>>8;
  lcdParameter->buffer[6] = (Xe+Xs);
  
  lcdParameter->buffer[7] = (Ye+Ys)>>8;
  lcdParameter->buffer[8] = (Ye+Ys);
  //再次显示的位置
  lcdParameter->buffer[9] = X>>8;
  lcdParameter->buffer[10] = X;
  
  lcdParameter->buffer[11] = Y>>8;
  lcdParameter->buffer[12] = Y;
  
  lcdParameter->len = 0x0D;
  sendCommand(); 
}

/**********************************************************
(Left,Top,Right,Buttom)
***********************************************************/
void LCDdisplay::CutPicAndShow_1(uint8_t Pic_ID ,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y)//剪切图标显示
{
  lcdParameter->command = 0x71;
  lcdParameter->buffer[0] = Pic_ID;
  
  lcdParameter->buffer[1] = Xs>>8;
  lcdParameter->buffer[2] = Xs;
  
  lcdParameter->buffer[3] = Ys>>8;
  lcdParameter->buffer[4] = Ys;
  
  lcdParameter->buffer[5] = Xe>>8;
  lcdParameter->buffer[6] = Xe;
  
  lcdParameter->buffer[7] = Ye>>8;
  lcdParameter->buffer[8] = Ye;
  //再次显示的位置
  lcdParameter->buffer[9] = X>>8;
  lcdParameter->buffer[10] = X;
  
  lcdParameter->buffer[11] = Y>>8;
  lcdParameter->buffer[12] = Y;
  
  lcdParameter->len = 0x0D;
  sendCommand(); 
}
void LCDdisplay::ConnectTwoPoint(uint8_t cmd,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye)//画直线
{
  lcdParameter->command = cmd;
  lcdParameter->buffer[0]=Xs>>8;
  lcdParameter->buffer[1]=Xs;
  lcdParameter->buffer[2]=Ys>>8;
  lcdParameter->buffer[3]=Ys;
   
  lcdParameter->buffer[4]=Xe>>8;
  lcdParameter->buffer[5]=Xe;
  lcdParameter->buffer[6]=Ye>>8;
  lcdParameter->buffer[7]=Ye;
  
  lcdParameter->len = 0x08;
  sendCommand();
}
void LCDdisplay::ShowFrequency(uint16_t X,uint16_t Y,uint8_t Hmax,uint8_t *Hi,uint8_t Len)//频谱显示 波形图
{
  lcdParameter->command = 0x75;
  lcdParameter->buffer[0]=X>>8;
  lcdParameter->buffer[1]=X;
  lcdParameter->buffer[2]=Y>>8;
  lcdParameter->buffer[3]=Y;
  lcdParameter->buffer[4]=Hmax;
	for(int i=0;i<Len;i++)
		lcdParameter->buffer[5+i]=Hi[i];
  lcdParameter->len = 0x05+Len;
  sendCommand();
}
void LCDdisplay::DisRecentage(uint8_t cmd,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye)//矩形区域显示
{
	lcdParameter->command = cmd;
	lcdParameter->buffer[0]=Xs>>8;
	lcdParameter->buffer[1]=Xs;
	lcdParameter->buffer[2]=Ys>>8;
	lcdParameter->buffer[3]=Ys;
	lcdParameter->buffer[4]=Xe>>8;
	lcdParameter->buffer[5]=Xe;
	lcdParameter->buffer[6]=Ye>>8;
	lcdParameter->buffer[7]=Ye;
	lcdParameter->len = 0x08;
	sendCommand();
}

void LCDdisplay::DisRound(uint8_t cmd,uint16_t type,uint16_t Xs,uint16_t Ys,uint16_t r)	//圆弧显示
{
  lcdParameter->command = cmd;
  lcdParameter->buffer[0]=type;
  lcdParameter->buffer[1]=Xs>>8;
  lcdParameter->buffer[2]=Xs;
  lcdParameter->buffer[3]=Ys>>8;
  lcdParameter->buffer[4]=Ys;
  lcdParameter->buffer[5]=r;
  lcdParameter->len = 0x06;
  sendCommand();
}

void LCDdisplay::movePic(uint8_t cmd,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t step)																	//指定区域移动
{
  lcdParameter->command = cmd;
	lcdParameter->buffer[0]=Xs>>8;
	lcdParameter->buffer[1]=Xs;
	lcdParameter->buffer[2]=Ys>>8;
	lcdParameter->buffer[3]=Ys;
	lcdParameter->buffer[4]=Xe>>8;
	lcdParameter->buffer[5]=Xe;
	lcdParameter->buffer[6]=Ye>>8;
	lcdParameter->buffer[7]=Ye;
	lcdParameter->buffer[8]=step;
  lcdParameter->len = 0x09;
  sendCommand();
}

#define BAR_WIDTH 14
#define BAR_LONG 121
void LCDdisplay::ShowBar(uint16_t Xs,uint16_t Ys,uint8_t step)//进度条显示
{
//   static unsigned char i=0;
//   if(i==0)
//   {
//     DisRecentage(0x59,Xs,Ys,Xs+BAR_LONG+2,Ys+BAR_WIDTH);
//     i=1;
//   }
  DisRecentage(0x5B,Xs+1,Ys+1,Xs+1+step,Ys+BAR_WIDTH-1);
  DisRecentage(0x5A,Xs+1+step,Ys+1,Xs+BAR_LONG,Ys+BAR_WIDTH-1);
	if(step==BAR_LONG-1)
		DisRecentage(0x5B,Xs+1+step,Ys+1,Xs+1+step,Ys+BAR_WIDTH-1);
		
}

//256色小液晶屏没有小动态曲线显示的功能
void LCDdisplay::ShowCurve(uint16_t X,uint16_t Ys,uint16_t Ye,uint16_t Y,uint16_t Fcolor,uint16_t BColor)						//小动态曲线显示
{
  lcdParameter->command = 0x74;
  lcdParameter->buffer[0]=X>>8;
  lcdParameter->buffer[1]=X;
  lcdParameter->buffer[2]=Ys>>8;
  lcdParameter->buffer[3]=Ys;
  lcdParameter->buffer[4]=Ye>>8;
  lcdParameter->buffer[5]=Ye;
  lcdParameter->buffer[6]=BColor>>8;
  lcdParameter->buffer[7]=BColor;
  lcdParameter->buffer[8]=Y>>8;
  lcdParameter->buffer[9]=Y;
  lcdParameter->buffer[10]=Fcolor>>8;
  lcdParameter->buffer[11]=Fcolor;
  lcdParameter->len = 0x0C;
  sendCommand();
}
void LCDdisplay::ShowPic(uint8_t picnum)//显示图片
{
	lcdParameter->command = 0x70;
	lcdParameter->buffer[0]=picnum;
	lcdParameter->len=0x01;
	sendCommand();
}
void LCDdisplay::ShakeHand(void)//握手
{
  lcdParameter->command = 0x00;
  lcdParameter->len=0x00;
  sendCommand();
}


