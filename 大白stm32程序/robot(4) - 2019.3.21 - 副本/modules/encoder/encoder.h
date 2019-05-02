#pragma	once

#include "stm32f1xx_hal.h"
void initialize_encoder(void);
void encoder_main();
void get_counder(int16_t counter[]);
