#pragma	once

#include "stm32f1xx_hal.h"

void initialize_motors(void);
void control_velocity_main(void);

void set_pid_para(uint8_t kp ,uint8_t ki, uint8_t kd);
void get_pid_para(uint8_t* kp ,uint8_t* ki, uint8_t* kd);
void set_desired_speed(const int16_t desired_speed[]);
void get_desired_speed(int16_t desired_speed[]);
