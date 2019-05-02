#ifndef __BSP_IIC2_H
#define __BSP_IIC2_H

////IO操作函数	 
#define SDA_IN2()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=8<<4;}
#define SDA_OUT2() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=3<<4;}

#define IIC_SCL2    PBout(0)   //SCL
#define WRITE_SDA2    PBout(1) //写入SDA	 
#define READ_SDA2   PBin(1)    //读取SDA 


//IIC所有操作函数
void IIC_Init2(void);                //初始化IIC的IO口				 
void IIC_Start2(void);				        //发送IIC开始信号
void IIC_Stop2(void);	  			      //发送IIC停止信号
void IIC_Send_Byte2(u8 txd);			    //IIC发送一个字节
u8 IIC_Read_Byte2(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack2(void); 				      //IIC等待ACK信号
void IIC_Ack2(void);					        //IIC发送ACK信号
void IIC_NAck2(void);				        //IIC不发送ACK信号

void IIC_Write_One_Byte2(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte2(u8 daddr,u8 addr);	  
#endif
















