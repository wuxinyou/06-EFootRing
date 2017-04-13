#include "ADC.h" 

// #define ADC1_DR_Address    ((uint32_t)0x4001244C)
int16_t AD_Value[50];   //AD采样值
int16_t After_filter;   //AD滤波后 

void ADC1_GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      //启动DMA时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);     //启动ADC1时钟   
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入   
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

//ADC1配置：（两外部输入，另采样内部温度传感器）
void ADC1_Configuration(void)   
{   
    ADC_InitTypeDef ADC_InitStructure;   
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 	//转换模式为独立，还有交叉等非常多样的选择   
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;       	//扫描方式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //连续转换开启    
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发禁止
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             //数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = STM32_ADC_CHANNEL_N;     		 //设置转换序列长度为1，一通道    
    ADC_Init(ADC1, &ADC_InitStructure);                                //配置ADC
       
    ADC_TempSensorVrefintCmd(ENABLE);    //ADC内置温度传感器使能（要使用片内温度传感器，切忌要开启它）    
     //输入参数：ADC外设，ADC通道，转换序列顺序，采样时间 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);  //角度值
//     ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);  //速度值

    ADC_Cmd(ADC1, ENABLE);      // Enable ADC1   
  
    ADC_DMACmd(ADC1, ENABLE);     // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）    
      
    // 下面是ADC自动校准，开机后需执行一次，保证精度     
    ADC_ResetCalibration(ADC1);      
    while(ADC_GetResetCalibrationStatus(ADC1));    
    ADC_StartCalibration(ADC1);    
    while(ADC_GetCalibrationStatus(ADC1));
    // ADC自动校准结束---------------    
     ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ADC启动    
}  

//DMA部分：(带中断滤波)
void ADC1_DMA_Configuration(void)   
{   
    DMA_InitTypeDef DMA_InitStructure;   
      
    DMA_DeInit(DMA1_Channel1);   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));//ADC1_DR_Address;   
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AD_Value;   
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;   
    //BufferSize=2，因为ADC转换序列有2个通道    
    //如此设置，使序列1结果放在AD_Value[0]，序列2结果放在AD_Value[1]    
    DMA_InitStructure.DMA_BufferSize = STM32_ADC_CHANNEL_N*STM32_ADC_CACHEL_M;//设置DMA在传输时缓冲区的长度 word
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   								//设置DMA的内存递增模式，
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   //外设数据字长 256
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           //内存数据字长 1024
    //循环模式开启，Buffer写满后，自动回到初始地址开始传输    
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   														//设置DMA的传输模式：连续不断的循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   												//设置DMA的优先级别
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;																	//设置DMA的2个memory中的变量互相访问
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);   																//配置DMA通道
    //配置完成后，启动DMA通道    
    DMA_Cmd(DMA1_Channel1, ENABLE);																								
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); 															//使能DMA传输完成中断    
} 
void ADC1_NVIC_Configure(void) //DMA中断优先级配置
{ 
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);          // Enable the DMA Interrupt 
}
void ADC1_Init(void)
{
    ADC1_GPIO_Configure();
    ADC1_Configuration();
    ADC1_DMA_Configuration();
    ADC1_NVIC_Configure();
}
//从新使能ADC，解决数据错位的问题
void ReEnableAdc(void)
{
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

//中断接收数据
void DMA1_Channel1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
  { 
    DMA_ClearITPendingBit(DMA1_IT_TC1); //清除全部中断标志    
    DMA_ClearFlag(DMA1_FLAG_TC1); 
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    FilteAdcVaule(); 
  }
}
//滤波部分：（均值滤波） 
void FilteAdcVaule(void)
{
	int32_t sum = 0;
	uint8_t count;
	for ( count=0;count<STM32_ADC_CACHEL_M;count++)
	{
		sum += AD_Value[count];
	}
	After_filter=sum/STM32_ADC_CACHEL_M;
	sum=0;
}

int16_t getADCValue( )
{
	int16_t temData=After_filter;
	if(temData<50)
		temData=0;
	else if(temData>4049)
	{
		temData=3999;
	}
	else
	{
		temData=temData-50;
	}
	return temData;
}
 










