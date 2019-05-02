#include "../modules/control_velocity/control_velocity.h"
#include "../modules/auto_disarming/auto_disarming.h"

static uint8_t _lost_cnt = 0;
static int16_t _armed_speed[2] = {0};

void auto_disarming_main(void)
{
	if(_lost_cnt)
	{
		_lost_cnt = 2;
		set_desired_speed(_armed_speed);
	}
	else
		_lost_cnt++;
}
void auto_disarming_clear_cnt(void)
{
	_lost_cnt = 0;
}

