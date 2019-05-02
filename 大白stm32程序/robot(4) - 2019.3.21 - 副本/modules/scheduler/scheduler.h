#pragma once
#include <stdint.h>

#define  SCHED_TASK_CLASS(func, _rate_hz, _max_time_micros) { \
    .function = func,\
    .name = #func,\
    .rate_hz = _rate_hz,\
    .max_time_micros = _max_time_micros\
}

struct  task
{
	void (*function)();
	const char *name;
	uint16_t rate_hz;
	uint16_t max_time_micros;
};
void initialize_scheduler(const struct task *tasks, uint8_t tasks_num);
void scheduler_run(void);
