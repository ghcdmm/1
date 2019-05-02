/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               main.c
** Last modified Date:      2008/01/15
** Last Version:            V1.00
** Description:             MicroMouse615上的步进电机实验
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
#include "Pwm.h"
#include "Type.h"


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


/*********************************************************************************************************
  常量宏定义--电机状态
*********************************************************************************************************/
#define  STOP              0                                            /*  电机停止                    */
#define  RUN               1                                            /*  电机运行                    */


/*********************************************************************************************************
  常量宏定义--电机运行方向
*********************************************************************************************************/
#define  GOAHEAD           0                                            /*  电机前进                    */
#define  GOBACK            1                                            /*  电机后退                    */


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
static MOTOR   GmRight             = {STOP, GOAHEAD, 0, 0, 0};          /*  定义并初始化右电机状态      */
static uint32  GuiAccelTable[300]  = {0};                               /*  电机加减速各阶段定时器值    */
static int32   GiMaxSpeed          = 200;                               /*  保存允许运行的最大速度      */
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

    case GOAHEAD:                                                       /*  向前步进                    */
        cStep = (cStep + 1) % 8;
        break;

    case GOBACK:                                                        /*  向后步进                    */
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
** Function name:       Timer0A_ISR
** Descriptions:        Timer0中断服务函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Timer0A_ISR (void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                     /*  清除定时器0中断。           */
    switch (GmRight.cState) {
        
    case STOP:                                                          /*  停止，同时清零脉冲值        */
        GmRight.uiPulse    = 0;
        GmRight.uiPulseCtr = 0;
        GmRight.iSpeed     = 0;
        break;

    case RUN:                                                           /*  电机运行                    */
        rightMotorContr();                                              /*  电机驱动程序                */
        speedContrR();
        break;

    default:
        break;
    }
    /*
     *  是否完成任务判断
     */
    if (GmRight.cState == RUN) {
        GmRight.uiPulseCtr++;                                           /*  运行脉冲计数                */
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
** Descriptions:        定时中断扫描。
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SysTick_ISR (void)
{
    static int32 iR = 0;
    /*
     *  如果右电机长时间停止，则断电
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
                   PHRB2,
                   GPIO_DIR_MODE_OUT);
    /*
     *  对左右电机转动的位置初始化
     */
    GPIOPinWrite(GPIO_PORTD_BASE,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2,
                 PHRA1 | PHRA2 | PHRB1 | PHRB2);
    
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
    SysTickPeriodSet(SysCtlClockGet() / 1000);                          /*  设置定时时钟为1ms           */
    SysTickEnable();                                                    /*  使能系统时钟                */
    SysTickIntEnable();                                                 /*  使能系统时钟中断            */
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

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );                      /*  使能GPIO C口外设            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );                      /*  使能GPIO D口外设            */
    
    keyInit();                                                          /*  按键初始化                  */
    sysTickInit();                                                      /*  系统时钟初始化              */
    stepMotorIint();                                                    /*  传感器初始化                */

    while(1){
        if (keyCheck() == true) {                                       /*  查询判断是否有按键按下      */
            GmRight.uiPulse = 2000;                                     /*  设定步进电机转动2000步      */
            GmRight.cDir    = GOAHEAD;                                  /*  设定步进电机向前转动        */
            GmRight.cState  = RUN;                                      /*  启动步进电机转动            */
        }
    }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
