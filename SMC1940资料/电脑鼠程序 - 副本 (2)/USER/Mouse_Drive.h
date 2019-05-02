/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           Mouse.Drive.h
** Last modified Date:  
** Last Version: 
** Description:         �ײ���������ͷ�ļ�
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


#ifndef __Mouse_Drive_h
#define __Mouse_Drive_h


/*********************************************************************************************************
  ����ͷ�ļ�
*********************************************************************************************************/
#include "stm32f10x.h"
#include "Type.h"
#include "Micromouse.h"
#include "Mouse_Config.h"
#include "stmflash.h"
#include "bsp_usart1.h"
#define BLUE_LED_OFF	GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define BLUE_LED_ON		GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define RED_LED_OFF		GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define RED_LED_ON		GPIO_SetBits(GPIOC,GPIO_Pin_3)
void BUZZ_ON(void);
void BUZZ_OFF(void);
#define Left_motor_go      (GPIO_ResetBits(GPIOB,GPIO_Pin_5),GPIO_SetBits(GPIOB,GPIO_Pin_8))//�ҵ��ǰ��cor
#define Left_motor_back    (GPIO_SetBits(GPIOB,GPIO_Pin_5),GPIO_ResetBits(GPIOB,GPIO_Pin_8))//��������
#define Left_motor_stop    (GPIO_SetBits(GPIOB,GPIO_Pin_5),GPIO_SetBits(GPIOB,GPIO_Pin_8)) 	//����ֹͣ	

#define Right_motor_go     (GPIO_ResetBits(GPIOC,GPIO_Pin_12),GPIO_SetBits(GPIOD,GPIO_Pin_2))//����ǰ��
#define Right_motor_back   (GPIO_SetBits(GPIOC,GPIO_Pin_12),GPIO_ResetBits(GPIOD,GPIO_Pin_2))//�ҵ������
#define Right_motor_stop   (GPIO_SetBits(GPIOC,GPIO_Pin_12),GPIO_SetBits(GPIOD,GPIO_Pin_2))	//�ҵ��ֹͣ
extern unsigned int pulse_L_counter;
extern unsigned int pulse_R_counter;
extern unsigned int pulse_L;
extern unsigned int pulse_R;
extern uint16 Sunlight[6];
extern uint16 ucIRCheck[6];
extern __IO uint16 ADC_ConvertedValue[6];
extern unsigned int aa,bb,cc;
extern float n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11;
extern unsigned int er,el;
extern int xx;
void search(void);

/*********************************************************************************************************
  �����궨��--������
*********************************************************************************************************/
#define __FRONTL              0                                           /*  ָ��ǰ����������          */
#define __LEFT           	  1                                           /*  ָ���󷽴�����            */
#define __RIGHT          	  2                                           /*  ָ���ҷ�������            */
#define __FRONTR			  3											  /*  ָ��ǰ����������          */


/*********************************************************************************************************
  �����궨��--������״̬
*********************************************************************************************************/
#define __STOP              0                                           /*  ������ֹͣ                  */
#define __GOAHEAD           1                                           /*  ������ǰ��                  */
#define __TURNLEFT          2                                           /*  ����������ת                */
#define __TURNRIGHT         3                                           /*  ����������ת                */
#define __TURNBACK          4                                           /*  ���������ת                */
#define __GOBACK            5											/*  �����󷵻�					*/
/*********************************************************************************************************
  �����궨��--����Ӽ��ٶ�
*********************************************************************************************************/
#define __SPEEDUP         0                                           /*  �������                    */
#define __SPEEDDOWN       1                                           /*  �������                */

/*********************************************************************************************************
  �����궨��--���״̬
*********************************************************************************************************/
#define __MOTORSTOP         0                                           /*  ���ֹͣ                    */
#define __WAITONESTEP       1                                           /*  �����ͣһ��                */
#define __MOTORRUN          2                                           /*  �������                    */
#define __MOTORTURNBACK		3
#define __MOTORTURN			4
/*********************************************************************************************************
  �����궨��--������з���
*********************************************************************************************************/
#define __MOTORGOAHEAD      0                                           /*  ���ǰ��                    */
#define __MOTORGOBACK       1                                           /*  �������                    */
#define __MOTORGOSTOP       2                                           /*  ���ֹͣ                    */

/*********************************************************************************************************
  �����궨��--PID
*********************************************************************************************************/
#define __KP 0.50 	  //���� 80
#define __KI 0.3       //���� 0.01
#define __KD 0.02        //΢��

#define U_MAX 400       //���ص����ֵ,��pwm������ֵ 
#define U_MIN 0 
#define error_IMAX 4   //�����޷� 
#define Deadband 2 	    //�ٶ�PID������������Χ

/*********************************************************************************************************
  �ṹ�嶨��
*********************************************************************************************************/
struct __motor {
    int8    cState;                                                     /*  �������״̬                */
    int8    cDir;                                                       /*  ������з���                */
    uint32  uiPulse;                                                    /*  �����Ҫ���е�����          */
    uint32  uiPulseCtr;                                                 /*  ��������е�����            */
    int16   sSpeed;                                                     /*  ��ǰռ�ձ�                    */
};
typedef struct __motor __MOTOR;

struct __pid       //���������������� 
{  
    uint16 usFeedBack;  //�ٶ�PID���ٶȷ���ֵ
    
    float sRef;
    int16 sFeedBack;
    float sPreError;  //�ٶ�PID��ǰһ�Σ��ٶ����,,vi_Ref - vi_FeedBack 
    float sPreDerror; //�ٶ�PID��ǰһ�Σ��ٶ����֮�d_error-PreDerror; 
  
    fp32 fKp;      //�ٶ�PID��Ka = Kp 
    fp32 fKi;      //�ٶ�PID��Kb = Kp * ( T / Ti ) 
    fp32 fKd;      //�ٶ�PID�� 
       
    int16 iPreU;    //����������ֵ      
};
typedef struct __pid __PID;
/*********************************************************************************************************
  �����궨��
*********************************************************************************************************/
extern __MOTOR  __GmRight;
extern __MOTOR  __GmLeft;
void mazeSearch(void);                                                  /*  ǰ��N��                     */

void mouseTurnleft(void);                                               /*  ����ת90��                  */
void mouseTurnright(void);                                              /*  ����ת90��                  */
void mouseTurnback(void);                                               /*  ���ת                      */
void onestep(void);														/*ǰ��һ��						*/
static void __delay(uint32  uiD);
static void __rightMotorContr(void);									/*��ֱ���������				*/
static void __leftMotorContr(void);										/*��ֱ���������				*/
static void __mouseCoorUpdate(void);									/* ���ݵ�ǰ�����������ֵ		*/
static void __irSend(int8  __cNumber);									/* ���ⷢ��						*/
unsigned char __irCheck(void);													/* �������						*/
void __Encoder(void);
static void __wallCheck(void);                                          /* ǽ�ڼ��                     */
void MouseInit(void);
void mouseGoahead(int8  cNBlock);                                       /*  ǰ��N��                     */
void mouseStop(void); 
extern void mouseInit (void);
void PIDInit(void);

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
