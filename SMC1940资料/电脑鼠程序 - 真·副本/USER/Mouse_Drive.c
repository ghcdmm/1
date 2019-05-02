#include "Mouse_Drive.h"
#include "bsp_usart1.h"
#include "Mouse_Drive_Init.h"
/*********************************************************************************************************
  ����ȫ�ֱ���
*********************************************************************************************************/
unsigned char mstate=1;
MAZECOOR          GmcMouse                        = {0,0};              /*  ���������ǰλ������      */
uint8             GucMouseDir                     = UP;                 /*  ���������ǰ����          */
uint8             GucMapBlock[MAZETYPE][MAZETYPE] = {0};                /*  GucMapBlock[x][y]           */
                                                                        /*  x,������;y,������;          */
uint8             GucMapBlock0[MAZETYPE][MAZETYPE] = {0};               /*  �����㷨         */
                                                                   /*  bit3~bit0�ֱ������������   */
                                                                        /*  0:�÷�����·��1:�÷�����·  */
uint8             GucMapBlock1[MAZETYPE][MAZETYPE]= {0x0f};				//��ˮ��
uint8             GucMouseStart                    = 0;                 /* ����������     			    */
	uint8             GucFrontJinju                    = 0;                 /* ǰ���������������ʱ�ȸ�ͼ��������   */
uint8             GucCrossroad                     = 0;                 /* ʮ��·���������ʱ�ã���ʮ��·�ڶཱུ����߳���ٶ�*/
 __MOTOR  __GmRight                         = {0, 0, 0, 0, 0}; /*  ���岢��ʼ���ҵ��״̬      */
 __MOTOR  __GmLeft                         = {0, 0, 0, 0, 0};    
static __PID    __GmLPID;                                               /*  ��������PID   			*/
static __PID    __GmRPID;                                               /*  �����ҵ��PID   			*/
static __PID    __GmSPID;                                               /*  ֱ��PID    					*/
static __PID    __GmWPID;                                               /*  ��תPID   					*/
static uint8    __GucMouseState                   = __STOP;             /*  ���������ǰ����״̬      */
static int32    __GiMaxSpeed                      = SEARCHSPEED;        /*  �����������е�����ٶ�      */
static uint8    __GucDistance[4]                  = {0};                /*  ��¼������״̬              */
volatile float    GsTpusle_T                      = 0;                  /*  ����У�����ٵ��ٶ�ֵ��WPID��*/
static uint8    GuiSpeedCtr                       = 0;					/*	��¼����Ӽ��ٶ�			*/
static uint16   GuiTpulse_LR                      = 0;
volatile static uint16   GuiTpulse_S                     = 0;
volatile static uint8    GucFrontNear                    = 0;			/*ǰ����ǽ����ǽ��־			*/
uint8    GucGoHead          	           = 0;							/*ǰ���Ƿ���ǽ��־				*/
uint8    GucFangXiang                      = 0;
uint8    GucDirTemp                        = 0;
unsigned int aa,bb,cc;
static float IPreErrorL=0;
static float IPreDerrorL=0;
static float IPUL=0;
static float IPreErrorR=0;
static float IPreDerrorR=0;
static float IPUR=0;
static unsigned char IDeadband=0;
float n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11;
unsigned int er,el;
int xx=0;

//uint16 count=0;
//float W;																/*�õ��Ľ��ٶȵ�ֵ				*/
uint16 Sunlight[6];														/*��Ȼ���ߵ�ֵ					*/
extern __IO uint16 ADC_ConvertedValue[6];								/*ֱ�Ӷ�ȡ�ĺ���ǿ��			*/
uint16 ucIRCheck[6];													/*�ų���Ȼ���߸��ź�ĺ���ǿ��	*/
//������趨ֵ
uint16 GusDistance_L_Near=1280; //������
uint16 GusDistance_L_Mid=740;  //�������
uint16 GusDistance_L_Far=270;  //�����Զ
uint16 GusDistance_R_Near=1400; //�Һ����
uint16 GusDistance_R_Mid=900;  //�Һ�����
uint16 GusDistance_R_Far=430;  //�Һ���Զ
uint16 GusDistance_FL_Near = 210; 
uint16 GusDistance_FR_Near = 345;
uint16 GusDistance_FL_Far = 200;   
uint16 GusDistance_FR_Far = 190;
u8 backflag=0;

#define rightn 1100
#define leftn 1280
#define rightf 805
#define leftf 725
#define frontn 1100
#define frontf 1085
#define frontf1 1077
#define lfn 825
#define rfn 690
#define V 30


