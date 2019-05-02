#ifndef __MOTER_H
#define __MOTER_H

void dcini(void);
void vsol(int vx,int vy,int w);
void TIM4_PWM_Init(unsigned int arr,unsigned int psc);
void steini(void);


extern int dis;
extern unsigned char delayx;

#endif
