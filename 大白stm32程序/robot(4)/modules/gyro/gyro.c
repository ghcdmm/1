#include "../modules/gyro/gyro.h"
#include "../Drivers/bsp/mpu6050/mpu6050.h"

static float _accel[3] = {0};
static float _gyro[3] = {0};
static float _tempreature = 0;
static float _yaw = 0;

void initialize_gyro(void)
{
	initialize_mpu6050(20);
}

void gyro_main(void)
{
	read_mpu6050_data(_accel, _gyro, &_tempreature);
	_yaw += _gyro[2] * 0.002f;
}

float get_gyro(void)
{
	return _gyro[2] / 180.0f * 3.1415926;
}

float get_yaw(void)
{
	return _yaw;
}