/*********************************************************************************************************
** Function name:       __delay
** Descriptions:        ��ʱ����
** input parameters:    uiD :��ʱ������ֵԽ����ʱԽ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __delay (uint32  uiD)
{
    for (; uiD; uiD--);
}


void Delay_us(u32 Nus)   
{    
	 SysTick->LOAD=Nus*9;          	  //ʱ����� 
	 SysTick->CTRL|=0x01;             //��ʼ����   
	 while(!(SysTick->CTRL&(1<<16))); //�ȴ�ʱ�䵽��   
	 SysTick->CTRL=0X00000000;        //��ʼ����  
	 SysTick->VAL=0X00000000;         //��ռ�����
}

void mstop()
{

	__GmRight.cDir=2;
	__GmLeft.cDir=2;
	__GmSPID.sRef=0;
	__rightMotorContr();
	__leftMotorContr();
	mstate=0;
//	__delay(10000000);
}

void mgo()
{
	__GmRight.cDir=0;
	__GmLeft.cDir=0;
	__GmSPID.sRef=V;
	__rightMotorContr();
	__leftMotorContr();
	mstate=1;
}
/*********************************************************************************************************
** Function name:       PIDInit
** Descriptions:        PID��ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void PIDInit(void) 
{  
    pulse_L_counter = 0;
    __GmLPID.usFeedBack = 0 ;  //�ٶȷ���ֵ
    __GmLPID.sFeedBack = 0 ;
    
    pulse_R_counter = 0;
    __GmRPID.usFeedBack = 0 ;  //�ٶȷ���ֵ
    __GmRPID.sFeedBack = 0 ;
    
    __GmSPID.sRef = 0 ;        //�ٶ��趨ֵ 
    __GmSPID.sFeedBack = 0 ;        
    __GmSPID.sPreError = 0 ;   //ǰһ�Σ��ٶ����,,vi_Ref - vi_FeedBack 
    __GmSPID.sPreDerror = 0 ;   //ǰһ�Σ��ٶ����֮�d_error-PreDerror; 
        
    __GmSPID.fKp = __KP; //80
    __GmSPID.fKi = __KI; //0.01
    __GmSPID.fKd = __KD; //0
       
    __GmSPID.iPreU = 0 ;      //����������ֵ 
    
    __GmWPID.sRef = 0 ;        
    __GmWPID.sFeedBack = 0 ;       
    __GmWPID.sPreError = 0 ; 
    __GmWPID.sPreDerror = 0 ; 
    
    __GmWPID.fKp = __KP;  //30
    __GmWPID.fKi = __KI;  //0.1,0.01
    __GmWPID.fKd = __KD; 
       
    __GmWPID.iPreU = 0 ;      //����������ֵ
}

/*********************************************************************************************************
** Function name:       __SPIDContr
** Descriptions:        ֱ��PID����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __SPIDContr(void) 
{ 
    float  error,d_error,dd_error;
    static uint8   K_I=1;
    error = __GmSPID.sRef - __GmSPID.sFeedBack; // ƫ�����
    d_error = error - __GmSPID.sPreError; 
    dd_error = d_error - __GmSPID.sPreDerror;
    if(error> Deadband)//������Χ
      error -= Deadband;
    else if(error < -Deadband)
      error += Deadband;
    else
      error = 0;
    if((error > error_IMAX)||(error < -error_IMAX))//�����޷�
      K_I=0;
    else
      K_I=1;
    
    __GmSPID.sPreError = error; //�洢��ǰƫ�� 
    __GmSPID.sPreDerror = d_error;
    
    __GmSPID.iPreU += (int16)(  __GmSPID.fKp * d_error + K_I*__GmSPID.fKi * error  + __GmSPID.fKd*dd_error); 
}

void __IPIDContrL(void) 
{ 
    float  error,d_error,dd_error;
    static uint8   K_I=1;
    error = leftf - ucIRCheck[2]; // ƫ�����
    d_error = error - IPreErrorL; 
    dd_error = d_error - IPreDerrorL;
//    if(error> IDeadband)//������Χ
//      error -= IDeadband;
//    else if(error < -IDeadband)
//      error += IDeadband;
//    else
//      error = 0;
//    if((error > error_IMAX)||(error < -error_IMAX))//�����޷�
//      K_I=0;
//    else
//      K_I=1;
    
    IPreErrorL = error; //�洢��ǰƫ�� 
    IPreDerrorL = d_error;
    
    IPUL += (int16)(  __KP * d_error + K_I*__KI * error  + __KD*dd_error); 
}

void __IPIDContrR(void) 
{ 
    float  error,d_error,dd_error;
    static uint8   K_I=1;
    error = rightf - ucIRCheck[3]; // ƫ�����
    d_error = error - IPreErrorR; 
    dd_error = d_error - IPreDerrorR;
//    if(error> IDeadband)//������Χ
//      error -= IDeadband;
//    else if(error < -IDeadband)
//      error += IDeadband;
//    else
//      error = 0;
//    if((error > error_IMAX)||(error < -error_IMAX))//�����޷�
//      K_I=0;
//    else
//      K_I=1;
    
    IPreErrorR = error; //�洢��ǰƫ�� 
    IPreDerrorR = d_error;
    
    IPUR += (int16)(  __KP * d_error + K_I*__KI * error  + __KD*dd_error); 
}
/*********************************************************************************************************
** Function name:       __WPIDContr
** Descriptions:        ��ת����PID����
** input parameters:    �� 
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __WPIDContr(void) 
{ 
    float  error,d_error,dd_error; 
    static uint8   K_I=1;
    error = __GmWPID.sRef + GsTpusle_T/*+ IPUL/200 -IPUR/400*/ - __GmWPID.sFeedBack; // ƫ�����
    d_error = error - __GmWPID.sPreError; 
    dd_error = d_error - __GmWPID.sPreDerror;
    if(error> Deadband)
      error -= Deadband;
    else if(error < -Deadband)
      error += Deadband;
    else
      error = 0;
    if((error > error_IMAX)||(error < -error_IMAX))
      K_I=0;
    else
      K_I=1;
    
    __GmWPID.sPreError = error; //�洢��ǰ���
    __GmWPID.sPreDerror = d_error;
    __GmWPID.iPreU += (int16)(  __GmWPID.fKp * d_error + K_I*__GmWPID.fKi * error  + __GmWPID.fKd*dd_error);
        
}
/*********************************************************************************************************
** Function name:      __PIDContr
** Descriptions:        PID���ƣ�ͨ�����������Ƶ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __PIDContr(void)
{
    __SPIDContr();
    __WPIDContr();
    __GmLeft.sSpeed = __GmSPID.iPreU - __GmWPID.iPreU ;
//    if(__GmLeft.sSpeed>=0){
//     __GmLeft.cDir=__MOTORGOAHEAD; 
    if( __GmLeft.sSpeed >= U_MAX )   //�ٶ�PID����ֹ���������� 
       __GmLeft.sSpeed = U_MAX;      
    if( __GmLeft.sSpeed <= U_MIN ) //�ٶ�PID����ֹ����������  
       __GmLeft.sSpeed = U_MIN;
//    }
//	
//    else{
//				  __GmLeft.cDir=__MOTORGOSTOP;
//				  __GmLeft.sSpeed =0; 
//    }
	
      
    __GmRight.sSpeed = __GmSPID.iPreU + __GmWPID.iPreU ;
    if(__GmRight.sSpeed>=0){
     __GmRight.cDir=__MOTORGOAHEAD; 
    if( __GmRight.sSpeed >= U_MAX )   //�ٶ�PID����ֹ���������� 
       __GmRight.sSpeed = U_MAX;      
    if( __GmRight.sSpeed <= U_MIN ) //�ٶ�PID����ֹ����������  
       __GmRight.sSpeed = U_MIN;
    }
    else{
				  __GmRight.cDir=__MOTORGOSTOP;
				  __GmRight.sSpeed =0; 
    }
		__GmRight.sSpeed+=10;
    __rightMotorContr();
    __leftMotorContr();
}
/*********************************************************************************************************
** Function name:       __Encoder
** Descriptions:        �ɼ����������
** input parameters:    �� 
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __Encoder(void)
{

	__GmLPID.usFeedBack = pulse_L_counter;
//printf ("counter=%d ",pulse_L_counter);
	pulse_L_counter=0;
	__GmLPID.sFeedBack= __GmLPID.usFeedBack;		
//printf ("pulseCtr%d\r\n",pulse_L);

	__GmRPID.usFeedBack = pulse_R_counter;
	pulse_R_counter=0;
	__GmRPID.sFeedBack = __GmRPID.usFeedBack;
el=__GmLPID.sFeedBack;
	er=__GmRPID.sFeedBack;
	__GmSPID.sFeedBack = (__GmRPID.sFeedBack + __GmLPID.sFeedBack)/2 ;
    __GmWPID.sFeedBack = (__GmRPID.sFeedBack - __GmLPID.sFeedBack)/2 ;	
}

void __irSend (int8  __cNumber)
{
    switch (__cNumber) 
    {
      case 0:                                                           
          GPIO_SetBits(GPIOC,GPIO_Pin_6);  //ָ��ǰ�� �� ��
           break;  
      case 1:                                                         
          GPIO_SetBits(GPIOC,GPIO_Pin_7);  //ָ���� �� ��
          break; 
      case 2:                                                         
          GPIO_SetBits(GPIOC,GPIO_Pin_8);  //ָ���ҷ� �� ��
          break;   
      case 3:                                                         
          GPIO_SetBits(GPIOA,GPIO_Pin_7);  //ָ��ǰ�� �� ��
          break;
			case 4:                                                         
          GPIO_SetBits(GPIOA,GPIO_Pin_5);  //ָ���ҷ� �� ��
          break;   
      case 5:                                                         
          GPIO_SetBits(GPIOA,GPIO_Pin_4);  //ָ��ǰ�� �� ��
          break;			
      default:
          break;
    }
}

unsigned char __irCheck (void)
{      
    static uint8 ucState = 0;
	
	unsigned char a;
    switch(ucState)
    {
        case 0://ָ�� ǰ �� ����ࣩ
					
            ucIRCheck[0] = Sunlight[0]-ADC_ConvertedValue[0];

          if((ucIRCheck[0]>5000))
            ucIRCheck[0]=7;
//         	printf("[0]= %d ",ucIRCheck[0]);
					BLUE_LED_OFF;
          GPIO_ResetBits(GPIOC,GPIO_Pin_6); 
					
//					__IPIDContrL();
//					__IPIDContrR();
					
					break;
//          if(ucIRCheck[0]>GusDistance_FL_Far)
//          {
//              __GucDistance[__FRONTL]  |= 0x01;//ǰ����ǽ��ĩλǿ�Ʊ�1
//          }
//          else
//          {
//              __GucDistance[__FRONTL]  &= 0xfe;//ǰ����ǽ��ĩλǿ�Ʊ�0             
//          }
//          
//          if(ucIRCheck[0]>GusDistance_FL_Near)
//          {
//              __GucDistance[__FRONTL]  |= 0x02;//����ǽ��ĩλǿ�Ʊ�1X
//          }
//          
//          else
//          {
//              __GucDistance[__FRONTL]  &= 0xfd;//������ǽ��ĩλ0X
//          }  
				case 1:
          bb=ADC_ConvertedValue[1];
					BLUE_LED_ON;
          __irSend (1);
          break;

        case 2:          
          ucIRCheck[1] = bb-ADC_ConvertedValue[1];
								aa=ucIRCheck[1];/*bb=Sunlight[1]*/;cc=ADC_ConvertedValue[1];
          if((ucIRCheck[1]>5000))
            ucIRCheck[1]=7;
