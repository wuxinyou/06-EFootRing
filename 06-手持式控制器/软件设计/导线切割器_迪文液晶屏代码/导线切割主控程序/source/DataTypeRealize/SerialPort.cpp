/**
  ******************************************************************************
	*ÎÄ¼ş£ºSerialPort.c
	*×÷Õß£ºÒ¶ÒÀË³
	*°æ±¾£º2.0
	*ÈÕÆÚ£º2014-01-02
	*¸ÅÒª£º´®¿ÚÀàµÄÊµÏÖ
	*±¸×¢£º¸ÃÀàÊÇ°ë·Ö×°µÄ
	*
  ******************************************************************************  
	*/
#include "SerialPort.h"	

#define PORT_1 1
#define PORT_2 2
#define PORT_3 3
#define PORT_4 4
#define PORT_5 5

//¸ù¾İ²»Í¬µÄCPUĞÍºÅ×ö¶ÏÑÔ
#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
#define ISSERIALPORT(PORT) (((PORT) == 1) || \
                                     ((PORT) == 2) || \
                                     ((PORT) == 3) || \
                                     ((PORT) == 4) || \
                                     ((PORT) == 5))
#endif

#if defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)
#define ISSERIALPORT(PORT) (((PORT) == 1) || \
                                     ((PORT) == 2) || \
                                     ((PORT) == 3))
#endif

#if defined(STM32F10X_LD)\
		||defined(STM32F10X_LD_VL)
#define ISSERIALPORT(PORT) (((PORT) == 1) || \
                                     ((PORT) == 2))
#endif

//¹ØÓÚ´®¿ÚÒı½ÅºÍÊ±ÖÓµÈµÄÅäÖÃºê

// #define USARTREMAP_1 //¶¨Òå¿ªÆô´®¿Ú1ÖØÓ³Éä

#ifndef USARTREMAP_1 //´®¿Ú1Ã»ÓĞÖØÓ³Éä

	#define USARTPORT_1 			GPIOA									//´®¿Ú1ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_1 		GPIO_Pin_10						//´®¿Ú1ÊäÈëÒı½Å
	#define USARTPIN_OUT_1 	  GPIO_Pin_9						//´®¿Ú1Êä³öÒı½Å
	#define USARTRCCPERIPH_1 	RCC_APB2Periph_GPIOA	//´®¿Ú1ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß

#else 							//´®¿Ú1¿ªÆôÖØÓ³Éä

	#define USARTPORT_1 			GPIOB									//´®¿Ú1ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_1 		GPIO_Pin_7						//´®¿Ú1ÊäÈëÒı½Å
	#define USARTPIN_OUT_1 		GPIO_Pin_6						//´®¿Ú1Êä³öÒı½Å
	#define USARTRCCPERIPH_1	RCC_APB2Periph_GPIOB	//´®¿Ú1ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß
	
#endif

// #define USARTREMAP_2 //¶¨Òå¿ªÆô´®¿Ú2ÖØÓ³Éä

#ifndef USARTREMAP_2 //´®¿Ú2Ã»ÓĞÖØÓ³Éä USART2_REMAP = 0

	#define USARTPORT_2 			GPIOA									//´®¿Ú2ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_2 		GPIO_Pin_3						//´®¿Ú2ÊäÈëÒı½Å
	#define USARTPIN_OUT_2	  GPIO_Pin_2						//´®¿Ú2Êä³öÒı½Å
	#define USARTRCCPERIPH_2 	RCC_APB2Periph_GPIOA	//´®¿Ú2ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß

#else 							//´®¿Ú2¿ªÆôÖØÓ³Éä USART2_REMAP = 1

	#define USARTPORT_2 			GPIOD									//´®¿Ú2ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_2 		GPIO_Pin_6						//´®¿Ú2ÊäÈëÒı½Å
	#define USARTPIN_OUT_2 		GPIO_Pin_5						//´®¿Ú2Êä³öÒı½Å
	#define USARTRCCPERIPH_2	RCC_APB2Periph_GPIOD	//´®¿Ú2ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß
	
#endif

//ÔÚ100ºÍ144Òı½Å·Ö×°µÄSTM32F10XÏµÁĞµ¥Æ¬»úÉÏ´®¿Ú3ÓĞÁ½ÖÖÖØÓ³Éä

