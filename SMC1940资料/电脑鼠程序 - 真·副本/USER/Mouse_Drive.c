#include "Mouse_Drive.h"
#include "bsp_usart1.h"
#include "Mouse_Drive_Init.h"
/*********************************************************************************************************
  定义全局变量
*********************************************************************************************************/
unsigned char mstate=1;
MAZECOOR          GmcMouse                        = {0,0};              /*  保存电脑鼠当前位置坐标      */
uint8             GucMouseDir                     = UP;                 /*  保存电脑鼠当前方向          */
uint8             GucMapBlock[MAZETYPE][MAZETYPE] = {0};                /*  GucMapBlock[x][y]           */
                                                                        /*  x,横坐标;y,纵坐标;          */
uint8             GucMapBlock0[MAZETYPE][MAZETYPE] = {0};               /*  中心算法         */
                                                                   /*  bit3~bit0分别代表左下右上   */
                                                                        /*  0:该方向无路，1:该方向有路  */
uint8             GucMapBlock1[MAZETYPE][MAZETYPE]= {0x0f};				//洪水用
uint8             GucMouseStart                    = 0;                 /* 电脑鼠启动     			    */
	uint8             GucFrontJinju                    = 0;                 /* 前方红外近距在搜索时等高图制作中用   */
uint8             GucCrossroad                     = 0;                 /* 十字路口数，冲刺时用，若十字路口多降低最高冲刺速度*/
 __MOTOR  __GmRight                         = {0, 0, 0, 0, 0}; /*  定义并初始化右电机状态      */
 __MOTOR  __GmLeft                         = {0, 0, 0, 0, 0};    
static __PID    __GmLPID;                                               /*  定义左电机PID   			*/
static __PID    __GmRPID;                                               /*  定义右电机PID   			*/
static __PID    __GmSPID;                                               /*  直线PID    					*/
static __PID    __GmWPID;                                               /*  旋转PID   					*/
static uint8    __GucMouseState                   = __STOP;             /*  保存电脑鼠当前运行状态      */
static int32    __GiMaxSpeed                      = SEARCHSPEED;        /*  保存允许运行的最大速度      */
static uint8    __GucDistance[4]                  = {0};                /*  记录传感器状态              */
volatile float    GsTpusle_T                      = 0;                  /*  左轮校正减少的速度值，WPID用*/
static uint8    GuiSpeedCtr                       = 0;					/*	记录电机加减速度			*/
static uint16   GuiTpulse_LR                      = 0;
volatile static uint16   GuiTpulse_S                     = 0;
volatile static uint8    GucFrontNear                    = 0;			/*前方有墙靠近墙标志			*/
uint8    GucGoHead          	           = 0;							/*前方是否有墙标志				*/
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
//float W;																/*得到的角速度的值				*/
uint16 Sunlight[6];														/*自然光线的值					*/
extern __IO uint16 ADC_ConvertedValue[6];								/*直接读取的红外强度			*/
uint16 ucIRCheck[6];													/*排除自然光线干扰后的红外强度	*/
//红外的设定值
uint16 GusDistance_L_Near=1280; //左红外近
uint16 GusDistance_L_Mid=740;  //左红外中
uint16 GusDistance_L_Far=270;  //左红外远
uint16 GusDistance_R_Near=1400; //右红外近
uint16 GusDistance_R_Mid=900;  //右红外中
uint16 GusDistance_R_Far=430;  //右红外远
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
** Descriptions:        延时函数
** input parameters:    uiD :延时参数，值越大，延时越久
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void __delay (uint32  uiD)
{
    for (; uiD; uiD--);
}


