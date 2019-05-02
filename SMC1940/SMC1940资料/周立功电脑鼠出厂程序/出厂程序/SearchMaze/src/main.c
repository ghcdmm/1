/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               main.c
** Last modified Date:      2008/02/14
** Last Version:            V1.00
** Description:             MicroMouse615�ϵ��޼��书�ܵ����Թ�ʵ��
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
#include "adc.h"
#include "Pwm.h"
#include "Type.h"
#include "Zlg7289.h"


/*********************************************************************************************************
  PB�˿ڶ���
*********************************************************************************************************/
#define IRSEND_BEVEL       GPIO_PIN_0                                   /*  ����б�Ǻ���ĵ����ź�      */

#define LEFTSIDE           GPIO_PIN_1                                   /*  �󷽴�����������ź�        */
#define FRONTSIDE_L        GPIO_PIN_2                                   /*  ��ǰ��������������ź�      */
#define FRONTSIDE          GPIO_PIN_3                                   /*  ǰ��������������ź�        */
#define FRONTSIDE_R        GPIO_PIN_4                                   /*  ��ǰ��������������ź�      */
#define RIGHTSIDE          GPIO_PIN_5                                   /*  �ҷ�������������ź�        */


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
#define  PHLA1             GPIO_PIN_4                                   /*  ��ಽ�������A1��          */
#define  PHLA2             GPIO_PIN_5                                   /*  ��ಽ�������A2��          */
#define  PHLB1             GPIO_PIN_6                                   /*  ��ಽ�������B1��          */
#define  PHLB2             GPIO_PIN_7                                   /*  ��ಽ�������B2��          */


/*********************************************************************************************************
  PE�˿ڶ���
*********************************************************************************************************/
#define IRSEND_SIDE        GPIO_PIN_0                                   /*  ������ǰ��������ⷢ��      */


/*********************************************************************************************************
  �����궨�� -- �Թ�����
*********************************************************************************************************/
#define MAZETYPE           8                                            /*  8: �ķ�֮һ�Թ���16: ȫ�Թ� */


/*********************************************************************************************************
  �����궨��--ǰ��һ���Թ��񲽽������Ҫ�ߵĲ���
*********************************************************************************************************/
#define ONEBLOCK           125


/*********************************************************************************************************
  �����궨��--������״̬
*********************************************************************************************************/
#define STOP               0                                            /*  ������ֹͣ                  */
#define GOAHEAD            1                                            /*  ������ǰ��                  */
#define TURNLEFT           3                                            /*  ����������ת                */
#define TURNRIGHT          4                                            /*  ����������ת                */
#define TURNBACK           5                                            /*  ���������ת                */


/*********************************************************************************************************
  �����궨��--������
*********************************************************************************************************/
#define LEFT               0                                            /*  �󷽴�����                  */
#define FRONTL             1                                            /*  ��ǰ��������                */
#define FRONT              2                                            /*  ǰ��������                  */
#define FRONTR             3                                            /*  ��ǰ��������                */
#define RIGHT              4                                            /*  �ҷ�������                  */


/*********************************************************************************************************
  �����궨��--���״̬
*********************************************************************************************************/
#define MOTORSTOP          0                                            /*  ���ֹͣ                    */
#define WAITONESTEP        1                                            /*  �����ͣһ��                */
#define MOTORRUN           2                                            /*  �������                    */


/*********************************************************************************************************
  �����궨��--������з���
*********************************************************************************************************/
#define MOTORGOAHEAD       0                                            /*  ���ǰ��                    */
#define MOTORGOBACK        1                                            /*  �������                    */


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
static MOTOR   GmRight  = {MOTORSTOP, MOTORGOAHEAD, 0, 0, 0};           /*  ���岢��ʼ���ҵ��״̬      */
static MOTOR   GmLeft   = {MOTORSTOP, MOTORGOAHEAD, 0, 0, 0};           /*  ���岢��ʼ������״̬      */
                                                                        