#define USARTREMAP_3_OFF 					//Ã»ÓĞ¿ªÆô´®¿Ú3ÖØÓ³Éä
// #define USARTREMAP_3_TO_PORTC	//´®¿ÚÖØÓ³Éäµ½PORTCÉÏ
// #define USARTREMAP_3_TO_PORTD	//´®¿ÚÖØÓ³Éäµ½PORTDÉÏ

#if !defined(USARTREMAP_3_OFF)&&!defined(USARTREMAP_3_TO_PORTC)&&!defined(USARTREMAP_3_TO_PORTD)
#error ´®¿Ú3Ã»ÓĞÖ¸¶¨Ó³ÉäÀàĞÍ
#endif


#ifdef USARTREMAP_3_OFF //´®¿Ú3Ã»ÓĞÖØÓ³Éä 

	#define USARTPORT_3 			GPIOB									//´®¿Ú3ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_3 		GPIO_Pin_11						//´®¿Ú3ÊäÈëÒı½Å
	#define USARTPIN_OUT_3	  GPIO_Pin_10						//´®¿Ú3Êä³öÒı½Å
	#define USARTRCCPERIPH_3 	RCC_APB2Periph_GPIOB	//´®¿Ú3ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß
#endif
							//´®¿Ú3¿ªÆôÖØÓ³Éä
#ifdef USARTREMAP_3_TO_PORTC	//´®¿ÚÖØÓ³Éäµ½PORTCÉÏ
	#define USARTPORT_3 			GPIOC									//´®¿Ú3ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_3 		GPIO_Pin_11						//´®¿Ú3ÊäÈëÒı½Å
	#define USARTPIN_OUT_3	  GPIO_Pin_10						//´®¿Ú3Êä³öÒı½Å
	#define USARTRCCPERIPH_3 	RCC_APB2Periph_GPIOC	//´®¿Ú3ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß
#endif

#ifdef USARTREMAP_3_TO_PORTD	//´®¿ÚÖØÓ³Éäµ½PORTCÉÏ	
	#define USARTPORT_3 			GPIOD									//´®¿Ú3ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_3 		GPIO_Pin_9						//´®¿Ú3ÊäÈëÒı½Å
	#define USARTPIN_OUT_3	  GPIO_Pin_8						//´®¿Ú3Êä³öÒı½Å
	#define USARTRCCPERIPH_3 	RCC_APB2Periph_GPIOD	//´®¿Ú3ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß
#endif

//´®¿Ú4ºÍ5Ã»ÓĞÖØÓ³Éä
	#define USARTPORT_4 			GPIOC									//´®¿Ú4ËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_4 		GPIO_Pin_11						//´®¿Ú4ÊäÈëÒı½Å
	#define USARTPIN_OUT_4	  GPIO_Pin_10						//´®¿Ú4Êä³öÒı½Å
	#define USARTRCCPERIPH_4 	RCC_APB2Periph_GPIOC	//´®¿Ú4ËùÔÚÒı½ÅµÄÊ±ÖÓ×ÜÏß

	#define USARTPORT_5_IN 				GPIOD									//´®¿Ú5ÊäÈëÒı½ÅËùÔÚ¶Ë¿Ú
	#define USARTPORT_5_OUT 			GPIOC									//´®¿Ú5Êä³öÒı½ÅËùÔÚ¶Ë¿Ú
	#define USARTPIN_IN_5 				GPIO_Pin_2						//´®¿Ú5ÊäÈëÒı½Å
	#define USARTPIN_OUT_5	  		GPIO_Pin_12						//´®¿Ú5Êä³öÒı½Å
	#define USARTRCCPERIPH_5_IN 	RCC_APB2Periph_GPIOD	//´®¿Ú5ËùÔÚÊäÈëÒı½ÅµÄÊ±ÖÓ×ÜÏß
	#define USARTRCCPERIPH_5_OUT 	RCC_APB2Periph_GPIOC	//´®¿Ú5ËùÔÚÊä³öÒı½ÅµÄÊ±ÖÓ×ÜÏß

#define USART_CR1_TXEIE_gm 	(uint16_t)(1<<7)

USART_data_t USART_data_1;
USART_data_t USART_data_2;
USART_data_t USART_data_3;
USART_data_t USART_data_4;
USART_data_t USART_data_5;
USART_data_t USART_data_6;

/**
  * ¹¦  ÄÜ£ºUSART_data_t¹¹Ôìº¯Êı
  * ²Î  Êı£º´®¿ÚµÄ¼Ä´æÆ÷
  * ·µ»ØÖµ£ºÎŞ
  */
