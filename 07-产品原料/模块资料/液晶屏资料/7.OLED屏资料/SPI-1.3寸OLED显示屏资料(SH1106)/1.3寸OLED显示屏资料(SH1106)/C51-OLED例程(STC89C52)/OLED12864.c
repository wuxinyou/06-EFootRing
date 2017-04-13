/************************************************************************************
*  Copyright (c), 2013, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: OLED12864.c
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
* Description: 128*64点整OLED模块功能演示程序的主函数，仅适用heltec.taobao.com所售产品
*
* Others: none;
*
* Function List:
*
* 1. void delay(unsigned int z) -- 主函数中用于调整显示效果的延时函数,STC89C52 12MHZ z=1时大约延时1ms,其他频率需要自己计算
* 2. void main(void) -- 主函数
*
* History: none;
*
*************************************************************************************/

#include "REG51.h"
#include "intrins.h"
#include "codetab.h"
#include "LQ12864.h"

void delay(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}

void main(void) 
{
	unsigned char i;
	   
	LCD_Init(); //OLED初始化  
	while(1)
	{
//		LCD_Fill(0xff); //屏全亮
//		delay(2000);
//		LCD_Fill(0x00); //屏全灭
//		delay(2000);
		for(i=0; i<8; i++)//通过点整显示汉字 -- i表示字表数组的位置
		{
			LCD_P16x16Ch(i*16,0,i);
		 	LCD_P16x16Ch(i*16,2,i+8);
		 	LCD_P16x16Ch(i*16,4,i+16);
		 	LCD_P16x16Ch(i*16,6,i+24);
		} 
		delay(4000);
		LCD_CLS();//清屏

		LCD_P8x16Str(44,0,"HelTec");//第一行 -- 8x16的显示单元显示ASCII码
		LCD_P8x16Str(20,2,"OLEDDISPLAY");
		LCD_P8x16Str(8,4,"www.heltec.cn");
		LCD_P6x8Str(16,6,"HelTec@gmail.com");
		LCD_P6x8Str(34,7,"2013-07-26");    
		delay(4000);
		LCD_CLS();

		Draw_BMP(0,0,132,8,BMP1);  //图片显示(图片显示慎用，生成的字表较大，会占用较多空间，FLASH空间8K以下慎用)
		delay(8000);
		Draw_BMP(0,0,132,8,BMP2);
		delay(8000);
	}
}