//			printf("[1]= %d ",ucIRCheck[1]);          
          GPIO_ResetBits(GPIOC,GPIO_Pin_7);
					__IPIDContrL();
					__IPIDContrR();
					break;
//          if(ucIRCheck[1]>GusDistance_L_Far)
//          {
//              __GucDistance[__LEFT]  |= 0x01;//����ǽ��ĩλ��1
//          }
//          else
//          {
//               __GucDistance[__LEFT] &= 0xfe;//����ǽ��ĩλ��0
//          } 
//          
//          if(ucIRCheck[1]>GusDistance_L_Mid)
//          {
//              __GucDistance[__LEFT]  |= 0x02;//����ƫ��11
//          }
//          else
//          {
//              __GucDistance[__LEFT]  &= 0xfd;//������ƫ��0X
//          }     
//		  
//          if(ucIRCheck[1]>GusDistance_L_Near)
//          {
//              __GucDistance[__LEFT]   |= 0x04;//��࿿��ǽ����Ҫ������111
//          }
//          else
//          {
//              __GucDistance[__LEFT]   &= 0xfb;//0XX
//          }  
				case 3:
          Sunlight[2]=ADC_ConvertedValue[2];
          __irSend (2);  
          break;
          
        case 4:
          ucIRCheck[2] = Sunlight[2]-ADC_ConvertedValue[2];
          if((ucIRCheck[2]>5000))
            ucIRCheck[2]=7;       
//			printf("[2]= %d ",ucIRCheck[2]);		  
          GPIO_ResetBits(GPIOC,GPIO_Pin_8);
					__IPIDContrL();
					__IPIDContrR();
					break;
//          if(ucIRCheck[2]>GusDistance_R_Far)
//          {
//              __GucDistance[__RIGHT]   |= 0x01;
//          }
//          else
//          {
//              __GucDistance[__RIGHT]   &= 0xfe;
//          }
//          
//          if(ucIRCheck[2]>GusDistance_L_Mid)
//          {
//              __GucDistance[__RIGHT]   |= 0x02;
//          }
//          else
//          {
//              __GucDistance[__RIGHT]   &= 0xfd;
//          }
//          
//          if(ucIRCheck[2]>GusDistance_L_Near)
//          {
//              __GucDistance[__RIGHT]   |= 0x04;
//          }
//          else
//          {
//              __GucDistance[__RIGHT]   &= 0xfb;
//          }  
				case 5:
          Sunlight[3]=ADC_ConvertedValue[3];
          __irSend (3);  
          break;
		  
        case 6://ָ��ǰ�� ��
          ucIRCheck[3] = Sunlight[3]-ADC_ConvertedValue[3];
          if((ucIRCheck[3]>5000))
            ucIRCheck[3]=7;
//          printf("[3]= %d ",ucIRCheck[3]);
//		    printf("\r\n");
          GPIO_ResetBits(GPIOA,GPIO_Pin_7); 
					__IPIDContrL();
					__IPIDContrR();
					break;
