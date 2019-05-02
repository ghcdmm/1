#include "stm32f10x.h"
#include "usart.h"
#include "exti.h"
//#include "tim.h"
#include<stdio.h>
#include "stm32f10x_dac.h"

unsigned int light=10;
unsigned char led=0;
unsigned char direct=0;
__IO unsigned short ADC_ConvertedValue[4];

void RCC_Configuration()
{
	ErrorStatus HSEStartUpStatus; 
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);                                            //�ⲿ����
	HSEStartUpStatus=RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus==SUCCESS) 
		{
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);             //Ԥȡֵ����ʹ��
			FLASH_SetLatency(FLASH_Latency_2);                                //������ʱʱֵ
			RCC_HCLKConfig(RCC_SYSCLK_Div1);                                  //����AHBʱ��=ϵͳʱ�� AHB��Ƶ�� �õ�HCLK
			RCC_PCLK2Config(RCC_HCLK_Div1);                                   //����APB=HCLK
			RCC_PCLK1Config(RCC_HCLK_Div2);                                   //APB1=HCLK/2
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);                              //PLLƵ�ʵ����ⲿ����Ƶ��,��Ƶϵ��*9   72MHz
			RCC_PLLCmd(ENABLE);                                               //�ȴ�PLL��ʼ���ɹ�
			while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET); 
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                         //ѡ��PLL��Ƶ���ʱ����Ϊϵͳʱ��;
			while(RCC_GetSYSCLKSource()!=0x08);                                //�ȴ�PLL��Ƶ���ʱ����Ϊϵͳʱ�ӳɹ�
			}
} 

void DAC_Configuration()
{
	
	DAC_InitTypeDef	DAC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE ); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; 	         //DAC_Trigger_Software
  	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;   //??????(DAC_Wave_Triangle) ?? ???(DAC_Wave_Noise)
  	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; 	  //????????????
  	DAC_Init(DAC_Channel_1, &DAC_InitStructure); 
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);

}

void TIMinit()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period = 100; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =7200; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//	
//	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
//	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	
	TIM2->CNT = 0;
//	
//	TIM_Cmd(TIM2, ENABLE); 
}

void delay(unsigned char n)
{
	static unsigned int i=0,j=0;
	for(j=0;j<n;j++)
		for(i=0;i<26;i++);//11600
}

//void ADC_DMA_Config(void)
//{
// DMA_InitTypeDef DMA_InitStructure;
//               
// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
// DMA_DeInit(DMA1_Channel1);
// DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
// DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
// DMA_InitStructure.DMA_BufferSize = 4;
// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ����
// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
// DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
// DMA_InitStructure.DMA_Priority = DMA_Priority_High;
// DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// DMA_Init(DMA1_Channel1, &DMA_InitStructure);
// DMA_Cmd(DMA1_Channel1,ENABLE);
//}
///*********************************************************************************************************
//** Function name:       ADC1_Config
//** Descriptions:        ADC1���ã���ͨ��ת����ȡ�������ŵĵ�ѹֵ
//** input parameters:    ��
//** output parameters:   ��
//** Returned value:      ��
//*********************************************************************************************************/
//void ADC1_Config(void)
//{
// GPIO_InitTypeDef GPIO_InitStructure;
// ADC_InitTypeDef ADC_InitStructure;
//   
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
//   
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ģ����������
// GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
// ADC_DMA_Config();
//   
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//// ADC_DeInit(ADC1);
// ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1��ADC2�����ڶ���ģʽ
// ADC_InitStructure.ADC_ScanConvMode = ENABLE;//��ͨ��
// ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��
// ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������ת��
// ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת������Ҷ���
// ADC_InitStructure.ADC_NbrOfChannel = 1;//ͨ����Ŀ
// ADC_Init(ADC1, &ADC_InitStructure); 
//   
// RCC_ADCCLKConfig(RCC_PCLK2_Div6); ////����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
// 
//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_71Cycles5);//front_L
// 
// ADC_DMACmd(ADC1, ENABLE);
//   
// ADC_Cmd(ADC1, ENABLE);
// ADC_ResetCalibration(ADC1);
// while(ADC_GetResetCalibrationStatus(ADC1));
// ADC_StartCalibration(ADC1);//��ʼУ׼
// while(ADC_GetCalibrationStatus(ADC1));
// ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ����ADC1�����ת����������
//}

int main()
{
	unsigned char day=0;
	RCC_Configuration();
	DAC_Configuration();
	
//	TIMinit();
	while(1)
	{
		day=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
		if(day==1){
		if(light>4090)direct=1;
		else if(light<2000)
		{
			direct=0;
			led++;
			if(led>2)led=0;
		}
		if(direct==0)light++;
		else light--;
		if(led==0)
			DAC_SetChannel1Data(DAC_Align_12b_R,light);
		else if(led==1)
			DAC_SetChannel2Data(DAC_Align_12b_R,light);
		else if(led==2)
		{
			;
		}
		delay(40);}
	}
}




