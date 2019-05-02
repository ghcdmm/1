#include "control.h"	
#include "filter.h"	
#include "ioi2c.h"
#include "usart.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Velocity_Pwm,Turn_Pwm,Yaw_2,delayx=0,YawFlag,Moto2flag,Flag1=0,Flag2=1,t=0,z=0,kp2=60,kd2=60;
float Accel_Y,Accel_X,Accel_Z,Gyro_Y,Gyro_Z,S,v,round1,round2,carvelocity,Velocity_Left,Velocity_Right;
u16 range1,RangeFinal;

int current_speed[2]={0},desired_speed[2]={500,500};
int kp3=0,ki3=0,kd3=0;

u32 range;
float n=0;
u8 Flag_Target;
union Max_Value
	{
		unsigned char buffers[8];
		struct _float_
			{
				float _float_left;
				float _float_right;
			}float_RAM;
	}Send_Data;
//////////IIC 控制超声波//////////////
//u32 Readiic(void)
//	{
//	 KS103_WriteOneByte(0xe8,0x02,0xb0);
//	delay_ms(100);
//		range= KS103_ReadOneByte(0xe8,0x02);
//		range<<=8;
//		range+=KS103_ReadOneByte(0xe8,0x03);
//	return (range);
//	}




int TIM1_UP_IRQHandler(void)  
{  
	int pidout= 0;
		
	static uint16_t pwm=4400;

	

	
	if(TIM1->SR&0X0001)//5ms定时中断
	{   
		  TIM1->SR&=~(1<<0);                                       //===清除定时器1中断标志位		 
		  Flag_Target=!Flag_Target;
		  if(Flag_Target==1)                                       //5ms读取一次陀螺仪和加速度计的值，更高的采样频率可以改善卡尔曼滤波和互补滤波的效果
			{
			Read_DMP();
			Get_Angle(Way_Angle);                                    //===更新姿态	
			return 0;	
			}  			//10ms控制一次，为了保证M法测速的时间基准，首先读取编码器数据
			
			Get_Angle(Way_Angle);
			Read_DMP();
			Encoder_Left=Read_Encoder(2);                           //===读取编码器的值，
			Encoder_Right=Read_Encoder(3);     											//===读取编码器的值
					

			if(z==8)
			{
				z=0;
				round1 = Encoder_Left * 0.002;
				round2 = Encoder_Right * 0.002;
				Velocity_Left = round1 * 2* PI * 0.1;
				Velocity_Right = round2 * 2 * PI * 0.1;
				carvelocity = (Velocity_Right + Velocity_Left) * 0.5;
				Send_Data.float_RAM._float_left =  carvelocity;
				Send_Data.float_RAM._float_right = Gyro_Turn;
				USART1->DR =0xFA;
				while((USART1->SR&0x40)==0);
				for(t=0;t<8;t++)
				{
					USART1->DR = Send_Data.buffers[t];
					while((USART1->SR&0x40)==0);
				}
				USART1->DR =0xAF;
				while((USART1->SR&0x40)==0);
		  }
			z++;
	  	                                    //===更新姿态	
  		                                         //===LED闪烁;指示单片机正常运行	
			//Key();			//===扫描按键状态 单击双击可以改变小车运行状态
			
			current_speed[0]=Encoder_Left;
			current_speed[1]=Encoder_Right;
			
			if(n>=800)
			{			
				Led_Flash(100); 
			//简单的前后左右
			if(Flag_Qian==1)//让其在初始化后仅进行一次Flag值标定
				{
					if(Flag2==1)
					{
						YawFlag = Yaw;
						Flag2=0;
					}
					
				pidout = velocity(Yaw,YawFlag);
				Moto1=pwm + pidout;
				Moto2=pwm - pidout;	
				
   	//	get_pid_output(desired_speed,current_speed,pwm);
					
//				Moto1 = pwm[0];
//				Moto2 = pwm[1];
				
				}
				
				if(Flag_Qian==0&&Flag_Hou==0&&Flag_Left==0&&Flag_Right==0)
				{
					Moto1=0;
					Moto2=0;
					Flag2=1;
				}
				Yaw_2 = Yaw;
				
				if(Flag_Right==1)
				{
					Flag2=1;
					Moto1=0;
					Moto2=4650;
					
				}
				if(Flag_Left==1)
				{
					Flag2=1;
					Moto1=4650;
					Moto2=0;
					
				}
			
				if(Flag_Hou==1)
				{
					Flag2=1;
					Moto1=-4650;
					Moto2=-4650;
					
				}
						
				Xianfu_Pwm();  //===PWM限幅
				
				if(KEY == 1 )
				{
				Set_Pwm(Moto1,Moto2);
				}
				else
				{
					Moto1 = 0;
					Moto2flag=Moto2;
					Moto2 = 0;
					Set_Pwm(Moto1,Moto2);
					Moto2 = Moto2flag;
				}
							 }	
	
			n++;
		  							
	}       	
	 return 0;	  
} 
 
/**************************************************************************
函数功能：直立PD控制
入口参数：角度、角速度
返回  值：直立控制PWM
作    者：平衡小车之家
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias,kp=300,kd=1;
	 int balance;
	 Bias=Angle-6;              //===求出平衡的角度中值 和机械相关 -0意味着身重中心在0度附近 如果身重中心在5度附近 那就应该减去5
	 balance=kp*Bias+Gyro*kd;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	 return balance;
}

/**************************************************************************
函数功能：速度PI控制 修改前进后退速度，请修改Movement的值，比如，改成-60和60就比较慢了
入口参数：左轮编码器、右轮编码器
返回  值：速度控制PWM
作    者：平衡小车之家
**************************************************************************/