//          if(ucIRCheck[3]>GusDistance_FR_Far)
//          {
//              __GucDistance[__FRONTR]  |= 0x01;
//          }         
//          else
//          {
//              __GucDistance[__FRONTR]  &= 0xfe;              
//          }
//          
//          if(ucIRCheck[3]>GusDistance_FR_Near)
//          {
//              __GucDistance[__FRONTR]  |= 0x02;
//          }
//          
//          else
//          {
//              __GucDistance[__FRONTR]  &= 0xfd;
//          }  
//          if((ucIRCheck[0]>GusDistance_FL_Far)&&(ucIRCheck[3]>GusDistance_FR_Far))
//            GucGoHead =1;
//          else
//          GucGoHead =0;
				case 7:
          Sunlight[4]=ADC_ConvertedValue[4];
          __irSend (4);
          break;
					
					case 8://
          ucIRCheck[4] = Sunlight[4]-ADC_ConvertedValue[4];
          if((ucIRCheck[4]>5000))
            ucIRCheck[4]=7;
					GPIO_ResetBits(GPIOA,GPIO_Pin_5); 
					__IPIDContrL();
					__IPIDContrR();
					break;
					case 9:
					Sunlight[5]=ADC_ConvertedValue[5];
          __irSend (5);
					break;
					
          case 10://
          ucIRCheck[5] = Sunlight[5]-ADC_ConvertedValue[5];
          if((ucIRCheck[5]>5000))
            ucIRCheck[5]=7;
					GPIO_ResetBits(GPIOA,GPIO_Pin_4); 
					__IPIDContrL();
					__IPIDContrR();
					break;
					case 11:
					Sunlight[0]=ADC_ConvertedValue[0];
          __irSend (0);
					break;
		default: 
            break;
    }
		a=ucState;
    ucState = (ucState + 1) % 12;   
return(a);		
}

/*********************************************************************************************************
** Function name:       __rightMotorContr
** Descriptions:        ��ֱ���������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
  void __rightMotorContr(void)
{
    switch (__GmRight.cDir) 
    {
    case __MOTORGOAHEAD: 
		Right_motor_go;
		TIM_SetCompare1(TIM4,__GmRight.sSpeed);
        break;

    case __MOTORGOBACK:                                                 
		Right_motor_back;
		TIM_SetCompare1(TIM4,__GmRight.sSpeed);
        break;
		
    case __MOTORGOSTOP:                                                   
		Right_motor_stop;		
		TIM_SetCompare1(TIM4,0);
        break;

    default:
        break;
    }
}

/*********************************************************************************************************
** Function name:       __leftMotorContr
** Descriptions:        ��ֱ���������
** input parameters:    __GmLeft.cDir :������з���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __leftMotorContr(void)
{
    switch (__GmLeft.cDir) 
    {
    case __MOTORGOAHEAD:                                            
		Left_motor_go;  
		TIM_SetCompare2(TIM4,__GmLeft.sSpeed);	
        break;

    case __MOTORGOBACK:                                                 
		Left_motor_back;
		TIM_SetCompare2(TIM4,__GmLeft.sSpeed);	
        break;
		
    case __MOTORGOSTOP:                                                 
		Left_motor_stop;                                         
		TIM_SetCompare2(TIM4,0);
        break;
		
    default:
        break;
    }
}

/*********************************************************************************************************
** Function name:       __SpeedUp
** Descriptions:        ��������ٳ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __SpeedUp (void)
{
    uint16 Speed;
    Speed=__GmSPID.sFeedBack;
    if(__GmSPID.sRef<__GiMaxSpeed){			//��С���������е�����ٶȣ�1ms��������
      if(Speed >=__GmSPID.sRef)				//�������ٶȴ����ٶ��趨ֵ
      {
        __GmSPID.sRef=__GmSPID.sRef+4;		//�����ٶ��趨ֵ10
      }
    }   
}
/*********************************************************************************************************
** Function name:       __SpeedDown
** Descriptions:        ��������ٳ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __SpeedDown (void)
{
    uint16 Speed;
    Speed=__GmSPID.sFeedBack;
    if(__GmSPID.sRef>=MINSPEED)				//�������������е���С�ٶ�
    {
      if(Speed <=__GmSPID.sRef+1)			//�������ٶ�С���ٶ��趨ֵ
      {
       __GmSPID.sRef=__GmSPID.sRef-1;		//��С�ٶ��趨ֵ3
      }
    }
}

/*********************************************************************************************************
** Function name:       SysTick_Handler
** Descriptions:        ��ʱ�ж�ɨ�衣
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SysTick_Handler(void)
{  
	
    static int8 n = 0,k=0,a=0,b=0,c=0,i=0,ch=0;
//	while(1);
     __Encoder();
//	__GmSPID.sRef=10;
	
//	__rightMotorContr();
	
//	__leftMotorContr();
	
//	BLUE_LED_OFF;
//	__delay(5000000);
//	BLUE_LED_ON;
//	__delay(5000000);
  i=__irCheck();
//	switch(i)
//	{
		
//		case 0:
//			if(ucIRCheck[0]>frontf)
//			{
//				__GmRight.cDir=2;
//				__GmLeft.cDir=2;
//				if(ucIRCheck[1]<lfn)
//				{
//					//__GucMouseState=__TURNLEFT;
//					pulse_R=0;
//					while(1)
//					{
//						if(pulse_R);
//					}
//				}
//			}
//			else// if(ucIRCheck[0]>frontn)
//			{
//				__GmRight.cDir=0;
//				__GmLeft.cDir=0;
//			}
//		case 6:
//if(ch>10)
//{
			if(ucIRCheck[3]>rightn)
				GsTpusle_T=2;
//			else
//				GsTpusle_T=0;
//			break;
//		case 4:
			else if(ucIRCheck[2]>leftn)
				GsTpusle_T=-2;
//			ch=1;
//}
			else
				GsTpusle_T=0;
//			break;
//	}
//	xx=GsTpusle_T;
	n1=__GmSPID.sRef;n2=__GmSPID.sFeedBack;n3=__GmSPID.sPreError;n4=__GmSPID.sPreDerror;n5=__GmSPID.iPreU;n6=__GmWPID.sRef;n7=__GmWPID.sFeedBack;n8=__GmWPID.sPreError;n9=__GmWPID.sPreDerror;n10=__GmWPID.iPreU;n11=__GmLeft.sSpeed;
	if(mstate==1)
	{
		__PIDContr();
	}
	
//      switch (__GmRight.cState) {
//          case __MOTORSTOP:                                                   /*  ֹͣ��ͬʱ�����ٶȺ�����ֵ  */
//              __GmRight.uiPulse    = 0;
//              pulse_R = 0;
//              __GmLeft.uiPulse    = 0;
//              pulse_L = 0;
//              break;
//      
//          case __WAITONESTEP:                                                 /*  ��ͣһ��                    */
//              __GmRight.cState = __MOTORRUN;
//              if((((ucIRCheck[1]>GusDistance_L_Near)&&(ucIRCheck[2]<GusDistance_R_Near))&&(ucIRCheck[2]>GusDistance_R_Mid)))          //ƫ��
//              {	
//                GsTpusle_T = -2;		/*  ����У�����ٵ��ٶ�ֵ*/			//���ּ���
//              }
//              else if((((ucIRCheck[1]<GusDistance_L_Near)&&(ucIRCheck[2]>GusDistance_R_Near))&&(ucIRCheck[1]>GusDistance_L_Mid)))     //ƫ��
//              {
//                GsTpusle_T = 2;												//���ּ���
//              }

