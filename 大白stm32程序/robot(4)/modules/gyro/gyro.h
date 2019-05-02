#pragma once
#include "stm32f1xx_hal.h"
void initialize_gyro(void);
void gyro_main(void);
float get_gyro(void);
float get_yaw(void);