USART_data_t::USART_data_t()
{
	useRS485Flag=false;
	ctrTxPort=NULL;
	ctrTxPin=0;
	usart=NULL;
	buffer.RX_Tail = 0;
	buffer.RX_Head = 0;
	buffer.TX_Tail = 0;
	buffer.TX_Head = 0;
}

/**
  * ¹¦  ÄÜ£ºÅĞ¶Ï´®¿Ú·¢ËÍ»º³åÇøÊÇ·ñÓĞ¿Õ¼ä
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º
	* true: »º³åÇøÓĞ¿Õ¼ä£¬¿ÉÒÔĞ´
	* false: »º³åÇøÃ»ÓĞ¿Õ¼ä£¬²»¿ÉÒÔĞ´
  */
bool USART_data_t::USART_TXBuffer_FreeSpace()
{
	/* Make copies to make sure that volatile access is specified. */
	uint8_t tempHead = (buffer.TX_Head + 1) & USART_TX_BUFFER_MASK;
	uint8_t tempTail = buffer.TX_Tail;

	/* There are data left in the buffer unless Head and Tail are equal. */
	return (tempHead != tempTail);
}

/**
  * ¹¦  ÄÜ£ºÅĞ¶Ï´®¿Ú»º³åÇøÊı¾İÊÇ·ñ·¢ËÍÍê±Ï
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º
	* true: Ã»ÓĞ·¢ËÍÍê
	* false: ·¢ËÍÍê±Ï
  */
bool USART_data_t::USART_TXBuffer_SendAll()
{
	uint8_t tempHead =  buffer.TX_Head;
	uint8_t tempTail =  buffer.TX_Tail;
	return (tempHead != tempTail);
}

/**
  * ¹¦  ÄÜ£ºÏò´®¿Ú»º³åÇøĞ´Ò»¸ö×Ö½Ú
  * ²Î  Êı£º
	* data  :´ıĞ´ÈëµÄÊı¾İ
  * ·µ»ØÖµ£º
	* true: ³É¹¦Ğ´Èë
	* false: Ğ´ÈëÊ§°Ü
  */
uint8_t USART_data_t::USART_TXBuffer_PutByte(uint8_t data)
{
// 	uint16_t tempCR1;
	uint8_t tempTX_Head;
	bool TXBuffer_FreeSpace;
	USART_Buffer_t * TXbufPtr;
	TXbufPtr = &buffer;
	TXBuffer_FreeSpace = USART_TXBuffer_FreeSpace();
	if(TXBuffer_FreeSpace)//´®¿Ú»º³åÇøÎ´Âú
	{
	  	tempTX_Head = TXbufPtr->TX_Head;
	  	TXbufPtr->TX[tempTX_Head]= data;
		/* Advance buffer head. */
		TXbufPtr->TX_Head = (tempTX_Head + 1) & USART_TX_BUFFER_MASK;
		/* Enable DRE interrupt.¿ªÖĞ¶Ï */
		USART_ITConfig(usart, USART_IT_TXE, ENABLE);//Ê¹ÄÜ·¢ËÍÖĞ¶Ï
// 		tempCR1 = this->usart->CR1;
// 		tempCR1 = (tempCR1 | USART_CR1_TXEIE_gm);
// 		this->usart->CR1 = tempCR1;
	}
	return TXBuffer_FreeSpace;
}

/**
  * ¹¦  ÄÜ£ºÅĞ¶Ï½ÓÊÕ»º³åÇøÊÇ·ñÓĞÊı¾İ
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º
	* true: 	ÓĞÊı¾İ
	* false: 	ÎŞÊı¾İ
  */
bool USART_data_t::USART_RXBufferData_Available()
{
	/* Make copies to make sure that volatile access is specified. */
	uint8_t tempHead = buffer.RX_Head;
	uint8_t tempTail = buffer.RX_Tail;

	/* There are data left in the buffer unless Head and Tail are equal. */
	return (tempHead != tempTail);
}

/**
  * ¹¦  ÄÜ£º´Ó´®¿Ú½ÓÊÕ»º³åÇø¶ÁÈ¡Ò»¸ö×Ö½Ú
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º¶ÁÈ¡µÄ×Ö½Ú
  */
uint8_t USART_data_t::USART_RXBuffer_GetByte()
{
	USART_Buffer_t * bufPtr;
	uint8_t ans;

	bufPtr = &buffer;
	ans = (bufPtr->RX[bufPtr->RX_Tail]);

	/* Advance buffer tail. */
	bufPtr->RX_Tail = (bufPtr->RX_Tail + 1) & USART_RX_BUFFER_MASK;
	return ans;
}