//              if((ucIRCheck[1]>GusDistance_L_Near)&&(ucIRCheck[2]<GusDistance_R_Mid))          //ƫ��
//              {
//                GsTpusle_T = -3;															   //��ת�ٶȼ�С
//              }
//              else if((ucIRCheck[2]>GusDistance_R_Near)&&(ucIRCheck[1]<GusDistance_L_Mid))     //ƫ��
//              {
//                GsTpusle_T = 3;
//              }          
//              else if((ucIRCheck[1]<GusDistance_L_Far)&&((ucIRCheck[2]>GusDistance_R_Far)&&(ucIRCheck[2]<GusDistance_R_Mid)))
//              {
//                  GsTpusle_T = -2;
//              }
//         
//              else if((ucIRCheck[2]<GusDistance_R_Far)&&((ucIRCheck[1]>GusDistance_L_Far)&&(ucIRCheck[1]<GusDistance_L_Mid)))
//              {
//                   GsTpusle_T = 2;
//              }
//                
//              __PIDContr();
//              break;
//      
//          case __MOTORRUN:                                                    /*  �������                    */

//            if (__GucMouseState == __GOAHEAD)                                 /*  ���ݴ�����״̬΢�����λ��  */
//            {                              
//                  if ((ucIRCheck[1]>GusDistance_L_Near)&&(ucIRCheck[2]<GusDistance_R_Near))	//ƫ��
//                  {
//                    if (n == 1)
//                    {
//                          __GmRight.cState = __WAITONESTEP;
//                    }               
//                    n++;
//                    n %= 2;
//                  }
//                  else if((ucIRCheck[1]<GusDistance_L_Near)&&(ucIRCheck[2]>GusDistance_R_Near))	//ƫ��
//                  {
//                    if (a == 1)
//                    {
//                          __GmRight.cState = __WAITONESTEP;
//                    }               
//                    a++;
//                    a %= 2;
//                  }
//                  
//                  else if((ucIRCheck[1]<GusDistance_L_Mid)&&(ucIRCheck[1]>GusDistance_L_Far))
//                  {
//                    if (b == 1)
//                    {
//                          __GmRight.cState = __WAITONESTEP;
//                    }               
//                    b++;
//                    b %= 2;
//                  }
//                  else if((ucIRCheck[2]<GusDistance_R_Mid)&&(ucIRCheck[2]>GusDistance_R_Far))
//                  {
//                   if (c == 1)
//                    {
//                          __GmRight.cState = __WAITONESTEP;
//                    }               
//                    c++;
//                    c %= 2;
//                  }                  
//                 else 
//                  {
//                      n = 0;
//                      GsTpusle_T = 0;
//                  }
//              
//                  if(GuiSpeedCtr==__SPEEDUP)
//                  { 
//                    k=(k+1)%5;//20
//                    if(k==4)
//                    __SpeedUp();
//                  }
//                  else if(GuiSpeedCtr==__SPEEDDOWN)
//                  {
//                      k=(k+1)%10;
//                      if(k==9)
//                      __SpeedDown(); 
//                  }
//                  else;
//              }
//            else{
//              GsTpusle_T = 0;
//            }     
//              __PIDContr();
//              break;
//			
//          case __MOTORTURN:
////��������������
//            break;

//		  case __MOTORTURNBACK:
////��������������			  
//		  break;
//		  
//          default:
//              break;
//    }           
}


/*********************************************************************************************************
** Function name:      mouseGoaheadhui
** Descriptions:        ǰ��N��,��������ã�Ҫ��ǰ������
** input parameters:    iNblock: ǰ���ĸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseGoahead (int8  cNBlock)                                    //����ת����
{
    int8 cL = 0, cR = 0, cCoor = 1,cB;
    GuiSpeedCtr=__SPEEDUP;
    if (__GmLeft.cState) {
        cCoor = 0;
    }
	
    if(cNBlock==1)
    {
        cL = 1;
        cR = 1;
        if(GucFangXiang == GucDirTemp)
        {
           //GucYiBaiBa=1;
           GuiTpulse_LR = 5460;
           GuiTpulse_S  = 0;
        }
        else
        {
           GuiTpulse_LR = 5460;
           GuiTpulse_S  = 252;
        }
        __GiMaxSpeed = 20;
    }
    else{
        GuiTpulse_LR = 0;
        GuiTpulse_S  = 0;
        __GiMaxSpeed = 20;
    }
    GucFangXiang = GucDirTemp;
    if(((GmcMouse.cX==7)&&(GmcMouse.cY==7))|| 
         ((GmcMouse.cX==8)&&(GmcMouse.cY==8))||
         ((GmcMouse.cX==7)&&(GmcMouse.cY==8))||
         ((GmcMouse.cX==8)&&(GmcMouse.cY==7)))
	{
       cL = 0;
       cR = 0;
       GuiTpulse_LR = 0;
       GuiTpulse_S  = 0;
    }
	
    if((__GucMouseState==__TURNRIGHT)||(__GucMouseState==__TURNLEFT))
    {
        pulse_L =12600;       //1182(34mm)
        pulse_R =12600;
    }    
	
    cB=cNBlock;
    __GucMouseState   =   __GOAHEAD;
   // __GiMaxSpeed      =   25;
    __GmRight.uiPulse = __GmRight.uiPulse + cNBlock * ONEBLOCK ;
    __GmLeft.uiPulse  = __GmLeft.uiPulse  + cNBlock * ONEBLOCK ;
    __GmRight.cState  = __MOTORRUN;
    __GmLeft.cState   = __MOTORRUN;
    while (__GmLeft.cState != __MOTORSTOP)
    {
        if (pulse_L >= ONEBLOCK) {                          /*  �ж��Ƿ�����һ��            */
            __GmLeft.uiPulse    -= ONEBLOCK;
            pulse_L -= ONEBLOCK;
            if (cCoor) {
                cNBlock--;
                if(cNBlock==0)
                   goto End;
                if(cNBlock<cB-1)//������һ��ʱ��  cB=cNBlock
                  GuiSpeedCtr=__SPEEDUP;
				} else {
                cCoor = 1;
            }
        }
        
        if (pulse_R >= ONEBLOCK) {                         /*  �ж��Ƿ�����һ��            */
            __GmRight.uiPulse    -= ONEBLOCK;
            pulse_R -= ONEBLOCK;
        }
        if (cNBlock < 2) {
          if(__GmSPID.sFeedBack>30){
              GuiSpeedCtr= 3;
             __GmSPID.sRef=30;//120
          }  
          if (cL) 
           {                 
                __GmRight.uiPulse = pulse_R  + 8800 - GuiTpulse_LR;    //3094(89mm)
                __GmLeft.uiPulse  =    + 8800 - GuiTpulse_LR;
                while (ucIRCheck[1]<GusDistance_L_Far) 
                {
                    
                    if ((pulse_L + 50) > __GmLeft.uiPulse) 
                    {
                        goto End;
                    }
                }
            }		  else {                                                    /*  �����ǽʱ��ʼ���������  */
                if (ucIRCheck[1]>GusDistance_L_Far) {
                    cL = 1;
                }
            }
         if (cR) 
            {                                                       /*  �Ƿ��������ұ�            */
            if (ucIRCheck[2]<GusDistance_R_Far)               /*  �ұ���֧·����������        */
            {                
                __GmRight.uiPulse  = pulse_R + 8800- GuiTpulse_LR;
                __GmLeft.uiPulse   = pulse_L + 8800- GuiTpulse_LR;
                while (ucIRCheck[2]<GusDistance_R_Far) 
                {
                    
                    if ((pulse_L + 50) > __GmLeft.uiPulse) 
                    {
                        goto End;
                    }
                }
            }
            } 
			else {
                if (ucIRCheck[2]>GusDistance_R_Far) {                   /*  �ұ���ǽʱ��ʼ�������ұ�  */
                    cR = 1;
                }
            }
        }
   }
    /*
     *  �趨���������õ������ߵ�֧·������λ��
     */
