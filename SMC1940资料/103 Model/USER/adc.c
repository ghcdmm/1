#include "adc.h"

__IO uint16 ADC_ConvertedValue[4];
/*********************************************************************************************************
** Function name:       ADC_DMA_Config
** Descriptions:        ADC_DMA配置，内存地址递增，内存大小：采样数*通道数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ADC_DMA_Config(void)
{
 DMA_InitTypeDef DMA_InitStructure;
               
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 DMA_DeInit(DMA1_Channel1);
 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
 DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
 DMA_InitStructure.DMA_BufferSize = 4;
 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
 DMA_InitStructure.DMA_Priority = DMA_Priority_High;
 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 DMA_Init(DMA1_Channel1, &DMA_InitStructure);
 DMA_Cmd(DMA1_Channel1,ENABLE);
}
/*********************************************************************************************************
** Function name:       ADC1_Config
** Descriptions:        ADC1配置，多通道转换读取六个引脚的电压值
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ADC1_Config(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 ADC_InitTypeDef ADC_InitStructure;
   
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
   
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入引脚
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入引脚
 GPIO_Init(GPIOB, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入引脚
 GPIO_Init(GPIOC, &GPIO_InitStructure);
	
 ADC_DMA_Config();
   
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
// ADC_DeInit(ADC1);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
 ADC_InitStructure.ADC_ScanConvMode = ENABLE;//多通道
 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件启动转换
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换结果右对齐
 ADC_InitStructure.ADC_NbrOfChannel = 4;//通道数目
 ADC_Init(ADC1, &ADC_InitStructure); 
   
 RCC_ADCCLKConfig(RCC_PCLK2_Div6); ////设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
 
ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_71Cycles5);//front_L
 //ADC_RegularChannelConfig(ADC1, ADC_Channel_15,2, ADC_SampleTime_71Cycles5);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_14,3, ADC_SampleTime_71Cycles5);//left
 ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_71Cycles5);//right
 //ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 5, ADC_SampleTime_71Cycles5);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 6, ADC_SampleTime_71Cycles5);//front_R
 
 ADC_DMACmd(ADC1, ENABLE);
   
 ADC_Cmd(ADC1, ENABLE);
 ADC_ResetCalibration(ADC1);
 while(ADC_GetResetCalibrationStatus(ADC1));
 ADC_StartCalibration(ADC1);//开始校准
 while(ADC_GetCalibrationStatus(ADC1));
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的ADC1的软件转换启动功能
}
