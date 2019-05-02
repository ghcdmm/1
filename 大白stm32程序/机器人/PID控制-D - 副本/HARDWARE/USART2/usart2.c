
#include "usart2.h"

void datalink_msg_encode(uint8_t data);
	
 u8 mode_data[8];
 u8 six_data_stop[3]={0X59,0X59,0X59};  //ֹͣ��������
 u8 six_data_start[3]={0X58,0X58,0X58};  //������������
 extern int desired_speed[2];                                                                    
 /**************************************************************************
�������ܣ�����2��ʼ��
��ڲ�����pclk2:PCLK2 ʱ��Ƶ��(Mhz)    bound:������
����  ֵ����
**************************************************************************/
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRL&=0XFFFF00FF; 
	GPIOA->CRL|=0X00008B00;//IO״̬����
	GPIOA->ODR|=1<<2;	  
	RCC->APB1RSTR|=1<<17;   //��λ����1
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	USART2->CR1|=1<<8;    //PE�ж�ʹ��
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
}

/**************************************************************************
�������ܣ�����2�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
void USART2_IRQHandler(void)
{	
if(USART2->SR&(1<<5))//���յ�����
	{	  
		
	 static	int uart_receive=0;//����������ر���
	 uart_receive=USART2->DR; 
	 mode_data[0]=uart_receive;
		datalink_msg_encode(uart_receive);
//			if(uart_receive==0x01)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//			else if(uart_receive==0x02)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ǰ
//			else if(uart_receive==0x03)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////��
//			else if(uart_receive==0x04)// kp2++;				
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //��
//			else if(uart_receive==0x05)//	kp2--;  //��
//												Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//		mode_data[2]=mode_data[1];
//		mode_data[1]=mode_data[0];
	}  											 				 
} 




void datalink_msg_encode(uint8_t data)
{           
	static uint8_t _state = 0;
	static uint8_t _data_len = 0;
	static uint8_t _data_cnt = 0;
	static uint8_t _rxbuffer[4] = {0};
  static int i=0;

	
	if(_state==0 && data == 0xAA)	
	 {
		_state++;
	 Flag_Qian=0;Flag_Hou=0;Flag_Left=0;Flag_Right=0;
	 }
	else if(_state==1&&data == 0xFF)	
	 { 
		_state++;
	 }
	else if(_state==2)	
	 {
		if(data==0x02)
		{ 
			_data_len=3;
			_state++;
			_data_cnt=1;
		}
		else if(data==0x03)
		{
			_data_len=4;
			_state++;
			_data_cnt=2;
		}
		else
		{
			  _state=0;
		  _data_cnt=0;
      _data_len=0;
		}
	}
	 else if(_state>=3 && _state<(3+_data_len))
	 { 
		  if(i<_data_len)
				{				
	 	     _rxbuffer[i]=data;
					_state++;
					i++;					
				}	      
     if(i==_data_len)
		 {
			 i=0;
//			 kp2=_rxbuffer[0];
//			 kd2=_rxbuffer[0];
		 }
	}		
	else if(_state==(3+_data_len) && data == 0xFF)	
	 {
		   _state++;
   }	 		 
	else if( _state==(4+_data_len) && data == 0xAA)	
	 {
		 if(_data_cnt==1)
		 {
//Flag_Qian=0;Flag_Hou=0;Flag_Left=1;Flag_Right=0;
		 kp3=_rxbuffer[0];
		 ki3=_rxbuffer[1];
		 kd3=_rxbuffer[2];
		 }	
		else if(_data_cnt==2)
		 {
      //Flag_Qian=0;Flag_Hou=0;Flag_Left=0;Flag_Right=1;
			desired_speed[0]=_rxbuffer[0]+(_rxbuffer[1]<<8)&0xff00;
			desired_speed[1]=_rxbuffer[2]+(_rxbuffer[3]<<8)&0xff00;
		 }				 
		_state=0;
		_data_cnt=0;
		_data_len=0; 
	 }
 else 
 {
	 		// Flag_Qian=1;Flag_Hou=0;Flag_Left=0;Flag_Right=0;
      _state=0;
		  _data_cnt=0;
      _data_len=0;
	    i=0;
 }
	 	
}

//void datalink_msg_encode(uint8_t data)
//{
//		static uint8_t _state = 0;
//		static uint8_t _data_len = 0;
//		static uint8_t _rxbuffer[10] = {0};
//		static uint8_t _data_cnt = 0;
//		switch(_state)
//		{
//				case 0:
//						if(data == 0xaa)
//						{
//						Flag_Qian=0;Flag_Hou=0;Flag_Left=0;Flag_Right=0;
//								_state = 1;
//						}                   
//						break;
//				case 1:
//						if(data == 0xff)
//						{
//								_state = 2;
//						}                   
//					break;
//				case 2:
//					if(data == 0x02)
//					{
//						Flag_Qian=0;Flag_Hou=0;Flag_Left=1;Flag_Right=0;
//						_rxbuffer[_data_cnt++] = data;
//						_state = 3;
//						_data_len = 3;
//					}
//					else if(data == 0x03)
//					{
//						_rxbuffer[_data_cnt++] = data;
//						_state = 3;
//						_data_len = 4;
//					}
//					else 
//					{
//						_state = 0;
//						_data_cnt = 0;;
//					}
//					break;
//				case 3:
//						_data_len--;
//						if (_data_len != 0)
//						{
//								_rxbuffer[_data_cnt++] = data;
//						 }
//						else
//						{
//							Flag_Qian=0;Flag_Hou=0;Flag_Left=0;Flag_Right=0;
//								_rxbuffer[_data_cnt++] = data;
//								_state = 4;
//						}
//						break;   
//				case 4:
//					if(data == 0xff)
//					{
//						_state = 5;
//					}
//					break;						
//				case 5:
//						if(data == 0xaa)
//						{
//						datalink_msg_parser(_rxbuffer);
//						}
//						_state = 0;
//						_data_cnt = 0;
//						break;
//				default:
//						_state = 0;
//						_data_cnt = 0;
//						break;

//		}
//}

//void datalink_msg_parser(uint8_t rxbuffer[])
//{
//		switch(rxbuffer[0])
//		{
//			case 0x02:
//			Flag_Qian=0;Flag_Hou=0;Flag_Left=0;Flag_Right=1;

//				break;
//			case 0x03:
//			desired_speed[0]=rxbuffer[1]+(rxbuffer[2]<<8)&0xff00;
//			desired_speed[1]=rxbuffer[3]+(rxbuffer[4]<<8)&0xff00;
//				break;
//			default:
//				break;				
//		}

//}

void USART1_IRQHandler(void)
{	
if(USART1->SR&(1<<5))//?????
	{	  
		
	 uint8_t uart_receive=0;//????????
	 uart_receive=USART1->DR; 
		//datalink_msg_encode(uart_receive);
	}  											 				 
} 
