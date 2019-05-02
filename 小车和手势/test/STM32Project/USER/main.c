#include "bsp.h" 

 
/*******************************************************************************
* 函数名	: main
* 描述	    : 主函数
* 输入参数  : 无
* 返回参数  : 无
*******************************************************************************/	
int main()
{
	s32 temp0,temp1,temp2,temp3;
	delay_init();
	uart_init(115200);
	IIC_Init();
	InitMultiFDC2214();//初始化单通道
	while(1)
	{
		temp0 = (ReadRegfdc2214(0x00)<<16)+(ReadRegfdc2214(0x01));
		temp1 = (ReadRegfdc2214(0x02)<<16)+(ReadRegfdc2214(0x03));
		temp2 = (ReadRegfdc2214(0x04)<<16)+(ReadRegfdc2214(0x05));
		temp3 = (ReadRegfdc2214(0x06)<<16)+(ReadRegfdc2214(0x07));
		//printf("DataTEMP0:");
		printf("%d ",temp0);
		//printf("DataTEMP1:");
		printf("%d ",temp1);
		//printf("DataTEMP2:");
		printf("%d ",temp2);
		//printf("DataTEMP3:");
		printf("%d\n",temp3);
	}		
		
}