static uint8   GucMouseState       = STOP;                              /*  ���������ǰ����״̬      */
static uint32  GuiAccelTable[300]  = {0};                               /*  ����Ӽ��ٸ��׶ζ�ʱ��ֵ    */
static int32   GiMaxSpeed          = 70;                                /*  �����������е�����ٶ�      */
static uint8   GucDistance[5]      = {0};                               /*  ��¼������״̬              */


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

    case MOTORGOAHEAD:                                                  /*  ��ǰ����                    */
        cStep = (cStep + 1) % 8;
        break;

    case MOTORGOBACK:                                                   /*  ��󲽽�                    */
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
** Function name:       leftMotorContr
** Descriptions:        �󲽽��������ʱ��
** input parameters:    GmLeft.cDir :������з���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void leftMotorContr (void)
{
    static int8 cStep = 0;                                              /*  ��������ǰλ��            */
    
    switch (GmLeft.cDir) {
        
    case MOTORGOAHEAD:                                                  /*  ��ǰ����                    */
        cStep = (cStep + 1) % 8;
        break;
        
    case MOTORGOBACK:                                                   /*  ��󲽽�                    */
        cStep = (cStep + 7) % 8;
        break;
        
    default:
        break;
    }
    
    switch (cStep) {

    case 0:                                                             /*  A2B2                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2);
        break;

    case 1:                                                             /*  B2                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLB1 | PHLB2);
        break;

    case 2:                                                             /*  A1B2                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLA2 | PHLB1 | PHLB2);
        break;

    case 3:                                                             /*  A1                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLA2);
        break;

    case 4:                                                             /*  A1B1                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLA2 | PHLB2);
        break;

    case 5:                                                             /*  B1                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLB2);
        break;

    case 6:                                                             /*  A2B1                        */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLA1 | PHLA2 | PHLB2);
        break;

    case 7:                                                             /*  A2                          */
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     PHLA1 | PHLA2);
        break;

    default:
        break;
    }
}


/*********************************************************************************************************
** Function name:       speedContrR
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
** Function name:       speedContrL
** Descriptions:        �����ٶȵ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void speedContrL (void)
{
    int32 iDPusle;
    
    iDPusle = GmLeft.uiPulse - GmLeft.uiPulseCtr;                       /*  ͳ�Ƶ����ʣ��Ĳ���        */
    if (iDPusle <= GmLeft.iSpeed) {
        GmLeft.iSpeed--;
    } else {                                                            /*  �Ǽ������䣬����ٵ����ֵ  */
        if (GmLeft.iSpeed < GiMaxSpeed) {
            GmLeft.iSpeed++;
        }
    }
    if (GmLeft.iSpeed < 0) {                                            /*  �����ٶ�����                */
        GmLeft.iSpeed = 0;
    }
    TimerLoadSet(TIMER1_BASE,TIMER_A,GuiAccelTable[GmLeft.iSpeed]);     /*  ���ö�ʱʱ��                */
}


/*********************************************************************************************************
** Function name:       Timer0A_ISR
** Descriptions:        Timer0�жϷ�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void Timer0A_ISR(void)
{
    static int8 n = 0,m = 0;
    
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                     /*  �����ʱ��0�жϡ�           */
    switch (GmRight.cState) {
        
    case MOTORSTOP:                                                     /*  ֹͣ��ͬʱ�����ٶȺ�����ֵ  */
        GmRight.iSpeed     = 0;
        GmRight.uiPulse    = 0;
        GmRight.uiPulseCtr = 0;
        break;

    case WAITONESTEP:                                                   /*  ��ͣһ��                    */
        GmRight.cState     = MOTORRUN;
        break;

    case MOTORRUN:                                                      /*  �������                    */
        if (GucMouseState == GOAHEAD) {                                 /*  ���ݴ�����״̬΢�����λ��  */
            if (GucDistance[FRONTL] && (GucDistance[FRONTR] == 0)) {    /*  ��ǰ�����ϰ�����ǰ���޵���  */
                if (n == 1) {
                    GmRight.cState = WAITONESTEP;                       /*  ��һ�ε����ͣ              */
                }
                n++;
                n %= 2;
            } else {
                n = 0;
            }        
            if ((GucDistance[RIGHT] == 1) && (GucDistance[LEFT] == 0)) {/*  ���ҵ���̫Զ�������޵���  */
                if(m == 3) {
                    GmRight.cState = WAITONESTEP;                       /*  ��һ�ε����ͣ              */
                }
                m++;
                m %= 6;
            } else {
                m  = 0;
            }
        }
        rightMotorContr();                                              /*  �ƶ����ת��һ��            */
        break;

    default:
        break;
    }
    /*
     *  �Ƿ���������ж�
     */
    if (GmRight.cState != MOTORSTOP) {
        GmRight.uiPulseCtr++;                                           /*  �����������                */
        speedContrR();                                                  /*  �ٶȵ���                    */
        if (GmRight.uiPulseCtr >= GmRight.uiPulse) {
            GmRight.cState      = MOTORSTOP;
            GmRight.uiPulseCtr  = 0;
            GmRight.uiPulse     = 0;
            GmRight.iSpeed      = 0;
        }
    }
}


