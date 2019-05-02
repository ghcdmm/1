#pragma once
#include "stm32f1xx_hal.h"

void initlialze_datalink();
void datalink_main();
void datalink_msg_string_enocde(uint16_t len);
void datalink_pc_msg_string_enocde(uint16_t len);
