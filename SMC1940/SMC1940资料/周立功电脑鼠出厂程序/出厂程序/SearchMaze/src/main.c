/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               main.c
** Last modified Date:      2008/02/14
** Last Version:            V1.00
** Description:             MicroMouse615上的无记忆功能的走迷宫实验
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              廖茂刚
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
  包含头文件
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
  PB端口定义
*********************************************************************************************************/
#define IRSEND_BEVEL       GPIO_PIN_0                                   /*  驱动斜角红外的调制信号      */

#define LEFTSIDE           GPIO_PIN_1                                   /*  左方传感器输出的信号        */
#define FRONTSIDE_L        GPIO_PIN_2                                   /*  左前方传感器输出的信号      */
#define FRONTSIDE          GPIO_PIN_3                                   /*  前方传感器输出的信号        */
#define FRONTSIDE_R        GPIO_PIN_4                                   /*  右前方传感器输出的信号      */
#define RIGHTSIDE          GPIO_PIN_5                                   /*  右方传感器输出的信号        */


/*********************************************************************************************************
  PC端口定义
*********************************************************************************************************/
#define  KEY               GPIO_PIN_4                                   /*  按键连接的端口              */


/*********************************************************************************************************
  PD端口定义
*********************************************************************************************************/
#define  PHRA1             GPIO_PIN_0                                   /*  右侧步进电机的A1相          */
#define  PHRA2             GPIO_PIN_1                                   /*  右侧步进电机的A2相          */
#define  PHRB1             GPIO_PIN_2                                   /*  右侧步进电机的B1相          */
#define  PHRB2             GPIO_PIN_3                                   /*  右侧步进电机的B2相          */
#define  PHLA1             GPIO_PIN_4                                   /*  左侧步进电机的A1相          */
#define  PHLA2             GPIO_PIN_5                                   /*  左侧步进电机的A2相          */
#define  PHLB1             GPIO_PIN_6                                   /*  左侧步进电机的B1相          */
#define  PHLB2             GPIO_PIN_7                                   /*  左侧步进电机的B2相          */


/*********************************************************************************************************
  PE端口定义
*********************************************************************************************************/
#define IRSEND_SIDE        GPIO_PIN_0                                   /*  驱动左前右正向红外发射      */


/*********************************************************************************************************
  常量宏定义 -- 迷宫类型
*********************************************************************************************************/
#define MAZETYPE           8                                            /*  8: 四分之一迷宫；16: 全迷宫 */


/*********************************************************************************************************
  常量宏定义--前进一个迷宫格步进电机需要走的步数
*********************************************************************************************************/
#define ONEBLOCK           125


/*********************************************************************************************************
  常量宏定义--电脑鼠状态
*********************************************************************************************************/
#define STOP               0                                            /*  电脑鼠停止                  */
#define GOAHEAD            1                                            /*  电脑鼠前进                  */
#define TURNLEFT           3                                            /*  电脑鼠向左转                */
#define TURNRIGHT          4                                            /*  电脑鼠向右转                */
#define TURNBACK           5                                            /*  电脑鼠向后转                */


/*********************************************************************************************************
  常量宏定义--传感器
*********************************************************************************************************/
#define LEFT               0                                            /*  左方传感器                  */
#define FRONTL             1                                            /*  左前方传感器                */
#define FRONT              2                                            /*  前方传感器                  */
#define FRONTR             3                                            /*  右前方传感器                */
#define RIGHT              4                                            /*  右方传感器                  */


/*********************************************************************************************************
  常量宏定义--电机状态
*********************************************************************************************************/
#define MOTORSTOP          0                                            /*  电机停止                    */
#define WAITONESTEP        1                                            /*  电机暂停一步                */
#define MOTORRUN           2                                            /*  电机运行                    */


/*********************************************************************************************************
  常量宏定义--电机运行方向
*********************************************************************************************************/
#define MOTORGOAHEAD       0                                            /*  电机前进                    */
#define MOTORGOBACK        1                                            /*  电机后退                    */


