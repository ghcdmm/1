
/****************************************Copyright (c)***************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:               main.c
** Last modified Date:      2008/01/06
** Last Version:            V1.0
** Description:             MicroMouse615����A/D����ص�ѹʵ�����
** 
**------------------------------------------------------------------------------------------------------
** Created By:              ��ï��
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
** Descriptions:        ��ʱ����
** input parameters:    d :��ʱ������ֵԽ����ʱԽ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void delay (unsigned long  ulD)
{
    for (; ulD; ulD--);
}


/*********************************************************************************************************
** Function name:       voltageDetect
** Descriptions:        ��ѹ��⣬�������7289 EX BOARD ����ʾ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void voltageDetect (void)
{
    unsigned long ulVoltage;
    
    ADCProcessorTrigger(ADC_BASE, 0);                                   /*  ����������һ��A/Dת��       */
    while (!ADCIntStatus(ADC_BASE, 0, false));                          /*  �ȴ�ת������                */
    ADCIntClear(ADC_BASE, 0);                                           /*  ����жϱ�׼λ              */
    ADCSequenceDataGet(ADC_BASE, 0, &ulVoltage);                        /*  ��ȡת�����                */
    
    ulVoltage = ulVoltage * 3000 / 1023;                                /*  ����ʵ�ʼ�⵽�ĵ�ѹֵ(mV)  */
    ulVoltage = ulVoltage * 3 + 350;                                    /*  �����ص�ѹֵ(mV)          */
    
    zlg7289Download(0,6,1,(ulVoltage % 10000) / 1000);                  /*  ��ʾ��ѹֵ�������֣���λV   */
    zlg7289Download(0,7,0,(ulVoltage % 1000 ) / 100 );                  /*  ��ʾ��ѹֵС�����֣���λV   */
}


/*********************************************************************************************************
** Function name:       ADCInit
** Descriptions:        �����Ӱ�����GPIO�ڳ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void ADCInit (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);                          /*  ʹ��ADCģ��                 */
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);                         /*  125KSps������               */

    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);        /*  ����0Ϊ���������������ȼ�Ϊ0*/
    ADCSequenceStepConfigure(ADC_BASE, 0, 0,
                             ADC_CTL_CH0 | 
                             ADC_CTL_IE  | 
                             ADC_CTL_END);                              /*  ���ò������з������Ĳ���    */
    
    ADCHardwareOversampleConfigure(ADC_BASE, 16);                       /*  ����ADC����ƽ�����ƼĴ���   */
    ADCSequenceEnable(ADC_BASE, 0);                                     /*  ʹ�ܲ�������0               */
}


/*********************************************************************************************************
** Function name:       main
** Descriptions:        ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
main (void)
{

    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_6MHZ );                                 /*  ʹ��PLL��50M                */

    ADCInit();                                                          /*  ADC��ʼ��                   */
    zlg7289Init();                                                      /*  zlg7289��ʼ��               */
    
    while (1) {
        voltageDetect();                                                /*  ��ص�ѹ���                */
        delay(1000000);
    }
} 
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
