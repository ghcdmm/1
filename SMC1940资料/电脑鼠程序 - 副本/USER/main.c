 /****************************************Copyright (c)****************************************************
**                               Tianjin Polytechnic University A408
**                                     
**                                 http://www.turnedback.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           maze.c
** Last modified Date:  2015/10/15
** Last Version:        V1.0
** Description:         根据底层程序取得的迷宫信息，经过该智能算法控制电脑鼠的下一状态，并送往底层驱动程
**                      序执行。
** 
**--------------------------------------------------------------------------------------------------------
** Created By:          Liao Maogang
** Created date:        2007/09/08
** Version:             V1.0
** Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Modified by:			Who Moved My Cheese
** Modified date:		
** Version:
** Description:
**             
*********************************************************************************************************/


/*********************************************************************************************************
  包含头文件
*********************************************************************************************************/
#include "Maze.h"
#include "stm32f10x.h"
#include "BitBand.h"
#include "Mouse_Drive.h"
#include "MPU6050.h"
/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
static uint8    GucXStart                           = 0;                /*  起点横坐标                  */
static uint8    GucYStart                           = 0;                /*  起点纵坐标                  */
static uint8    GucXGoal0                           = XDST0;            /*  终点X坐标，有两个值         */
static uint8    GucXGoal1                           = XDST1;
static uint8    GucYGoal0                           = YDST0;            /*  终点Y坐标，有两个值         */
static uint8    GucYGoal1                           = YDST1;
static uint8    GucMouseTask                        = WAIT;             /*  状态机，初始状态为等待      */
static uint8    GucMapStep[MAZETYPE][MAZETYPE]      = {0xff};           /*  保存各坐标的等高值          */
static MAZECOOR GmcStack[MAZETYPE * MAZETYPE]       = {0};              /*  在mapStepEdit()中作堆栈使用 */
static MAZECOOR GmcCrossway[MAZETYPE * MAZETYPE]    = {0};              /*  Main()中暂存未走过支路坐标  */
//unsigned int aa,bb,cc;

//extern uint8    GucGoHead;
/*********************************************************************************************************
** Function name:       Delay
** Descriptions:        延时函数
** input parameters:    uiD :延时参数，值越大，延时越久
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void delay (uint32 uiD)
{
    for (; uiD; uiD--);
}

/*********************************************************************************************************
** Function name:       mapStepEdit
** Descriptions:        制作以目标点为起点的等高图
** input parameters:    uiX:    目的地横坐标
**                      uiY:    目的地纵坐标
** output parameters:   GucMapStep[][]:  各坐标上的等高值
** Returned value:      无
*********************************************************************************************************/
void mapStepEdit (int8  cX, int8  cY)
{
    uint8 n         = 0;                                                /*  GmcStack[]下标              */
    uint8 ucStep    = 1;                                                /*  等高值                      */
    uint8 ucStat    = 0;                                                /*  统计可前进的方向数          */
    uint8 i,j;
    
    GmcStack[n].cX  = cX;                                               /*  起点X值入栈                 */
    GmcStack[n].cY  = cY;                                               /*  起点Y值入栈                 */
    n++;
    /*
     *  初始化各坐标等高值
     */
    for (i = 0; i < MAZETYPE; i++) {
        for (j = 0; j < MAZETYPE; j++) {
            GucMapStep[i][j] = 0xff;
        }
    }	
    /*
     *  制作等高图，直到堆栈中所有数据处理完毕
     */
    while (n) {
        GucMapStep[cX][cY] = ucStep++;                                  /*  填入等高值                  */

        /*
         *  对当前坐标格里可前进的方向统计
         */
        ucStat = 0;
        if ((GucMapBlock[cX][cY] & 0x01) &&                             /*  前方有路                    */
            (GucMapStep[cX][cY + 1] > (ucStep))) {                      /*  前方等高值大于计划设定值    */
            ucStat++;                                                   /*  可前进方向数加1             */
        }
        if ((GucMapBlock[cX][cY] & 0x02) &&                             /*  右方有路                    */
            (GucMapStep[cX + 1][cY] > (ucStep))) {                      /*  右方等高值大于计划设定值    */
            ucStat++;                                                   /*  可前进方向数加1             */
        }
        if ((GucMapBlock[cX][cY] & 0x04) &&
            (GucMapStep[cX][cY - 1] > (ucStep))) {
            ucStat++;                                                   /*  可前进方向数加1             */
        }
        if ((GucMapBlock[cX][cY] & 0x08) &&
            (GucMapStep[cX - 1][cY] > (ucStep))) {
            ucStat++;                                                   /*  可前进方向数加1             */
        }
        /*
         *  没有可前进的方向，则跳转到最近保存的分支点
         *  否则任选一可前进方向前进
         */
        if (ucStat == 0) {
            n--;
            cX = GmcStack[n].cX;
            cY = GmcStack[n].cY;
            ucStep = GucMapStep[cX][cY];
        } else {
            if (ucStat > 1) {                                           /*  有多个可前进方向，保存坐标  */
                GmcStack[n].cX = cX;                                    /*  横坐标X值入栈               */
                GmcStack[n].cY = cY;                                    /*  纵坐标Y值入栈               */
                n++;
            }
            /*
             *  任意选择一条可前进的方向前进
             */
            if ((GucMapBlock[cX][cY] & 0x01) &&                         /*  上方有路                    */
                (GucMapStep[cX][cY + 1] > (ucStep))) {                  /*  上方等高值大于计划设定值    */
                cY++;                                                   /*  修改坐标                    */
                continue;
            }
            if ((GucMapBlock[cX][cY] & 0x02) &&                         /*  右方有路                    */
                (GucMapStep[cX + 1][cY] > (ucStep))) {                  /*  右方等高值大于计划设定值    */
                cX++;                                                   /*  修改坐标                    */
                continue;
            }
            if ((GucMapBlock[cX][cY] & 0x04) &&                         /*  下方有路                    */
                (GucMapStep[cX][cY - 1] > (ucStep))) {                  /*  下方等高值大于计划设定值    */
                cY--;                                                   /*  修改坐标                    */
                continue;
            }
            if ((GucMapBlock[cX][cY] & 0x08) &&                         /*  左方有路                    */
                (GucMapStep[cX - 1][cY] > (ucStep))) {                  /*  左方等高值大于计划设定值    */
                cX--;                                                   /*  修改坐标                    */
                continue;
            }
        }
    }
}


