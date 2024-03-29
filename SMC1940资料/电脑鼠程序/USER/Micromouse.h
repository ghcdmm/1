/****************************************Copyright (c)***************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info--------------------------------------------------------------------------------
** File Name:           Micromouse.h
** Last modified Date: 
** Last Version: 
** Description:         电脑鼠控制程序头文件
** 
**-------------------------------------------------------------------------------------------------------
** Created By: 
** Created date: 
** Version: 
** Descriptions: 
**
**-------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/


#ifndef __MICROMOUSE_H
#define __MICROMOUSE_H


/*********************************************************************************************************
  包含头文件
*********************************************************************************************************/
#include "Type.h"


/*********************************************************************************************************
  常量宏定义 -- 搜索法则
*********************************************************************************************************/
#define RIGHTMETHOD         0
#define LEFTMETHOD          1
#define CENTRALMETHOD       2
#define FRONTRIGHTMETHOD    3
#define FRONTLEFTMETHOD     4
#define CORNERMETHOD        5



/*********************************************************************************************************
  常量宏定义 -- 电脑鼠运行绝对方向
*********************************************************************************************************/
#define UP                  0
#define RIGHT               1
#define DOWN                2
#define LEFT                3


/*********************************************************************************************************
  常量宏定义 -- 电脑鼠运行相对方向
*********************************************************************************************************/
#define MOUSELEFT           0
#define MOUSEFRONT          1
#define MOUSERIGHT          2


/*********************************************************************************************************
  常量宏定义 -- 用绝对方向来表示相对方向上的资料表位置
*********************************************************************************************************/
#define MOUSEWAY_F          (1 <<   GucMouseDir)
#define MOUSEWAY_R          (1 << ((GucMouseDir + 1) % 4))
#define MOUSEWAY_B          (1 << ((GucMouseDir + 2) % 4))
#define MOUSEWAY_L          (1 << ((GucMouseDir + 3) % 4))


/*********************************************************************************************************
  定义结构体类型
*********************************************************************************************************/
struct mazecoor
{
    int8 cX;
    int8 cY;
};
typedef struct mazecoor MAZECOOR;


#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