void Delay_us(u32 Nus)   
{    
	 SysTick->LOAD=Nus*9;          	  //时间加载 
	 SysTick->CTRL|=0x01;             //开始倒数   
	 while(!(SysTick->CTRL&(1<<16))); //等待时间到达   
	 SysTick->CTRL=0X00000000;        //开始倒数  
	 SysTick->VAL=0X00000000;         //清空计数器
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
** Descriptions:        PID初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PIDInit(void) 
{  
    pulse_L_counter = 0;
    __GmLPID.usFeedBack = 0 ;  //速度反馈值
    __GmLPID.sFeedBack = 0 ;
    
    pulse_R_counter = 0;
    __GmRPID.usFeedBack = 0 ;  //速度反馈值
    __GmRPID.sFeedBack = 0 ;
    
    __GmSPID.sRef = 0 ;        //速度设定值 
    __GmSPID.sFeedBack = 0 ;        
    __GmSPID.sPreError = 0 ;   //前一次，速度误差,,vi_Ref - vi_FeedBack 
    __GmSPID.sPreDerror = 0 ;   //前一次，速度误差之差，d_error-PreDerror; 
        
    __GmSPID.fKp = __KP; //80
    __GmSPID.fKi = __KI; //0.01
    __GmSPID.fKd = __KD; //0
       
    __GmSPID.iPreU = 0 ;      //电机控制输出值 
    
    __GmWPID.sRef = 0 ;        
    __GmWPID.sFeedBack = 0 ;       
    __GmWPID.sPreError = 0 ; 
    __GmWPID.sPreDerror = 0 ; 
    
    __GmWPID.fKp = __KP;  //30
    __GmWPID.fKi = __KI;  //0.1,0.01
    __GmWPID.fKd = __KD; 
       
    __GmWPID.iPreU = 0 ;      //电机控制输出值
}

/*********************************************************************************************************
** Function name:       __SPIDContr
** Descriptions:        直线PID控制
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void __SPIDContr(void) 
{ 
    float  error,d_error,dd_error;
    static uint8   K_I=1;
    error = __GmSPID.sRef - __GmSPID.sFeedBack; // 偏差计算
    d_error = error - __GmSPID.sPreError; 
    dd_error = d_error - __GmSPID.sPreDerror;
    if(error> Deadband)//死区范围
      error -= Deadband;
    else if(error < -Deadband)
      error += Deadband;
    else
      error = 0;
    if((error > error_IMAX)||(error < -error_IMAX))//积分限幅
      K_I=0;
    else
      K_I=1;
    
    __GmSPID.sPreError = error; //存储当前偏差 
    __GmSPID.sPreDerror = d_error;
    
    __GmSPID.iPreU += (int16)(  __GmSPID.fKp * d_error + K_I*__GmSPID.fKi * error  + __GmSPID.fKd*dd_error); 
}

void __IPIDContrL(void) 
{ 
    float  error,d_error,dd_error;
    static uint8   K_I=1;
    error = leftf - ucIRCheck[2]; // 偏差计算
    d_error = error - IPreErrorL; 
    dd_error = d_error - IPreDerrorL;
//    if(error> IDeadband)//死区范围
//      error -= IDeadband;
//    else if(error < -IDeadband)
//      error += IDeadband;
//    else
//      error = 0;
//    if((error > error_IMAX)||(error < -error_IMAX))//积分限幅
//      K_I=0;
//    else
//      K_I=1;
    
    IPreErrorL = error; //存储当前偏差 
    IPreDerrorL = d_error;
    
    IPUL += (int16)(  __KP * d_error + K_I*__KI * error  + __KD*dd_error); 
}

void __IPIDContrR(void) 
{ 
    float  error,d_error,dd_error;
    static uint8   K_I=1;
    error = rightf - ucIRCheck[3]; // 偏差计算
    d_error = error - IPreErrorR; 
    dd_error = d_error - IPreDerrorR;
//    if(error> IDeadband)//死区范围
//      error -= IDeadband;
//    else if(error < -IDeadband)
//      error += IDeadband;
//    else
//      error = 0;
//    if((error > error_IMAX)||(error < -error_IMAX))//积分限幅
//      K_I=0;
//    else
//      K_I=1;
    
    IPreErrorR = error; //存储当前偏差 
    IPreDerrorR = d_error;
    
    IPUR += (int16)(  __KP * d_error + K_I*__KI * error  + __KD*dd_error); 
}
/*********************************************************************************************************
** Function name:       __WPIDContr
** Descriptions:        旋转方向PID控制
** input parameters:    无 
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void __WPIDContr(void) 
{ 
    float  error,d_error,dd_error; 
    static uint8   K_I=1;
    error = __GmWPID.sRef + GsTpusle_T/*+ IPUL/200 -IPUR/400*/ - __GmWPID.sFeedBack; // 偏差计算
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
    
    __GmWPID.sPreError = error; //存储当前误差
    __GmWPID.sPreDerror = d_error;
    __GmWPID.iPreU += (int16)(  __GmWPID.fKp * d_error + K_I*__GmWPID.fKi * error  + __GmWPID.fKd*dd_error);
        
}
/*********************************************************************************************************
** Function name:      __PIDContr
** Descriptions:        PID控制，通过脉冲数控制电机
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void __PIDContr(void)
{
    __SPIDContr();
    __WPIDContr();
    __GmLeft.sSpeed = __GmSPID.iPreU - __GmWPID.iPreU ;
//    if(__GmLeft.sSpeed>=0){
//     __GmLeft.cDir=__MOTORGOAHEAD; 
    if( __GmLeft.sSpeed >= U_MAX )   //速度PID，防止调节最高溢出 
       __GmLeft.sSpeed = U_MAX;      
    if( __GmLeft.sSpeed <= U_MIN ) //速度PID，防止调节最低溢出  
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
    if( __GmRight.sSpeed >= U_MAX )   //速度PID，防止调节最高溢出 
       __GmRight.sSpeed = U_MAX;      
    if( __GmRight.sSpeed <= U_MIN ) //速度PID，防止调节最低溢出  
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
** Descriptions:        采集输出的脉冲
** input parameters:    无 
** output parameters:   无
** Returned value:      无
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
          GPIO_SetBits(GPIOC,GPIO_Pin_6);  //指向前方 左 开
           break;  
      case 1:                                                         
          GPIO_SetBits(GPIOC,GPIO_Pin_7);  //指向左方 左 开
          break; 
      case 2:                                                         
          GPIO_SetBits(GPIOC,GPIO_Pin_8);  //指向右方 右 开
          break;   
      case 3:                                                         
          GPIO_SetBits(GPIOA,GPIO_Pin_7);  //指向前方 右 开
          break;
			case 4:                                                         
          GPIO_SetBits(GPIOA,GPIO_Pin_5);  //指向右方 右 开
          break;   
      case 5:                                                         
          GPIO_SetBits(GPIOA,GPIO_Pin_4);  //指向前方 右 开
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
        case 0://指向 前 方 （左侧）
					
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
//              __GucDistance[__FRONTL]  |= 0x01;//前方有墙，末位强制变1
//          }
//          else
//          {
//              __GucDistance[__FRONTL]  &= 0xfe;//前方无墙，末位强制变0             
//          }
//          
//          if(ucIRCheck[0]>GusDistance_FL_Near)
//          {
//              __GucDistance[__FRONTL]  |= 0x02;//靠近墙，末位强制变1X
//          }
//          
//          else
//          {
//              __GucDistance[__FRONTL]  &= 0xfd;//不靠近墙，末位0X
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
//              __GucDistance[__LEFT]  |= 0x01;//左方有墙，末位置1
//          }
//          else
//          {
//               __GucDistance[__LEFT] &= 0xfe;//左方无墙，末位置0
//          } 
//          
//          if(ucIRCheck[1]>GusDistance_L_Mid)
//          {
//              __GucDistance[__LEFT]  |= 0x02;//赛道偏左，11
//          }
//          else
//          {
//              __GucDistance[__LEFT]  &= 0xfd;//赛道不偏左，0X
//          }     
//		  
//          if(ucIRCheck[1]>GusDistance_L_Near)
//          {
//              __GucDistance[__LEFT]   |= 0x04;//左侧靠近墙壁需要调整，111
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
		  
        case 6://指向前方 右
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
** Descriptions:        右直流电机驱动
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        左直流电机驱动
** input parameters:    __GmLeft.cDir :电机运行方向
** output parameters:   无
** Returned value:      无
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
** Descriptions:        电脑鼠加速程序
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void __SpeedUp (void)
{
    uint16 Speed;
    Speed=__GmSPID.sFeedBack;
    if(__GmSPID.sRef<__GiMaxSpeed){			//若小于允许运行的最大速度（1ms脉冲数）
      if(Speed >=__GmSPID.sRef)				//若反馈速度大于速度设定值
      {
        __GmSPID.sRef=__GmSPID.sRef+4;		//增大速度设定值10
      }
    }   
}
/*********************************************************************************************************
** Function name:       __SpeedDown
** Descriptions:        电脑鼠减速程序
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void __SpeedDown (void)
{
    uint16 Speed;
    Speed=__GmSPID.sFeedBack;
    if(__GmSPID.sRef>=MINSPEED)				//若大于允许运行的最小速度
    {
      if(Speed <=__GmSPID.sRef+1)			//若反馈速度小于速度设定值
      {
       __GmSPID.sRef=__GmSPID.sRef-1;		//减小速度设定值3
      }
    }
}

/*********************************************************************************************************
** Function name:       SysTick_Handler
** Descriptions:        定时中断扫描。
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
//          case __MOTORSTOP:                                                   /*  停止，同时清零速度和脉冲值  */
//              __GmRight.uiPulse    = 0;
//              pulse_R = 0;
//              __GmLeft.uiPulse    = 0;
//              pulse_L = 0;
//              break;
//      
//          case __WAITONESTEP:                                                 /*  暂停一步                    */
//              __GmRight.cState = __MOTORRUN;
//              if((((ucIRCheck[1]>GusDistance_L_Near)&&(ucIRCheck[2]<GusDistance_R_Near))&&(ucIRCheck[2]>GusDistance_R_Mid)))          //偏左
//              {	
//                GsTpusle_T = -2;		/*  左轮校正减少的速度值*/			//左轮加速
//              }
//              else if((((ucIRCheck[1]<GusDistance_L_Near)&&(ucIRCheck[2]>GusDistance_R_Near))&&(ucIRCheck[1]>GusDistance_L_Mid)))     //偏右
//              {
//                GsTpusle_T = 2;												//左轮减速
//              }