void mouseSpurt (void)//冲刺到终点
{
    uint8 ucTemp = 0xff;
    int8 cXdst = 0,cYdst = 0;
    /*
     *  对终点的四个坐标分别制作等高图
     *  取离起点最近的一个点作为目标点
     */
    if (GucMapBlock[GucXGoal0][GucYGoal0] & 0x0c) {                     /*  判断该终点坐标是否有出口    */
        mapStepEdit(GucXGoal0,GucYGoal0);                               /*  制作等高图                  */
        if (ucTemp > GucMapStep[GucXStart][GucYStart]) {                /*  保存离起点最近的坐标        */
            cXdst  = GucXGoal0;
            cYdst  = GucYGoal0;
            ucTemp = GucMapStep[GucXStart][GucYStart];
        }
    }
    if (GucMapBlock[GucXGoal0][GucYGoal1] & 0x09) {                     /*  判断该终点坐标是否有出口    */
        mapStepEdit(GucXGoal0,GucYGoal1);                               /*  制作等高图                  */
        if (ucTemp > GucMapStep[GucXStart][GucYStart]) {                /*  保存离起点最近的坐标        */
            cXdst  = GucXGoal0;
            cYdst  = GucYGoal1;
            ucTemp = GucMapStep[GucXStart][GucYStart];
        }
    }
    if (GucMapBlock[GucXGoal1][GucYGoal0] & 0x06) {                     /*  判断该终点坐标是否有出口    */
        mapStepEdit(GucXGoal1,GucYGoal0);                               /*  制作等高图                  */
        if (ucTemp > GucMapStep[GucXStart][GucYStart]) {                /*  保存离起点最近的坐标        */
            cXdst  = GucXGoal1;
            cYdst  = GucYGoal0;
            ucTemp = GucMapStep[GucXStart][GucYStart];
        }
    }
    if (GucMapBlock[GucXGoal1][GucYGoal1] & 0x03) {                     /*  判断该终点坐标是否有出口    */
        mapStepEdit(GucXGoal1,GucYGoal1);                               /*  制作等高图                  */
        if (ucTemp > GucMapStep[GucXStart][GucYStart]) {                /*  保存离起点最近的坐标        */
            cXdst  = GucXGoal1;
            cYdst  = GucYGoal1;
            ucTemp = GucMapStep[GucXStart][GucYStart];
        }
    }
    
    objectGoTo_L(cXdst,cYdst);                                            /*  运行到指定目标点            */
    
}

