#include "control.h"	
#include "filter.h"	
#include "ioi2c.h"
#include "usart.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
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
//////////IIC ���Ƴ�����//////////////
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

	

	
	if(TIM1->SR&0X0001)//5ms��ʱ�ж�
	{   
		  TIM1->SR&=~(1<<0);                                       //===�����ʱ��1�жϱ�־λ		 
		  Flag_Target=!Flag_Target;
		  if(Flag_Target==1)                                       //5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��
			{
			Read_DMP();
			Get_Angle(Way_Angle);                                    //===������̬	
			return 0;	
			}  			//10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
			
			Get_Angle(Way_Angle);
			Read_DMP();
			Encoder_Left=Read_Encoder(2);                           //===��ȡ��������ֵ��
			Encoder_Right=Read_Encoder(3);     											//===��ȡ��������ֵ
					

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
	  	                                    //===������̬	
  		                                         //===LED��˸;ָʾ��Ƭ����������	
			//Key();			//===ɨ�谴��״̬ ����˫�����Ըı�С������״̬
			
			current_speed[0]=Encoder_Left;
			current_speed[1]=Encoder_Right;
			
			if(n>=800)
			{			
				Led_Flash(100); 
			//�򵥵�ǰ������
			if(Flag_Qian==1)//�����ڳ�ʼ���������һ��Flagֵ�궨
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
						
				Xianfu_Pwm();  //===PWM�޷�
				
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
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias,kp=300,kd=1;
	 int balance;
	 Bias=Angle-6;              //===���ƽ��ĽǶ���ֵ �ͻ�е��� -0��ζ������������0�ȸ��� �������������5�ȸ��� �Ǿ�Ӧ�ü�ȥ5
	 balance=kp*Bias+Gyro*kd;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ����޸�Movement��ֵ�����磬�ĳ�-60��60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
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
�������ܣ�ת��PD����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int turn(float encoder_left,float encoder_right,float gyro)//ת�����
{
	  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=88/Flag_sudu,Kp=42,Kd=0;     
	  //=============ң��������ת����=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
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
	
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  Kd=1;        
		else Kd=0;   //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��
  	//=============ת��PD������=======================//
		  Turn=-Turn_Target*Kp -gyro*Kd;                 //===���Z�������ǽ���PD����
	  return Turn;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
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
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=5500 ;    //===PWM������7200 ������7100
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
	    u8 temp;
			if(angle<-40||angle>40||1==Flag_Stop)
			{	                                                 //===��Ǵ���40�ȹرյ��
      temp=1;                                            //===Flag_Stop��1�رյ��
			AIN1=0;                                            //===���������������¶ȹ���ʱ�رյ��
			AIN2=0;
			BIN1=0;
			BIN2=0;
      }
			else
      temp=0;
      return temp;			
}
	
/**************************************************************************
�������ܣ���ȡ�Ƕ�
��ڲ�������ȡ�Ƕȵ��㷨 1����  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(u8 way)
{ 
	    if(way==1)                                      //DMPû���漰���ϸ��ʱ�����⣬����������ȡ
			{	
			}			
      else
      {
			Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //��ȡY��������
			Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ��������
		  Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ�
	  	Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�
		  if(Gyro_Y>32768)  Gyro_Y-=65536;     //��������ת��  Ҳ��ͨ��shortǿ������ת��
			if(Gyro_Z>32768)  Gyro_Z-=65536;     //��������ת��
	  	if(Accel_X>32768) Accel_X-=65536;    //��������ת��
		  if(Accel_Z>32768) Accel_Z-=65536;    //��������ת��
			Gyro_Balance=-Gyro_Y;                                  //����ƽ����ٶ�
	   	//Accel_Y=atan2(Accel_X,Accel_Z)*180/PI;                 //���������ļн�	
		  Gyro_Y=Gyro_Y/16.4;                                    //����������ת��	
      if(Way_Angle==2)		  	Kalman_Filter(Accel_Z,-Gyro_Z);//�������˲�	
			else if(Way_Angle==3)   Yijielvbo(Accel_Y,-Gyro_Y);    //�����˲�
	    Angle_Balance=angle;                                   //����ƽ�����
			Gyro_Turn=Gyro_Z * 0.001064;                                      //����ת����ٶ�
			//GYROY = Accel_Y + GYROY; 
				
	  	}
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

