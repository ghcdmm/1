#include "usart.h"
#include "tim.h"

#include "../modules/datalink/datalink.h"
#include "../control_velocity/control_velocity.h"
#include "../modules/encoder/encoder.h"
#include "../modules/gyro/gyro.h"
#include "../modules/auto_disarming/auto_disarming.h"

unsigned char gyro_rec[20]={0};
unsigned char k=0,stat=0,j=0,sum=0;

static union rxbuffer 
{
	unsigned char buffers[8];
	struct rx_data
		{
			float vel;
			float omega;
		}data;
}_recive_data_un;

static union txbuffer 
{
	unsigned char buffers[8];
	struct tx_data
		{
			float vel;
			float gyro;
		}data;
}_data_to_send_un;


static void datalink_msg_encode(uint8_t data);

uint8_t _msg = 0;
uint8_t _rxbuffer[128] = {0};
uint8_t _rxbuffer_pc[128] = {0};
static	uint8_t _data[11]={0};
static	uint8_t _data_pc[10]={0};
static float const PI = 3.14159265;
static float const L = 0.426;
void initlialze_datalink()
{
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, &_msg, 128);
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, _rxbuffer_pc, 128);
	
	//HAL_UART_Receive_IT(&huart1, &_msg, 1);
	while(huart3.Instance->CR3!=1)
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_ERR);
	HAL_UART_Receive_IT(&huart3, &Uart3_recb, 1);
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
}

void datalink_main()
{
	int16_t counter[2]={0};
	float gyro = 0;
	uint8_t kp, ki, kd;
	
	get_counder(counter);
	get_pid_para(&kp, &ki ,&kd);
	gyro = get_gyro();
	
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
	
	
	_data_to_send_un.data.gyro = gyro;
	_data_to_send_un.data.vel = (counter[0] + counter[1])*0.5 * 0.02 * 2* PI * 0.036*17;//(counter[0] + counter[1])*0.5 * 0.002 * 2* PI * 0.1;
	
	_data_pc[0] = 0xfa;
	
	for(int i =0; i < 8; i++)
		_data_pc[1 + i] = _data_to_send_un.buffers[i];
		
	_data_pc[9] = 0xaf;

	
	HAL_UART_Transmit_DMA(&huart2, _data,11);
	HAL_UART_Transmit_DMA(&huart1, _data_pc,10);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static unsigned char tim3_cnt=0;
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
	if (htim==&htim3)
    {
			tim3_cnt++;
    }

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart); 
	__HAL_UART_CLEAR_OREFLAG(huart);
	k=0;
	stat=0;
	HAL_UART_Receive_IT(&huart3, &Uart3_recb, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	if(huart==&huart3)
	{
		if(Uart3_recb==0x55&&stat==0)
		{
			stat++;
			__HAL_TIM_ENABLE(&htim3);
		}
		else if(Uart3_recb==0x52&&stat==1)
			stat++;
		else if(stat>=2&&stat<11)
		{
			Uart3_rec[k++]=Uart3_recb;
			stat++;
		}
		else if(stat==11)
		{
			sum=0x55+0x52;
			for(j=0;j<k-1;j++)
				sum+=Uart3_rec[j];
			if(sum==Uart3_rec[k-1])
			{
				memcpy(gyro_rec, Uart3_rec, sizeof(unsigned char)*20);
			}
			sum=0;
			k=0;
			stat=0;
			__HAL_TIM_DISABLE(&htim3);
		}
		else
		{
			k=0;stat=0;
		}
		HAL_UART_Receive_IT(&huart3, &Uart3_recb, 1);
	}
}

void datalink_msg_string_enocde(uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		datalink_msg_encode(_rxbuffer[i]);
	}
	//auto_disarming_clear_cnt();
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
			desired_speed[0]=_rxbuffer[0]+((_rxbuffer[1]<<8)&0xff00);
			desired_speed[1]=_rxbuffer[2]+((_rxbuffer[3]<<8)&0xff00);				 
			
			auto_disarming_clear_cnt();			 
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

static void datalink_pc_msg_encode(uint8_t data)
{
	static uint8_t _state = 0;
	static uint8_t _data_len = 0;
	static uint8_t _data_cnt = 0;
	
	float desired_speed_float[2] = {0};
	int16_t desired_speed_int16[2] = {0};
	
	switch(_state)
	{
			case 0:
					if(data == 0xaa)
					{
							_state = 1;
					}                   
					break;
			case 1:
					if(data == 0xaa)
					{
							_state = 2;
							_data_len = 8;
					}                   
					break;
			case 2:
					_data_len--;
					if (_data_len != 0)
					{
							_rxbuffer[_data_cnt++] = data;
					 }
					else
					{
							_rxbuffer[_data_cnt++] = data;
							_state = 4;
					}
					break;
			case 4:
					if(data == 0xff)
					{
							_state = 5;
					}                   
					break;					
			case 5:
					if(data == 0xff)
					{
						for(int i = 0; i < 8; i++)
							_recive_data_un.buffers[i] = _rxbuffer[i];
						
						desired_speed_float[0] = (2 * _recive_data_un.data.vel + _recive_data_un.data.omega * L)/2.0;
						desired_speed_float[1] = (2 * _recive_data_un.data.vel - _recive_data_un.data.omega * L)/2.0;
						for(int i = 0; i < 2; i++)
						{  
							desired_speed_float[i] = desired_speed_float[i] /(0.002 * 2* PI * 0.1);
							desired_speed_int16[i] = desired_speed_float[i];
						}
             	auto_disarming_clear_cnt();
							set_desired_speed(desired_speed_int16);
					}
					else
					{
						_state = 0;
						_data_cnt = 0;
					}
					_state = 0;
					_data_cnt = 0;
					break;
			default:
					_state = 0;
					_data_cnt = 0;
					break;

	}
}	

void datalink_pc_msg_string_enocde(uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		datalink_pc_msg_encode(_rxbuffer_pc[i]);
	}
	HAL_UART_Receive_DMA(&huart1, _rxbuffer_pc, 128);
}
