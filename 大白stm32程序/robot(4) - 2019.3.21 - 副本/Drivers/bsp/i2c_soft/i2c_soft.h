#pragma once
#include "stm32f1xx_hal.h"
#include <stdbool.h>

void i2c_soft_use_fast_mode(bool use_fast_mode);
void i2c_soft_send_byte(uint8_t SendByte);
uint8_t i2c_soft_read_byte(uint8_t);

//int I2c_Soft_Single_Write(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
//int I2c_Soft_Single_Read(uint8_t SlaveAddress,uint8_t REG_Address);
//int I2c_Soft_Mult_Read(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t * ptChar,uint8_t size);

uint8_t i2c_write_1byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
uint8_t i2c_read_1byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t *REG_data);
uint8_t i2c_write_nbyte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);
uint8_t i2c_read_nbyte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf);