End:     ;
}

void mouseStop(void)
{   
  __GmSPID.sRef=0;
  __GmWPID.sRef=0;
  GuiSpeedCtr=3;  //�Ȳ�����Ҳ������
}
/****************************************************************************************************
** Function name:       mouseTurnright
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��      
*********************************************************************************************************/
void mouseTurnright(void)
{    
  
   __GucMouseState   = __TURNRIGHT; 
   __GmRight.cState =__MOTORRUN;        //�üӵ�����״̬
   __GmLeft.cState  =__MOTORRUN;
   GucMouseDir     = (GucMouseDir + 1) % 4;                            /*  ������                    */

   __GmWPID.sRef=-70;
    while(1)
   {
	   //�����������Ƕ��ж��������
//       if(W>53000)   
//       {
         break;
//       } 
   }

   __GmWPID.sRef=0;     

   GuiSpeedCtr=3;
   __GmLeft.uiPulse =3000;    //1200          
   __GmLeft.uiPulseCtr=0;
   __GmRight.uiPulse =3000;           
   __GmRight.uiPulseCtr=0;
  while ((__GmRight.uiPulseCtr+100 ) <= __GmRight.uiPulse);
  while ((__GmLeft.uiPulseCtr+100 ) <= __GmLeft.uiPulse);
   __GucMouseState   = __TURNRIGHT;
   GuiSpeedCtr=__SPEEDUP;
    __GmRight.cState = __MOTORSTOP;       
    __GmLeft.cState  = __MOTORSTOP;
    __GmRight.sSpeed = 0;
    __rightMotorContr();
    __GmLeft.sSpeed = 0;
    __leftMotorContr();
    __GmRight.uiPulseCtr = 0;
    __GmLeft.uiPulseCtr = 0;   
}
/*********************************************************************************************************
** Function name:       mouseTurnleft
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnleft(void)
{
   __GucMouseState  = __TURNLEFT; 
   __GmRight.cState = __MOTORTURN;        //�üӵ�����״̬
   __GmLeft.cState  = __MOTORTURN;
     GucMouseDir    = (GucMouseDir + 3) % 4;                            /*  ������                    */
//������һ���������
}

void onestep(void)
{
     __GmRight.uiPulse =14000;             
     pulse_R=0;
     __GmLeft.uiPulse =14000;
     pulse_L=0;
     while ((pulse_R+100 ) <= __GmRight.uiPulse);    
     while ((pulse_L+100 ) <= __GmLeft.uiPulse); 
}


