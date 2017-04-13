/************************************************************************************
*  Copyright (c), 2013, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: LQ12864.h
* Project  : OLED
* Processor: STC89C52
* Compiler : Keil C51 Compiler
* 
* Author : 小林
* Version: 1.00
* Date   : 2013.8.8
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64点整OLED模块驱动文件，仅适用heltec.taobao.com所售产品
*
* Others: none;
*
* Function List:
*
* 1. void LCD_DLY_ms(unsigned int ms) -- OLED驱动程序用的延时程序,建议主函数中不要调用此程序
* 2. void LCD_WrDat(unsigned char dat) -- 向OLED屏写数据
* 3. void LCD_WrCmd(unsigned char cmd) -- 向OLED屏写命令
* 4. void LCD_Set_Pos(unsigned char x, unsigned char y) -- 设置显示坐标
* 5. void LCD_Fill(unsigned char bmp_dat) -- 全屏显示(显示BMP图片时才会用到此功能)
* 6. void LCD_CLS(void) -- 复位/清屏
* 7. void LCD_Init(void) -- OLED屏初始化程序，此函数应在操作屏幕之前最先调用
* 8. void LCD_P6x8Str(unsigned char x, y,unsigned char ch[]) -- 6x8点整，用于显示ASCII码的最小阵列，不太清晰
* 9. void LCD_P8x16Str(unsigned char x, y,unsigned char ch[]) -- 8x16点整，用于显示ASCII码，非常清晰
* 10.void LCD_P16x16Ch(unsigned char x, y, N) -- 16x16点整，用于显示汉字的最小阵列，可设置各种字体、加粗、倾斜、下划线等
* 11.void Draw_BMP(unsigned char x0, y0,x1, y1,unsigned char BMP[]) -- 将128x64像素的BMP位图在取字软件中算出字表，然后复制到codetab中，此函数调用即可
*
* History: none;
*
*************************************************************************************/

#include "REG51.h"

/********************
*
* 用IO口模拟SPI
* P1.0口接模块上的D/C
* P1.1口接模块上的RST
* P1.2口接模块上的SDA
* P1.3口接模块上的SCL
*
*********************/

sbit LCD_SCL=P1^3; //时钟 D0（SCLK）
sbit LCD_SDA=P1^2; //D1（MOSI） 数据
sbit LCD_RST=P1^1; //复位 
sbit LCD_DC =P1^0; //数据/命令控制

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	132
#define Max_Row		64
#define	Brightness	0xCF 
#define X_WIDTH 	132
#define Y_WIDTH 	64
/*********************OLED驱动程序用的延时程序************************************/
void LCD_DLY_ms(unsigned int ms)
{                         
	unsigned int a;
	while(ms)
	{
		a=1800;
		while(a--);
		ms--;
	}
	return;
}
/*********************LCD写数据************************************/ 
void LCD_WrDat(unsigned char dat)	 
{
	unsigned char i;
	LCD_DC=1;  
	for(i=0;i<8;i++) //发送一个八位数据 
	{
		if((dat << i) & 0x80)
		{
			LCD_SDA  = 1;
		}
		else  LCD_SDA  = 0;
		LCD_SCL = 0;
		LCD_SCL = 1;
	}
}
/*********************LCD写命令************************************/										
void LCD_WrCmd(unsigned char cmd)
{
	unsigned char i;
	LCD_DC=0;
	for(i=0;i<8;i++) //发送一个八位数据 
	{
		if((cmd << i) & 0x80)
		{
			LCD_SDA  = 1;
		}
		else 
		{
			LCD_SDA  = 0;
		}
		LCD_SCL = 0;
		LCD_SCL = 1;
	}
}
/*********************LCD 设置坐标************************************/
void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
	LCD_WrCmd(0xb0+y);
	LCD_WrCmd(((x&0xf0)>>4)|0x10);//LCD_WrCmd(((x&0xf0)>>4)|0x10);
	LCD_WrCmd((x&0x0f)|0x01);//LCD_WrCmd((x&0x0f)|0x01);
} 
/*********************LCD全屏************************************/
void LCD_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		LCD_WrDat(bmp_dat);
	}
}
/*********************LCD复位************************************/
void LCD_CLS(void)
{
	unsigned char y,x;	
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10); 
		for(x=0;x<X_WIDTH;x++)
		LCD_WrDat(0);
	}
}
/*********************LCD初始化************************************/
void LCD_Init(void)     
{  
	LCD_SCL=1;
	LCD_RST=0;
	LCD_DLY_ms(50);
	LCD_RST=1;       //从上电到下面开始初始化要有足够的时间，即等待RC复位完毕   
	LCD_WrCmd(0xae);//--turn off oled panel
	LCD_WrCmd(0x00);//---set low column address
	LCD_WrCmd(0x10);//---set high column address
	LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	LCD_WrCmd(0x81);//--set contrast control register
	LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
	LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	LCD_WrCmd(0xa6);//--set normal display
	LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	LCD_WrCmd(0x3f);//--1/64 duty
	LCD_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	LCD_WrCmd(0x00);//-not offset
	LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	LCD_WrCmd(0xd9);//--set pre-charge period
	LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	LCD_WrCmd(0xda);//--set com pins hardware configuration
	LCD_WrCmd(0x12);
	LCD_WrCmd(0xdb);//--set vcomh
	LCD_WrCmd(0x40);//Set VCOM Deselect Level
	LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	LCD_WrCmd(0x02);//
	LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
	LCD_WrCmd(0x14);//--set(0x10) disable
	LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	LCD_WrCmd(0xaf);//--turn on oled panel
	LCD_Fill(0x00);  //初始清屏
	LCD_Set_Pos(0,0); 	
} 
/***************功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
void LCD_P6x8Str(unsigned char x, y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		LCD_Set_Pos(x,y);
		for(i=0;i<6;i++)
		LCD_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}
/*******************功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7****************/
void LCD_P8x16Str(unsigned char x, y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		LCD_Set_Pos(x,y);
		for(i=0;i<8;i++)
		LCD_WrDat(F8X16[c*16+i]);
		LCD_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		LCD_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}
/*****************功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7****************************/
void LCD_P16x16Ch(unsigned char x, y, N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	LCD_Set_Pos(x+2, y);
	for(wm = 0;wm < 16;wm++)
	{
		LCD_WrDat(F16x16[adder]);
		adder += 1;
	}
	LCD_Set_Pos(x+2,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		LCD_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void Draw_BMP(unsigned char x0, y0,x1, y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		LCD_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	LCD_WrDat(BMP[j++]);	    	
	    }
	}
} 
