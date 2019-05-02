#include "stm32f10x.h"

void RCC_Init(void)
{    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | 
                           RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);
}

int main (void)
{
	RCC_Init();
	SystemInit();
	while(1)
	{
		;
	}
}