int velocity(int yaw,int yaw_target)
{
	
	static int error_pre = 0;
	static int error_last = 0;
	int pid_out=0;

	error_pre = error_last;
		
	if(yaw < 0)
		yaw = 360 + yaw;
		
		
	error_last = yaw_target - yaw;

	if(error_last > 180)		
		error_last -= 360;
	else if(error_last < -180)
		error_last += 360;
	
	
	pid_out = kp2 * error_last + kd2 * (error_last - error_pre);

	
	if(pid_out > 100)
		pid_out = 100;
	
	return pid_out;
	
}

void get_pid_output(int desired_speed[2], int current_speed[2],int output[2])
{
	
	static float _output[2] = { 0,0};
	static float _current_error[2] = { 0 };
	static float _error_last[2] = { 0 };
	static float _error_pre[2] = { 0 };
	int i;
	
	for (i=0;i<2;i++)
	{
		_error_pre[i] = _error_last[i];
		_error_last[i] = _current_error[i];
		_current_error[i] = desired_speed[i] - current_speed[i];
	
		_output[i] += kp3 * (_current_error[i] - _error_last[i]) + ki3 * _current_error[i] + kd3 * (_current_error[i] - 2 * _error_last[i] + _error_pre[i]);
		if (_output[i] > 6000)
		{
			_output[i] = 6000;
		}
		else if (_output[i] < -6000)
			_output[i] = -6000;
		output[i] = _output[i];
	}
}



/**************************************************************************
函数功能：转向PD控制
入口参数：左轮编码器、右轮编码器、Z轴陀螺仪
返回  值：转向控制PWM
作    者：平衡小车之家
**************************************************************************/
int turn(float encoder_left,float encoder_right,float gyro)//转向控制
{
	  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=88/Flag_sudu,Kp=42,Kd=0;     
	  //=============遥控左右旋转部分=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
		{
			if(++Turn_Count==1)
			Encoder_temp=myabs(encoder_left+encoder_right);
			Turn_Convert=50/Encoder_temp;
			if(Turn_Convert<0.6)Turn_Convert=0.6;
			if(Turn_Convert>3)Turn_Convert=3;
		}	
	  else
		{
			Turn_Convert=0.9;
			Turn_Count=0;
			Encoder_temp=0;
		}			
		if(1==Flag_Left)	           Turn_Target-=Turn_Convert;
		else if(1==Flag_Right)	     Turn_Target+=Turn_Convert; 
		else Turn_Target=0;
	
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===转向速度限幅
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  Kd=1;        
		else Kd=0;   //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
  	//=============转向PD控制器=======================//
		  Turn=-Turn_Target*Kp -gyro*Kd;                 //===结合Z轴陀螺仪进行PD控制
	  return Turn;
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
			if(moto1>0)			AIN2=1,			AIN1=0;
			else 	          AIN2=0,			AIN1=1;
			PWMA=myabs(moto1);
		  if(moto2<0)	BIN1=1,			BIN2=0;
			else        BIN1=0,			BIN2=1;
			PWMB=myabs(moto2);	
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=5500 ;    //===PWM满幅是7200 限制在7100
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：倾角和电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
	    u8 temp;
			if(angle<-40||angle>40||1==Flag_Stop)
			{	                                                 //===倾角大于40度关闭电机
      temp=1;                                            //===Flag_Stop置1关闭电机
			AIN1=0;                                            //===可自行增加主板温度过高时关闭电机
			AIN2=0;
			BIN1=0;
			BIN2=0;
      }
			else
      temp=0;
      return temp;			
}
	
/**************************************************************************
函数功能：获取角度
入口参数：获取角度的算法 1：无  2：卡尔曼 3：互补滤波
返回  值：无
**************************************************************************/
void Get_Angle(u8 way)
{ 
	    if(way==1)                                      //DMP没有涉及到严格的时序问题，在主函数读取
			{	
			}			
      else
      {
			Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //读取Y轴陀螺仪
			Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //读取Z轴陀螺仪
		  Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //读取X轴加速度记
	  	Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //读取Z轴加速度记
		  if(Gyro_Y>32768)  Gyro_Y-=65536;     //数据类型转换  也可通过short强制类型转换
			if(Gyro_Z>32768)  Gyro_Z-=65536;     //数据类型转换
	  	if(Accel_X>32768) Accel_X-=65536;    //数据类型转换
		  if(Accel_Z>32768) Accel_Z-=65536;    //数据类型转换
			Gyro_Balance=-Gyro_Y;                                  //更新平衡角速度
	   	//Accel_Y=atan2(Accel_X,Accel_Z)*180/PI;                 //计算与地面的夹角	
		  Gyro_Y=Gyro_Y/16.4;                                    //陀螺仪量程转换	
      if(Way_Angle==2)		  	Kalman_Filter(Accel_Z,-Gyro_Z);//卡尔曼滤波	
			else if(Way_Angle==3)   Yijielvbo(Accel_Y,-Gyro_Y);    //互补滤波
	    Angle_Balance=angle;                                   //更新平衡倾角
			Gyro_Turn=Gyro_Z * 0.001064;                                      //更新转向角速度
			//GYROY = Accel_Y + GYROY; 
				
	  	}
}
/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

