#include "bsp.h"

/*******************************************************************************
* 函数名	: WriteRegfdc2214 
* 描述	    : 写寄存器数据
* 输入参数  : add，value
* 返回参数  : 无
*******************************************************************************/
void WriteRegfdc22142(u8 add,u16 value)
{
    IIC_Start2();  
    IIC_Send_Byte2(0X54);	 //ADDR=0时，地址0X2A<<1+0=0X54  
    IIC_Wait_Ack2();          //等应答
    IIC_Send_Byte2(add);      //写地址
    IIC_Wait_Ack2();          //等应答
    IIC_Send_Byte2(value>>8); //写高8位
    IIC_Wait_Ack2();
    IIC_Send_Byte2(value&0xff);//写低8位
    IIC_Wait_Ack2();
    IIC_Stop2();               //产生一个停止条件 
    delay_ms(10);	 
}
/*******************************************************************************
* 函数名	: ReadRegfdc2214 
* 描述	    : 读寄存器数据
* 输入参数  : add地址
* 返回参数  : 无
*******************************************************************************/
unsigned int ReadRegfdc22142(unsigned char add)
{
    u16 status;
    u16 a,b;
    IIC_Start2(); 
    IIC_Send_Byte2(0X54);	   //写命令ADDR=0
    IIC_Wait_Ack2();
    IIC_Send_Byte2(add);     //地址
    IIC_Wait_Ack2();
    IIC_Start2();            //重新开始
    IIC_Send_Byte2(0X55);	   //发送读命令ADDR=0
    IIC_Wait_Ack2(); 
    a=IIC_Read_Byte2(1);     //读高位
    b=IIC_Read_Byte2(0);		 //读低位
    status=(a<<8)+b;
    IIC_Stop2();
    return (status);
}
/*******************************************************************************
* 函数名	: InitSingleLDC1314 
* 描述	    : 初始化单通道
* 输入参数  : 无
* 返回参数  : 无
*******************************************************************************/
void InitSingleFDC22142(void)
{
    WriteRegfdc22142(0x08,0xFFFF);//设置转换时间  设置达到最高精度

    WriteRegfdc22142(0x10,0x0064);//FDC2214_SETTLECOUNT_CH0 计数

    WriteRegfdc22142(0x14,0x2001); //分频洗系数
    //    LDC_Write_Reg(0x0C,0x0F00); //调零
    WriteRegfdc22142(0x19,0x0000); //ERROE_CONFIG
	
    WriteRegfdc22142(0x1B,0x020D); //通道配置
    WriteRegfdc22142(0x1E,0xF800); //配置驱动电流
    WriteRegfdc22142(0x1A,0x1C81); //参数配置
}
/*******************************************************************************
* 函数名	: InitMultiLDC1314 
* 描述	    : 初始化多通道
* 输入参数  : 无
* 返回参数  : 无
*******************************************************************************/
void InitMultiFDC22142(void)//双通道
{
    WriteRegfdc22142(0x08,0x04D6);//转换时间
  	WriteRegfdc22142(0x09,0x04D6);
	WriteRegfdc22142(0x0A,0x04D6);//转换时间
  	WriteRegfdc22142(0x0B,0x04D6);
	
	WriteRegfdc22142(0x0C,0x0F00); //调0值 	CH0
	WriteRegfdc22142(0x0D,0x0F00); //调0值   CH1
	WriteRegfdc22142(0x0E,0x0F00); //调0值   CH2
	WriteRegfdc22142(0x0F,0x0F00); //调0值   CH3
  	
	WriteRegfdc22142(0x10,0x000A);//作用时间 CH1 
  	WriteRegfdc22142(0x11,0x000A);//CH2
	WriteRegfdc22142(0x12,0x000A);//CH3
	WriteRegfdc22142(0x13,0x000A);//CH4
  	
	WriteRegfdc22142(0x14,0x2002);//分频
  	WriteRegfdc22142(0x15,0x2002);
    WriteRegfdc22142(0x16,0x2002);//分频
  	WriteRegfdc22142(0x17,0x2002);
	
  	WriteRegfdc22142(0x19,0x0000);
  	WriteRegfdc22142(0x1B,0xA20D);//配置多通道   2通道--0x820D
	
  	WriteRegfdc22142(0x1E,0x9000); //驱动电流 CH0
  	WriteRegfdc22142(0x1F,0x9000); //CH1
	WriteRegfdc22142(0x20,0x9000); //CH2
  	WriteRegfdc22142(0x21,0x9000); //CH3
		  
//  	LDC_Write_Reg(0x1A,0x1401);


	WriteRegfdc22142(0x1A,0x3C81);//配置寄存器
	
}
	//Configuration register
	//	Active channel Select: b00 = ch0; b01 = ch1; b10 = ch2; b11 = ch3;
	//  |Sleep Mode: 0 - device active; 1 - device in sleep;
	//  ||Reserved, reserved, set to 1
	//  |||Sensor Activation Mode: 0 - drive sensor with full current. 1 - drive sensor with current set by DRIVE_CURRENT_CHn 
	//  ||||Reserved, set to 1
	//  |||||Reference clock: 0 - use internal; 1 - use external clock
	//  ||||||Reserved, set to 0
	//  |||||||Disable interrupt. 0 - interrupt output on INTB pin; 1 - no interrupt output
	//  ||||||||High current sensor mode: 0 - 1.5mA max. 1 - > 1.5mA, not available if Autoscan is enabled
	//  |||||||||     Reserved, set to 000001
	//  |||||||||     |
	// CCS1A1R0IH000000 -> 0001 1100 1000 0001 -> 0x1E81      0001 1100 1000 0001 -> 0x1C81


