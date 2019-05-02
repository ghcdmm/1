#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;

extern float Pitch,Roll,Yaw;
int TIM1_UP_IRQHandler(void);  
int balance(float angle,float gyro);
int velocity(int encoder_left,int encoder_right);
int turn(float encoder_left,float encoder_right,float gyro);
void Set_Pwm(int moto1,int moto2);
void Key(void);
void Xianfu_Pwm(void);
u8 Turn_Off(float angle, int voltage);
void Get_Angle(u8 way);
int myabs(int a);

void get_pid_output(int desired_speed[2], int current_speed[2],int output[2]);

#endif
