
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "../modules/control_velocity/control_velocity.h"
#include "../modules/encoder/encoder.h"

static void set_pwm(int16_t pwm[]);

static uint8_t _kp = 1 ;//40
static uint8_t _ki = 1 ;//6
static uint8_t _kd = 1 ;//0

static float _output[2] = { 0,0};
static int16_t _pwm[2] = { 0,0};
static float _current_error[2] = { 0 };
static float _error_last[2] = { 0 };
static float _error_pre[2] = { 0 };

static int16_t _desired_speed[2] = { 0 };

void initialize_motors(void)
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void control_velocity_main(void)
{
	int16_t counter[2]={0};
	get_counder(counter);
	
	
	for (int i=0;i<2;i++)
	{
		
		if(_desired_speed[i] == 0)
		{
			_error_pre[i] = 0;
			_error_last[i] = 0;
			_current_error[i] = 0;
			_output[i] = 0;
		}
		else
		{
			_error_pre[i] = _error_last[i];
			_error_last[i] = _current_error[i];
			_current_error[i] = _desired_speed[i] - counter[i];
		
			_output[i] += 0.1f * ( _kp * (_current_error[i] - _error_last[i]) + _ki * _current_error[i] + _kd * (_current_error[i] - 2 * _error_last[i] + _error_pre[i]));
		}			
			_pwm[i] = _output[i];	
     //_pwm[i]=0;		

		set_pwm(_pwm);
	
	
	}
}
void set_pid_para(uint8_t kp ,uint8_t ki, uint8_t kd)
{
	_kp = kp;
	_ki = ki;
	_kd = kd;
}

void get_pid_para(uint8_t* kp ,uint8_t* ki, uint8_t* kd)
{
		*kp = _kp ;
		*ki = _ki ;
		*kd = _kd ;	
}

void set_desired_speed(const int16_t desired_speed[])
{
	_desired_speed[0] = desired_speed[0];
	_desired_speed[1] = desired_speed[1];
}

void get_desired_speed(int16_t desired_speed[])
{
	desired_speed[0] = _desired_speed[0];
	desired_speed[1] = _desired_speed[1];
}


static inline uint16_t abs(int16_t a)
{ 		   
	  uint16_t temp;
		if(a<0)  
			temp=-a;  
	  else 
			temp=a;
	  return temp;
}

static void set_pwm(int16_t pwm[])
{
	if(pwm[0] > 0)
	{
			HAL_GPIO_WritePin(MOTORA2_GPIO_Port, MOTORA2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MOTORA1_GPIO_Port, MOTORA1_Pin, GPIO_PIN_RESET);
	}
	else
	{
			HAL_GPIO_WritePin(MOTORA2_GPIO_Port, MOTORA2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(MOTORA1_GPIO_Port, MOTORA1_Pin, GPIO_PIN_SET);
	}

	if(pwm[1] < 0)
	{
			HAL_GPIO_WritePin(MOTORB1_GPIO_Port, MOTORB1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MOTORB2_GPIO_Port, MOTORB2_Pin, GPIO_PIN_RESET);
	}
	else
	{
			HAL_GPIO_WritePin(MOTORB1_GPIO_Port, MOTORB1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(MOTORB2_GPIO_Port, MOTORB2_Pin, GPIO_PIN_SET);
	}
	
	for(int i = 0; i<2; i++)
	{
				
		if (pwm[i] > 4800)
		{
			pwm[i] = 4800;
		}
		else if (pwm[i] < -4800)
			pwm[i] = -4800;
		
	}
//	printf("%d %d\r\n",pwm[0],pwm[1]);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, abs(pwm[0]));
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, abs(pwm[1]));
	
}