/*********************************************************************************************************
  结构体定义
*********************************************************************************************************/
struct motor {
    int8    cState;                                                     /*  电机运行状态                */
    int8    cDir;                                                       /*  电机运行方向                */
    uint32  uiPulse;                                                    /*  电机需要转动的步数          */
    uint32  uiPulseCtr;                                                 /*  电机已转动的步数            */
    int32   iSpeed;                                                     /*  电机转动速度                */
};
typedef struct motor MOTOR;


/*********************************************************************************************************
  定义全局变量
*********************************************************************************************************/
static MOTOR   GmRight  = {MOTORSTOP, MOTORGOAHEAD, 0, 0, 0};           /*  定义并初始化右电机状态      */
static MOTOR   GmLeft   = {MOTORSTOP, MOTORGOAHEAD, 0, 0, 0};           /*  定义并初始化左电机状态      */
                                                                        
static uint8   GucMouseState       = STOP;                              /*  保存电脑鼠当前运行状态      */
static uint32  GuiAccelTable[300]  = {0};                               /*  电机加减速各阶段定时器值    */
static int32   GiMaxSpeed          = 70;                                /*  保存允许运行的最大速度      */
static uint8   GucDistance[5]      = {0};                               /*  记录传感器状态              */


/*********************************************************************************************************
** Function name:       delay
** Descriptions:        延时函数
** input parameters:    uiD :延时参数，值越大，延时越久
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void delay (uint32  uiD)
{
    for (; uiD; uiD--);
}


/*********************************************************************************************************
** Function name:       rightMotorContr
** Descriptions:        右步进电机驱动时序
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void rightMotorContr (void)
{
    static int8 cStep = 0;                                              /*  保存电机当前位置            */
    
    switch (GmRight.cDir) {

    case MOTORGOAHEAD:                                                  /*  向前步进                    */
        cStep = (cStep + 1) % 8;
        break;

    case MOTORGOBACK:                                                   /*  向后步进                    */
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
** Descriptions:        左步进电机驱动时序
** input parameters:    GmLeft.cDir :电机运行方向
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void leftMotorContr (void)
{
    static int8 cStep = 0;                                              /*  保存电机当前位置            */
    
    switch (GmLeft.cDir) {
        
    case MOTORGOAHEAD:                                                  /*  向前步进                    */
        cStep = (cStep + 1) % 8;
        break;
        
    case MOTORGOBACK:                                                   /*  向后步进                    */
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
** Descriptions:        右电机速度调节
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void speedContrR (void)
{
    int32 iDPusle;
    
    iDPusle = GmRight.uiPulse - GmRight.uiPulseCtr;                     /*  统计电机还剩余的步数        */
    if (iDPusle <= GmRight.iSpeed) {
        GmRight.iSpeed--;
    } else {                                                            /*  非减速区间，则加速到最大值  */
        if (GmRight.iSpeed < GiMaxSpeed) {
            GmRight.iSpeed++;
        } else {
            GmRight.iSpeed--;
        }
    }
    if (GmRight.iSpeed < 0) {                                           /*  设置速度下限                */
        GmRight.iSpeed = 0;
    }
    TimerLoadSet(TIMER0_BASE, TIMER_A, GuiAccelTable[GmRight.iSpeed]);  /*  设置定时时间                */
}


/*********************************************************************************************************
** Function name:       speedContrL
** Descriptions:        左电机速度调节
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void speedContrL (void)
{
    int32 iDPusle;
    
    iDPusle = GmLeft.uiPulse - GmLeft.uiPulseCtr;                       /*  统计电机还剩余的步数        */
    if (iDPusle <= GmLeft.iSpeed) {
        GmLeft.iSpeed--;
    } else {                                                            /*  非减速区间，则加速到最大值  */
        if (GmLeft.iSpeed < GiMaxSpeed) {
            GmLeft.iSpeed++;
        }
    }
    if (GmLeft.iSpeed < 0) {                                            /*  设置速度下限                */
        GmLeft.iSpeed = 0;
    }
    TimerLoadSet(TIMER1_BASE,TIMER_A,GuiAccelTable[GmLeft.iSpeed]);     /*  设置定时时间                */
}


/*********************************************************************************************************
** Function name:       Timer0A_ISR
** Descriptions:        Timer0中断服务函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Timer0A_ISR(void)
{
    static int8 n = 0,m = 0;
    
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                     /*  清除定时器0中断。           */
    switch (GmRight.cState) {
        
    case MOTORSTOP:                                                     /*  停止，同时清零速度和脉冲值  */
        GmRight.iSpeed     = 0;
        GmRight.uiPulse    = 0;
        GmRight.uiPulseCtr = 0;
        break;

    case WAITONESTEP:                                                   /*  暂停一步                    */
        GmRight.cState     = MOTORRUN;
        break;

    case MOTORRUN:                                                      /*  电机运行                    */
        if (GucMouseState == GOAHEAD) {                                 /*  根据传感器状态微调电机位置  */
            if (GucDistance[FRONTL] && (GucDistance[FRONTR] == 0)) {    /*  左前方有障碍，且前方无挡板  */
                if (n == 1) {
                    GmRight.cState = WAITONESTEP;                       /*  下一次电机暂停              */
                }
                n++;
                n %= 2;
            } else {
                n = 0;
            }        
            if ((GucDistance[RIGHT] == 1) && (GucDistance[LEFT] == 0)) {/*  离右挡板太远，且左方无挡板  */
                if(m == 3) {
                    GmRight.cState = WAITONESTEP;                       /*  下一次电机暂停              */
                }
                m++;
                m %= 6;
            } else {
                m  = 0;
            }
        }
        rightMotorContr();                                              /*  推动电机转动一步            */
        break;

    default:
        break;
    }
    /*
     *  是否完成任务判断
     */
    if (GmRight.cState != MOTORSTOP) {
        GmRight.uiPulseCtr++;                                           /*  运行脉冲计数                */
        speedContrR();                                                  /*  速度调节                    */
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
** Descriptions:        Timer1中断服务函数
** input parameters:    GmLeft.cState :驱动步进电机的时序状态
**                      GmLeft.cDir   :步进电机运动的方向
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Timer1A_ISR(void)
{
    static int8 n = 0, m = 0;
    
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);                     /*  清除定时器1中断。           */
    switch (GmLeft.cState) {
        
    case MOTORSTOP:                                                     /*  停止，同时清零速度和脉冲值  */
        GmLeft.iSpeed     = 0;
        GmLeft.uiPulse    = 0;
        GmLeft.uiPulseCtr = 0;
        break;
        
    case WAITONESTEP:                                                   /*  暂停一步                    */
        GmLeft.cState     = MOTORRUN;
        break;

    case MOTORRUN:                                                      /*  电机运行                    */
        if (GucMouseState == GOAHEAD) {                                 /*  根据传感器状态微调电机位置  */
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
        leftMotorContr();                                               /*  电机驱动程序                */
        break;

    default:
        break;
    }
    /*
     *  是否完成任务判断
     */
    if (GmLeft.cState != MOTORSTOP) {
        GmLeft.uiPulseCtr++;                                            /*  运行脉冲计数                */
        speedContrL();                                                  /*  速度调节                    */
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
** Descriptions:        发送红外线。
** input parameters:    uiFreq:  红外线调制频率
**                      cNumber: 选择需要设置的PWM模块
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void irSendFreq (uint32  uiFreq, int8  cNumber)
{
    uiFreq = SysCtlClockGet() / uiFreq;
    switch (cNumber) {

    case 1:
        PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, uiFreq);                   /*  设置PWM发生器1的周期        */
        PWMPulseWidthSet(PWM_BASE, PWM_OUT_2, uiFreq / 2);              /*  设置PWM2输出的脉冲宽度      */
        PWMGenEnable(PWM_BASE, PWM_GEN_1);                              /*  使能PWM发生器1              */
        break;

    case 2:
        PWMGenPeriodSet(PWM_BASE, PWM_GEN_2, uiFreq);                   /*  设置PWM发生器2的周期        */
        PWMPulseWidthSet(PWM_BASE, PWM_OUT_4, uiFreq / 2);              /*  设置PWM4输出的脉冲宽度      */
        PWMGenEnable(PWM_BASE, PWM_GEN_2);                              /*  使能PWM发生器2              */
        break;

    default:
        break;
    }
}


/*********************************************************************************************************
** Function name:       irCheck
** Descriptions:        红外线传感器检测。
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void irCheck (void)
{
    static uint8 ucState = 0;
    static uint8 ucIRCheck;
    
    switch (ucState) {

    case 0:
        irSendFreq(32200, 2);                                           /*  探测左右两侧近距            */
        irSendFreq(35000, 1);                                           /*  驱动斜角上的传感器检测      */
        break;
        
    case 1:
        ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x3e);                 /*  读取传感器状态              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  禁止PWM发生器2              */
        PWMGenDisable(PWM_BASE, PWM_GEN_1);                             /*  禁止PWM发生器1              */
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
        irSendFreq(36000, 2);                                           /*  驱动检测左前右三个方向远距  */
        break;
        
    case 3:
        ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                 /*  读取传感器状态              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  禁止PWM发生器2              */
        break;

    case 4:
        irSendFreq(36000, 2);                                           /*  重复检测左前右三个方向远距  */
        break;
        
    case 5:
        ucIRCheck &= GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                /*  读取传感器状态              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  禁止PWM发生器2              */
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
    ucState = (ucState + 1) % 6;                                        /*  循环检测                    */
}


/*********************************************************************************************************
** Function name:       mazeSearch
** Descriptions:        迷宫搜索，若发现分支路或者前方有挡板则退出
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mazeSearch(void)
{
    int8 cL = 0, cR = 0;
    /*
     *  设定运行任务
     */
    GucMouseState   = GOAHEAD;
    GmRight.cDir    = MOTORGOAHEAD;
    GmLeft.cDir     = MOTORGOAHEAD;
    GmRight.uiPulse = MAZETYPE * ONEBLOCK;
    GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
    GmRight.cState  = MOTORRUN;
    GmLeft.cState   = MOTORRUN;
    
    while (GmLeft.cState != MOTORSTOP) {
        if (GucDistance[FRONT]) {                                       /*  前方有墙                    */
            /*
             *  设置继续前进的步数，以实现能停止在单元格中心的目的
             */
            GmRight.uiPulse = GmRight.uiPulseCtr + 70;
            GmLeft.uiPulse  = GmLeft.uiPulseCtr  + 70;
            /*
             *  反复检测前方传感器的状态，以消除误判
             */
            while (GucDistance[FRONT]) {
                if ((GmLeft.uiPulseCtr + 50) > GmLeft.uiPulse) {
                    goto End;                                           /*  跳出循环，跳到程序结束处    */
                }
            }
            /*
             *  程序执行到此步表明出现误判，则重新设定电机运行任务
             */             

            GmRight.uiPulse = MAZETYPE * ONEBLOCK;
            GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
        }
        if (cL) {                                                       /*  是否允许检测左边            */
            if ((GucDistance[LEFT] & 0x01) == 0) {                      /*  左边有支路                  */
                /*
                 *  设置继续前进的步数，以实现能停止在单元格中心的目的
                 */
                GmRight.uiPulse = GmRight.uiPulseCtr + 74;
                GmLeft.uiPulse  = GmLeft.uiPulseCtr  + 74;
                /*
                 *  反复检测前方传感器的状态，以消除误判
                 */
                while ((GucDistance[LEFT] & 0x01) == 0) {
                    if ((GmLeft.uiPulseCtr + 50) > GmLeft.uiPulse) {
                        goto End;                                       /*  跳出循环，跳到程序结束处    */
                    }
                }
                /*
                 *  程序执行到此步表明出现误判，则重新设定电机运行任务
                 */
                GmRight.uiPulse = MAZETYPE * ONEBLOCK;
                GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
            }
        } else {                                                        /*  左边有墙时开始允许检测左边  */
            if ( GucDistance[LEFT] & 0x01) {
                cL = 1;
            }
        }
        if (cR) {                                                       /*  是否允许检测右边            */
            if ((GucDistance[RIGHT] & 0x01) == 0) {                     /*  右边有支路                  */
                /*
                 *  设置继续前进的步数，以实现能停止在单元格中心的目的
                 */
                GmRight.uiPulse = GmRight.uiPulseCtr + 74;
                GmLeft.uiPulse  = GmLeft.uiPulseCtr  + 74;
                /*
                 *  反复检测前方传感器的状态，以消除误判
                 */
                while ((GucDistance[ RIGHT] & 0x01) == 0) {
                    if ((GmLeft.uiPulseCtr + 50) > GmLeft.uiPulse) {
                        goto End;                                       /*  跳出循环，跳到程序结束处    */
                    }
                }
                /*
                 *  程序执行到此步表明出现误判，则重新设定电机运行任务
                 */
                GmRight.uiPulse = MAZETYPE * ONEBLOCK;
                GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
            }
        } else {
            if ( GucDistance[RIGHT] & 0x01) {                           /*  右边有墙时开始允许检测右边  */
                cR = 1;
            }
        }
    }
End:;
}


/*********************************************************************************************************
** Function name:       mouseTurnright
** Descriptions:        右转
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mouseTurnright(void)
{
    /*
     *  等待停止
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
    
    GucMouseState   = TURNRIGHT;                                        /*  标记电脑鼠的运行状态        */
    /*
     *  开始右转
     */
    GmRight.cDir    = MOTORGOBACK;                                      /*  控制右轮向后转动            */
    GmRight.uiPulse = 41;                                               /*  设定右轮转动的步数          */
    
    GmLeft.cDir     = MOTORGOAHEAD;                                     /*  控制左轮向前转动            */
    GmLeft.uiPulse  = 41;                                               /*  设定左轮转动的步数          */
    
    GmRight.cState  = MOTORRUN;                                         /*  使能右轮转动                */
    GmLeft.cState   = MOTORRUN;                                         /*  使能左轮转动                */
    /*
     *  等待右转完成
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
}


/*********************************************************************************************************
** Function name:       mouseTurnleft
** Descriptions:        左转
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mouseTurnleft(void)
{
    /*
     *  等待停止
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
    
    GucMouseState   = TURNLEFT;                                         /*  标记电脑鼠的运行状态        */
    /*
     *  开始左转
     */
    GmRight.cDir    = MOTORGOAHEAD;                                     /*  控制右轮向前转动            */
    GmRight.uiPulse = 41;                                               /*  设定右轮转动的步数          */
    
    GmLeft.cDir     = MOTORGOBACK;                                      /*  控制左轮向后转动            */
    GmLeft.uiPulse  = 41;                                               /*  设定左轮转动的步数          */
    
    GmRight.cState  = MOTORRUN;                                         /*  使能右轮转动                */
    GmLeft.cState   = MOTORRUN;                                         /*  使能左轮转动                */
    /*
     *  等待左转完成
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
}


/*********************************************************************************************************
** Function name:       mouseTurnback
** Descriptions:        后转
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mouseTurnback(void)
{
    /*
     *  等待停止
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
    
    GucMouseState   = TURNBACK;
    /*
     *  开始后转
     */
    GmRight.cDir    = MOTORGOBACK;
    GmRight.uiPulse = 81;
    
    GmLeft.cDir     = MOTORGOAHEAD;
    GmLeft.uiPulse  = 81;
    
    GmLeft.cState   = MOTORRUN;
    GmRight.cState  = MOTORRUN;
    /*
     *  等待后转完成
     */
    while (GmLeft.cState  != MOTORSTOP);
    while (GmRight.cState != MOTORSTOP);
}


