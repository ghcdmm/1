#include "bsp.h"

/*******************************************************************************
* ������	: WriteRegfdc2214 
* ����	    : д�Ĵ�������
* �������  : add��value
* ���ز���  : ��
*******************************************************************************/
void WriteRegfdc22142(u8 add,u16 value)
{
    IIC_Start2();  
    IIC_Send_Byte2(0X54);	 //ADDR=0ʱ����ַ0X2A<<1+0=0X54  
    IIC_Wait_Ack2();          //��Ӧ��
    IIC_Send_Byte2(add);      //д��ַ
    IIC_Wait_Ack2();          //��Ӧ��
    IIC_Send_Byte2(value>>8); //д��8λ
    IIC_Wait_Ack2();
    IIC_Send_Byte2(value&0xff);//д��8λ
    IIC_Wait_Ack2();
    IIC_Stop2();               //����һ��ֹͣ���� 
    delay_ms(10);	 
}
/*******************************************************************************
* ������	: ReadRegfdc2214 
* ����	    : ���Ĵ�������
* �������  : add��ַ
* ���ز���  : ��
*******************************************************************************/
unsigned int ReadRegfdc22142(unsigned char add)
{
    u16 status;
    u16 a,b;
    IIC_Start2(); 
    IIC_Send_Byte2(0X54);	   //д����ADDR=0
    IIC_Wait_Ack2();
    IIC_Send_Byte2(add);     //��ַ
    IIC_Wait_Ack2();
    IIC_Start2();            //���¿�ʼ
    IIC_Send_Byte2(0X55);	   //���Ͷ�����ADDR=0
    IIC_Wait_Ack2(); 
    a=IIC_Read_Byte2(1);     //����λ
    b=IIC_Read_Byte2(0);		 //����λ
    status=(a<<8)+b;
    IIC_Stop2();
    return (status);
}
/*******************************************************************************
* ������	: InitSingleLDC1314 
* ����	    : ��ʼ����ͨ��
* �������  : ��
* ���ز���  : ��
*******************************************************************************/
void InitSingleFDC22142(void)
{
    WriteRegfdc22142(0x08,0xFFFF);//����ת��ʱ��  ���ôﵽ��߾���

    WriteRegfdc22142(0x10,0x0064);//FDC2214_SETTLECOUNT_CH0 ����

    WriteRegfdc22142(0x14,0x2001); //��Ƶϴϵ��
    //    LDC_Write_Reg(0x0C,0x0F00); //����
    WriteRegfdc22142(0x19,0x0000); //ERROE_CONFIG
	
    WriteRegfdc22142(0x1B,0x020D); //ͨ������
    WriteRegfdc22142(0x1E,0xF800); //������������
    WriteRegfdc22142(0x1A,0x1C81); //��������
}
/*******************************************************************************
* ������	: InitMultiLDC1314 
* ����	    : ��ʼ����ͨ��
* �������  : ��
* ���ز���  : ��
*******************************************************************************/
void InitMultiFDC22142(void)//˫ͨ��
{
    WriteRegfdc22142(0x08,0x04D6);//ת��ʱ��
  	WriteRegfdc22142(0x09,0x04D6);
	WriteRegfdc22142(0x0A,0x04D6);//ת��ʱ��
  	WriteRegfdc22142(0x0B,0x04D6);
	
	WriteRegfdc22142(0x0C,0x0F00); //��0ֵ 	CH0
	WriteRegfdc22142(0x0D,0x0F00); //��0ֵ   CH1
	WriteRegfdc22142(0x0E,0x0F00); //��0ֵ   CH2
	WriteRegfdc22142(0x0F,0x0F00); //��0ֵ   CH3
  	
	WriteRegfdc22142(0x10,0x000A);//����ʱ�� CH1 
  	WriteRegfdc22142(0x11,0x000A);//CH2
	WriteRegfdc22142(0x12,0x000A);//CH3
	WriteRegfdc22142(0x13,0x000A);//CH4
  	
	WriteRegfdc22142(0x14,0x2002);//��Ƶ
  	WriteRegfdc22142(0x15,0x2002);
    WriteRegfdc22142(0x16,0x2002);//��Ƶ
  	WriteRegfdc22142(0x17,0x2002);
	
  	WriteRegfdc22142(0x19,0x0000);
  	WriteRegfdc22142(0x1B,0xA20D);//���ö�ͨ��   2ͨ��--0x820D
	
  	WriteRegfdc22142(0x1E,0x9000); //�������� CH0
  	WriteRegfdc22142(0x1F,0x9000); //CH1
	WriteRegfdc22142(0x20,0x9000); //CH2
  	WriteRegfdc22142(0x21,0x9000); //CH3
		  
//  	LDC_Write_Reg(0x1A,0x1401);


	WriteRegfdc22142(0x1A,0x3C81);//���üĴ���
	
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


