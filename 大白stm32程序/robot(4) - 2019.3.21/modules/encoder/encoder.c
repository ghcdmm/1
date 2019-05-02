#include "../modules/encoder/encoder.h"
#include "tim.h"
#include <string.h>

static int16_t _counter[2] = {0};

void initialize_encoder(void)
{
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);

}

void encoder_main()
{
	_counter[1] = __HAL_TIM_GET_COUNTER(&htim2);
	_counter[0] = __HAL_TIM_GET_COUNTER(&htim1);
//	printf("%d	%d\r\n",_counter[1],_counter[0]);
	__HAL_TIM_SET_COUNTER(&htim2,0);
	__HAL_TIM_SET_COUNTER(&htim1,0);
}

void get_counder(int16_t counter[])
{
		memcpy(counter, _counter, sizeof(_counter[0]) * 2);
}