/*********************************************************************************************************
** Function name:       keyCheck
** Descriptions:        读取按键
** input parameters:    无
** output parameters:   无
** Returned value:      true:  按键已按下
**                      false: 按键未按下
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
** Descriptions:        定时中断扫描。
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SysTick_ISR(void)
{
    static int32 iL = 0, iR = 0;
    
    /*
     *  如果左电机长时间停止，则断电
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
     *  如果右电机长时间停止，则断电
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
     *  红外线检测
     */
    irCheck();
}


/*********************************************************************************************************
** Function name:       wallCheck
** Descriptions:        根据传感器检测结果判断是否存在墙壁
** input parameters:    无
** output parameters:   无
** Returned value:      ucWall: 低三位从左到右一次代表左前右。1为有墙，0为没墙。
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
** Function name:       sensorInit
** Descriptions:        传感器控制初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void sensorInit (void)
{
    /*
     *  设置连接到传感器信号输出脚的I/O口为输入模式
     */
    GPIODirModeSet(GPIO_PORTB_BASE,
                   LEFTSIDE    |
                   FRONTSIDE_L |
                   FRONTSIDE   |
                   FRONTSIDE_R |
                   RIGHTSIDE,  
                   GPIO_DIR_MODE_IN);
    /*
     *  用PWM驱动红外线发射头产生调制的红外线信号
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);                          /*  使能PWM模块                 */
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);                                 /*  PWM时钟配置：不分频         */
    /*
     *  初始化PWM2，该PWM驱动斜角红外发射头
     */
    GPIOPinTypePWM(GPIO_PORTB_BASE, IRSEND_BEVEL);                      /*  PB0配置为PWM功能            */
    PWMGenConfigure(PWM_BASE, PWM_GEN_1,                                /*  配置PWM发生器1              */
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);       /*  加计数，立即更新            */

    PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true);                      /*  使能PWM2输出                */
    PWMGenDisable(PWM_BASE, PWM_GEN_1);                                 /*  禁止PWM发生器1              */
    /*
     *  初始化PWM4，该PWM驱动左前右正方向红外发射头
     */
    GPIOPinTypePWM(GPIO_PORTE_BASE, IRSEND_SIDE);                       /*  PE0配置为PWM功能            */
    PWMGenConfigure(PWM_BASE, PWM_GEN_2,                                /*  配置PWM发生器2              */
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);       /*  加计数，立即更新            */

    PWMOutputState(PWM_BASE, PWM_OUT_4_BIT, true);                      /*  使能PWM4输出                */
    PWMGenDisable(PWM_BASE, PWM_GEN_2);                                 /*  禁止PWM发生器2              */
}


