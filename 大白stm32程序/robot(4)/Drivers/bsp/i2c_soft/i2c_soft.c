#include "i2c_soft.h"


/*configuration*/
#define SCL_H         HAL_GPIO_WritePin(I2C_GPIO,I2C_Pin_SCL,GPIO_PIN_SET)
#define SCL_L         HAL_GPIO_WritePin(I2C_GPIO,I2C_Pin_SCL,GPIO_PIN_RESET)
#define SDA_H         HAL_GPIO_WritePin(I2C_GPIO,I2C_Pin_SDA,GPIO_PIN_SET)
#define SDA_L         HAL_GPIO_WritePin(I2C_GPIO,I2C_Pin_SDA,GPIO_PIN_RESET)
#define SCL_read      HAL_GPIO_ReadPin(I2C_GPIO,I2C_Pin_SCL)
#define SDA_read      HAL_GPIO_ReadPin(I2C_GPIO,I2C_Pin_SDA)

#define I2C_GPIO		GPIOB
#define I2C_Pin_SCL		i2c_pin_scl_Pin
#define I2C_Pin_SDA		i2c_pin_sda_Pin
static bool _use_fast_mode = false;

void i2c_soft_use_fast_mode(bool use_fast_mode)
{
	_use_fast_mode = use_fast_mode;
}

void i2c_soft_delay()
{ 
	__nop();__nop();__nop();
	__nop();__nop();__nop();
	__nop();__nop();__nop();

	if (_use_fast_mode==false)
	{
		int i = 15;
		while (i--);
	}

}


int i2c_soft_start()
{
	SDA_H;
	SCL_H;
	i2c_soft_delay();
	if(!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
	SDA_L;
	i2c_soft_delay();
	if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
	SDA_L;
	i2c_soft_delay();
	return 1;	

}

void i2c_soft_stop()
{
	SCL_L;
	i2c_soft_delay();
	SDA_L;
	i2c_soft_delay();
	SCL_H;
	i2c_soft_delay();
	SDA_H;
	i2c_soft_delay();
}

void i2c_soft_ask()
{
	SCL_L;
	i2c_soft_delay();
	SDA_L;
	i2c_soft_delay();
	SCL_H;
	i2c_soft_delay();
	SCL_L;
	i2c_soft_delay();
}

void i2c_soft_noask()
{
	SCL_L;
	i2c_soft_delay();
	SDA_H;
	i2c_soft_delay();
	SCL_H;
	i2c_soft_delay();
	SCL_L;
	i2c_soft_delay();
}

int i2c_soft_wait_ask(void) 	 //返回为:=1无ASK,=0有ASK
{
  uint8_t ErrTime = 0;
	SCL_L;
	i2c_soft_delay();
	SDA_H;			
	i2c_soft_delay();
	SCL_H;
	i2c_soft_delay();
	while(SDA_read)
	{
		ErrTime++;
		if(ErrTime>50)
		{
			i2c_soft_stop();
			return 1;
		}
	}
	SCL_L;
	i2c_soft_delay();
	return 0;
}

void i2c_soft_send_byte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        i2c_soft_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        i2c_soft_delay();
				SCL_H;
				i2c_soft_delay();
    }
    SCL_L;
}  

//读1个字节，ack=1时，发送ACK，ack=0，发送NACK
uint8_t i2c_soft_read_byte(uint8_t ask)  //数据从高位到低位//
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      i2c_soft_delay();
			SCL_H;
      i2c_soft_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;

	if (ask)
		i2c_soft_ask();
	else
		i2c_soft_noask();  
    return ReceiveByte;
} 


// IIC写一个字节数据
uint8_t i2c_write_1byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data)
{
	i2c_soft_start();
	i2c_soft_send_byte(SlaveAddress<<1);   
	if(i2c_soft_wait_ask())
	{
		i2c_soft_stop();
		return 1;
	}
	i2c_soft_send_byte(REG_Address);       
	i2c_soft_wait_ask();	
	i2c_soft_send_byte(REG_data);
	i2c_soft_wait_ask();   
	i2c_soft_stop(); 
	return 0;
}

// IIC读1字节数据
uint8_t i2c_read_1byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t *REG_data)
{      		
	i2c_soft_start();
	i2c_soft_send_byte(SlaveAddress<<1); 
	if(i2c_soft_wait_ask())
	{
		i2c_soft_stop();
		return 1;
	}
	i2c_soft_send_byte(REG_Address);     
	i2c_soft_wait_ask();
	i2c_soft_start();
	i2c_soft_send_byte(SlaveAddress<<1 | 0x01);
	i2c_soft_wait_ask();
	*REG_data= i2c_soft_read_byte(0);
	i2c_soft_stop();
	return 0;
}	

// IIC写n字节数据
uint8_t i2c_write_nbyte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{	
	i2c_soft_start();
	i2c_soft_send_byte(SlaveAddress<<1); 
	if(i2c_soft_wait_ask())
	{
		i2c_soft_stop();
		return 1;
	}
	i2c_soft_send_byte(REG_Address); 
	i2c_soft_wait_ask();
	while(len--) 
	{
		i2c_soft_send_byte(*buf++); 
		i2c_soft_wait_ask();
	}
	i2c_soft_stop();
	return 0;
}

// IIC读n字节数据
uint8_t i2c_read_nbyte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t len, uint8_t *buf)
{	
	i2c_soft_start();
	i2c_soft_send_byte(SlaveAddress<<1); 
	if(i2c_soft_wait_ask())
	{
		i2c_soft_stop();
		return 1;
	}
	i2c_soft_send_byte(REG_Address); 
	i2c_soft_wait_ask();
	
	i2c_soft_start();
	i2c_soft_send_byte(SlaveAddress<<1 | 0x01); 
	i2c_soft_wait_ask();
	while(len) 
	{
		if(len == 1)
		{
			*buf = i2c_soft_read_byte(0);
		}
		else
		{
			*buf = i2c_soft_read_byte(1);
		}
		buf++;
		len--;
	}
	i2c_soft_stop();
	return 0;
}


