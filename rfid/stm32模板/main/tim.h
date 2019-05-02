#ifndef __TIM_H
#define __TIM_H

extern unsigned int tmr1s;          //  200ms计数
extern unsigned int flag1s;         //  200ms溢出标志位

void TIM3_INIT(unsigned int arr,unsigned int psc);

#endif