/*********************************************************************************************************
** Function name:       stepMotorIint
** Descriptions:        步进电机控制初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void stepMotorIint (void)
{
    uint16 n = 0;
    /*
     *  设置驱动步进电机的八个I/O口为输出模式
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
     *  对左右电机转动的位置初始化
     */
    GPIOPinWrite(GPIO_PORTD_BASE,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2);
    
    GPIOPinWrite(GPIO_PORTD_BASE,
                 PHLA1 | PHLA2 | PHLB1 | PHLB2,
                 PHLA1 | PHLA2 | PHLB1 | PHLB2);
    /*
     *  初始化加速/减速时定时器加载值的数据表
     */
    GuiAccelTable[0] = 2236068;
    GuiAccelTable[1] = 926179;
    for(n = 2; n < 300; n++) {
        GuiAccelTable[n] = GuiAccelTable[n - 1] - (2 * GuiAccelTable[n - 1] / (4 * n + 1));
    }
    /*
     *  初始化定时器0，用来控制右电机的转速
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);                       /*  使能定时器0模块             */
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);                  /*  配置为32位周期计数模式      */
    TimerLoadSet(TIMER0_BASE, TIMER_A, GuiAccelTable[0]);               /*  设置定时时间                */
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                    /*  设置为溢出中断              */

    IntEnable(INT_TIMER0A);                                             /*  使能定时器0中断             */
    TimerEnable(TIMER0_BASE, TIMER_A);                                  /*  使能定时器0                 */
    
    /*
     *  初始化定时器1，用来控制电机的转速
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);                       /*  使能定时器1模块             */
    TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);                  /*  配置为32位周期计数模式      */
    TimerLoadSet(TIMER1_BASE, TIMER_A, GuiAccelTable[0]);               /*  设置定时时间                */
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);                    /*  设置为溢出中断              */

    IntEnable(INT_TIMER1A);                                             /*  使能定时器1中断             */
    TimerEnable(TIMER1_BASE, TIMER_A);                                  /*  使能定时器1                 */
}


