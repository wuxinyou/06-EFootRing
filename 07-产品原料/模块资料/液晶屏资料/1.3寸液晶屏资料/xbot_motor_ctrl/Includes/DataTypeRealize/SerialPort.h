
#ifndef _SERIALPORT_H
#define _SERIALPORT_H

#include <stdlib.h>	
#include <stdint.h>	
// #include <stdbool.h>	
#include "stm32f10x.h"
#include "STM32_GpioInit.h"
#include "STM32F10x_PriorityControl.h"


	typedef USART_TypeDef USART_t;
	#define USART_RX_BUFFER_SIZE 32
	#define USART_TX_BUFFER_SIZE 32
	#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )
	#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )
	#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
	#error RX buffer size is not a power of 2
	#endif
	#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
	#endif

	typedef struct USART_Buffer
	{
			__IO uint8_t RX[USART_RX_BUFFER_SIZE];
			__IO uint8_t TX[USART_TX_BUFFER_SIZE];
			__IO uint8_t RX_Head;
			__IO uint8_t RX_Tail;
			__IO uint8_t TX_Head;
			__IO uint8_t TX_Tail;
	}USART_Buffer_t;

	class USART_data_t
	{
		public:
			USART_data_t();
		public:
			bool USART_TXBuffer_FreeSpace();
			bool USART_TXBuffer_SendAll();
			uint8_t USART_TXBuffer_PutByte(uint8_t);
			bool USART_RXBufferData_Available();
			uint8_t USART_RXBuffer_GetByte();
			bool USART_RXComplete();
			void USART_DataRegEmpty();
			void enableTX(void);				//使能485发送
			void disabledTX(void);			//失能485发送
		public:
			USART_t * usart;
			USART_Buffer_t buffer;
			bool useRS485Flag;
			GPIO_TypeDef* ctrTxPort;			//控制485发送的端口号
			uint16_t ctrTxPin;					//控制485发送的引脚号
	};
	
	class SerialPort
	{
		public:
			SerialPort(int s_port=1,int s_baudrate=19200);
			~SerialPort();
			int getPort();
			bool readAllow();
			uint8_t getChar(void);
			bool putChar(uint8_t);
			void write( uint8_t * data, uint8_t maxSize );
			void writeLine(const char*);
			void open();							  //打开串口
			void close();								//关闭串口
			void setRS485CtrTx(GPIO_TypeDef*,uint16_t);
			void setPriority(uint8_t,uint8_t);	
		private:
			int USART_IRQn;								//中断号
			void initUsart();
			void usartConfig(USART_TypeDef*,uint32_t,uint16_t);
			void usartGpioInit();
		private:
			USART_data_t *COM;
			int port;
			int baudrate;
	};

#endif

