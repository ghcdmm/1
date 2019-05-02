#include "usart.h"

#include "../modules/datalink/datalink.h"
#include "../control_velocity/control_velocity.h"
#include "../modules/encoder/encoder.h"

static void datalink_msg_encode(uint8_t data);

uint8_t _msg = 0;
uint8_t _rxbuffer[128] = {0};
static	uint8_t _data[11]={0};

void initlialze_datalink()
{
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, &_msg, 128);
	//HAL_UART_Receive_IT(&huart1, &_msg, 1);
}

void datalink_main()
{
	int16_t counter[2]={0};

	uint8_t kp, ki, kd;
	
	get_counder(counter);
	get_pid_para(&kp, &ki ,&kd);
	
	_data[0]=0xfa;
	_data[1]=((int16_t)counter[0]) & 0xff;
	_data[2]=((int16_t)counter[0]>>8) & 0xff;
	_data[3]=((int16_t)counter[1]) & 0xff;
	_data[4]=((int16_t)counter[1]>>8) & 0xff;
	_data[5]= 0;
	_data[6]= 0;
	_data[7]=kp;  
	_data[8]=ki;	
	_data[9]=kd;
	_data[10]=0xaf;
	
	HAL_UART_Transmit_DMA(&huart2, _data,11);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}

void datalink_msg_string_enocde(uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		datalink_msg_encode(_rxbuffer[i]);
	}
	HAL_UART_Receive_DMA(&huart2, _rxbuffer, 128);
}

static void datalink_msg_encode(uint8_t data)
{           
	static uint8_t _state = 0;
	static uint8_t _data_len = 0;
	static uint8_t _data_cnt = 0;
	static uint8_t _rxbuffer[4] = {0};
	static int16_t desired_speed[2] = {0};
  static int i=0;
	
	
	if(_state==0 && data == 0xAA)	
	 {
		_state++;
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
				set_pid_para(_rxbuffer[0], _rxbuffer[1], _rxbuffer[2]);
		 }	
		else if(_data_cnt==2)
		 {
			desired_speed[0]=_rxbuffer[0]+(_rxbuffer[1]<<8)&0xff00;
			desired_speed[1]=_rxbuffer[2]+(_rxbuffer[3]<<8)&0xff00;			
			set_desired_speed(desired_speed);
		 }				 
		_state=0;
		_data_cnt=0;
		_data_len=0; 
	 }
 else 
 {
      _state=0;
		  _data_cnt=0;
      _data_len=0;
	    i=0;
 }
	 	
}



