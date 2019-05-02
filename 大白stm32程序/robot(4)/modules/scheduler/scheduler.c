#include "../modules/scheduler/scheduler.h"
#include "tim.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>

static const struct task *_tasks;

//number of tasks in _tasks list
static uint8_t _num_tasks;

// overall scheduling rate in Hz
static uint16_t _loop_rate_hz;

// tick counter at the time we last ran each task
static uint32_t *_last_run;

// number of microseconds allowed for the current task
static uint32_t _task_time_allowed;

// the time in microseconds when the task started
static uint32_t _task_time_started;

// number of spare microseconds accumulated
static uint32_t _spare_micros;

// number of ticks that _spare_micros is counted over
static uint8_t _spare_ticks;

void initialize_scheduler(const struct task *tasks, uint8_t tasks_num)
{
	_loop_rate_hz = 1000;

	_tasks = tasks;
	_num_tasks = tasks_num;
	_last_run=(uint32_t*)malloc(sizeof(_last_run[0]) * _num_tasks);
	memset(_last_run, 0, sizeof(_last_run[0]) * _num_tasks);

}


void scheduler_run(void)
{
	uint32_t time_available = 1000;
	uint32_t run_started_usec = get_micros();
	uint32_t now = run_started_usec;


	for (uint8_t i = 0; i < _num_tasks; i++)
	{
		uint16_t dt = HAL_GetTick() - _last_run[i];
		uint16_t interval_ticks = _loop_rate_hz / _tasks[i].rate_hz;
		if (interval_ticks < 1)
		{
			interval_ticks = 1;
		}
		if (dt >= interval_ticks) {
			// this task is due to run. Do we have enough time to run it?
			_task_time_allowed = _tasks[i].max_time_micros;

			if (dt >= interval_ticks * 2)
			{
				// we've slipped a whole run of this task!

			}

			if (_task_time_allowed <= time_available)
			{
				// run it
				_task_time_started = now;
				_tasks[i].function();
				_last_run[i] = HAL_GetTick();

				// work out how long the event actually took
				now = get_micros();
				uint32_t time_taken = now - _task_time_started;
				if (time_taken > _task_time_allowed)
				{
					// the event overran!

				}
				if (time_taken >= time_available)
				{
					goto update_spare_ticks;
				}
				time_available -= time_taken;
			}
		}
	}

	// update number of spare microseconds
	_spare_micros += time_available;

update_spare_ticks:
	_spare_ticks++;
	if (_spare_ticks == 32) {
		_spare_ticks /= 2;
		_spare_micros /= 2;
	}
}

/*
return number of micros until the current task reaches its deadline
*/
uint16_t get_time_available_usec(void)
{
	uint32_t dt = HAL_GetTick() - _task_time_started;
	if (dt > _task_time_allowed) 
	{
		return 0;
	}
	return _task_time_allowed - dt;
}

/*
calculate load average as a number from 0 to 1
*/
float get_load_average(uint32_t tick_time_usec) 
{
	if (_spare_ticks == 0) 
	{
		return 0.0f;
	}
	uint32_t used_time = tick_time_usec - (_spare_micros / _spare_ticks);
	return used_time / (float)tick_time_usec;
}