//              if((ucIRCheck[1]>GusDistance_L_Near)&&(ucIRCheck[2]<GusDistance_R_Mid))          //偏左
//              {
//                GsTpusle_T = -3;															   //旋转速度减小
//              }
//              else if((ucIRCheck[2]>GusDistance_R_Near)&&(ucIRCheck[1]<GusDistance_L_Mid))     //偏右
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
//          case __MOTORRUN:                                                    /*  电机运行                    */

//            if (__GucMouseState == __GOAHEAD)                                 /*  根据传感器状态微调电机位置  */
//            {                              
//                  if ((ucIRCheck[1]>GusDistance_L_Near)&&(ucIRCheck[2]<GusDistance_R_Near))	//偏左
//                  {
//                    if (n == 1)
//                    {
//                          __GmRight.cState = __WAITONESTEP;
//                    }               
//                    n++;
//                    n %= 2;
//                  }
//                  else if((ucIRCheck[1]<GusDistance_L_Near)&&(ucIRCheck[2]>GusDistance_R_Near))	//偏右
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
////具体操作自行添加
//            break;

//		  case __MOTORTURNBACK:
////具体操作自行添加			  
//		  break;
//		  
//          default:
//              break;
//    }           
}


/*********************************************************************************************************
** Function name:      mouseGoaheadhui
** Descriptions:        前进N格,最后回起点用，要用前方红外
** input parameters:    iNblock: 前进的格数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mouseGoahead (int8  cNBlock)                                    //连续转弯用
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
        if (pulse_L >= ONEBLOCK) {                          /*  判断是否走完一格            */
            __GmLeft.uiPulse    -= ONEBLOCK;
            pulse_L -= ONEBLOCK;
            if (cCoor) {
                cNBlock--;
                if(cNBlock==0)
                   goto End;
                if(cNBlock<cB-1)//给回速一个时间  cB=cNBlock
                  GuiSpeedCtr=__SPEEDUP;
				} else {
                cCoor = 1;
            }
        }
        
        if (pulse_R >= ONEBLOCK) {                         /*  判断是否走完一格            */
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
            }		  else {                                                    /*  左边有墙时开始允许检测左边  */
                if (ucIRCheck[1]>GusDistance_L_Far) {
                    cL = 1;
                }
            }
         if (cR) 
            {                                                       /*  是否允许检测右边            */
            if (ucIRCheck[2]<GusDistance_R_Far)               /*  右边有支路，跳出程序        */
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
                if (ucIRCheck[2]>GusDistance_R_Far) {                   /*  右边有墙时开始允许检测右边  */
                    cR = 1;
                }
            }
        }
   }
    /*
     *  设定运行任务，让电脑鼠走到支路的中心位置
     */
