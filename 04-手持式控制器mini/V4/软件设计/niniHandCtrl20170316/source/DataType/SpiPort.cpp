/**
  ******************************************************************************
	*文件：SpiPort.c
	*作者：叶依顺
	*版本：2.0
	*日期：2014-01-02
	*概要：SpiPort类的实现
	*备注：SPI1和SPI3不能同时开启重映射
  ******************************************************************************  
	*/

#include "SpiPort.h"
#include "string.h"
//根据不同的CPU型号做断言
#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
#define ISSPIPORT(PORT) (((PORT) == 1) || \
                                     ((PORT) == 2) || \
                                     ((PORT) == 3)
#endif

#if defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)
#define ISSPIPORT(PORT) (((PORT) == 1) || \
                                     ((PORT) == 2)
#endif

#if defined(STM32F10X_LD)\
		||defined(STM32F10X_LD_VL)
#define ISSPIPORT(PORT) (((PORT) == 1)
#endif

//SPI重映射开关 SPI2没有重映射
// #define SPIREMAP_1 //定义开启串口1重映射
// #define SPIREMAP_3 //定义开启串口1重映射

SpiPort::SpiPort(uint8_t s_port)
{
// 	assert_param(ISSPIPORT(s_port));						//断言spi端口号是否合法
	port=s_port;
	
	if(port==1)
	{
		SPIX=SPI1;
		SPI_IRQn=SPI1_IRQn;
	}
#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)\
		||defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)
	else if(port==2)
	{
		SPIX=SPI2;
		SPI_IRQn=SPI2_IRQn;
	}
#endif	
	
#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
	else if(port==3)
	{
		SPIX=SPI3;
		SPI_IRQn=SPI3_IRQn;
	}
#endif
	spiConfig();
}
SpiPort::~SpiPort()
{
	
}

/**
  * 功  能：发送一个数据
  * 参  数：待发送的数据
  * 返回值：无
  */
void SpiPort::putData(uint8_t Data)
{
	uint16_t temp = Data;
	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIX, temp); 
	while(SPI_I2S_GetFlagStatus(SPIX,SPI_I2S_FLAG_RXNE) == RESET); 
	temp = (SPI_I2S_ReceiveData(SPIX));
}

void SpiPort::open()
{
  SPI_Cmd(SPIX, ENABLE);
}

void SpiPort::close()
{
  SPI_Cmd(SPIX, DISABLE);
}

void SpiPort::setPriority(uint8_t Preemption,uint8_t Sub)
{
	uint8_t temPreemptionPriority = Preemption;
	uint8_t temSubPriority = Sub;
	setIRQPriority(SPI_IRQn,temPreemptionPriority,temSubPriority);
}

void SpiPort::spiConfig()
{
	SPI_InitTypeDef   SPI_InitStructure;
  spiGpioInit();																												//初始化SPI引脚和时钟
	SPI_Cmd(SPIX, DISABLE);																								//失能SPI
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//双线输入输出全双工模式
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;													//设置为SPI的主机模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;											//SPI数据大小，发送8位帧结构数据
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;														//设备空闲状态时同步时钟SCK的状态，HIGH表示高电平，LOW表示低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;													//时钟相位，1表示在同步SCK的奇数沿边采样，2表示偶数沿边采样
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;															//NSS由软件控制片选
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		//时钟的预分频值 值越小速度越快
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;										//MSB高位在前
  SPI_InitStructure.SPI_CRCPolynomial = 7;															//CRC校验和的多项式
//   SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;												//
  SPI_Init(SPIX, &SPI_InitStructure);																		//初始化SPI的配置项
  SPI_SSOutputCmd(SPIX, ENABLE);																				//使能SPI1_NSS 片选拉高
}

