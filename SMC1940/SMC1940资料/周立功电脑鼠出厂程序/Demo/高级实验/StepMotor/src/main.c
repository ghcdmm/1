/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               main.c
** Last modified Date:      2008/01/15
** Last Version:            V1.00
** Description:             MicroMouse615�ϵĲ������ʵ��
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
#include "Timer.h"
#include "Pwm.h"
#include "Type.h"


/*********************************************************************************************************
  PC�˿ڶ���
*********************************************************************************************************/
#define  KEY               GPIO_PIN_4                                   /*  �������ӵĶ˿�              */


/*********************************************************************************************************
  PD�˿ڶ���
*********************************************************************************************************/
#define  PHRA1             GPIO_PIN_0                                   /*  �Ҳಽ�������A1��          */
#define  PHRA2             GPIO_PIN_1                                   /*  �Ҳಽ�������A2��          */
#define  PHRB1             GPIO_PIN_2                                   /*  �Ҳಽ�������B1��          */
#define  PHRB2             GPIO_PIN_3                                   /*  �Ҳಽ�������B2��          */


/*********************************************************************************************************
  �����궨��--���״̬
*********************************************************************************************************/
#define  STOP              0                                            /*  ���ֹͣ                    */
#define  RUN               1                                            /*  �������                    */


/*********************************************************************************************************
  �����궨��--������з���
*********************************************************************************************************/
#define  GOAHEAD           0                                            /*  ���ǰ��                    */
#define  GOBACK            1                                            /*  �������                    */


/*********************************************************************************************************
  �ṹ�嶨��
*********************************************************************************************************/
struct motor {
    int8    cState;                                                     /*  �������״̬                */
    int8    cDir;                                                       /*  ������з���                */
    uint32  uiPulse;                                                    /*  �����Ҫת���Ĳ���          */
    uint32  uiPulseCtr;                                                 /*  �����ת���Ĳ���            */
    int32   iSpeed;                                                     /*  ���ת���ٶ�                */
};
typedef struct motor MOTOR;


/*********************************************************************************************************
  ����ȫ�ֱ���
*********************************************************************************************************/
static MOTOR   GmRight             = {STOP, GOAHEAD, 0, 0, 0};          /*  ���岢��ʼ���ҵ��״̬      */
static uint32  GuiAccelTable[300]  = {0};                               /*  ����Ӽ��ٸ��׶ζ�ʱ��ֵ    */
static int32   GiMaxSpeed          = 200;                               /*  �����������е�����ٶ�      */
/*********************************************************************************************************
** Function name:       delay
** Descriptions:        ��ʱ����
** input parameters:    uiD :��ʱ������ֵԽ����ʱԽ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void delay (uint32  uiD)
{
    for (; uiD; uiD--);
}


/*********************************************************************************************************
** Function name:       rightMotorContr
** Descriptions:        �Ҳ����������ʱ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void rightMotorContr (void)
{
    static int8 cStep = 0;                                              /*  ��������ǰλ��            */
    
    switch (GmRight.cDir) {

    case GOAHEAD:                                                       /*  ��ǰ����                    */
        cStep = (cStep + 1) % 8;
        break;

    case GOBACK:                                                        /*  ��󲽽�                    */
        cStep = (cStep + 7) % 8;
        break;

    default:
        break;
    }
    
    switch (cStep) {

    case 0:                                                             /*  A2B2                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2);
        break;

    case 1:                                                             /*  A2                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRA1 | PHRA2);
        break;

    case 2:                                                             /*  A2B1                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRA1 | PHRA2 | PHRB2);
        break;

    case 3:                                                             /*  B1                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRB2);
        break;

    case 4:                                                             /*  A1B1                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRA2 | PHRB2);
        break;

    case 5:                                                             /*  A1                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRA2);
        break;

    case 6:                                                             /*  A1B2                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRA2 | PHRB1 | PHRB2);
        break;

    case 7:                                                             /*  B2                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     PHRB1 | PHRB2);
        break;

    default:
        break;
    }
}


/*********************************************************************************************************
** Function name:       __speedContrR
** Descriptions:        �ҵ���ٶȵ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void speedContrR (void)
{
    int32 iDPusle;
    
    iDPusle = GmRight.uiPulse - GmRight.uiPulseCtr;                     /*  ͳ�Ƶ����ʣ��Ĳ���        */
    if (iDPusle <= GmRight.iSpeed) {
        GmRight.iSpeed--;
    } else {                                                            /*  �Ǽ������䣬����ٵ����ֵ  */
        if (GmRight.iSpeed < GiMaxSpeed) {
            GmRight.iSpeed++;
        } else {
            GmRight.iSpeed--;
        }
    }
    if (GmRight.iSpeed < 0) {                                           /*  �����ٶ�����                */
        GmRight.iSpeed = 0;
    }
    TimerLoadSet(TIMER0_BASE, TIMER_A, GuiAccelTable[GmRight.iSpeed]);  /*  ���ö�ʱʱ��                */
}


