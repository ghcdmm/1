
#include "io430.h"
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

int Encoder_A,Encoder_B,Encoder_C; //������������
uchar fa,fb,fc; //��������־
long Position_A,Position_B,Position_C,Rate_A,Rate_B,Rate_C; //PID����
long Motor_A,Motor_B,Motor_C; //PWM����
long Target_A,Target_B,Target_C; //�����׼ֵ
uchar delay_50,delay_flag; //������
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z; //������Ƕȡ���׼�ٶ�
float	Position_KP=20,Position_KI=0.00,Position_KD=200; //PID����
float Velocity_KP=12,Velocity_KI=12; //ͬ��
/*
void Encoder_Init_timA0()//������
{
  TA0CTL=;
}*/

/*#pragma vector=PORT2_VECTOR //�ⲿ�ж�
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

void PutString(unsigned char *ptr)              //�����ַ���
{
      while(*ptr != '\0')
      {
            while (!(UCA1IFG&UCTXIFG));                // TX������У����е�ʱ��UTXIFG0Ϊ1���жϱ�־λIFG1Ϊ1(�Ѿ���Ӧ�ж�)
            UCA1TXBUF = *ptr++;                       // ��������   TXBUF0�������ݻ����������ݷ��͵�PC����Ȼ��ָ���ַ��һ
      }
      UCA1TXBUF = '\n';
}

#pragma vector=TIMER0_B0_VECTOR //��ʱ��B�ж�
__interrupt void TB0_int()
{
  P4OUT^=BIT7+BIT1;
}

#pragma vector=TIMER0_A0_VECTOR //��ʱ��A0�ж�
__interrupt void TA0_int()
{
  PutString("�����ж�\n");
  P4OUT^=BIT1;
  P4OUT^=BIT7;
  PutString("�ж�ִ�����\n");
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

void uart_A1_init()   //����A1�ĳ�ʼ��
{
    P4SEL |= BIT4+BIT5;           //���������⹦�ܣ�������ͨI/OP3.3,4=USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;          //��λ�Ĵ��� 8λ���ݣ�1λֹͣλ����У��
    UCA1CTL1 |= UCSSEL_2;         //ѡ�񴮿ڵ�ʱ��ΪACLK
    UCA1BR0 = 0X0D;               //20M��115200    
    UCA1BR1 = 0;                        
    UCA1MCTL |=UCBRS_0 + UCBRF_10+1;   //���ݼĴ�������
    UCA1CTL1 &= ~UCSWRST;             //��ʼ���Ĵ���
    UCA1IE |= UCRXIE;                //ʹ��USCI_A0_RX�ж�
    //__bis_SR_register(LPM0_bits + GIE);
    //__no_operation();
}

void ucs_init()                //ϵͳʱ������ MCLK=SMCLK=20M ACLK=2.5k
{
    UCSCTL6 |= XCAP_3;          //���õ���Ϊ12pF 0x000Cu
    UCSCTL6 &= ~XT1OFF;         //ʹ��XT1 �ⲿ��Դ 32.768K
    //!���Vcore ��ѹ����߼��������㱶Ƶ����ú���λ��HAL_PMM.H
    P5SEL |= BIT4|BIT5;         //����ΪXT1����,��·���Ͼ��������������
    __bis_SR_register(SCG0);    //��CPU�ܵ�SG0λ��1
    UCSCTL0 = 0;                //DCO ��DCO Ƶ��ѡ(���ù�)
    UCSCTL1 = DCORSEL_6;        //DCO Ƶ�ʷ�Χѡ��ʹ�ܵ�����
    UCSCTL2 = FLLD_1 | 305;     //FLLD=1,FLLN=305,��Ƶ��Ϊ2*��305+1��*32.768=20.054MHZ
    __bic_SR_register(SCG0);
    //_delay_cycles(782000);      //ϵͳ�Դ��ľ�ȷ��ʱ����λus
    while(SFRIFG1 & OFIFG) 
    {
            UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
            SFRIFG1 &= ~OFIFG;
    }
    UCSCTL4 = UCSCTL4&(~(SELS_7|SELM_7))|SELS_3|SELM_3; //0x0333uѡ��DCO ��Ϊʱ��Դ
}



int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  //WDTCTL = WDTPW + WDTCNTCL+WDTSSEL0+WDTIS2;
  unsigned int i;
  //clk_init();//ȫ��ʱ��
  //��ʱ��ʼ��
  //������

  __disable_interrupt();
  UCSCTL4=UCSCTL4&(~(SELA_7))|SELA_3;
  UCSCTL5=UCSCTL5&(~(DIVPA_7|DIVA_0))|DIVPA_0|DIVA_0;
  clk_test();
  TimerA0_init();//��ʱ��B
  P4DIR|=BIT7+BIT1;
  P3DIR|=BIT1;
  
  uart_A1_init();
  PutString("��ʼ���ɹ�\n");
  __enable_interrupt();
  PutString("�����жϡ�\n");
  while(1)
  {
    P3OUT^=BIT1;
    for(i=0;i<30000;i++);
  }

  
}
