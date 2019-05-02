/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           maze.h
** Last modified Date:  2007/09/24
** Last Version:        V1.0
** Description:         根据底层程序取得的迷宫信息，经过该智能算法控制电脑鼠的下一状态，并送往底层驱动程
**                      序执行。
** 
**--------------------------------------------------------------------------------------------------------
** Created By:          廖茂刚
** Created date:        2007/09/08
** Version:             V1.0
** Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#include "Zlg7289.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "gpio.h"
#include "sysctl.h"


/*********************************************************************************************************
** Function name:       GPIO_Port_A_ISR
** Descriptions:        端口A中断服务函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void GPIO_Port_A_ISR (void)
{
    uint8 ucIntStatus;
    uint8 ucKey;
    ucIntStatus = GPIOPinIntStatus(GPIO_PORTA_BASE, true);              /*  读PA口中断状态              */
    if(ucIntStatus & ZLG7289_KEY)                                       /*  判断是否为按键中断          */
    {
        GPIOPinIntClear(GPIO_PORTA_BASE, ZLG7289_KEY);                  /*  清中断                      */
        
        ucKey = zlg7289Key();                                           /*  读按键值                    */
        
        /*
         *  若按键有效，则让8个数码管一起显示出来
         */
        if (ucKey != 0xff) {
            zlg7289Download(1, 0, 0, ucKey);
            zlg7289Download(1, 1, 0, ucKey);
            zlg7289Download(1, 2, 0, ucKey);
            zlg7289Download(1, 3, 0, ucKey);
            zlg7289Download(1, 4, 0, ucKey);
            zlg7289Download(1, 5, 0, ucKey);
            zlg7289Download(1, 6, 0, ucKey);
            zlg7289Download(1, 7, 0, ucKey);
        }
    }
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

    zlg7289Init();                                                      /*  初始化ZLG7289               */
    
    GPIODirModeSet(GPIO_PORTA_BASE, ZLG7289_KEY, GPIO_DIR_MODE_IN);     /*  设置KEY端口为输入           */
    GPIOIntTypeSet(GPIO_PORTA_BASE, ZLG7289_KEY, GPIO_FALLING_EDGE);    /*  配置引脚下降沿触发中断      */
    GPIOPinIntEnable(GPIO_PORTA_BASE, ZLG7289_KEY);                     /*  使能引脚输入中断            */
    IntEnable(INT_GPIOA);                                               /*  使能GPIO PA口中断           */
    /*
     *  显示数据
     */
    zlg7289Download (0, 0, 0, 0);
    zlg7289Download (0, 1, 0, 1);
    zlg7289Download (0, 2, 0, 2);
    zlg7289Download (0, 3, 0, 3);
    zlg7289Download (0, 4, 0, 4);
    zlg7289Download (0, 5, 0, 5);
    zlg7289Download (0, 6, 0, 6);
    zlg7289Download (0, 7, 0, 7);
    
    while (1);                                                          /*  等待按键中断                */
} 
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
