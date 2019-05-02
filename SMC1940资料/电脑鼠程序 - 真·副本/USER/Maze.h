/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           Maze.h
** Last modified Date: 
** Last Version: 
** Description:         �����󶥲���Ƴ���ͷ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** Created By: 
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


#ifndef __Maze_h
#define __Maze_h


/*********************************************************************************************************
  ����ͷ�ļ�
*********************************************************************************************************/
#include "Type.h"
#include "Micromouse.h"
#include "Mouse_Config.h"
/*********************************************************************************************************
  �����궨�� -- ��������������״̬
*********************************************************************************************************/
#define  WAIT           0                                               /*  �ȴ�״̬                    */
#define  START          1                                               /*  ����״̬                    */
#define  MAZESEARCH     2                                               /*  ��Ѱ״̬                    */
#define  SPURT          3                                               /*  ���״̬                    */
#define  RESTART        4                                               /*  ��������״̬                    */
#define  SPURTL         5                                               /*  ���״̬                    */
#define  RESTARTL       6                                               /*  ��������״̬                    */


/*********************************************************************************************************
  ������Ҫʹ�õ��ⲿ����
*********************************************************************************************************/
extern void  mouseInit(void);                                           /*  �ײ����������ʼ��          */
extern void  mouseGoahead(int8  cNBlock);                               /*  ǰ��N��                     */
extern void  mazeSearch(void);                                          /*  �Թ�����                    */
extern void  mouseTurnleft(void);                                       /*  ����ת90��                  */
extern void  mouseTurnright(void);                                      /*  ����ת90��                  */
extern void  mouseTurnback(void);                                       /*  ���ת                      */
extern void  mouseStop(void);
extern void  __keyIntDisable (void);
extern void  mouseGoahead_Llow (int8  cNBlock);
extern void mouseTurnleft_C(void);                                               /*  ����ת90��                  */
extern void mouseTurnright_C(void);                                              /*  ����ת90��                  */
extern void mouseTurnback_Y(void);                                               /*  ���ת                      */
extern uint32 read_fm24LC16(uint32 *array,uint32 address,uint32 control,uint32 len);
void cornerMethodgo(void);
void centralMethodnew(void);
/*********************************************************************************************************
  ������Ҫʹ�õ��ⲿ����
*********************************************************************************************************/
extern MAZECOOR GmcMouse;                                               /*  GmcMouse.x :�����������    */
                                                                        /*  GmcMouse.y :������������    */
                                                                        
extern uint8    GucMouseDir;                                            /*  �������ǰ������            */
extern uint8    GucMapBlock[MAZETYPE][MAZETYPE];                        /*  GucMapBlock[x][y]           */
                                                                         /*  x,������;y,������;          */
                                                       /*  bit3~bit0�ֱ������������   */
extern uint8    map;                                                                        /*  0:�÷�����·��1:�÷�����·  */
extern uint8    GucYiBaiBa;                   
extern uint8    GucDirTemp;
extern uint8    GucMapBlock1[MAZETYPE][MAZETYPE];  
extern uint8    GucMapBlock0[MAZETYPE][MAZETYPE];  /*���ˮʱǽ������*/
extern uint8    GucMouseStart;
extern uint8    GucFrontJinju;
extern uint8    GucCrossroad;
              
//������趨ֵ�����ĳ�������ʽ
extern uint16 GusDistance_L_Near; //������
extern uint16 GusDistance_L_Mid;  //�������
extern uint16 GusDistance_L_Far;  //�����Զ
extern uint16 GusDistance_R_Near; //�Һ����
extern uint16 GusDistance_R_Mid;  //�Һ�����
extern uint16 GusDistance_R_Far;  //�Һ����
extern uint16 GusDistance_FL_Near;  //���࣬�����ж�ֹͣ 
extern uint16 GusDistance_FR_Near;
extern uint16 GusDistance_FL_Far;   //Զ�������ж�ǽ�� 
extern uint16 GusDistance_FR_Far;
//extern uint16 GusDistance_DL_Near; //��б�����
//extern uint16 GusDistance_DR_Near; //��б�����
//extern uint16 GusDistance_DL_Far;  //��б����Զ
//extern uint16 GusDistance_DR_Far;  //��б����Զ
static void  mapStepEdit(int8  cX, int8  cY);
static void  mouseSpurt(void);
static void  objectGoTo(int8  cXdst, int8  cYdst);
static void  objectGoTo_L(int8  cXdst, int8  cYdst);
static uint8 mazeBlockDataGet(uint8  ucDirTemp);
static void  rightMethod(void);
static void  leftMethod(void);
static void  frontRightMethod(void);
static void  frontLeftMethod(void);
static void  centralMethod(void);


#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
