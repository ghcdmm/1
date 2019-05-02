#include "bsp.h"
//初始化IIC
void IIC_Init2(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1); 	          //初始化高电平
}
//产生IIC起始信号，SDA拉低后SCL拉低
void IIC_Start2(void)
{
	SDA_OUT2();     //sda线输出
	WRITE_SDA2=1;	  	    
	IIC_SCL2=1;
	delay_us(2);
 	WRITE_SDA2=0;
	delay_us(2);
	IIC_SCL2=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop2(void)
{
	SDA_OUT2();//sda线输出
	IIC_SCL2=0;
	WRITE_SDA2=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	IIC_SCL2=1; 
	WRITE_SDA2=1;//发送I2C总线结束信号
	delay_us(2);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack2(void)
{
	u8 ucErrTime=0;
	SDA_IN2();      //SDA设置为输入  
	WRITE_SDA2=1;delay_us(1);	   
	IIC_SCL2=1;delay_us(1);	 
	while(READ_SDA2)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop2();
			return 1;
		}
	}
	IIC_SCL2=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack2(void)
{
	IIC_SCL2=0;
	SDA_OUT2();
	WRITE_SDA2=0;
	delay_us(2);
	IIC_SCL2=1;
	delay_us(2);
	IIC_SCL2=0;
}
//不产生ACK应答		    
void IIC_NAck2(void)
{
	IIC_SCL2=0;
	SDA_OUT2();
	WRITE_SDA2=1;
	delay_us(2);
	IIC_SCL2=1;
	delay_us(2);
	IIC_SCL2=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte2(u8 txd)
{                        
    u8 t;   
	  SDA_OUT2(); 	    
    IIC_SCL2=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {                   
		if((txd<<t)&0x80)
			WRITE_SDA2=1;
		else
			WRITE_SDA2=0; 
		delay_us(2);   //这三个延时都是必须的
		IIC_SCL2=1;
		delay_us(2); 
		IIC_SCL2=0;	
		delay_us(2);
    }	 
		
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte2(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN2();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL2=0; 
        delay_us(2);
		IIC_SCL2=1;
        receive<<=1;
        if(READ_SDA2)receive++;   
		delay_us(2); 
		IIC_SCL2=0;
		delay_us(2); 
    }					 
    if (!ack)
        IIC_NAck2();//发送nACK
    else
        IIC_Ack2(); //发送ACK   
    return receive;
}



























