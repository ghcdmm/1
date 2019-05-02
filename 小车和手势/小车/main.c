
#include "io430.h"
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

int Encoder_A,Encoder_B,Encoder_C; //编码器脉冲数
uchar fa,fb,fc; //编码器标志
long Position_A,Position_B,Position_C,Rate_A,Rate_B,Rate_C; //PID变量
long Motor_A,Motor_B,Motor_C; //PWM变量
long Target_A,Target_B,Target_C; //点击标准值
uchar delay_50,delay_flag; //不解释
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z; //坐标轴角度、标准速度
float	Position_KP=20,Position_KI=0.00,Position_KD=200; //PID参数
float Velocity_KP=12,Velocity_KI=12; //同上
/*
void Encoder_Init_timA0()//编码器
{
  TA0CTL=;
}*/

/*#pragma vector=PORT2_VECTOR //外部中断
__interrupt void oint()
{
  uint i;
  i=P1IFG;
  P1IFG=0;
  switch(i)
  {
    case 0x01:;
    case 0x02:;
    case 0x04:;
    case 0x08:;
    case 0x10:;
    case 0x20:;
    case 0x40:;
    case 0x80:;
  }
    
}*/

void clk_test()
{
  P1SEL|=BIT0;
  P1DIR|=BIT0;
  P2SEL|=BIT2;
  P2DIR|=BIT2;
  P7SEL|=BIT7;
  P7DIR|=BIT7;
}

void PutString(unsigned char *ptr)              //发送字符串
{
      while(*ptr != '\0')
      {
            while (!(UCA1IFG&UCTXIFG));                // TX缓存空闲？空闲的时候UTXIFG0为1，中断标志位IFG1为1(已经响应中断)
            UCA1TXBUF = *ptr++;                       // 发送数据   TXBUF0发送数据缓冲器把数据发送到PC机，然后指针地址加一
      }
      UCA1TXBUF = '\n';
}

#pragma vector=TIMER0_B0_VECTOR //定时器B中断
__interrupt void TB0_int()
{
  P4OUT^=BIT7+BIT1;
}

#pragma vector=TIMER0_A0_VECTOR //定时器A0中断
__interrupt void TA0_int()
{
  PutString("进入中断\n");
  P4OUT^=BIT1;
  P4OUT^=BIT7;
  PutString("中断执行完毕\n");
}
void TimerB_init()
{  
  
  TB0CTL|=TBCLR;
  TB0CTL|=TBSSEL_2+MC_1+TBIE;
  TB0CCTL0|=CCIE;
  TB0CCR0=30000;

}

void TimerA0_init()
{  
  TA0CTL|=TACLR;
  TA0CTL|=TASSEL_2+MC_1+TAIE;
  TA0CCTL0|=CCIE;
  TA0CCR0=30000;
}

void clk_init()
{
  //PMMCTL0=PMMCOREV_3;
  UCSCTL0=0;
  UCSCTL1=DCORSEL_6;
  UCSCTL2=FLLD_1|365;
  UCSCTL3=FLLREFDIV_0;
  UCSCTL4=UCSCTL4&(~(SELS_7|SELM_7))|SELS_3|SELM_3;
  UCSCTL5=UCSCTL5&(~(DIVPA_7|DIVA_7|DIVS_7|DIVM_7))|DIVPA_0|DIVA_0|DIVS_0|DIVM_0;
}

void uart_A1_init()   //串口A1的初始化
{
    P4SEL |= BIT4+BIT5;           //声明有特殊功能，不做普通I/OP3.3,4=USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;          //复位寄存器 8位数据，1位停止位，奇校验
    UCA1CTL1 |= UCSSEL_2;         //选择串口的时钟为ACLK
    UCA1BR0 = 0X0D;               //20M下115200    
    UCA1BR1 = 0;                        
    UCA1MCTL |=UCBRS_0 + UCBRF_10+1;   //根据寄存器设置
    UCA1CTL1 &= ~UCSWRST;             //初始化寄存器
    UCA1IE |= UCRXIE;                //使能USCI_A0_RX中断
    //__bis_SR_register(LPM0_bits + GIE);
    //__no_operation();
}

void ucs_init()                //系统时钟设置 MCLK=SMCLK=20M ACLK=2.5k
{
    UCSCTL6 |= XCAP_3;          //配置电容为12pF 0x000Cu
    UCSCTL6 &= ~XT1OFF;         //使能XT1 外部振荡源 32.768K
    //!提高Vcore 电压到最高级，以满足倍频需求该函数位于HAL_PMM.H
    P5SEL |= BIT4|BIT5;         //配置为XT1功能,电路板上晶振接于这两脚中
    __bis_SR_register(SCG0);    //将CPU总的SG0位置1
    UCSCTL0 = 0;                //DCO ：DCO 频拍选(不用管)
    UCSCTL1 = DCORSEL_6;        //DCO 频率范围选择；使能调制器
    UCSCTL2 = FLLD_1 | 305;     //FLLD=1,FLLN=305,则频率为2*（305+1）*32.768=20.054MHZ
    __bic_SR_register(SCG0);
    //_delay_cycles(782000);      //系统自带的精确延时，单位us
    while(SFRIFG1 & OFIFG) 
    {
            UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
            SFRIFG1 &= ~OFIFG;
    }
    UCSCTL4 = UCSCTL4&(~(SELS_7|SELM_7))|SELS_3|SELM_3; //0x0333u选择DCO 作为时钟源
}



int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  //WDTCTL = WDTPW + WDTCNTCL+WDTSSEL0+WDTIS2;
  unsigned int i;
  //clk_init();//全局时钟
  //延时初始化
  //编码器

  __disable_interrupt();
  UCSCTL4=UCSCTL4&(~(SELA_7))|SELA_3;
  UCSCTL5=UCSCTL5&(~(DIVPA_7|DIVA_0))|DIVPA_0|DIVA_0;
  clk_test();
  TimerA0_init();//定时器B
  P4DIR|=BIT7+BIT1;
  P3DIR|=BIT1;
  
  uart_A1_init();
  PutString("初始化成功\n");
  __enable_interrupt();
  PutString("打开总中断。\n");
  while(1)
  {
    P3OUT^=BIT1;
    for(i=0;i<30000;i++);
  }

  
}
