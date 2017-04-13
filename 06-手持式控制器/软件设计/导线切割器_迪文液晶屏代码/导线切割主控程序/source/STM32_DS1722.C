//#include "STM32_DS1722.h"
// 
//#define	 DS1722_GLOBALS   1
//DS1722_SPI_STRUCT	str_ds1722_spi_data;
///* Private macro -------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
///* Private function prototypes -----------------------------------------------*/
///* Private functions ---------------------------------------------------------*/
//void SpiInit(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  SPI_InitTypeDef SPI_InitStructure; 
//
//  /* 1 bit for pre-emption priority, 3 bits for subpriority */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//
//  /* Configure and enable SPI1 interrupt -------------------------------------*/
//  NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//  /* SPI1 configuration ------------------------------------------------------*/
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI设置为双线双向全双工
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//数据捕获于第1个时钟沿
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_InitStructure.SPI_CRCPolynomial = 7;//定义了用于CRC值计算的多项式。
////	**CPOL    0                时钟极性
////	**CPHA    1                时钟相位
//      /*Enable SPI1.NSS as a GPIO*/
//  SPI_SSOutputCmd(SPI1, ENABLE);
//
//  SPI_Init(SPI1, &SPI_InitStructure);
//  
////  /* Enable SPI1 TXE interrupt */
////  SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
//
//  /* Enable SPI1 RXNE interrupt */
//  SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
//  
//  /* Enable SPI1 */
//  SPI_Cmd(SPI1, ENABLE);
//}
////PA3~~7:  SS2,SS1 SCK MISO mosi
//void Ds1722GpioInit(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//    /* Enable SPI1 and GPIO clocks */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA
//                         , ENABLE);
//
//  /* Configure SPI1 pins: SCK, MISO and MOSI */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7; //
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//   
//  
//    /*Configure PA.4(NSS)--------------------------------------------*/
//  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4;
//  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  
//  /* Deselect the FLASH: Chip Select high */
//  DISDS1722;
//}
//
//void Ds1722Init(void)
//{
//  Ds1722GpioInit();
//  SpiInit();
//  str_ds1722_spi_data.state = 0x00;
//  str_ds1722_spi_data.temperature = 0x0000;
//  ENDS1722;
//  SPI_SendHalfWord(DS1722INITWORD);
//}
////SPI 中断
//void SPI1_IRQHandler(void)
//{
//  int16_t spirecdat;
//  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) != RESET)
//  {
//    SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
//     /* Store SPI2 received data */
//    spirecdat = SPI_I2S_ReceiveData(SPI1);
//    InterruptDealDs1722(spirecdat); 
//  } 
//}
////查询方式发送一个字节
//void SpiSendByte(uint8_t dat)
//{
//  /* Loop while DR register in not emplty */
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//
//  /* Send byte through the SPI1 peripheral */
//  SPI_I2S_SendData(SPI1, dat);
//}
///*******************************************************************************
//* Function Name  : SPI_FLASH_SendHalfWord
//* Description    : Sends a Half Word through the SPI interface and return the
//*                  Half Word received from the SPI bus.
//* Input          : Half Word : Half Word to send.
//* Output         : None
//* Return         : The value of the received Half Word.
//*******************************************************************************/
//int16_t SPI_SendHalfWord(int16_t HalfWord)
//{
//  /* Loop while DR register in not emplty */
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//
//  /* Send Half Word through the SPI1 peripheral */
//  SPI_I2S_SendData(SPI1, HalfWord);
//
////  /* Wait to receive a Half Word */
////  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
////
////  /* Return the Half Word read from the SPI bus */
////  return SPI_I2S_ReceiveData(SPI1);
//  return 1;
//}
//// 通过SPI0向Ds1722发出读取高字节的地址，启动温度值的读取过程。
//void StartTranDs1722(void)
//{ 
//  ENDS1722;                                                //Ds1722通讯使能
//  SPI_SendHalfWord(0x01FF);                              //开始接收Ds1722高字节数据
// 
//  DISDS1722;
//}
////用于判别是否完成温度值传输
//uint8_t FinishTranDs1722(void)
//{
//  if(str_ds1722_spi_data.state & 0x0004)
//  {
//    str_ds1722_spi_data.state &= ~0x0004;
//    return TRUE;
//  }
//  else
//  {
//    return FALSE;
//  }
//} 
////处理DS1722接收到的数据
//void InterruptDealDs1722(int16_t chr)
//{
//  static int16_t s_recv_spi0_buffer;                            //接收数据缓存
//
//   DISDS1722;                                              //终止与DS1722通讯
//  
//  if (str_ds1722_spi_data.state & 0x0001)                              //检查Ds1722初始化是否完成
//  {
//      if (str_ds1722_spi_data.state & 0x0002)                          //检查低字节是否接收完成
//      {
//              str_ds1722_spi_data.temperature = (chr << 8);                 		//获取温度值
//              str_ds1722_spi_data.temperature |= (s_recv_spi0_buffer & 0x00FF);
//              str_ds1722_spi_data.state |= 0x04;                         //接收完成标志置位
//              str_ds1722_spi_data.state &= ~0x02;                        //低字节接收完成标志清零
//      }
//      else
//      {
//              s_recv_spi0_buffer = chr;                                   //缓存低字节数据
//              str_ds1722_spi_data.state |= 0x02;                          //低字节接收完成标志置位
//                                                                           //Ds1722通讯使能
//              ENDS1722;
//              SPI_SendHalfWord(0x02FF);                                   //开始接收Ds1722高字节数据
//      }
//  }
//  else
//  {
//      str_ds1722_spi_data.state |= 0x01;                             //初始化完成标志置位
//  }
//}
//
//#define UNTEMPPROT1 (sys_protect.over_temp_flag == SET_PROTECT_FLAG)
//#define UNTEMPPROT2 (GetTemperature()<=50)
//#define TEMPPROT1	(GetTemperature()>=80)
//
//void TemperatureProtect(void)
//{
//  if(UNTEMPPROT1&&UNTEMPPROT2)
//          sys_protect.over_temp_flag = CLR_PROTECT_FLAG;
//  else
//  {
//          if(TEMPPROT1)
//     sys_protect.over_temp_flag = SET_PROTECT_FLAG;		
//  }	
//}
//
//void Ds1722Work(void)
//{
// //在定时器中会100ms打开一次over_send_flag = 1
//  if(str_ds1722_spi_data.over_send_flag == 1)
//  {
//    str_ds1722_spi_data.over_send_flag = 0;
//    SpiInit();
//    if(FinishTranDs1722())
//    {
//      TemperatureProtect();
//    }  
//    StartTranDs1722(); 
//  }
//}	