/**
  * ¹¦  ÄÜ£º´Ó´®¿ÚDR¼Ä´æÆ÷ÖĞ½ÓÊÕÒ»¸öÊı¾İĞ´Èë½ÓÊÕ»º³åÇø
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º
	* true:  ½ÓÊÕ»º³åÇøÎ´Âú
	* false: ½ÓÊÕ»º³åÇøÂú
  */
bool USART_data_t::USART_RXComplete()
{
	USART_Buffer_t * bufPtr;
	uint8_t ans;
	uint8_t tempRX_Head;
	uint8_t tempRX_Tail;
	uint8_t data;

	bufPtr = &buffer;
	/* Advance buffer head. */
	tempRX_Head = (bufPtr->RX_Head + 1) & USART_RX_BUFFER_MASK;

	/* Check for overflow. */
	tempRX_Tail = bufPtr->RX_Tail;
	data = usart->DR;

	if (tempRX_Head == tempRX_Tail) {
	  	ans = 0;
	}else{
		ans = 1;
		buffer.RX[buffer.RX_Head] = data;
		buffer.RX_Head = tempRX_Head;
	}
	return ans;
}

/**
  * ¹¦  ÄÜ£ºÏò´®¿ÚÊı¾İ¼Ä´æÆ÷ÖĞĞ´ÈëÒ»¸öÊı¾İ
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£ºÎŞ
  */
void USART_data_t::USART_DataRegEmpty()
{
	USART_Buffer_t * bufPtr;
	uint8_t tempTX_Tail;
	uint16_t tempCR1;
	uint8_t data;
	bufPtr = &buffer;


	/* Check if all data is transmitted. */
	tempTX_Tail = buffer.TX_Tail;
	if (bufPtr->TX_Head == tempTX_Tail){
	    /* Disable DRE interrupts. */
		tempCR1 = usart->CR1;
		tempCR1 = (tempCR1 & ~USART_CR1_TXEIE_gm) ;
		usart->CR1 = tempCR1;

	}else{
		/* Start transmitting. */
		enableTX();
		data = bufPtr->TX[buffer.TX_Tail];
		usart->DR = data;
		/* Advance buffer tail. */
		bufPtr->TX_Tail = (bufPtr->TX_Tail + 1) & USART_TX_BUFFER_MASK;
	}
}

/**
  * ¹¦  ÄÜ£ºÔÊĞí485·¢ËÍ
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£ºÎŞ
  */
void USART_data_t::enableTX(void)
{
	if(!useRS485Flag)
		return;
	else//Ê¹ÄÜ·¢ËÍ
		GPIO_SetBits(ctrTxPort, ctrTxPin);
}

/**
  * ¹¦  ÄÜ£º½ûÖ¹485·¢ËÍ
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£ºÎŞ
  */
void USART_data_t::disabledTX(void)
{
	if(!useRS485Flag)
		return;
	else//Ê§ÄÜ·¢ËÍ
		GPIO_ResetBits(ctrTxPort, ctrTxPin);
}

/**
  * ¹¦  ÄÜ£ºSerialPortÀàµÄ¹¹Ôìº¯Êı
  * ²Î  Êı£º
	* s_port£º´®¿ÚºÅ
	* s_baudrate£º²¨ÌØÂÊ
  * ·µ»ØÖµ£ºÎŞ
  */
SerialPort::SerialPort(int s_port,int s_baudrate)
{
	assert_param(ISSERIALPORT(s_port));						//¶ÏÑÔ´®¿ÚÊÇ·ñºÏ·¨ 1 2 3 4 5
	assert_param(IS_USART_BAUDRATE(s_baudrate));	//¶ÏÑÔ²¨ÌØÂÊÊÇ·ñºÏ·¨
	port=s_port; 
	baudrate=s_baudrate;
	initUsart();//´®¿Ú³õÊ¼»¯
}

/**
  * ¹¦  ÄÜ£ºSerialPortÀàµÄÎö¹¹º¯Êı
  * ²Î  Êı£º
  * ·µ»ØÖµ£ºÎŞ
  */
SerialPort::~SerialPort()
{
	close();	//stop Í£Ö¹·¢ËÍ
	COM=NULL;
}

