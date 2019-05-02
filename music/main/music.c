#include "stm32f10x.h"
#include "music.h"
#include "tim.h"

#define C3 7644
#define _C3 7216
#define D3 6811
#define _D3 6428
#define E3 6068
#define F3 5727
#define _F3 5405
#define G3 5102
#define _G3 4816
#define A3 4545
#define _A3 4290
#define B3 4050
#define C4 3822
#define _C4 3608
#define D4 3405
#define _D4 3214
#define E4 3034
#define F4 2863
#define _F4 2703
#define G4 2551
#define _G4 2408
#define A4 2273
#define _A4 2145
#define B4 2025
#define C5 1911
#define _C5 1804
#define D5 1703
#define _D5 1607
#define E5 1517
#define F5 1431
#define _F5 1351
#define G5 1276
#define _G5 1204
#define A5 1136
#define _A5 1073
#define B5 1012
#define C6 956
#define _C6 902
#define D6 851
#define _D6 804
#define E6 758
#define F6 716
#define _F6 676
#define G6 638
#define _G6 602
#define A6 568
#define _A6 536
#define B6 506

#define speed 100
#define b4 1000*60/speed

unsigned int *t;
float *b;

unsigned int m1_t[]={D4,D4,C4,D4,D4,C4,D4,D4,C4,D4,F4,A4,D4,D4,C4,D4,D4,C4,D4,F4,G4,A4,G4,A4,D4,C4,D4,C4,G4,A4,D4,C4,D4,C4,
	G4,A4,D4,C4,D4,C4,F4,E4,F4,E4,D4,C4,G4,A4,D4,C4,D4,C4,G4,A4,D4,C4,D4,C4,G4,A4,C5,F5,E5,F5,E5,D5,C5,A4,G4,A4,
	D4,C4,D4,C4,G4,A4,D4,C4,D4,C4,G4,A4,D4,C4,D4,C4,F4,E4,F4,E4,D4,C4,D4,C4,D4,F4,D4,F4,G4,C5,A4,A4,C5,F5,A4,G5,F5,
E5,F5,E5,D5,C5,D5,C5,D5,F5,
	G5,A5,D5,C5,D5,C5,G5,A5,D5,C5,D5,C5,G5,A5,D5,C5,D5,C5,F5,E5,F5,E5,D5,C5,G5,A5,D5,C5,D5,C5,G5,A5,D5,C5,D5,C5,
G5,A5,C6,F6,E6,F6,E6,D6,C6,A5,G5,A5,D5,C5,D5,C5,G5,A5,D5,C5,D5,C5,G5,A5,D5,C5,D5,C5,F5,E5,F5,E5,D5,C5,
G5,A5,C6,D6,C6,A5,G5,D5,F5,G5,A5,D5,D5,C5,D5,D4,D4,C4,F4,G4,G4,A4,
D4,D4,C4,D4,C4,A3,C4,D4,D4,C4,C4,D4,F4,G4,A4,G4,A4,G4,F4,D4};
	
float m1_b[]={0.75,0.75,0.5,0.75,0.75,0.5,0.75,0.75,0.5,1,1,1,0.75,0.75,0.5,0.75,0.75,0.5,1,1,1,1,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,
	0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.25,0.25,0.25,0.5,0.5,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.5,0.5,
	0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.25,0.25,0.25,0.5,0.5,0.5,0.25,0.25,0.5,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
	0.5,0.25,0.25,0.25,0.5,0.5,0.5,0.5,0.5,0.5,
0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.25,0.25,0.25,0.5,0.5,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,
0.5,0.5,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.5,0.25,0.25,0.25,0.5,0.5,
0.5,0.25,0.25,0.25,0.25,0.25,0.25,0.5,0.5,0.5,0.5,0.75,0.75,0.5,2,1,0.75,0.25,0.5,0.5,0.5,0.5,
1,0.75,0.25,0.5,0.5,0.5,0.5,1,0.75,0.25,0.5,0.5,0.5,0.5,0};

unsigned int m2_t[]={A4,E5,E5,E5,E5,E5,E5,E5,E5,E5,E5,D5,A4,A4,A4,A4,
E5,E5,E5,E5,E5,E5,E5,E5,E5,E5,F5,E5,E5,E5,A4,D5,D5,C5,D5,D5,A4,D5,D5,C5,C5,D5,D5,A4,B4,B4,B4,B4,B4,B4,B4,B4,B4,B4,D5,G5,E5,A4};
	
float m2_b[]={0.5,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.5};

void TIM3_IRQHandler(void)
{
	static unsigned char f=1;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		if(f==1)
		{
			TIM3->ARR=*b*b4;
			b++;
			TIM_SetCompare1(TIM2,*t/2);
			TIM2->ARR=*t;
			t++;
			f=0;
		}
		else
		{
			TIM3->ARR=10;
			TIM2->ARR=1;
			f=1;
		}
	}
}
