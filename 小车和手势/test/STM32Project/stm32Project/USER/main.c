#include "bsp.h" 

 
/*******************************************************************************
* 函数名	: main
* 描述	    : 主函数
* 输入参数  : 无
* 返回参数  : 无
*******************************************************************************/	
int main()
{
	s32 temp;
	delay_init();
	uart_init(115200);
	IIC_Init();
	InitSingleFDC2214();//初始化单通道
	while(1)
	{
		temp = ReadRegfdc2214(0x00);
		temp=temp;
		//printf("DataTEMP1:\n");
		printf("%d\n",temp);
		delay_ms(200);
	}		
		
}

