/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               Main.h
** Last modified Date:      2008/01/11
** Last Version:            V1.00
** Description:             MicroMouse615�ϵĺ��⴫����ʵ��
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              ��ï��
** Created date: 
** Version: 
** Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/


/*********************************************************************************************************
  ����ͷ�ļ�
*********************************************************************************************************/
#include "hw_memmap.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "interrupt.h"
#include "gpio.h"
#include "sysctl.h"
#include "Systick.h"
#include "Pwm.h"
#include "Zlg7289.h"
#include "Type.h"


/*********************************************************************************************************
  PB�˿ڶ���
*********************************************************************************************************/
#define IRSEND_BEVEL        GPIO_PIN_0                                  /*  ����б�Ǻ����PWM           */

#define LEFTSIDE            GPIO_PIN_1                                  /*  �󷽴�����������ź�        */
#define FRONTSIDE_L         GPIO_PIN_2                                  /*  ��ǰ��������������ź�      */
#define FRONTSIDE           GPIO_PIN_3                                  /*  ǰ��������������ź�        */
#define FRONTSIDE_R         GPIO_PIN_4                                  /*  ��ǰ��������������ź�      */
#define RIGHTSIDE           GPIO_PIN_5                                  /*  �ҷ�������������ź�        */


/*********************************************************************************************************
  PE�˿ڶ���
*********************************************************************************************************/
#define IRSEND_SIDE         GPIO_PIN_0                                  /*  ������ǰ����������PWM     */


/*********************************************************************************************************
** Function name:       irSendFreq
** Descriptions:        ����PWMƵ��
** input parameters:    uiFreq:  �������ز�Ƶ��
**                      cNumber: ѡ����Ҫ���õ�PWMģ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void irSendFreq (uint32  uiFreq, int8  cNumber)
{
    uiFreq = SysCtlClockGet() / uiFreq;
    switch (cNumber) {

    case 1:
        PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, uiFreq);                   /*  ����PWM������1������        */
        PWMPulseWidthSet(PWM_BASE, PWM_OUT_2, uiFreq / 2);              /*  ����PWM2�����������      */
        PWMGenEnable(PWM_BASE, PWM_GEN_1);                              /*  ʹ��PWM������1              */
        break;

    case 2:
        PWMGenPeriodSet(PWM_BASE, PWM_GEN_2, uiFreq);                   /*  ����PWM������2������        */
        PWMPulseWidthSet(PWM_BASE, PWM_OUT_4, uiFreq / 2);              /*  ����PWM4�����������      */
        PWMGenEnable(PWM_BASE, PWM_GEN_2);                              /*  ʹ��PWM������2              */
        break;

    default:
        break;
    }
}


/*********************************************************************************************************
** Function name:       SysTick_ISR
** Descriptions:        ϵͳ��ʱ���жϷ�������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SysTick_ISR (void)
{
    static uint8 ucState = 0;
    uint8 ucIRCheck;
    
    switch (ucState) {

    case 0:
        irSendFreq(32200, 2);                                           /*  ̽�������������            */
        irSendFreq(35000, 1);                                           /*  ����б���ϵĴ��������      */
        break;
        
    case 1:
        ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x3e);                 /*  ��ȡ������״̬              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
        PWMGenDisable(PWM_BASE, PWM_GEN_1);                             /*  ��ֹPWM������1              */
        if (ucIRCheck & RIGHTSIDE) {
            zlg7289SegOff(38);
        } else {
            zlg7289SegOn(38);
        }
        if (ucIRCheck & LEFTSIDE) {
            zlg7289SegOff(6);
        } else {
            zlg7289SegOn(6);
        }
        if (ucIRCheck & FRONTSIDE_R) {
            zlg7289SegOff(24);
        } else {
            zlg7289SegOn(24);
        }
        if (ucIRCheck & FRONTSIDE_L) {
            zlg7289SegOff(8);
        } else {
            zlg7289SegOn(8);
        }
        break;

    case 2:
        irSendFreq(36000, 2);                                           /*  ���������ǰ����������Զ��  */
        break;
        
    case 3:
        ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                 /*  ��ȡ������״̬              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
        if (ucIRCheck & RIGHTSIDE) {
            zlg7289SegOff(32);
        } else {
            zlg7289SegOn(32);
        }
        if (ucIRCheck & LEFTSIDE) {
            zlg7289SegOff(0);
        } else {
            zlg7289SegOn(0);
        }
        if (ucIRCheck & FRONTSIDE) {
            zlg7289SegOff(16);
        } else {
            zlg7289SegOn(16);
        }
        break;

    default:
        break;
    }
    ucState = (ucState + 1) % 4;                                        /*  ѭ�����                    */
}



/*********************************************************************************************************
** Function name:       sensorInit
** Descriptions:        ���������Ƴ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sensorInit (void)
{
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );                      /*  ʹ��GPIO B������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );                      /*  ʹ��GPIO E������            */
    /*
     *  �������ӵ��������ź�����ŵ�I/O��Ϊ����ģʽ
     */
    GPIODirModeSet(GPIO_PORTB_BASE,
                   LEFTSIDE    |
                   FRONTSIDE_L |
                   FRONTSIDE   |
                   FRONTSIDE_R |
                   RIGHTSIDE,  
                   GPIO_DIR_MODE_IN);
    /*
     *  ��PWM���������߷���ͷ�������Ƶĺ������ź�
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);                          /*  ʹ��PWMģ��                 */
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);                                 /*  PWMʱ�����ã�����Ƶ         */
    /*
     *  ��ʼ��PWM2����PWM����б�Ǻ��ⷢ��ͷ
     */
    GPIOPinTypePWM(GPIO_PORTB_BASE, IRSEND_BEVEL);                      /*  PB0����ΪPWM����            */
    PWMGenConfigure(PWM_BASE, PWM_GEN_1,                                /*  ����PWM������1              */
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);       /*  �Ӽ�������������            */

    PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true);                      /*  ʹ��PWM2���                */
    PWMGenDisable(PWM_BASE, PWM_GEN_1);                                 /*  ��ֹPWM������1              */
    /*
     *  ��ʼ��PWM4����PWM������ǰ����������ⷢ��ͷ
     */
    GPIOPinTypePWM(GPIO_PORTE_BASE, IRSEND_SIDE);                       /*  PE0����ΪPWM����            */
    PWMGenConfigure(PWM_BASE, PWM_GEN_2,                                /*  ����PWM������2              */
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);       /*  �Ӽ�������������            */

    PWMOutputState(PWM_BASE, PWM_OUT_4_BIT, true);                      /*  ʹ��PWM4���                */
    PWMGenDisable(PWM_BASE, PWM_GEN_2);                                 /*  ��ֹPWM������2              */
}
/*********************************************************************************************************
** Function name:       sysTickInit
** Descriptions:        ϵͳ���Ķ�ʱ����ʼ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sysTickInit (void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1600);                          /*  ���ö�ʱʱ��Ϊ625us         */
    SysTickEnable();                                                    /*  ʹ��ϵͳʱ��                */
    SysTickIntEnable();                                                 /*  ʹ��ϵͳʱ���ж�            */
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

    sensorInit();                                                       /*  ��������ʼ��                */
    sysTickInit();                                                      /*  ϵͳʱ�ӳ�ʼ��              */
    zlg7289Init();                                                      /*  ��ʾģ���ʼ��              */
    while(1);
}
