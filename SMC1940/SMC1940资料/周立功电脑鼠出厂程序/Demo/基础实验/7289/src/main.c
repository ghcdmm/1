/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           maze.h
** Last modified Date:  2007/09/24
** Last Version:        V1.0
** Description:         ���ݵײ����ȡ�õ��Թ���Ϣ�������������㷨���Ƶ��������һ״̬���������ײ�������
**                      ��ִ�С�
** 
**--------------------------------------------------------------------------------------------------------
** Created By:          ��ï��
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
** Descriptions:        �˿�A�жϷ�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void GPIO_Port_A_ISR (void)
{
    uint8 ucIntStatus;
    uint8 ucKey;
    ucIntStatus = GPIOPinIntStatus(GPIO_PORTA_BASE, true);              /*  ��PA���ж�״̬              */
    if(ucIntStatus & ZLG7289_KEY)                                       /*  �ж��Ƿ�Ϊ�����ж�          */
    {
        GPIOPinIntClear(GPIO_PORTA_BASE, ZLG7289_KEY);                  /*  ���ж�                      */
        
        ucKey = zlg7289Key();                                           /*  ������ֵ                    */
        
        /*
         *  ��������Ч������8�������һ����ʾ����
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
** Descriptions:        ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
main (void)
{
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_6MHZ );                                 /*  ʹ��PLL��50M                */

    zlg7289Init();                                                      /*  ��ʼ��ZLG7289               */
    
    GPIODirModeSet(GPIO_PORTA_BASE, ZLG7289_KEY, GPIO_DIR_MODE_IN);     /*  ����KEY�˿�Ϊ����           */
    GPIOIntTypeSet(GPIO_PORTA_BASE, ZLG7289_KEY, GPIO_FALLING_EDGE);    /*  ���������½��ش����ж�      */
    GPIOPinIntEnable(GPIO_PORTA_BASE, ZLG7289_KEY);                     /*  ʹ�����������ж�            */
    IntEnable(INT_GPIOA);                                               /*  ʹ��GPIO PA���ж�           */
    /*
     *  ��ʾ����
     */
    zlg7289Download (0, 0, 0, 0);
    zlg7289Download (0, 1, 0, 1);
    zlg7289Download (0, 2, 0, 2);
    zlg7289Download (0, 3, 0, 3);
    zlg7289Download (0, 4, 0, 4);
    zlg7289Download (0, 5, 0, 5);
    zlg7289Download (0, 6, 0, 6);
    zlg7289Download (0, 7, 0, 7);
    
    while (1);                                                          /*  �ȴ������ж�                */
} 
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