void objectGoTo (int8  cXdst, int8  cYdst)
{
    uint8 ucStep = 1;
    int8  cNBlock = 0, cDirTemp;
    int8 cX,cY;
    cX = GmcMouse.cX;
    cY = GmcMouse.cY;
    mapStepEdit(cXdst,cYdst);                                           /*  制作等高图                  */
    /*
     *  根据等高值向目标点运动，直到达到目的地
     */
    while ((cX != cXdst) || (cY != cYdst)) {
        
        ucStep = GucMapStep[cX][cY];
        /*
         *  任选一个等高值比当前自身等高值小的方向前进
         */
        if ((GucMapBlock[cX][cY] & 0x01) &&                             /*  上方有路                    */
            (GucMapStep[cX][cY + 1] < ucStep)) {                        /*  上方等高值较小              */
            cDirTemp = UP;                                              /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cY++;
                continue;                                               /*  跳过本次循环                */
            }
        }
        if ((GucMapBlock[cX][cY] & 0x02) &&                             /*  右方有路                    */
            (GucMapStep[cX + 1][cY] < ucStep)) {                        /*  右方等高值较小              */
            cDirTemp = RIGHT;                                           /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cX++;
                continue;                                               /*  跳过本次循环                */
            }
        }
        if ((GucMapBlock[cX][cY] & 0x04) &&                             /*  下方有路                    */
            (GucMapStep[cX][cY - 1] < ucStep)) {                        /*  下方等高值较小              */
            cDirTemp = DOWN;                                            /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cY--;
                continue;                                               /*  跳过本次循环                */
            }
        }
        if ((GucMapBlock[cX][cY] & 0x08) &&                             /*  左方有路                    */
            (GucMapStep[cX - 1][cY] < ucStep)) {                        /*  左方等高值较小              */
            cDirTemp = LEFT;                                            /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cX--;
                continue;                                               /*  跳过本次循环                */
            }
        }
        cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;                      /*  计算方向偏移量              */
        GucDirTemp = cDirTemp;
        if (cNBlock) {
            mouseGoahead(cNBlock);                                    /*  前进cNBlock步               */         
        }        
        cNBlock = 0;                                                    /*  任务清零                    */
        
        /*
         *  控制电脑鼠转弯
         */
        switch (cDirTemp) {

        case 1:
            mouseTurnright();
            break;

        case 2:
            mouseTurnback();
            break;

        case 3:
            mouseTurnleft();
            break;

        default:
            break;
        }
      GmcMouse.cX=cX;
      GmcMouse.cY=cY;
    }
    /*
     *  判断任务是否完成，否则继续前进
     */
    
      if (cNBlock) {
          mouseGoahead(cNBlock);
      GmcMouse.cX=cX;
      GmcMouse.cY=cY;
      }
}