End:     ;
}

void mouseStop(void)
{   
  __GmSPID.sRef=0;
  __GmWPID.sRef=0;
  GuiSpeedCtr=3;  //既不加速也不减速
}
/****************************************************************************************************
** Function name:       mouseTurnright
** Descriptions:        右转
** input parameters:    无
** output parameters:   无
** Returned value:      无      
*********************************************************************************************************/
void mouseTurnright(void)
{    
  
   __GucMouseState   = __TURNRIGHT; 
   __GmRight.cState =__MOTORRUN;        //得加电脑鼠状态
   __GmLeft.cState  =__MOTORRUN;
   GucMouseDir     = (GucMouseDir + 1) % 4;                            /*  方向标记                    */

   __GmWPID.sRef=-70;
    while(1)
   {
	   //根据用脉冲或角度判断自行添加
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
** Descriptions:        左转
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mouseTurnleft(void)
{
   __GucMouseState  = __TURNLEFT; 
   __GmRight.cState = __MOTORTURN;        //得加电脑鼠状态
   __GmLeft.cState  = __MOTORTURN;
     GucMouseDir    = (GucMouseDir + 3) % 4;                            /*  方向标记                    */
//仿照上一个自行添加
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
** Descriptions:        根据前方近距，旋转180度
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        前进N格
** input parameters:    iNblock: 前进的格数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void mazeSearch(void)                  //搜索连续转弯
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
        {                          /*  判断是否走完一格*/
            __GmLeft.uiPulse    -= ONEBLOCK;
            pulse_L -= ONEBLOCK;
            if (cCoor) 
            {
                if(((__GucDistance[__FRONTR]!=0)&&(__GucDistance[__FRONTL]!=0))&&(ucIRCheck[2]>GusDistance_L_Far)&&(ucIRCheck[3]>GusDistance_R_Far))//0x01
              {          //左方右方前方都有墙
               GucFrontNear=1; 
                goto End;
              }
              __mouseCoorUpdate();                                    /*  更新坐标                    */
            } 
            else 
            {
                cCoor = 1;
            }
        }
        if (pulse_R >= ONEBLOCK) {                         /*  判断是否走完一格            */
            __GmRight.uiPulse    -= ONEBLOCK;
            pulse_R -= ONEBLOCK;
        }
        
        if (cL) {                                                       /*  是否允许检测左边            */
            if  ((__GucDistance[__LEFT]  & 0x01)==0)
            {                 /*  左边有支路，跳出程序        */
            
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
        } else {                                                        /*  左边有墙时开始允许检测左边  */
            if (ucIRCheck[1]>GusDistance_L_Far) {
                cL = 1; 
               
            }
        }
        if (cR) {                                                       /*  是否允许检测右边            */
            if ((__GucDistance[__RIGHT]  & 0x01)==0){        	       /*  右边有支路，跳出程序        */
            
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
          __mouseCoorUpdate();                                            /*  更新坐标                    */

    
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
** Descriptions:        根据当前方向更新坐标值
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        根据传感器检测结果判断是否存在墙壁
** input parameters:    无
** output parameters:   无
** Returned value:      cValue: 低三位从左到右一次代表左前右。1为有墙，0为没墙。
*********************************************************************************************************/
void __wallCheck (void)
{
    uint8 ucMap = 0;
    uint8 uctemp = 0;
    ucMap |= MOUSEWAY_B;
    
    if (ucIRCheck[1]>GusDistance_L_Far)//左边有挡板，但是距离较远
    {
        ucMap &= ~MOUSEWAY_L;          //相对方向的左边有墙，并却转换成绝对坐标
        uctemp |= 0x06;
    }
    else 
    {
        ucMap |=  MOUSEWAY_L;         //相对方向的左边无墙，并却转换成绝对坐标   
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
        {GucMapBlock1[GmcMouse.cX][GmcMouse.cY+1] &= ~(((~ucMap)&0x01)*4);}       /*将该坐标周围坐标墙壁资料更改  注：洪水用*/
       if(GmcMouse.cX<(MAZETYPE-1))
        { GucMapBlock1[GmcMouse.cX+1][GmcMouse.cY]&= ~(((~ucMap)&0x02)*4);}
         if(GmcMouse.cY>0)
         {GucMapBlock1[GmcMouse.cX][GmcMouse.cY-1]&= ~(((~ucMap)&0x04)/4);}
        if(GmcMouse.cX>0)
         {GucMapBlock1[GmcMouse.cX-1][GmcMouse.cY]&= ~(((~ucMap)&0x08)/4);}
              
      if(GmcMouse.cY<(MAZETYPE-1))
         {GucMapBlock[GmcMouse.cX][GmcMouse.cY+1] |=    ((ucMap&0x01)*4);}        /*将该坐标周围坐标墙壁资料更改  注：在初始为有墙时管用*/
      if(GmcMouse.cX<(MAZETYPE-1))
         { GucMapBlock[GmcMouse.cX+1][GmcMouse.cY]|=  ((ucMap&0x02)*4);}
        if(GmcMouse.cY>0)
         {GucMapBlock[GmcMouse.cX][GmcMouse.cY-1]|=  ((ucMap&0x04)/4);}
        if(GmcMouse.cX>0)
          {GucMapBlock[GmcMouse.cX-1][GmcMouse.cY]|=  ((ucMap&0x08)/4);}
    
 
}