/*********************************************************************************************************
** Function name:       mouseTurnback
** Descriptions:        ����ǰ�����࣬��ת180��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnback(void)
{ 
  if(GucFrontNear)
  {
      __GmSPID.sRef=30;
       while((ucIRCheck[0]<GusDistance_FL_Near)||(ucIRCheck[1]<GusDistance_FR_Near));
     __GmRight.uiPulse =400;             
     __GmRight.uiPulseCtr=0;
     __GmLeft.uiPulse = 400;
     __GmLeft.uiPulseCtr=0;
     while ((__GmRight.uiPulseCtr+100 ) <= __GmRight.uiPulse);    
     while ((__GmLeft.uiPulseCtr+100 ) <= __GmLeft.uiPulse);      
  }
  GucFrontNear=0;
  backflag=1;
   __GmSPID.sRef=0; 
   mouseStop();
    __GucMouseState   = __TURNBACK;
    __GmLeft.cState   = __MOTORRUN;
    __GmRight.cState  = __MOTORRUN;
    GucMouseDir = (GucMouseDir + 2) % 4;    
        __GmWPID.sRef=20;
   while(1)
   {
//       if(GW>)  
//       {
         break;
//       }                        
   }
    __GmWPID.sRef=0;
    __GucMouseState   = __GOBACK ;
    __GmSPID.sRef=-20;
     __GmRight.uiPulse =2800;             
     __GmRight.uiPulseCtr=0;
     __GmLeft.uiPulse = 2800;
     __GmLeft.uiPulseCtr=0;
     while ((__GmRight.uiPulseCtr+100 ) <= __GmRight.uiPulse);    
     while ((__GmLeft.uiPulseCtr+100 ) <= __GmLeft.uiPulse);     
    mouseStop();
   __GucMouseState   = __GOAHEAD ;
   __delay(5000000);
   
    GuiSpeedCtr=__SPEEDUP;
    __GmSPID.sRef=30;
    __GmRight.uiPulse =1000;             
     __GmRight.uiPulseCtr=0;
     __GmLeft.uiPulse = 1000;
     __GmLeft.uiPulseCtr=0;
     while ((__GmRight.uiPulseCtr+100 ) <= __GmRight.uiPulse);    
     while ((__GmLeft.uiPulseCtr+100 ) <= __GmLeft.uiPulse);     
    __GmRight.cState = __MOTORSTOP;       
    __GmLeft.cState  = __MOTORSTOP;

    __GmRight.sSpeed = 0;
    __rightMotorContr();
    __GmLeft.sSpeed = 0;
    __leftMotorContr();
    __GmRight.uiPulseCtr = 12000;
    __GmLeft.uiPulseCtr = 12000;
}
/*********************************************************************************************************
** Function name:       mazeSearch
** Descriptions:        ǰ��N��
** input parameters:    iNblock: ǰ���ĸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mazeSearch(void)                  //��������ת��
{
    int8 cL = 0, cR = 0, cCoor = 1;
    if (__GmLeft.cState)
    {
        cCoor = 0;
    }
    if((__GucMouseState==__TURNRIGHT)||(__GucMouseState==__TURNLEFT))
    {
        pulse_L =12600;
        pulse_R =12600;
        cL = 1;
        cR = 1;
        if(((__GucDistance[__FRONTR]!=0)&&(__GucDistance[__FRONTL]!=0))||((__GucDistance[ __LEFT] & 0x01) == 0)||((__GucDistance[__RIGHT] & 0x01) == 0)){
          if((__GucDistance[__FRONTR]!=0)&&(__GucDistance[__FRONTL]!=0))
            GuiTpulse_LR = 6300;
          else
            GuiTpulse_LR =6300;
        }
        else{
          GuiTpulse_LR =0; 
        }
    }
    else{
      GuiTpulse_LR =0;
    }
    __GucMouseState   = __GOAHEAD;
    __GiMaxSpeed      =   SEARCHSPEED;       
    __GmRight.uiPulse =   MAZETYPE * ONEBLOCK;
    __GmLeft.uiPulse  =   MAZETYPE * ONEBLOCK;
    __GmRight.cState  = __MOTORRUN;
    __GmLeft.cState   = __MOTORRUN;
     GuiSpeedCtr=__SPEEDUP;
    while (__GmLeft.cState != __MOTORSTOP) 
    {
       
        if (pulse_L >= ONEBLOCK)
        {                          /*  �ж��Ƿ�����һ��*/
            __GmLeft.uiPulse    -= ONEBLOCK;
            pulse_L -= ONEBLOCK;
            if (cCoor) 
            {
                if(((__GucDistance[__FRONTR]!=0)&&(__GucDistance[__FRONTL]!=0))&&(ucIRCheck[2]>GusDistance_L_Far)&&(ucIRCheck[3]>GusDistance_R_Far))//0x01
              {          //���ҷ�ǰ������ǽ
               GucFrontNear=1; 
                goto End;
              }
              __mouseCoorUpdate();                                    /*  ��������                    */
            } 
            else 
            {
                cCoor = 1;
            }
        }
        if (pulse_R >= ONEBLOCK) {                         /*  �ж��Ƿ�����һ��            */
            __GmRight.uiPulse    -= ONEBLOCK;
            pulse_R -= ONEBLOCK;
        }
        
        if (cL) {                                                       /*  �Ƿ����������            */
            if  ((__GucDistance[__LEFT]  & 0x01)==0)
            {                 /*  �����֧·����������        */
            
                __GmRight.uiPulse =  pulse_R + 8800 - GuiTpulse_LR; //10800 //7500//5000    //1500
                __GmLeft.uiPulse  =  pulse_L + 8800 - GuiTpulse_LR;
                while ((__GucDistance[__LEFT]  & 0x01)==0)
                {
                 
                    if ((pulse_L + 50) > __GmLeft.uiPulse) 
                    {
                __GmRight.uiPulse = MAZETYPE * ONEBLOCK;
                __GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
                GuiSpeedCtr=__SPEEDUP;
                        goto End;
                    }
                }

            }
        } else {                                                        /*  �����ǽʱ��ʼ���������  */
            if (ucIRCheck[1]>GusDistance_L_Far) {
                cL = 1; 
               
            }
        }
        if (cR) {                                                       /*  �Ƿ��������ұ�            */
            if ((__GucDistance[__RIGHT]  & 0x01)==0){        	       /*  �ұ���֧·����������        */
            
                __GmRight.uiPulse = pulse_R + 11000 - GuiTpulse_LR; //6500 //5000   //3300
                __GmLeft.uiPulse  = pulse_L  + 11000 - GuiTpulse_LR;
                while ((__GucDistance[__RIGHT]  & 0x01)==0) {
                 
                    if ((pulse_L + 50) > __GmLeft.uiPulse) 
                    {                         
                __GmRight.uiPulse = MAZETYPE * ONEBLOCK;
                __GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
                GuiSpeedCtr=__SPEEDUP;
				goto End;
                        
                    }
                }

            }
        } else {
            if (ucIRCheck[2]>GusDistance_R_Far)
            {                       
                cR = 1;
              //  flag=1;
            }
        }
    }
End:   
          __mouseCoorUpdate();                                            /*  ��������                    */

    
}

void tleft()
{
	mstop();
	pulse_R=0;
	__GmRight.cDir=0;
	__GmRight.sSpeed=900;
	__rightMotorContr();
	while(pulse_R<2750);
	__GmRight.cDir=1;
		__rightMotorContr();
	__delay(1000);
	__GmRight.cDir=2;
	__rightMotorContr();
		__delay(1000);

}

void tright()
{
	mstop();
	pulse_L=0;
	__GmLeft.cDir=0;
	__GmLeft.sSpeed=900;
	__leftMotorContr();
	while(pulse_L<2400);
		__GmLeft.cDir=1;
		__leftMotorContr();
		__delay(1000);
	__GmLeft.cDir=2;
	__leftMotorContr();
		__delay(1000);

}

void aleft()
{
	mstop();
	pulse_L=0;
	pulse_R=0;
	__GmRight.cDir=0;
	__GmRight.sSpeed=500;
	__GmLeft.cDir=1;
	__GmLeft.sSpeed=500;
	__rightMotorContr();
	__leftMotorContr();
	while(pulse_R<1700);
	__GmRight.cDir=1;
	__GmRight.sSpeed=1000;
	__GmLeft.cDir=0;
	__GmLeft.sSpeed=1000;
	__rightMotorContr();
	__leftMotorContr();
		__delay(210000);
	__GmRight.cDir=2;
	__rightMotorContr();
	__GmLeft.cDir=2;
	__leftMotorContr();
		__delay(1000);
}