void objectGoTo_L (int8  cXdst, int8  cYdst)
{
    uint8 ucStep = 1;
    int8  cNBlock = 0, cDirTemp;
    int8 cX,cY;
    cX = GmcMouse.cX;
    cY = GmcMouse.cY;
    mapStepEdit(cXdst,cYdst);                                           /*  制作等高图                  */
    /*
     *  根据等高值向目标点运动，直到达到目的地
     */
    while ((cX != cXdst) || (cY != cYdst)) {
        
        ucStep = GucMapStep[cX][cY];
        /*
         *  任选一个等高值比当前自身等高值小的方向前进
         */
        if ((GucMapBlock[cX][cY] & 0x01) &&                             /*  上方有路                    */
            (GucMapStep[cX][cY + 1] < ucStep)) {                        /*  上方等高值较小              */
            cDirTemp = UP;                                              /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cY++;
                continue;                                               /*  跳过本次循环                */
            }
        }
        if ((GucMapBlock[cX][cY] & 0x02) &&                             /*  右方有路                    */
            (GucMapStep[cX + 1][cY] < ucStep)) {                        /*  右方等高值较小              */
            cDirTemp = RIGHT;                                           /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cX++;
                continue;                                               /*  跳过本次循环                */
            }
        }
        if ((GucMapBlock[cX][cY] & 0x04) &&                             /*  下方有路                    */
            (GucMapStep[cX][cY - 1] < ucStep)) {                        /*  下方等高值较小              */
            cDirTemp = DOWN;                                            /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cY--;
                continue;                                               /*  跳过本次循环                */
            }
        }
        if ((GucMapBlock[cX][cY] & 0x08) &&                             /*  左方有路                    */
            (GucMapStep[cX - 1][cY] < ucStep)) {                        /*  左方等高值较小              */
            cDirTemp = LEFT;                                            /*  记录方向                    */
            if (cDirTemp == GucMouseDir) {                              /*  优先选择不需要转弯的方向    */
                cNBlock++;                                              /*  前进一个方格                */
                cX--;
                continue;                                               /*  跳过本次循环                */
            }
        }
        cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;                      /*  计算方向偏移量              */
        GucDirTemp = cDirTemp;
        if (cNBlock) {
            mouseGoahead(cNBlock);                                    /*  前进cNBlock步               */         
        }        
        cNBlock = 0;                                                    /*  任务清零                    */
        
        /*
         *  控制电脑鼠转弯
         */
        switch (cDirTemp) {

        case 1:
            mouseTurnright();
            break;

        case 2:
            mouseTurnback();
            break;

        case 3:
            mouseTurnleft();
            break;

        default:
            break;
        }
      GmcMouse.cX=cX;
      GmcMouse.cY=cY;
    }
    /*
     *  判断任务是否完成，否则继续前进
     */
    
      if (cNBlock) {
          mouseGoahead(cNBlock);
      GmcMouse.cX=cX;
      GmcMouse.cY=cY;
      }
}


/*********************************************************************************************************
** Function name:       mazeBlockDataGet
** Descriptions:        根据电脑鼠的相对方向，取出该方向上迷宫格的墙壁资料
** input parameters:    ucDir: 电脑鼠的相对方向
** output parameters:   无
** Returned value:      GucMapBlock[cX][cY] : 墙壁资料
*********************************************************************************************************/
uint8 mazeBlockDataGet (uint8  ucDirTemp)
{
    int8 cX = 0,cY = 0;
    
    /*
     *  把电脑鼠的相对方向转换为绝对方向
     */
    switch (ucDirTemp) {

    case MOUSEFRONT:
        ucDirTemp = GucMouseDir;
        break;

    case MOUSELEFT:
        ucDirTemp = (GucMouseDir + 3) % 4;
        break;

    case MOUSERIGHT:
        ucDirTemp = (GucMouseDir + 1) % 4;
        break;

    default:
        break;
    }
    
    /*
     *  根据绝对方向计算该方向上相邻格的坐标
     */
    switch (ucDirTemp) {

    case 0:
        cX = GmcMouse.cX;
        cY = GmcMouse.cY + 1;
        break;
        
    case 1:
        cX = GmcMouse.cX + 1;
        cY = GmcMouse.cY;
        break;
        
    case 2:
        cX = GmcMouse.cX;
        cY = GmcMouse.cY - 1;
        break;
        
    case 3:
        cX = GmcMouse.cX - 1;
        cY = GmcMouse.cY;
        break;
        
    default:
        break;
    }
    
    return(GucMapBlock0[cX][cY]);                                        /*  返回迷宫格上的资料          */
}
/*********************************************************************************************************
** Function name:       rightMethod
** Descriptions:        右手法则，优先向右前进
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void rightMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  电脑鼠的右边有路            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) {                       /*  电脑鼠的右边没有走过        */
        mouseTurnright();                                               /*  电脑鼠右转                  */
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  电脑鼠的前方有路            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) {                       /*  电脑鼠的前方没有走过        */
        return;                                                         /*  电脑鼠不用转弯              */
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  电脑鼠的左边有路            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) {                       /*  电脑鼠的左边没有走过        */
        mouseTurnleft();                                                /*  电脑鼠左转                  */
        return;
    }
}
/*********************************************************************************************************
** Function name:       leftMethod
** Descriptions:        左手法则，优先向左运动
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void leftMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  电脑鼠的左边有路            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) {                       /*  电脑鼠的左边没有走过        */
        mouseTurnleft();                                                 /*  电脑鼠左转                  */
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  电脑鼠的前方有路            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) {                       /*  电脑鼠的前方没有走过        */
        return;                                                         /*  电脑鼠不用转弯              */
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  电脑鼠的右边有路            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) {                       /*  电脑鼠的右边没有走过        */
        mouseTurnright();                                               /*  电脑鼠右转                  */
        return;
    }
}
/*********************************************************************************************************
** Function name:       frontRightMethod
** Descriptions:        中右法则，优先向前运行，其次向右
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void frontRightMethod (void)
 {
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  电脑鼠的前方有路            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) {                       /*  电脑鼠的前方没有走过        */
        
        return;                                                         /*  电脑鼠不用转弯              */
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  电脑鼠的右边有路            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) {                       /*  电脑鼠的右边没有走过        */
        mouseTurnright();                                               /*  电脑鼠右转                  */
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  电脑鼠的左边有路            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) {                       /*  电脑鼠的左边没有走过        */
        mouseTurnleft();                                                /*  电脑鼠左转                  */
        return;
    }
}
/*********************************************************************************************************
** Function name:       frontLeftMethod
** Descriptions:        中左法则，优先向前运行，其次向左
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void frontLeftMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  电脑鼠的前方有路            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) {                       /*  电脑鼠的前方没有走过        */
        return;                                                         /*  电脑鼠不用转弯              */
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  电脑鼠的左边有路            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) {                       /*  电脑鼠的左边没有走过        */
        mouseTurnleft();                                                /*  电脑鼠左转                  */
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  电脑鼠的右边有路            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) {                       /*  电脑鼠的右边没有走过        */
        mouseTurnright();                                               /*  电脑鼠右转                  */
        return;
    }
}

