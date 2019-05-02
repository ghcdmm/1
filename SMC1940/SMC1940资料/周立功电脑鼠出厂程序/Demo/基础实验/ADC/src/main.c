
/****************************************Copyright (c)***************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:               main.c
** Last modified Date:      2008/01/06
** Last Version:            V1.0
** Description:             MicroMouse615上用A/D检测电池电压实验程序
** 
**------------------------------------------------------------------------------------------------------
** Created By:              廖茂刚
** Created date:            2008/01/06
** Version:                 V1.0
** Descriptions: 
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/


#include "hw_memmap.h"
#include "hw_types.h"
#include "sysctl.h"
#include "adc.h"
#include "Zlg7289.h"
#include "Type.h"


/*********************************************************************************************************
** Function name:       delay
** Descriptions:        延时函数
** input parameters:    d :延时参数，值越大，延时越久
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void delay (unsigned long  ulD)
{
    for (; ulD; ulD--);
}


/*********************************************************************************************************
** Function name:       voltageDetect
** Descriptions:        电压检测，检测结果在7289 EX BOARD 上显示出来
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void voltageDetect (void)
{
    unsigned long ulVoltage;
    
    ADCProcessorTrigger(ADC_BASE, 0);                                   /*  处理器触发一次A/D转换       */
    while (!ADCIntStatus(ADC_BASE, 0, false));                          /*  等待转换结束                */
    ADCIntClear(ADC_BASE, 0);                                           /*  清除中断标准位              */
    ADCSequenceDataGet(ADC_BASE, 0, &ulVoltage);                        /*  读取转换结果                */
    
    ulVoltage = ulVoltage * 3000 / 1023;                                /*  计算实际检测到的电压值(mV)  */
    ulVoltage = ulVoltage * 3 + 350;                                    /*  计算电池电压值(mV)          */
    
    zlg7289Download(0,6,1,(ulVoltage % 10000) / 1000);                  /*  显示电压值整数部分，单位V   */
    zlg7289Download(0,7,0,(ulVoltage % 1000 ) / 100 );                  /*  显示电压值小数部分，单位V   */
}


/*********************************************************************************************************
** Function name:       ADCInit
** Descriptions:        对连接按键的GPIO口初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ADCInit (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);                          /*  使能ADC模块                 */
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);                         /*  125KSps采样率               */

    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);        /*  序列0为处理器触发，优先级为0*/
    ADCSequenceStepConfigure(ADC_BASE, 0, 0,
                             ADC_CTL_CH0 | 
                             ADC_CTL_IE  | 
                             ADC_CTL_END);                              /*  配置采样序列发生器的步进    */
    
    ADCHardwareOversampleConfigure(ADC_BASE, 16);                       /*  设置ADC采样平均控制寄存器   */
    ADCSequenceEnable(ADC_BASE, 0);                                     /*  使能采样序列0               */
}


/*********************************************************************************************************
** Function name:       main
** Descriptions:        主函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
main (void)
{

    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_6MHZ );                                 /*  使能PLL，50M                */

    ADCInit();                                                          /*  ADC初始化                   */
    zlg7289Init();                                                      /*  zlg7289初始化               */
    
    while (1) {
        voltageDetect();                                                /*  电池电压检测                */
        delay(1000000);
    }
} 
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