void SpiPort::spiGpioInit()
{
	switch(port)
	{
		case 1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#ifndef 	SPIREMAP_1  //spi1没有重映射
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
			STM32_GpioOneInit(GPIO_Pin_4,GPIO_Speed_50MHz, GPIO_Mode_Out_PP,GPIOA); 				//SPI1_NSS  片选 推挽输出
			STM32_GpioOneInit(GPIO_Pin_5,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOA);					//SPI1_SCK  时钟 复用推挽输出
			STM32_GpioOneInit(GPIO_Pin_6,GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,GPIOA);		//SPI1_MOSO 主机输入/从机输出 浮空输入
			STM32_GpioOneInit(GPIO_Pin_7,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOA);					//SPI1_MISI 主机输出/从机输入 复用推挽输出
// 			GPIO_ResetBits(GPIOA, GPIO_Pin_4);//片选拉高
#else									//spi1开启重映射
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
			STM32_GpioOneInit(GPIO_Pin_15,GPIO_Speed_50MHz, GPIO_Mode_Out_PP,GPIOA); 				//SPI1_NSS  片选 推挽输出
			STM32_GpioOneInit(GPIO_Pin_3,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOB);					//SPI1_SCK  时钟 复用推挽输出
			STM32_GpioOneInit(GPIO_Pin_4,GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,GPIOB);		//SPI1_MOSO 主机输入/从机输出 浮空输入
			STM32_GpioOneInit(GPIO_Pin_5,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOB);					//SPI1_MISI 主机输出/从机输入 复用推挽输出
// 			GPIO_ResetBits(GPIOA, GPIO_Pin_15);//片选拉高
#endif
			break;
#if defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)		
		case 2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE );														//SPI2时钟使能
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			STM32_GpioOneInit(GPIO_Pin_12,GPIO_Speed_50MHz, GPIO_Mode_Out_PP,GPIOB); 				//SPI2_NSS  片选 推挽输出
			STM32_GpioOneInit(GPIO_Pin_13,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOB);					//SPI2_SCK  时钟 复用推挽输出
			STM32_GpioOneInit(GPIO_Pin_14,GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,GPIOB);		//SPI2_MOSO 主机输入/从机输出 浮空输入
			STM32_GpioOneInit(GPIO_Pin_15,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOB);					//SPI2_MISI 主机输出/从机输入 复用推挽输出
// 			GPIO_ResetBits(GPIOB, GPIO_Pin_12);//片选拉高
			break;
#endif
		
#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)		
		case 3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
#ifndef 	SPIREMAP_3  //spi3没有重映射
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
			STM32_GpioOneInit(GPIO_Pin_15,GPIO_Speed_50MHz, GPIO_Mode_Out_PP,GPIOA); 				//SPI3_NSS  片选 推挽输出
			STM32_GpioOneInit(GPIO_Pin_3,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOB);					//SPI3_SCK  时钟 复用推挽输出
			STM32_GpioOneInit(GPIO_Pin_4,GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,GPIOB);		//SPI3_MOSO 主机输入/从机输出 浮空输入
			STM32_GpioOneInit(GPIO_Pin_5,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOB);					//SPI3_MISI 主机输出/从机输入 复用推挽输出
// 			GPIO_ResetBits(GPIOA, GPIO_Pin_15);//片选拉高
#else									//spi3开启重映射
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
			STM32_GpioOneInit(GPIO_Pin_4,GPIO_Speed_50MHz, GPIO_Mode_Out_PP,GPIOA); 				//SPI3_NSS  片选 推挽输出
			STM32_GpioOneInit(GPIO_Pin_10,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOC);					//SPI3_SCK  时钟 复用推挽输出
			STM32_GpioOneInit(GPIO_Pin_11,GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,GPIOC);		//SPI3_MOSO 主机输入/从机输出 浮空输入
			STM32_GpioOneInit(GPIO_Pin_12,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,GPIOC);					//SPI3_MISI 主机输出/从机输入 复用推挽输出
// 			GPIO_ResetBits(GPIOA, GPIO_Pin_4);//片选拉高
#endif
			break;
#endif
	}
}