void aright()
{
	mstop();
	pulse_L=0;
	pulse_R=0;
	__GmRight.cDir=1;
	__GmRight.sSpeed=500;
	__GmLeft.cDir=0;
	__GmLeft.sSpeed=500;
	__rightMotorContr();
	__leftMotorContr();
	while(pulse_L<1550);
		__GmRight.cDir=0;
	__GmRight.sSpeed=1000;
	__GmLeft.cDir=1;
	__GmLeft.sSpeed=1000;
	__rightMotorContr();
	__leftMotorContr();
	__delay(210000);
	__GmRight.cDir=2;
	__rightMotorContr();
	__GmLeft.cDir=2;
	__leftMotorContr();
		__delay(1000);
}

void aback()
{
	mstop();
	pulse_L=0;
	pulse_R=0;
	__GmRight.cDir=1;
	__GmRight.sSpeed=500;
	__GmLeft.cDir=0;
	__GmLeft.sSpeed=500;
	__rightMotorContr();
	__leftMotorContr();
	while(pulse_L<2995);
	__GmRight.cDir=0;
	__GmLeft.cDir=1;
		__GmRight.sSpeed=1000;
		__GmLeft.sSpeed=1000;
	__rightMotorContr();
	__leftMotorContr();
__delay(400000);
	__GmRight.cDir=2;
	__rightMotorContr();
	__GmLeft.cDir=2;
	__leftMotorContr();
		__delay(1000);

}

void goback()
{
	__GmRight.cDir=1;
	__GmRight.sSpeed=1800;
	__GmLeft.cDir=1;
	__GmLeft.sSpeed=1800;
	__rightMotorContr();
	__leftMotorContr();
	__delay(el*750000/22);
	__GmRight.cDir=2;
	__GmLeft.cDir=2;
	__rightMotorContr();
	__leftMotorContr();
}

void search()
{
	if(ucIRCheck[0]>frontn)
	{

		mstop();
		goback();
		if(ucIRCheck[3]<rightf)
		{aright();xx=1;}
//		__delay(20000000);
		else if(ucIRCheck[2]<leftf)
		{aleft();xx=2;}
		else
		{
			__GmRight.cDir=1;
			__GmLeft.cDir=1;
			__GmRight.sSpeed=500;
			__GmLeft.sSpeed=500;
			__rightMotorContr();
			__leftMotorContr();
			__delay(1000);
			aback();xx=3;
	
		}
	}
//	else if(ucIRCheck[0]>frontf&&ucIRCheck[5]>frontf1)
//	{
//		if(ucIRCheck[4]<rfn)
//		{
//			tright();
//			xx=4;
//		}
//		else if(ucIRCheck[1]<lfn)
//		{
//			tleft();
//			xx=5;
//		}
//	}
	else
	{
		mgo();
		xx=0;
	}
}

/*********************************************************************************************************
** Function name:       __mouseCoorUpdate
** Descriptions:        ���ݵ�ǰ�����������ֵ
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __mouseCoorUpdate (void)
{

    switch (GucMouseDir) {

    case 0:
        GmcMouse.cY++;
        break;

    case 1:
        GmcMouse.cX++;
        break;

    case 2:
        GmcMouse.cY--;
        break;

    case 3:
        GmcMouse.cX--;
        break;

    default:
        break;
    }
	//printf("Dir=%d",GucMouseDir);
    __wallCheck();
}


/*********************************************************************************************************
** Function name:       __wallCheck
** Descriptions:        ���ݴ�����������ж��Ƿ����ǽ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      cValue: ����λ������һ�δ�����ǰ�ҡ�1Ϊ��ǽ��0Ϊûǽ��
*********************************************************************************************************/
void __wallCheck (void)
{
    uint8 ucMap = 0;
    uint8 uctemp = 0;
    ucMap |= MOUSEWAY_B;
    
    if (ucIRCheck[1]>GusDistance_L_Far)//����е��壬���Ǿ����Զ
    {
        ucMap &= ~MOUSEWAY_L;          //��Է���������ǽ����ȴת���ɾ�������
        uctemp |= 0x06;
    }
    else 
    {
        ucMap |=  MOUSEWAY_L;         //��Է���������ǽ����ȴת���ɾ�������   
    }
    
    if ((ucIRCheck[0]>GusDistance_FL_Far)&&(ucIRCheck[3]>GusDistance_FR_Far)) 
    {
        ucMap &= ~MOUSEWAY_F;
        uctemp |= 0x40;
    }
    else 
    {
        ucMap |=  MOUSEWAY_F;
    }
    if (ucIRCheck[2]>GusDistance_R_Far) 
    {
        ucMap &= ~MOUSEWAY_R;
        uctemp |= 0x30;
    }else {
        ucMap |=  MOUSEWAY_R;
    }
    GucMapBlock0[GmcMouse.cX][GmcMouse.cY]=ucMap;
    GucMapBlock[GmcMouse.cX][GmcMouse.cY]=ucMap;
    GucMapBlock1[GmcMouse.cX][GmcMouse.cY]=ucMap;
    if(GmcMouse.cY<(MAZETYPE-1))
        {GucMapBlock1[GmcMouse.cX][GmcMouse.cY+1] &= ~(((~ucMap)&0x01)*4);}       /*����������Χ����ǽ�����ϸ���  ע����ˮ��*/
       if(GmcMouse.cX<(MAZETYPE-1))
        { GucMapBlock1[GmcMouse.cX+1][GmcMouse.cY]&= ~(((~ucMap)&0x02)*4);}
         if(GmcMouse.cY>0)
         {GucMapBlock1[GmcMouse.cX][GmcMouse.cY-1]&= ~(((~ucMap)&0x04)/4);}
        if(GmcMouse.cX>0)
         {GucMapBlock1[GmcMouse.cX-1][GmcMouse.cY]&= ~(((~ucMap)&0x08)/4);}
              
      if(GmcMouse.cY<(MAZETYPE-1))
         {GucMapBlock[GmcMouse.cX][GmcMouse.cY+1] |=    ((ucMap&0x01)*4);}        /*����������Χ����ǽ�����ϸ���  ע���ڳ�ʼΪ��ǽʱ����*/
      if(GmcMouse.cX<(MAZETYPE-1))
         { GucMapBlock[GmcMouse.cX+1][GmcMouse.cY]|=  ((ucMap&0x02)*4);}
        if(GmcMouse.cY>0)
         {GucMapBlock[GmcMouse.cX][GmcMouse.cY-1]|=  ((ucMap&0x04)/4);}
        if(GmcMouse.cX>0)
          {GucMapBlock[GmcMouse.cX-1][GmcMouse.cY]|=  ((ucMap&0x08)/4);}
    
 
}

