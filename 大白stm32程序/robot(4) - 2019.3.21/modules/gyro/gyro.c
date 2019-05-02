#include "../modules/gyro/gyro.h"
#include "../Drivers/bsp/mpu6050/mpu6050.h"
#include "../datalink/datalink.h"

static float _accel[3] = {0};
static float _gyro[3] = {0};
static float _tempreature = 0;
static float _yaw = 0;

//void initialize_gyro(void)
//{
//	initialize_mpu6050(20);
//}

void read_mpu6050_data_uart(float accel[3], float gyro_angle[3], float *tempreature)
{
	gyro_angle[0]=(float)(((signed char)gyro_rec[1]<<8)+gyro_rec[0])/32768*2000;
	gyro_angle[1]=(float)(((signed char)gyro_rec[3]<<8)+gyro_rec[2])/32768*2000;
	gyro_angle[2]=(float)(((signed char)gyro_rec[5]<<8)+gyro_rec[4])/32768*2000;
	*tempreature=(float)(((signed char)gyro_rec[7]<<8)+gyro_rec[6])/340+36.53;
}


void gyro_main(void)
{
	read_mpu6050_data_uart(_accel, _gyro, &_tempreature);
	_yaw += _gyro[2] * 0.002f;
}

float get_gyro(void)
{
//	printf("\r\n%f\r\n",_gyro[2]);
	return _gyro[2] / 180.0f * 3.1415926;
}

float get_yaw(void)
{
	return _yaw;
}
