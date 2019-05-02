/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               Main.h
** Last modified Date:      2008/01/11
** Last Version:            V1.00
** Description:             MicroMouse615上的红外传感器实验
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
#include "Pwm.h"
#include "Zlg7289.h"
#include "Type.h"


/*********************************************************************************************************
  PB端口定义
*********************************************************************************************************/
#define IRSEND_BEVEL        GPIO_PIN_0                                  /*  驱动斜角红外的PWM           */

#define LEFTSIDE            GPIO_PIN_1                                  /*  左方传感器输出的信号        */
#define FRONTSIDE_L         GPIO_PIN_2                                  /*  左前方传感器输出的信号      */
#define FRONTSIDE           GPIO_PIN_3                                  /*  前方传感器输出的信号        */
#define FRONTSIDE_R         GPIO_PIN_4                                  /*  右前方传感器输出的信号      */
#define RIGHTSIDE           GPIO_PIN_5                                  /*  右方传感器输出的信号        */


/*********************************************************************************************************
  PE端口定义
*********************************************************************************************************/
#define IRSEND_SIDE         GPIO_PIN_0                                  /*  驱动左前右正向红外的PWM     */


/*********************************************************************************************************
** Function name:       irSendFreq
** Descriptions:        设置PWM频率
** input parameters:    uiFreq:  红外线载波频率
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
** Function name:       SysTick_ISR
** Descriptions:        系统定时器中断服务函数。
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SysTick_ISR (void)
{
    static uint8 ucState = 0;
    uint8 ucIRCheck;
    
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
        irSendFreq(36000, 2);                                           /*  驱动检测左前右三个方向远距  */
        break;
        
    case 3:
        ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                 /*  读取传感器状态              */
        PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  禁止PWM发生器2              */
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
    ucState = (ucState + 1) % 4;                                        /*  循环检测                    */
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
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );                      /*  使能GPIO B口外设            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );                      /*  使能GPIO E口外设            */
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

    sensorInit();                                                       /*  传感器初始化                */
    sysTickInit();                                                      /*  系统时钟初始化              */
    zlg7289Init();                                                      /*  显示模块初始化              */
    while(1);
}
