#pragma once
#include "stm32f1xx_hal.h"
void initialize_mpu6050(uint16_t lpf);
void read_mpu6050_data(float accel[3], float gyro_angle[3], float *tempreature);

