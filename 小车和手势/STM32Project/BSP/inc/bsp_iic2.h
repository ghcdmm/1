#ifndef __BSP_IIC2_H
#define __BSP_IIC2_H

////IO��������	 
#define SDA_IN2()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=8<<4;}
#define SDA_OUT2() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=3<<4;}

#define IIC_SCL2    PBout(0)   //SCL
#define WRITE_SDA2    PBout(1) //д��SDA	 
#define READ_SDA2   PBin(1)    //��ȡSDA 


//IIC���в�������
void IIC_Init2(void);                //��ʼ��IIC��IO��				 
void IIC_Start2(void);				        //����IIC��ʼ�ź�
void IIC_Stop2(void);	  			      //����IICֹͣ�ź�
void IIC_Send_Byte2(u8 txd);			    //IIC����һ���ֽ�
u8 IIC_Read_Byte2(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack2(void); 				      //IIC�ȴ�ACK�ź�
void IIC_Ack2(void);					        //IIC����ACK�ź�
void IIC_NAck2(void);				        //IIC������ACK�ź�

void IIC_Write_One_Byte2(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte2(u8 daddr,u8 addr);	  
#endif
















