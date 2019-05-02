#include "bsp.h"
//��ʼ��IIC
void IIC_Init2(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1); 	          //��ʼ���ߵ�ƽ
}
//����IIC��ʼ�źţ�SDA���ͺ�SCL����
void IIC_Start2(void)
{
	SDA_OUT2();     //sda�����
	WRITE_SDA2=1;	  	    
	IIC_SCL2=1;
	delay_us(2);
 	WRITE_SDA2=0;
	delay_us(2);
	IIC_SCL2=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop2(void)
{
	SDA_OUT2();//sda�����
	IIC_SCL2=0;
	WRITE_SDA2=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	IIC_SCL2=1; 
	WRITE_SDA2=1;//����I2C���߽����ź�
	delay_us(2);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack2(void)
{
	u8 ucErrTime=0;
	SDA_IN2();      //SDA����Ϊ����  
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
	IIC_SCL2=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte2(u8 txd)
{                        
    u8 t;   
	  SDA_OUT2(); 	    
    IIC_SCL2=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {                   
		if((txd<<t)&0x80)
			WRITE_SDA2=1;
		else
			WRITE_SDA2=0; 
		delay_us(2);   //��������ʱ���Ǳ����
		IIC_SCL2=1;
		delay_us(2); 
		IIC_SCL2=0;	
		delay_us(2);
    }	 
		
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte2(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN2();//SDA����Ϊ����
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
        IIC_NAck2();//����nACK
    else
        IIC_Ack2(); //����ACK   
    return receive;
}



