/*********************************************************************************************************
** Function name:       Timer1A_ISR
** Descriptions:        Timer1�жϷ�����
** input parameters:    GmLeft.cState :�������������ʱ��״̬
**                      GmLeft.cDir   :��������˶��ķ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void Timer1A_ISR(void)
{
    static int8 n = 0, m = 0;
    
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);                     /*  �����ʱ��1�жϡ�           */
    switch (GmLeft.cState) {
        
    case MOTORSTOP:                                                     /*  ֹͣ��ͬʱ�����ٶȺ�����ֵ  */
        GmLeft.iSpeed     = 0;
        GmLeft.uiPulse    = 0;
        GmLeft.uiPulseCtr = 0;
        break;
        
    case WAITONESTEP:                                                   /*  ��ͣһ��                    */
        GmLeft.cState     = MOTORRUN;
        break;

    case MOTORRUN:                                                      /*  �������                    */
        if (GucMouseState == GOAHEAD) {                                 /*  ���ݴ�����״̬΢�����λ��  */
            if (GucDistance[FRONTR] &&(GucDistance[FRONTL]==0)) {
                if (n == 1) {
                    GmLeft.cState = WAITONESTEP;
                }
                n++;
                n %= 2;
            } else {
                n = 0;
            }
            if ((GucDistance[LEFT] == 1) && (GucDistance[RIGHT] == 0)) {
                if(m == 3) {
                    GmLeft.cState = WAITONESTEP;
                }
                m++;
                m %= 6;
            } else {
                m  = 0;
            }
        }
        leftMotorContr();                                               /*  �����������                */
        break;

    default:
        break;
    }
    /*
     *  �Ƿ���������ж�
     */
    if (GmLeft.cState != MOTORSTOP) {
        GmLeft.uiPulseCtr++;                                            /*  �����������                */
        speedContrL();                                                  /*  �ٶȵ���                    */
        if (GmLeft.uiPulseCtr >= GmLeft.uiPulse) {
            GmLeft.cState      = MOTORSTOP;
            GmLeft.uiPulseCtr  = 0;
            GmLeft.uiPulse     = 0;
            GmLeft.iSpeed      = 0;
        }
    }
}


/*********************************************************************************************************
** Function name:       irSendFreq
** Descriptions:        ���ͺ����ߡ�
** input parameters:    uiFreq:  �����ߵ���Ƶ��
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
** Function name:       irCheck
** Descriptions:        �����ߴ�������⡣
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void irCheck (void)
{
    static uint8 ucState = 0;
    static uint8 ucIRCheck;
    
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
            GucDistance[RIGHT]  &= 0xfd;
        } else {
            GucDistance[RIGHT]  |= 0x02;
        }
        if (ucIRCheck & LEFTSIDE) {
            GucDistance[LEFT]   &= 0xfd;
        } else {
            GucDistance[LEFT]   |= 0x02;
        }
        if (ucIRCheck & FRONTSIDE_R) {
            GucDistance[FRONTR]  = 0x00;
        } else {
            GucDistance[FRONTR]  = 0x01;
        }
        if (ucIRCheck & FRONTSIDE_L) {
            GucDistance[FRONTL]  = 0x00;
        } else {
            GucDistance[FRONTL]  = 0x01;
        }
        break;

    case 2:
        irSendFreq(36000, 2);                                           /*  ���������ǰ����������Զ��  */
        break;
        
    case 3:
        ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                 /*  ��ȡ������״̬              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
        break;

    case 4:
        irSendFreq(36000, 2);                                           /*  �ظ������ǰ����������Զ��  */
        break;
        
    case 5:
        ucIRCheck &= GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                /*  ��ȡ������״̬              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
        if (ucIRCheck & RIGHTSIDE) {
            GucDistance[RIGHT] &= 0xfe;
        } else {
            GucDistance[RIGHT] |= 0x01;
        }
        if (ucIRCheck & LEFTSIDE) {
            GucDistance[LEFT]  &= 0xfe;
        } else {
            GucDistance[LEFT]  |= 0x01;
        }
        if (ucIRCheck & FRONTSIDE) {
            GucDistance[FRONT] &= 0xfe;
        } else {
            GucDistance[FRONT] |= 0x01;
        }
        break;

    default:
        break;
    }
    ucState = (ucState + 1) % 6;                                        /*  ѭ�����                    */
}