/*********************************************************************************************************
** Function name:       Timer0A_ISR
** Descriptions:        Timer0�жϷ�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void Timer0A_ISR (void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                     /*  �����ʱ��0�жϡ�           */
    switch (GmRight.cState) {
        
    case STOP:                                                          /*  ֹͣ��ͬʱ��������ֵ        */
        GmRight.uiPulse    = 0;
        GmRight.uiPulseCtr = 0;
        GmRight.iSpeed     = 0;
        break;

    case RUN:                                                           /*  �������                    */
        rightMotorContr();                                              /*  �����������                */
        speedContrR();
        break;

    default:
        break;
    }
    /*
     *  �Ƿ���������ж�
     */
    if (GmRight.cState == RUN) {
        GmRight.uiPulseCtr++;                                           /*  �����������                */
        if (GmRight.uiPulseCtr >= GmRight.uiPulse) {
            GmRight.cState      = STOP;
            GmRight.uiPulseCtr  = 0;
            GmRight.uiPulse     = 0;
            GmRight.iSpeed      = 0;
        }
    }
}


/*********************************************************************************************************
** Function name:       SysTick_ISR
** Descriptions:        ��ʱ�ж�ɨ�衣
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SysTick_ISR (void)
{
    static int32 iR = 0;
    /*
     *  ����ҵ����ʱ��ֹͣ����ϵ�
     */
    if (GmRight.cState == STOP) {
        iR++;
    } else {
        iR  = 0;
    }
    if (iR >= 300) {
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     0x00);
    }
}


/*********************************************************************************************************
** Function name:       keyCheck
** Descriptions:        ��ȡ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      true:  �����Ѱ���
**                      false: ����δ����
*********************************************************************************************************/
uint8 keyCheck (void)
{
    if (GPIOPinRead(GPIO_PORTC_BASE, KEY) == 0) {
        delay(50);
        while(GPIOPinRead(GPIO_PORTC_BASE, KEY) == 0);
        return(true);
    }else {
        return(false);
    }
}

/*********************************************************************************************************
** Function name:       stepMotorIint
** Descriptions:        ����������Ƴ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void stepMotorIint (void)
{
    uint16 n = 0;
    
    /*
     *  ����������������İ˸�I/O��Ϊ���ģʽ
     */
    GPIODirModeSet(GPIO_PORTD_BASE,
                   PHRA1 |
                   PHRA2 |
                   PHRB1 |
                   PHRB2,
                   GPIO_DIR_MODE_OUT);
    /*
     *  �����ҵ��ת����λ�ó�ʼ��
     */
    GPIOPinWrite(GPIO_PORTD_BASE,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2);
    
    /*
     *  ��ʼ������/����ʱ��ʱ������ֵ�����ݱ�
     */
    GuiAccelTable[0] = 2236068;
    GuiAccelTable[1] = 926179;
    for(n = 2; n < 300; n++) {
        GuiAccelTable[n] = GuiAccelTable[n - 1] - (2 * GuiAccelTable[n - 1] / (4 * n + 1));
    }
    /*
     *  ��ʼ����ʱ��0�����������ҵ����ת��
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);                       /*  ʹ�ܶ�ʱ��0ģ��             */
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);                  /*  ����Ϊ32λ���ڼ���ģʽ      */
    TimerLoadSet(TIMER0_BASE, TIMER_A, GuiAccelTable[0]);               /*  ���ö�ʱʱ��                */
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                    /*  ����Ϊ����ж�              */

    IntEnable(INT_TIMER0A);                                             /*  ʹ�ܶ�ʱ��0�ж�             */
    TimerEnable(TIMER0_BASE, TIMER_A);                                  /*  ʹ�ܶ�ʱ��0                 */
}


/*********************************************************************************************************
** Function name:       keyInit
** Descriptions:        �����Ӱ�����GPIO�ڳ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void keyInit (void)
{
    GPIODirModeSet(GPIO_PORTC_BASE, KEY, GPIO_DIR_MODE_IN);             /*  ���ð�����Ϊ����            */
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
    SysTickPeriodSet(SysCtlClockGet() / 1000);                          /*  ���ö�ʱʱ��Ϊ1ms           */
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

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );                      /*  ʹ��GPIO C������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );                      /*  ʹ��GPIO D������            */
    
    keyInit();                                                          /*  ������ʼ��                  */
    sysTickInit();                                                      /*  ϵͳʱ�ӳ�ʼ��              */
    stepMotorIint();                                                    /*  ��������ʼ��                */

    while(1){
        if (keyCheck() == true) {                                       /*  ��ѯ�ж��Ƿ��а�������      */
            GmRight.uiPulse = 2000;                                     /*  �趨�������ת��2000��      */
            GmRight.cDir    = GOAHEAD;                                  /*  �趨���������ǰת��        */
            GmRight.cState  = RUN;                                      /*  �����������ת��            */
        }
    }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