/**
  * ¹¦  ÄÜ£º´®¿ÚÅäÖÃº¯Êı
  * ²Î  Êı£º
	* USART_x£º´®¿ÚÅäÖÃ¼Ä´æÆ÷
	* Baudrate£º²¨ÌØÂÊ
	*	USART_Mode£º´®¿ÚÄ£Ê½ Èç·¢ËÍ½ÓÊÕµÈ
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::usartConfig(USART_TypeDef *USART_x, uint32_t Baudrate,uint16_t USART_Mode)
{
	USART_InitTypeDef USART_InitStructure;

	//½«½á¹¹ÌåÉèÖÃÎªÈ±Ê¡×´Ì¬
  USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = Baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode;

	/* Configure USART1 */
	USART_Init(USART_x, &USART_InitStructure);
}

/**
  * ¹¦  ÄÜ£º´®¿ÚGPIOÅäÖÃ
  * ²Î  Êı£º
	* usartPort£º´®¿ÚºÅ
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::usartGpioInit()
{
	int temPort=port;
	close();//Ê§ÄÜ´®¿Ú
	switch(temPort)
	{
#if defined(STM32F10X_LD)\
		||defined(STM32F10X_LD_VL)\
		||defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
		case 1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//Ê¹ÄÜ´®¿ÚÊ±ÖÓ
#ifndef USARTREMAP_1	
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_1,ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);	//½ûÖ¹´®¿Ú¹Ü½ÅÖØÓ³Éä
#else
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_1|RCC_APB2Periph_AFIO,ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);		//´®¿Ú¹Ü½ÅÖØÓ³Éä
#endif
			STM32_GpioOneInit(USARTPIN_IN_1, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,USARTPORT_1);	//¸¡¿ÕÊäÈë£¨Ò²¿ÉÒÔÉèÖÃ³É´øÉÏÀ­ÊäÈGPIO_Mode_IPUë£©
			STM32_GpioOneInit(USARTPIN_OUT_1,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,USARTPORT_1);				//ÍÆÍì¸´ÓÃÊä³ö
			break;
		case 2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//Ê¹ÄÜ´®¿ÚÊ±ÖÓ
#ifndef USARTREMAP_2	
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_2,ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);	//½ûÖ¹´®¿Ú¹Ü½ÅÖØÓ³Éä
#else
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_2|RCC_APB2Periph_AFIO,ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);		//´®¿Ú¹Ü½ÅÖØÓ³Éä
#endif
			STM32_GpioOneInit(USARTPIN_IN_2, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,USARTPORT_2);	//¸¡¿ÕÊäÈë£¨Ò²¿ÉÒÔÉèÖÃ³É´øÉÏÀ­ÊäÈGPIO_Mode_IPUë£©
			STM32_GpioOneInit(USARTPIN_OUT_2,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,USARTPORT_2);				//ÍÆÍì¸´ÓÃÊä³ö
			break;
#endif
#if defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)		
		case 3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//Ê¹ÄÜ´®¿ÚÊ±ÖÓ
#ifdef USARTREMAP_3_OFF	
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_3,ENABLE);
			GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, DISABLE);	//½ûÖ¹´®¿Ú3µÄ²¿·ÖÖØÓ³Ïñ
			GPIO_PinRemapConfig(GPIO_FullRemap_USART3, DISABLE);		//½ûÖ¹´®¿Ú3µÄÍêÈ«ÖØÓ³Ïñ
#endif
		
#ifdef USARTREMAP_3_TOPORTC
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_3|RCC_APB2Periph_AFIO,ENABLE);
			GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	//¿ªÆô´®¿Ú3µÄ²¿·ÖÖØÓ³Ïñ
#endif
		
#ifdef USARTREMAP_3_TOPORTD
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_3|RCC_APB2Periph_AFIO,ENABLE);
			GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);			//¿ªÆô´®¿Ú3µÄÍêÈ«ÖØÓ³Ïñ
#endif
		
			STM32_GpioOneInit(USARTPIN_IN_3, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,USARTPORT_3);	//¸¡¿ÕÊäÈë£¨Ò²¿ÉÒÔÉèÖÃ³É´øÉÏÀ­ÊäÈGPIO_Mode_IPUë£©
			STM32_GpioOneInit(USARTPIN_OUT_3,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,USARTPORT_3);				//ÍÆÍì¸´ÓÃÊä³ö
			break;
#endif

#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)		
		case 4:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);//Ê¹ÄÜ´®¿ÚÊ±ÖÓ
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_4,ENABLE);
			STM32_GpioOneInit(USARTPIN_IN_4, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,USARTPORT_4);	//¸¡¿ÕÊäÈë£¨Ò²¿ÉÒÔÉèÖÃ³É´øÉÏÀ­ÊäÈGPIO_Mode_IPUë£©
			STM32_GpioOneInit(USARTPIN_OUT_4,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,USARTPORT_4);				//ÍÆÍì¸´ÓÃÊä³ö
			break;
		case 5:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//Ê¹ÄÜ´®¿ÚÊ±ÖÓ
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_5_IN,ENABLE);
			RCC_APB2PeriphClockCmd(USARTRCCPERIPH_5_OUT,ENABLE);
			STM32_GpioOneInit(USARTPIN_IN_5, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING,USARTPORT_5_IN);	//¸¡¿ÕÊäÈë£¨Ò²¿ÉÒÔÉèÖÃ³É´øÉÏÀ­ÊäÈGPIO_Mode_IPUë£©
			STM32_GpioOneInit(USARTPIN_OUT_5,GPIO_Speed_50MHz, GPIO_Mode_AF_PP,USARTPORT_5_OUT);				//ÍÆÍì¸´ÓÃÊä³ö
			break;
#endif
	}
}
/**
  * ¹¦  ÄÜ£ºÅäÖÃ´®¿ÚµÄÓÅÏÈ¼¶
  * ²Î  Êı£º
	* Preemption£ºÇÀÕ¼ÓÅÏÈ¼¶
	* Sub				£ºÏìÓ¦ÓÅÏÈ¼¶
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::setPriority(uint8_t Preemption,uint8_t Sub)
{
	uint8_t temPreemptionPriority = Preemption;
	uint8_t temSubPriority = Sub;
	setIRQPriority(USART_IRQn,temPreemptionPriority,temSubPriority);
}

/**
  * ¹¦  ÄÜ£º´®¿ÚµÄ³õÊ¼»¯
  * ²Î  Êı£º
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::initUsart()
{
	switch(port)
	{
#if defined(STM32F10X_LD)\
		||defined(STM32F10X_LD_VL)\
		||defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
		case PORT_1:
			COM=&USART_data_1;
			COM->usart=USART1;
			USART_IRQn=USART1_IRQn;
			break;
		case PORT_2:
			COM=&USART_data_2;
			COM->usart=USART2;
			USART_IRQn=USART2_IRQn;
			break;
#endif

#if defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)		
		case PORT_3:
			COM=&USART_data_3;
			COM->usart=USART3;
			USART_IRQn=USART3_IRQn;
			break;
#endif
		
#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)		
		case PORT_4:
			COM=&USART_data_4;
			COM->usart=UART4;
			USART_IRQn=UART4_IRQn;
			break;
		case PORT_5:
			COM=&USART_data_5;
			COM->usart=UART5;
			USART_IRQn=UART5_IRQn;
			break;
#endif
		default: //´®¿ÚºÅ²»·ûºÏ¹æ·¶
// 			while(1);
			break;
	}
		usartGpioInit();
		setIRQPriority(USART_IRQn,0X00,0X00);//ÅäÖÃÓÅÏÈ¼¶ ,Ä¬ÈÏµÄÊÇ×î¸ßÓÅÏÈ¼¶
		usartConfig(COM->usart, baudrate, USART_Mode_Tx|USART_Mode_Rx);//ÅäÖÃ´®¿Ú
		USART_ITConfig(COM->usart, USART_IT_RXNE, ENABLE);//Ê¹ÄÜ½ÓÊÕÖĞ¶Ï
		USART_ITConfig(COM->usart, USART_IT_TC, ENABLE);	//Ê¹ÄÜ·¢ËÍÍê³ÉÖĞ¶Ï
}

/**
  * ¹¦  ÄÜ£ºÉèÖÃ485Í¨ĞÅ·½Ê½µÄ·¢ËÍ¿ØÖÆÒı½Å
  * ²Î  Êı£º
	*	port_ctrTx£º¿ØÖÆÒı½ÅËùÔÚµÄ¶Ë¿ÚºÅ
	* pin_ctrTx £º¿ØÖÆÒı½ÅËùÔÚµÄÒı½ÅºÅ
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::setRS485CtrTx(GPIO_TypeDef* port_ctrTx,uint16_t pin_ctrTx)			//ÉèÖÃ485¿ØÖÆÒı½ÅºÍ¶Ë¿ÚµÈ
{
	COM->useRS485Flag=true;			//ÉèÖÃ´®¿ÚÊôĞÔÊÇ485ÊôĞÔ
	COM->ctrTxPort=port_ctrTx;
	COM->ctrTxPin=pin_ctrTx;
	//ÉèÖÃÊ±ÖÓµÈ
	if(COM->ctrTxPort==GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else 	if(COM->ctrTxPort==GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	else 	if(COM->ctrTxPort==GPIOC)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	else 	if(COM->ctrTxPort==GPIOD)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	else 	if(COM->ctrTxPort==GPIOE)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	else 	if(COM->ctrTxPort==GPIOF)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	else 	if(COM->ctrTxPort==GPIOG)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	STM32_GpioOneInit(COM->ctrTxPin, GPIO_Speed_50MHz,GPIO_Mode_Out_PP,COM->ctrTxPort);

	//GPIO_ResetBits(COM->ctrTxPort, COM->ctrTxPin);
}

/**
  * ¹¦  ÄÜ£º»ñÈ¡´®¿ÚµÄ¶Ë¿ÚºÅ
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º¶Ë¿ÚºÅ
  */