/*********************************************************************************************************
** Function name:       mazeSearch
** Descriptions:        �Թ������������ַ�֧·����ǰ���е������˳�
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mazeSearch(void)
{
    int8 cL = 0, cR = 0;
    /*
     *  �趨��������
     */
    GucMouseState   = GOAHEAD;
    GmRight.cDir    = MOTORGOAHEAD;
    GmLeft.cDir     = MOTORGOAHEAD;
    GmRight.uiPulse = MAZETYPE * ONEBLOCK;
    GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
    GmRight.cState  = MOTORRUN;
    GmLeft.cState   = MOTORRUN;
    
    while (GmLeft.cState != MOTORSTOP) {
        if (GucDistance[FRONT]) {                                       /*  ǰ����ǽ                    */
            /*
             *  ���ü���ǰ���Ĳ�������ʵ����ֹͣ�ڵ�Ԫ�����ĵ�Ŀ��
             */
            GmRight.uiPulse = GmRight.uiPulseCtr + 70;
            GmLeft.uiPulse  = GmLeft.uiPulseCtr  + 70;
            /*
             *  �������ǰ����������״̬������������
             */
            while (GucDistance[FRONT]) {
                if ((GmLeft.uiPulseCtr + 50) > GmLeft.uiPulse) {
                    goto End;                                           /*  ����ѭ�����������������    */
                }
            }
            /*
             *  ����ִ�е��˲������������У��������趨�����������
             */             

            GmRight.uiPulse = MAZETYPE * ONEBLOCK;
            GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
        }
        if (cL) {                                                       /*  �Ƿ����������            */
            if ((GucDistance[LEFT] & 0x01) == 0) {                      /*  �����֧·                  */
                /*
                 *  ���ü���ǰ���Ĳ�������ʵ����ֹͣ�ڵ�Ԫ�����ĵ�Ŀ��
                 */
                GmRight.uiPulse = GmRight.uiPulseCtr + 74;
                GmLeft.uiPulse  = GmLeft.uiPulseCtr  + 74;
                /*
                 *  �������ǰ����������״̬������������
                 */
                while ((GucDistance[LEFT] & 0x01) == 0) {
                    if ((GmLeft.uiPulseCtr + 50) > GmLeft.uiPulse) {
                        goto End;                                       /*  ����ѭ�����������������    */
                    }
                }
                /*
                 *  ����ִ�е��˲������������У��������趨�����������
                 */
                GmRight.uiPulse = MAZETYPE * ONEBLOCK;
                GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
            }
        } else {                                                        /*  �����ǽʱ��ʼ���������  */
            if ( GucDistance[LEFT] & 0x01) {
                cL = 1;
            }
        }
        if (cR) {                                                       /*  �Ƿ��������ұ�            */
            if ((GucDistance[RIGHT] & 0x01) == 0) {                     /*  �ұ���֧·                  */
                /*
                 *  ���ü���ǰ���Ĳ�������ʵ����ֹͣ�ڵ�Ԫ�����ĵ�Ŀ��
                 */
                GmRight.uiPulse = GmRight.uiPulseCtr + 74;
                GmLeft.uiPulse  = GmLeft.uiPulseCtr  + 74;
                /*
                 *  �������ǰ����������״̬������������
                 */
                while ((GucDistance[ RIGHT] & 0x01) == 0) {
                    if ((GmLeft.uiPulseCtr + 50) > GmLeft.uiPulse) {
                        goto End;                                       /*  ����ѭ�����������������    */
                    }
                }
                /*
                 *  ����ִ�е��˲������������У��������趨�����������
                 */
                GmRight.uiPulse = MAZETYPE * ONEBLOCK;
                GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
            }
        } else {
            if ( GucDistance[RIGHT] & 0x01) {                           /*  �ұ���ǽʱ��ʼ�������ұ�  */
                cR = 1;
            }
        }
    }
End:;
}


