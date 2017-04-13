#include "iostm8s103f3.h"
#define LCD_SCL  PC_ODR_ODR4//LCD时钟
#define LCD_SDA  PC_ODR_ODR3 //LCD数据 
#define LCD_RST  PC_ODR_ODR6 //LCD复位
#define LCD_DC   PC_ODR_ODR5  //LCD片选
#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	132
#define Max_Row		64
#define	Brightness	0xCF 
#define X_WIDTH 132
#define Y_WIDTH 64

/*********************LCD写数据************************************/ 
void LCD_WrDat(unsigned char dat)	 
{
unsigned char i;
LCD_DC=1;  
for(i=0;i<8;i++) //发送一个八位数据 
{
if((dat << i) & 0x80)
{LCD_SDA  = 1;}
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
{LCD_SDA  = 1;}
else  LCD_SDA  = 0;
LCD_SCL = 0;
LCD_SCL = 1;
}
}
/*********************LCD 设置坐标************************************/
void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
LCD_WrCmd(0xb0+y);
LCD_WrCmd(((x&0xf0)>>4)|0x10);
LCD_WrCmd((x&0x0f)|0x01); 
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
void OLED_Init(void)     
{  
LCD_SCL=1;
LCD_RST=0;
DelayMS(50);
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
/*****************功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7****************************/
void LCD_P16x16Ch(unsigned char x, unsigned char y, unsigned char N)
{
unsigned char wm=0;
unsigned int adder=32*N;  //  	
LCD_Set_Pos(x , y);
for(wm = 0;wm < 16;wm++)  //             
{
LCD_WrDat(F16x16[adder]);	
adder += 1;
}      
LCD_Set_Pos(x,y + 1); 
for(wm = 0;wm < 16;wm++) //         
{
LCD_WrDat(F16x16[adder]);
adder += 1;
} 	  	
}

/***********功能描述：显示缓存单字16*24 起始点坐标(x,y),x的范围0～127，y为页的范围0～7**********/
void WRT_HIGDATA(unsigned char x,unsigned char y,unsigned char txd)
{
 unsigned char i;
  LCD_Set_Pos(x, y);
  for(i=0;i<12;i++)  
 {LCD_WrDat(F16x24[ txd*36+i]); } 
 LCD_Set_Pos(x, y+1);
 for(i=0;i<12;i++) 
 {LCD_WrDat(F16x24[txd*36+i+12]);} 
 LCD_Set_Pos(x, y+2);
  for(i=0;i<12;i++)  
 {LCD_WrDat(F16x24[ txd*36+i+24]); }  
}


/*****************功能描述：显示缓存单字24*24 显示的坐标（x,y），y为页范围0～7****************************/
void WRT_DATA(unsigned char x,unsigned char y,unsigned char txd)
{
  unsigned char i;
  LCD_Set_Pos(x, y);
  for(i=0;i<24;i++)  
 {LCD_WrDat(F24x24[ txd*72+i]); } 
 LCD_Set_Pos(x, y+1);
 for(i=0;i<24;i++) 
 {LCD_WrDat(F24x24[txd*72+i+24]);} 
 LCD_Set_Pos(x, y+2);
  for(i=0;i<24;i++)  
 {LCD_WrDat(F24x24[ txd*72+i+48]); } 
}


/***********功能描述：显示缓存单字8*16 起始点坐标(x,y),x的范围0～127，y为页的范围0～7**********/
void WRT_LOWDATA(unsigned char x,unsigned char y,unsigned char txd)
{
  unsigned char i;
  LCD_Set_Pos(x, y);
  for(i=0;i<8;i++)  
 {LCD_WrDat(F8X16[ txd*16+i]); } 
 LCD_Set_Pos(x+8, y);
 for(i=0;i<8;i++) 
 {LCD_WrDat(F8X16[txd*16+i+8]);} 
}