int SerialPort::getPort()
{
	return port;
}

/**
  * ¹¦  ÄÜ£ºÅĞ¶¨´®¿Ú½ÓÊÕ»º³åÇøÊÇ·ñÓĞÊı¾İ
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º
	*			true:ÓĞÊı¾İ
  * 		false:ÎŞÊı¾İ
  */
bool SerialPort::readAllow()
{
		return COM->USART_RXBufferData_Available();
}

/**
  * ¹¦  ÄÜ£º´Ó´®¿Ú»º³åÇøÖĞ¶ÁÈ¡Ò»¸ö×Ö½Ú
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£º¶ÁÈ¡µÄÖµ
  */
uint8_t SerialPort::getChar(void)
{
	return COM->USART_RXBuffer_GetByte();
}

/**
  * ¹¦  ÄÜ£ºĞ´Ò»¸öÊı¾İµ½´®¿Ú·¢ËÍ»º³åÇø
  * ²Î  Êı£º´ıĞ´µÄÊı¾İ
  * ·µ»ØÖµ£º
	*			true:Ğ´Èë³É¹¦
	*			false:Ğ´ÈëÊ§°Ü
  */
bool SerialPort::putChar(uint8_t data)
{
	return COM->USART_TXBuffer_PutByte(data);
}

/**
  * ¹¦  ÄÜ£ºĞ´Ò»´®Êı¾İµ½´®¿Ú»º³åÇø
  * ²Î  Êı£º
	*			data£ºÊı×éµÄÖ¸Õë
	*			maxSize£º´ıĞ´µÄ³¤¶È
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::write(uint8_t * data, uint8_t maxSize )
{
	int i;
	for(i=0;i<maxSize;i++)
		while(!putChar(data[i]));
}

/**
  * ¹¦  ÄÜ£ºĞ´Ò»¸ö×Ö·û´®µ½´®¿Ú»º³åÇø
  * ²Î  Êı£º
	*			data£º×Ö·û´®Ö¸Õë
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::writeLine(const char* data)
{
	while(*data)
	{
		while(!putChar(*data));
		data++;
	}
}

/**
  * ¹¦  ÄÜ£º´ò¿ª´®¿Ú
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::open()						  //´ò¿ª´®¿Ú
{
	if(COM->usart==NULL)
		return;
	USART_Cmd(COM->usart, ENABLE);
}

/**
  * ¹¦  ÄÜ£º¹Ø±Õ´®¿Ú
  * ²Î  Êı£ºÎŞ
  * ·µ»ØÖµ£ºÎŞ
  */