/*********************************************************************************************************
** Function name:       mouseTurnright
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnright(void)
{
    /*
     *  �ȴ�ֹͣ
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
    
    GucMouseState   = TURNRIGHT;                                        /*  ��ǵ����������״̬        */
    /*
     *  ��ʼ��ת
     */
    GmRight.cDir    = MOTORGOBACK;                                      /*  �����������ת��            */
    GmRight.uiPulse = 41;                                               /*  �趨����ת���Ĳ���          */
    
    GmLeft.cDir     = MOTORGOAHEAD;                                     /*  ����������ǰת��            */
    GmLeft.uiPulse  = 41;                                               /*  �趨����ת���Ĳ���          */
    
    GmRight.cState  = MOTORRUN;                                         /*  ʹ������ת��                */
    GmLeft.cState   = MOTORRUN;                                         /*  ʹ������ת��                */
    /*
     *  �ȴ���ת���
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
}


/*********************************************************************************************************
** Function name:       mouseTurnleft
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnleft(void)
{
    /*
     *  �ȴ�ֹͣ
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
    
    GucMouseState   = TURNLEFT;                                         /*  ��ǵ����������״̬        */
    /*
     *  ��ʼ��ת
     */
    GmRight.cDir    = MOTORGOAHEAD;                                     /*  ����������ǰת��            */
    GmRight.uiPulse = 41;                                               /*  �趨����ת���Ĳ���          */
    
    GmLeft.cDir     = MOTORGOBACK;                                      /*  �����������ת��            */
    GmLeft.uiPulse  = 41;                                               /*  �趨����ת���Ĳ���          */
    
    GmRight.cState  = MOTORRUN;                                         /*  ʹ������ת��                */
    GmLeft.cState   = MOTORRUN;                                         /*  ʹ������ת��                */
    /*
     *  �ȴ���ת���
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
}


/*********************************************************************************************************
** Function name:       mouseTurnback
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnback(void)
{
    /*
     *  �ȴ�ֹͣ
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
    
    GucMouseState   = TURNBACK;
    /*
     *  ��ʼ��ת
     */
    GmRight.cDir    = MOTORGOBACK;
    GmRight.uiPulse = 81;
    
    GmLeft.cDir     = MOTORGOAHEAD;
    GmLeft.uiPulse  = 81;
    
    GmLeft.cState   = MOTORRUN;
    GmRight.cState  = MOTORRUN;
    /*
     *  �ȴ���ת���
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
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
** Function name:       SysTick_ISR
** Descriptions:        ��ʱ�ж�ɨ�衣
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SysTick_ISR(void)
{
    static int32 iL = 0, iR = 0;
    
    /*
     *  ���������ʱ��ֹͣ����ϵ�
     */
    if (GmLeft.cState == MOTORSTOP) {
        iL++;
    } else {
        iL = 0;
    }
    if (iL >= 500) {
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHLA1 | PHLA2 | PHLB1 | PHLB2,
                     0x00);
    }
    /*
     *  ����ҵ����ʱ��ֹͣ����ϵ�
     */
    if (GmRight.cState == MOTORSTOP) {
        iR++;
    } else {
        iR = 0;
    }
    if (iR >= 500) {
        GPIOPinWrite(GPIO_PORTD_BASE,
                     PHRA1 | PHRA2 | PHRB1 | PHRB2,
                     0x00);
    }
    /*
     *  �����߼��
     */
    irCheck();
}