/*********************************************************************************************************
** Function name:       centralMethod
** Descriptions:        中心法则，根据电脑鼠目前在迷宫中所处的位置觉定使用何种搜索法则
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void centralMethod (void)
{
    if (GmcMouse.cX & 0x08) {
        if (GmcMouse.cY & 0x08) {

            /*
             *  此时电脑鼠在迷宫的右上角
             */ 
            switch (GucMouseDir) {
                
            case UP:                                                    /*  当前电脑鼠向上              */
                leftMethod();                                           /*  左手法则                    */
                break;

            case RIGHT:                                                 /*  当前电脑鼠向右              */
                rightMethod();                                          /*  右手法则                    */
                break;

            case DOWN:                                                  /*  当前电脑鼠向下              */
                frontRightMethod();                                     /*  中右法则                    */
                break;

            case LEFT:                                                  /*  当前电脑鼠向左              */
                frontLeftMethod();                                      /*  中左法则                    */
                break;

            default:
                break;
            }
        } else {

            /*
             *  此时电脑鼠在迷宫的右下角
             */    
            switch (GucMouseDir) {
                
            case UP:                                                    /*  当前电脑鼠向上              */
                frontLeftMethod();                                      /*  中左法则                    */
                break;

            case RIGHT:                                                 /*  当前电脑鼠向右              */
                leftMethod();                                           /*  左手法则                    */
                break;

            case DOWN:                                                  /*  当前电脑鼠向下              */
                rightMethod();                                          /*  右手法则                    */
                break;

            case LEFT:                                                  /*  当前电脑鼠向左              */
                frontRightMethod();                                     /*  中右法则                    */
                break;

            default:
                break;
            }
        }
    } else {
        if (GmcMouse.cY & 0x08) {

            /*
             *  此时电脑鼠在迷宫的左上角
             */    
            switch (GucMouseDir) {
                
            case UP:                                                    /*  当前电脑鼠向上              */
                rightMethod();                                          /*  右手法则                    */
                break;

            case RIGHT:                                                 /*  当前电脑鼠向右              */
                frontRightMethod();                                     /*  中右法则                    */
                break;

            case DOWN:                                                  /*  当前电脑鼠向下              */
                frontLeftMethod();                                      /*  中左法则                    */
                break;

            case LEFT:                                                  /*  当前电脑鼠向左              */
                leftMethod();                                           /*  左手法则                    */
                break;

            default:
                break;
            }
        } else {

            /*
             *  此时电脑鼠在迷宫的左下角
             */    
            switch (GucMouseDir) {
                
            case UP:                                                    /*  当前电脑鼠向上              */
                frontRightMethod();                                     /*  中右法则                    */
                break;

            case RIGHT:                                                 /*  当前电脑鼠向右              */
                frontLeftMethod();                                      /*  中左法则                    */
                break;

            case DOWN:                                                  /*  当前电脑鼠向下              */
                leftMethod();                                           /*  左手法则                    */
                break;

            case LEFT:                                                  /*  当前电脑鼠向左              */
                rightMethod();                                          /*  右手法则                    */
                break;

            default:
                break;
            }
        }
    }
}
/*********************************************************************************************************
** Function name:       crosswayCheck
** Descriptions:        统计某坐标存在还未走过的支路数
** input parameters:    ucX，需要检测点的横坐标
**                      ucY，需要检测点的纵坐标
** output parameters:   无
** Returned value:      ucCt，未走过的支路数
*********************************************************************************************************/
uint8 crosswayCheck (int8  cX, int8  cY)
{
    uint8 ucCt = 0;
    if ((GucMapBlock[cX][cY] & 0x01) &&                                 /*  绝对方向，迷宫上方有路      */
        (GucMapBlock0[cX][cY + 1]) == 0x00) {                            /*  绝对方向，迷宫上方未走过    */
        ucCt++;                                                         /*  可前进方向数加1             */
    }
    if ((GucMapBlock[cX][cY] & 0x02) &&                                 /*  绝对方向，迷宫右方有路      */
        (GucMapBlock0[cX + 1][cY]) == 0x00) {                            /*  绝对方向，迷宫右方没有走过  */
        ucCt++;                                                         /*  可前进方向数加1             */
    }
    if ((GucMapBlock[cX][cY] & 0x04) &&                                 /*  绝对方向，迷宫下方有路      */
        (GucMapBlock0[cX][cY - 1]) == 0x00) {                            /*  绝对方向，迷宫下方未走过    */
        ucCt++;                                                         /*  可前进方向数加1             */
    }
    if ((GucMapBlock[cX][cY] & 0x08) &&                                 /*  绝对方向，迷宫左方有路      */
        (GucMapBlock0[cX - 1][cY]) == 0x00) {                            /*  绝对方向，迷宫左方未走过    */
        ucCt++;                                                         /*  可前进方向数加1             */
    }
    return ucCt;
}
/*********************************************************************************************************
** Function name:       crosswayChoice
** Descriptions:        选择一条支路作为前进方向
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void crosswayChoice (void)
{
    switch (SEARCHMETHOD) {
        
    case RIGHTMETHOD:
        rightMethod();
        break;
    
    case LEFTMETHOD:
        leftMethod();
        break;
    
    case CENTRALMETHOD:
        centralMethod();
        break;

    case FRONTRIGHTMETHOD:
        frontRightMethod();
        break;

    case FRONTLEFTMETHOD:
        frontLeftMethod();
        break;
       

    default:
        break;
    }
}

void RCC_Init(void)
{    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | 
                           RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);
}

void ProStt()
{
	delay(10000000);
	BLUE_LED_OFF;	//PA0 蓝色LED灭
	RED_LED_ON;	//PC3 红色亮	 
	BUZZ_ON();		//蜂鸣器响
	delay(10000000);

	BUZZ_OFF();
	BLUE_LED_ON;	//PA0 蓝色LED亮
	RED_LED_OFF;	//PC3 红色灭
	delay(1000);

}

void Search()
{
	;
}

int main (void)
{
	RCC_Init();
	SystemInit ();
	MouseInit();
	USART1_Config();
	PIDInit();
	ProStt();
	while(1)
	{
		BUZZ_ON();
//		printf("%d %d %d\n",aa,bb,cc);
//		printf("%d %d %d\n",ucIRCheck[1],Sunlight[1],ADC_ConvertedValue[1]);
//		printf("%d %d\n",ADC_ConvertedValue[0],cc);
		delay(300000);
//		printf("%d %d %d %d %d %d \n",ADC_ConvertedValue[0],ADC_ConvertedValue[1],ADC_ConvertedValue[2],ADC_ConvertedValue[3],ADC_ConvertedValue[4],ADC_ConvertedValue[5]);
//		printf("%d %d %d %d %d %d \n",Sunlight[0],Sunlight[1],Sunlight[2],Sunlight[3],Sunlight[4],Sunlight[5]);
//		printf("%d %d %d %d %d %d \n",ucIRCheck[0],ucIRCheck[1],ucIRCheck[2],ucIRCheck[3],ucIRCheck[4],ucIRCheck[5]);
		printf("%d\n",__GmRight.cDir);
//		delay(3000000);
		BUZZ_OFF();
		delay(300000);
		
//		delay(3000000);
	}
}
     
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