void SerialPort::close()							//¹Ø±Õ´®¿Ú
{
	if(COM->usart==NULL)
		return;
	USART_Cmd(COM->usart, DISABLE);
}




//ÒÔÏÂÊÇ´®¿ÚÖĞ¶Ïº¯ÊıµÄÊµÏÖ
/**************************************×¢ÒâÊÂÏî********************************************

*´®¿Ú3¡¢4¡¢5Ö»Ö§³ÖÒ»Ğ©ÌØ¶¨µÄµ¥Æ¬»úÀàĞÍ£¬ÒÔÏÂÓÃºê¿ØÖÆ¶¨ÒåÁËÖĞ¶Ïº¯ÊıµÄÓĞÎŞ
*ÁíÍâ£¬Æô¶¯ÎÄ¼ş×îºÃÊ¹ÓÃMDK×Ô¶¯Éú³ÉµÄ£¬¹¤³ÌÖĞµÄÅäÖÃTarget Options->c/c++->Define
*ÊäÈë¿òÖĞÌîĞ´ÏàÓ¦µÄCPUÀàĞÍºê£¬·ñÔò»á³öÏÖÖĞ¶Ï½ø²»È¥µÄÇé¿ö
*´®¿Ú·¢ËÍÍê³ÉµÄÖĞ¶Ï²»±ØÒª´ò¿ª£¬¸ÃÖĞ¶ÏÔÚ¶à»º´æ·¢ËÍÊ±²ÅÓĞ±ØÒªÊ¹ÓÃ
*USART_IT_TXE±êÖ¾ÇåÁãÊÇÓ²¼ş×Ô¶¯ÇåÁãµÄ£¬ÀÏ³ÌĞòÓĞ´íÎó£¬´ò¿ª¶ÏÑÔ£¬USART_ClearITPendingBit(USART3,USART_IT_TXE)£¬Í¨²»¹ı
********************************************************************************************/

