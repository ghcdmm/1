
#include "usart3.h"

/**************************************************************************
�������ܣ�����3��ʼ��
��ڲ�����pclk2:PCLK2 ʱ��Ƶ��(Mhz)    bound:������
����  ֵ����
**************************************************************************/
void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���ʱ�� 
	GPIOB->CRH&=0XFFFF00FF; 
	GPIOB->CRH|=0X00008B00;//IO״̬����
	GPIOB->ODR|=1<<10;	  
	RCC->APB1RSTR|=1<<18;   //��λ����1
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	   	   
	//����������
 	USART3->BRR=mantissa; // ����������	 
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	USART3->CR1|=1<<8;    //PE�ж�ʹ��
	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(0,1,USART3_IRQn,2);//��2��������ȼ� 
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
void USART3_IRQHandler(void)
{	
	if(USART3->SR&(1<<5))//���յ�����
	{	
		static u8 Flag_PID,i,j,Receive[50];
		static float Data;
		Usart3_Receive=USART3->DR;
	
	  if(Usart3_Receive==0x4B) Turn_Flag=1;  //����ת����ƽ���
	  else	if(Usart3_Receive==0x49||Usart3_Receive==0x4A) 	 Turn_Flag=0;	//������ƽ���
		
		if(Run_Flag==0)//�ٶȿ���ģʽ
		{			
				if(Turn_Flag==0)//�ٶȿ���ģʽ
				{
						if(Usart3_Receive>=0x41&&Usart3_Receive<=0x48)  
						{	
							Flag_Direction=Usart3_Receive-0x40;
						}
						else	if(Usart3_Receive<=8)   
						{			
							Flag_Direction=Usart3_Receive;
						}	
						else  Flag_Direction=0;
				}
				else	 if(Turn_Flag==1)//���������ת����ƽ���
				 {
				 if(Usart3_Receive==0x43) Flag_Left=0,Flag_Right=1;    
				 else if(Usart3_Receive==0x47) Flag_Left=1,Flag_Right=0;
				 else Flag_Left=0,Flag_Right=0;
				 if(Usart3_Receive==0x41||Usart3_Receive==0x45)Flag_Direction=Usart3_Receive-0x40;
				 else  Flag_Direction=0;
				 }
	  }	
		//��������APP���Խ���ͨѶ
		if(Usart3_Receive==0x7B) Flag_PID=1;   //APP����ָ����ʼλ
		if(Usart3_Receive==0x7D) Flag_PID=2;   //APP����ָ��ֹͣλ

		 if(Flag_PID==1)  //�ɼ�����
		 {
			Receive[i]=Usart3_Receive;
			i++;
		 }
		 if(Flag_PID==2)  //��������
		 {
			     if(Receive[3]==0x50) 	 PID_Send=1;
					 else  if(Receive[3]==0x57) 	 Flash_Send=1;
					 else  if(Receive[1]!=0x23) 
					 {								
						for(j=i;j>=4;j--)
						{
						  Data+=(Receive[j-1]-48)*pow(10,i-j);
						}
						switch(Receive[1])
						 {
							 case 0x30:  RC_Velocity=Data;break;
							 case 0x31:  RC_Position=Data;break;
							 case 0x32:  Position_KP=Data;break;
							 case 0x33:  Position_KI=Data;break;
							 case 0x34:  Position_KD=Data;break;
							 case 0x35:  Velocity_KP=Data;break;
							 case 0x36:  Velocity_KI=Data;break;
							 case 0x37:  break; //Ԥ��
							 case 0x38:  break; //Ԥ��
						 }
					 }				 
					 Flag_PID=0;//��ر�־λ����
					 i=0;
					 j=0;
					 Data=0;
					 memset(Receive, 0, sizeof(u8)*50);//��������
		 } 	 
	}  	
} 


/**************************************************************************
�������ܣ�����ɨ��
**************************************************************************/
u8 click_RC (void)
{
			static u8 flag_key=1;//�������ɿ���־
	    u8 temp;
			if(flag_key&&Usart3_Receive!=0x5A)
			{
			flag_key=0;
		  if(Usart3_Receive>=0x01&&Usart3_Receive<=0x08)temp=Usart3_Receive;
		  else	if(Usart3_Receive>=0x41&&Usart3_Receive<=0x48)temp=Usart3_Receive-0x40;	
		//	else 	temp=0;
			return temp;	// ��������
			}
			else if(Usart3_Receive==0x5A)			flag_key=1;
			return 0;//�ް�������
}