/*********************************************************************************************************
** Function name:       wallCheck
** Descriptions:        ���ݴ�����������ж��Ƿ����ǽ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ucWall: ����λ������һ�δ�����ǰ�ҡ�1Ϊ��ǽ��0Ϊûǽ��
*********************************************************************************************************/
uint8 wallCheck (void)
{
    uint8 ucWall = 0;
    
    if (GucDistance[LEFT]  & 0x01) {
        ucWall |=  0x04;
    }else {
        ucWall &= ~0x04;
    }
    if (GucDistance[FRONT] & 0x01) {
        ucWall |=  0x02;
    }else {
        ucWall &= ~0x02;
    }
    if (GucDistance[RIGHT] & 0x01) {
        ucWall |=  0x01;
    }else {
        ucWall &= ~0x01;
    }
    return(ucWall);
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
** Function name:       sensorInit
** Descriptions:        ���������Ƴ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sensorInit (void)
{
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
                   PHRB2 |
                   PHLA1 |
                   PHLA2 |
                   PHLB1 |
                   PHLB2,
                   GPIO_DIR_MODE_OUT);
    /*
     *  �����ҵ��ת����λ�ó�ʼ��
     */
    GPIOPinWrite(GPIO_PORTD_BASE,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2);
    
    GPIOPinWrite(GPIO_PORTD_BASE,
                 PHLA1 | PHLA2 | PHLB1 | PHLB2,
                 PHLA1 | PHLA2 | PHLB1 | PHLB2);
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
    
    /*
     *  ��ʼ����ʱ��1���������Ƶ����ת��
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);                       /*  ʹ�ܶ�ʱ��1ģ��             */
    TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);                  /*  ����Ϊ32λ���ڼ���ģʽ      */
    TimerLoadSet(TIMER1_BASE, TIMER_A, GuiAccelTable[0]);               /*  ���ö�ʱʱ��                */
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);                    /*  ����Ϊ����ж�              */

    IntEnable(INT_TIMER1A);                                             /*  ʹ�ܶ�ʱ��1�ж�             */
    TimerEnable(TIMER1_BASE, TIMER_A);                                  /*  ʹ�ܶ�ʱ��1                 */
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
    SysTickPeriodSet(SysCtlClockGet() / 1600);                          /*  ���ö�ʱʱ��Ϊ625us         */
    SysTickEnable();                                                    /*  ʹ��ϵͳʱ��                */
    SysTickIntEnable();                                                 /*  ʹ��ϵͳʱ���ж�            */
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
** Function name:       SensorDebug
** Descriptions:        ���������ʾ��������״̬���������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sensorDebug (void)
{
    zlg7289Download(2, 0, 0, GucDistance[LEFT  ]);
    zlg7289Download(2, 1, 0, GucDistance[FRONTL]);
    zlg7289Download(2, 2, 0, GucDistance[FRONT ]);
    zlg7289Download(2, 3, 0, GucDistance[FRONTR]);    
    zlg7289Download(2, 4, 0, GucDistance[RIGHT ]);    
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
    uint8 ucWall;
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_6MHZ );                                 /*  ʹ��PLL��50M                */

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );                      /*  ʹ��GPIO B������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );                      /*  ʹ��GPIO C������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );                      /*  ʹ��GPIO D������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );                      /*  ʹ��GPIO E������            */

    zlg7289Init();                                                      /*  ��ʾģ���ʼ��              */
    ADCInit();                                                          /*  ADC��ʼ��                   */
    keyInit();                                                          /*  ������ʼ��                  */
    sysTickInit();                                                      /*  ϵͳʱ�ӳ�ʼ��              */
    sensorInit();                                                       /*  ��������ʼ��                */
    stepMotorIint();                                                    /*  ���������ʼ��              */

    while (keyCheck() == false) {                                       /*  �ȴ�����                    */
        delay(20000);
        sensorDebug();                                                  /*  ������״̬��ʾ              */
        voltageDetect();                                                /*  ��ص�ѹ���                */
    }

    zlg7289Reset();                                                     /*  ��λ������ʾ                */
    
    while(1){
        
        ucWall = wallCheck();                                           /*  ��⵲����Ϣ                */
        
        switch (ucWall) {
            
        case 0x00:                                                      /*  ���߶�û�е��壬��ת��      */
            mouseTurnright();
            break;
            
        case 0x01:                                                      /*  ֻ���ұߴ��ڵ��壬��ת��    */
            break;
            
        case 0x02:                                                      /*  ֻ��ǰ�ߴ��ڵ��壬��ת��    */
            mouseTurnright();
            break;
            
        case 0x03:                                                      /*  ֻ�����û�е��壬��ת��    */
            mouseTurnleft();
            break;
            
        case 0x04:                                                      /*  ֻ����ߴ��ڵ��壬��ת��    */
            mouseTurnright();
            break;
            
        case 0x05:                                                      /*  ֻ��ǰ��û�е��壬��ת��    */
            break;
            
        case 0x06:                                                      /*  ֻ���ұ�û�е��壬��ת��    */
            mouseTurnright();
            break;
            
        case 0x07:                                                      /*  ���߶����ڵ��壬���ת      */
            mouseTurnback();
            break;
            
        default:
            break;
        }
        mazeSearch();                                                   /*  ����ǰ��                    */
    }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
