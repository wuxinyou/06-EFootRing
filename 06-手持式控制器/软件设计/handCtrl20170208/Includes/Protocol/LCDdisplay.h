/**
  ******************************************************************************
	*文件：LCDdisplay.h
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：和小液晶屏交互的协议的头文件
	*备注：此版本针对国外用户，语言不保存，
  ******************************************************************************  
	*/ 

/* 定义防止递归包含的控制宏----------------------------------------------------*/
	#ifndef _LCDDISPLAY_H_
	#define _LCDDISPLAY_H_
	/* 头文件包含 ------------------------------------------------------------------*/
#include "SerialPort.h"

/* 类型定义 --------------------------------------------------------------------*/

//LCD通信参数的结构体
typedef struct Parameter
{
  uint8_t head;				//帧头
  uint8_t command;		//命令字
  uint8_t buffer[64];	//数据
  uint8_t end[4];    	//帧尾
  uint8_t len;	   		//数据长度
}LCDParameter;
/* 宏定义-----------------------------------------------------------------------*/ 

class LCDdisplay
{
public:
	LCDdisplay(SerialPort *);
	~LCDdisplay();


public:

	void ShowPic(uint8_t picnum);														//显示图片



	void SetColor(uint16_t fcolor,uint16_t bcolor);					//设置调色板
	void ShowBar(uint16_t Xs,uint16_t Ys,uint8_t step);			//进度条显示
	void DisplayTxt_FixedMode(uint8_t,uint16_t x,uint16_t y,uint8_t *pdata,uint8_t Len);//标准字库显示
	void DisRecentage(uint8_t cmd,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye);														//矩形区域显示
	void CutPicAndShow(uint8_t Pic_ID ,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y);//剪切图标显示
	void movePic(uint8_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);																	//指定区域移动
	void setLanguageBuf(uint8_t);																																				//设置语言 0x00： 英文， 其他：中文
	void readLanguageBuf();																		//返回0 英文 其他 中文
	void backlightOn();																				//打开背光
	void backlightOff();																			//关闭背光
	void ShakeHand(void);																			//握手
	uint8_t riceveData();																			//接收数据

	void SetSpace(uint8_t clum,uint8_t row); 								//设置字间距
	void GetColor(uint8_t,uint16_t x,uint16_t y);						//取指定位置颜色到调色板
	void PWMLight(uint8_t pwm);															//设置背光亮度
	void ClearScreen(void);																	//以背景色清屏		
	void DisplayTxt(uint16_t x,uint16_t y,uint8_t *pdata,uint8_t Len);					//选择字库显示	
	void ConnectTwoPoint(uint8_t cmd,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye);												//画直线
	void ShowFrequency(uint16_t X,uint16_t Y,uint8_t Hmax,uint8_t* Hi,uint8_t Len);		  											//频谱显示 波形图
	void DisRound(uint8_t cmd,uint16_t type,uint16_t Xs,uint16_t Ys,uint16_t r);															//圆弧显示
	void ShowCurve(uint16_t X,uint16_t Ys,uint16_t Ye,uint16_t Y,uint16_t Fcolor,uint16_t BColor);						//小动态曲线显示
	void CutPicAndShow_1(uint8_t Pic_ID ,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y);//剪切图标显示
	void DisplayTxt_FixedMode(uint8_t size,uint16_t x,uint16_t y,const char *pdata);
private:
	LCDParameter *lcdParameter;		//LCD命令结构体
	SerialPort *lcddisPort;				//显示LCD的通信串口
	void sendCommand();          //发送数据至LCD，与普通串口有区别
};
	
	
	#endif


