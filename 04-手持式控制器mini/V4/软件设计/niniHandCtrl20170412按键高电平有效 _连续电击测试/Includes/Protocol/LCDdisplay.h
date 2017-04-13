/**
  ******************************************************************************
	*文件：LCDdisplay.h
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：和小液晶屏交互的协议的头文件
	*备注：
  ******************************************************************************  
	*/ 

/* 定义防止递归包含的控制宏----------------------------------------------------*/
	#ifndef _LCDDISPLAY_H_
	#define _LCDDISPLAY_H_
	/* 头文件包含 ------------------------------------------------------------------*/
#include "SpiPort.h"
#include "STM32_SysTick.h"
#include "string.h"
/* 宏定义------------------------------------------------------------------*/
/*********************************************************
端口定义
*********************************************************/
/* 类型定义 --------------------------------------------------------------------*/




class LCDdisplay
{
public:
	LCDdisplay(SpiPort *);
	~LCDdisplay();

public:
	void clearScreen(uint8_t);				//清屏
	void enabledDisplay();						//使能显示
	void disabledDisplay();						//禁能显示
  void setContrast(uint8_t);				//设置屏幕的对比度
	bool setStartLine(int);						//设置起始行地址
	bool setPageAddress(uint8_t);			//设置页地址
	bool setColumnAddress(uint8_t);		//设置列地址
	bool setXY(uint8_t,uint8_t);			//设置起始坐标
	void writeMediumChar(uint8_t,bool color=true,bool fill=true);			//显示中号字符6*8
	void writeMediumCharLine(uint8_t,uint8_t,const char*,uint8_t,bool color=true,bool fill=true,uint8_t start=0);			//显示中号字符串

	void writeMediumChar_1(uint8_t,uint8_t,uint8_t,bool color=true,bool fill=true,bool underline=false);			//居中显示中号字符6*16
	void writeMediumCharLine_1(uint8_t,uint8_t,const char*,uint8_t,bool color=true,bool fill=true,bool underline=false);			//居中显示中号字符串

	void writeLargeChar(uint8_t,uint8_t,uint8_t,bool color=true,bool fill=true,bool underline=false);			//显示大号字符8*16
	void writeLargeChar_1(uint8_t,uint8_t,uint8_t,bool color=true);			//显示大号字符10*16 右下角带点
	void writeLargeCharLine(uint8_t,uint8_t,const char*,uint8_t len=1,bool color=true,bool fill=true,bool underline=false);			//显示大号字符串
	void writeHanzi(uint8_t,uint8_t,const char*,bool color=true,uint8_t fill=0,uint8_t underline=0);					//显示汉字12*16
	void writeHanziLine(uint8_t,uint8_t,const char*,uint8_t len=1,bool color=true,uint8_t fill=0,uint8_t underline=false,uint8_t start=0);					//显示汉字字符串
	void displayUpArrows(uint8_t xPos,uint8_t yPos,bool color,bool rim);
	void displayDownArrows(uint8_t xPos,uint8_t yPos,bool color,bool rim);
	void displayHoldOff(uint8_t xPos,uint8_t yPos);
	void displayVertical(uint8_t xPos,uint8_t yPos);
	void displayLevel(uint8_t xPos,uint8_t yPos);
	void displayHoldOn(uint8_t xPos,uint8_t yPos);
	void displayProgressBar(uint8_t xPos,uint8_t yPos,int8_t value);
	void disLargeNum(uint8_t xPos,uint8_t yPos,int8_t data,bool color=true,bool fill=true,bool underline=false);
	uint8_t disMediumNum(uint8_t xPos,uint8_t yPos,int16_t data,bool color=true,bool fill=true);
	void sendCommand(uint8_t);				//发送命令
	void sendData(uint8_t);						//发送数据
public:
// 	void sendCommand(uint8_t);				//发送命令
// 	void sendData(uint8_t);						//发送数据
	void LcdInit();



// 		void LcdCmd(u8 cmd);
// 		void LcdDat(u8 cmd); 
// 		void LcdDisplay(uint8_t on);
// 		void LcdSetPageAddress(uint8_t page);
// 		void LcdSetColumnAddress(uint8_t column);
// 		void LcdSetXy(u8 x,u8 y);
// 		void LcdSetStartLine(int line);
// 		void LcdContrast(u8 contrast);
// 		void LcdDelay(void);
// 	void Lcdclear();
// void LcdDrawStart(u16 x0, u16 y0, u16 x1, u16 y1, enum DrawDir _dir);
// void LcdDrawStop(void);
// 	void LcdDrawPixel(u16 color);
// void LcdDrawPixelXY(u16 x, u16 y, u16 color);
// 	void LcdDrawRLE(const u8 *data, int len, u32 color);
private:
		SpiPort *lcddisPort;				//显示LCD的通信SPI口
	
};

#endif