/*********************************************************************************************************
** Function name:       keyInit
** Descriptions:        对连接按键的GPIO口初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void keyInit (void)
{
    GPIODirModeSet(GPIO_PORTC_BASE, KEY, GPIO_DIR_MODE_IN);             /*  设置按键口为输入            */
}


/*********************************************************************************************************
** Function name:       sysTickInit
** Descriptions:        系统节拍定时器初始化。
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void sysTickInit (void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1600);                          /*  设置定时时钟为625us         */
    SysTickEnable();                                                    /*  使能系统时钟                */
    SysTickIntEnable();                                                 /*  使能系统时钟中断            */
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
** Function name:       SensorDebug
** Descriptions:        用数码管显示出传感器状态，方便调试
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        主函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
main (void)
{
    uint8 ucWall;
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_6MHZ );                                 /*  使能PLL，50M                */

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );                      /*  使能GPIO B口外设            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );                      /*  使能GPIO C口外设            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );                      /*  使能GPIO D口外设            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );                      /*  使能GPIO E口外设            */

    zlg7289Init();                                                      /*  显示模块初始化              */
    ADCInit();                                                          /*  ADC初始化                   */
    keyInit();                                                          /*  按键初始化                  */
    sysTickInit();                                                      /*  系统时钟初始化              */
    sensorInit();                                                       /*  传感器初始化                */
    stepMotorIint();                                                    /*  步进电机初始化              */

    while (keyCheck() == false) {                                       /*  等待按键                    */
        delay(20000);
        sensorDebug();                                                  /*  传感器状态显示              */
        voltageDetect();                                                /*  电池电压检测                */
    }

    zlg7289Reset();                                                     /*  复位数码显示                */
    
    while(1){
        
        ucWall = wallCheck();                                           /*  检测挡板信息                */
        
        switch (ucWall) {
            
        case 0x00:                                                      /*  三边都没有挡板，右转弯      */
            mouseTurnright();
            break;
            
        case 0x01:                                                      /*  只有右边存在挡板，不转弯    */
            break;
            
        case 0x02:                                                      /*  只有前边存在挡板，右转弯    */
            mouseTurnright();
            break;
            
        case 0x03:                                                      /*  只有左边没有挡板，左转弯    */
            mouseTurnleft();
            break;
            
        case 0x04:                                                      /*  只有左边存在挡板，右转弯    */
            mouseTurnright();
            break;
            
        case 0x05:                                                      /*  只有前边没有挡板，不转弯    */
            break;
            
        case 0x06:                                                      /*  只有右边没有挡板，右转弯    */
            mouseTurnright();
            break;
            
        case 0x07:                                                      /*  三边都存在挡板，向后转      */
            mouseTurnback();
            break;
            
        default:
            break;
        }
        mazeSearch();                                                   /*  搜索前进                    */
    }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