extern "C" {
	
/**************************** USART1 IRQHandler ********************************/
#if defined(STM32F10X_LD)\
		||defined(STM32F10X_LD_VL)\
		||defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
	
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
  	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    /* Read one byte from the receive data register */
		// 		USART_data_1.disabledTX();
		USART_data_1.USART_RXComplete();

  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  { 
//   	USART_ClearITPendingBit(USART1,USART_IT_TXE);  
    /* Write one byte to the transmit data register */
		USART_data_1.USART_DataRegEmpty();
  }                             

  if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  { 
  	USART_ClearITPendingBit(USART1,USART_IT_TC); 
		USART_data_1.disabledTX();
  }
}

/**************************** USART2 IRQHandler ********************************/
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
  	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    /* Read one byte from the receive data register */
		USART_data_2.USART_RXComplete();
  
  }
  
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {   
//   	USART_ClearITPendingBit(USART2,USART_IT_TXE);
	/* Write one byte to the transmit data register */
		USART_data_2.USART_DataRegEmpty(); 
  }

   if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  { 
  	USART_ClearITPendingBit(USART2,USART_IT_TC);  
		USART_data_2.disabledTX();
  }
}
#endif

#if defined(STM32F10X_MD)\
		||defined(STM32F10X_MD_VL)\
		||defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)
/**************************** USART3 IRQHandler ********************************/
void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
  	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    /* Read one byte from the receive data register */
		USART_data_3.USART_RXComplete();
  
  }
  
  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  { 
//   	USART_ClearITPendingBit(USART3,USART_IT_TXE);  
    /* Write one byte to the transmit data register */
		USART_data_3.USART_DataRegEmpty();
   
  }

   if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  { 
  	USART_ClearITPendingBit(USART3,USART_IT_TC);  
		USART_data_3.disabledTX();
  }
}
#endif


#if defined(STM32F10X_HD)\
		||defined(STM32F10X_HD_VL)\
		||defined(STM32F10X_XL)\
		||defined(STM32F10X_CL)


/**************************** USART4 IRQHandler ********************************************/
void UART4_IRQHandler(void)
{
	
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
  {
		
  	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    /* Read one byte from the receive data register */
		USART_data_4.USART_RXComplete();
		
  }
  
  if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
  {
//   	USART_ClearITPendingBit(UART4,USART_IT_TXE); 
    /* Write one byte to the transmit data register */
 		USART_data_4.USART_DataRegEmpty();
   
  }

   if(USART_GetITStatus(UART4, USART_IT_TC) != RESET)
  { 

  	USART_ClearITPendingBit(UART4,USART_IT_TC);  
		USART_data_4.disabledTX();
	}
}


/**************************** USART5 IRQHandler ********************************/
void UART5_IRQHandler(void)
{
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  {
  	USART_ClearITPendingBit(UART5, USART_IT_RXNE);
    /* Read one byte from the receive data register */
		USART_data_5.USART_RXComplete();
  
  }
  
  if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
  {  
//   	USART_ClearITPendingBit(UART5,USART_IT_TXE); 
    /* Write one byte to the transmit data register */
		USART_data_5.USART_DataRegEmpty();
   
  }

  if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)
  { 
  	USART_ClearITPendingBit(UART5,USART_IT_TC);  
		USART_data_5.disabledTX();
  }
}
#endif

}//end of extern "C"



