#include "stm32f10x.h"
#include "contain.h"
#include "delay.h"
#include "usart.h"
#include "RF100_DRV.h"
#include "stdio.h"
#include "time.h"
#include "exti.h"

/*************Single Inventory example *****************/
void contain_test(void)
{
	RCC_HSICmd(ENABLE);
	delay_init();
	TIM3_INIT(100-1,480-1);  //1000us 
	USART1_INIT(115200);	//PA9;PA10; 
	
	delay_ms(200);

	while(1)
	{  
		if(flag1s == 1)
		{
			flag1s = 0; //1s;
			SendCmdFrame(SingleInventCMD);
		}
		USART_RXD_Data_Process();
		RF100_FRAME_RX_HANDLE();		
	}
}
/*************Multi Inventory example *****************/
void contain_test1(void)
{
	RCC_HSICmd(ENABLE);
	delay_init();
	TIM3_INIT(100-1,480-1);  //1000us 
	USART1_INIT(115200);	//PA9;PA10; 
	
	delay_ms(200);
	
	SendCmdFrame(MultiInventCMD);
	
	while(1)
	{  
		USART_RXD_Data_Process();
		RF100_FRAME_RX_HANDLE();		
	}
}
/*************Read/Write User bank example*****************/
uint8_t TagAccessPassword[4]={0x00,0x00,0x00,0x00};
uint8_t TagWriteDATA[8]={0x12,0x34,0x56,0x78,0x78,0x56,0x34,0x16};
//void contain_test2(void)
//{
//	unsigned char a=0;
//	RCC_HSICmd(ENABLE);
//	delay_init();
//	TIM3_INIT(100-1,480-1);  //1000us 
//	USART1_INIT(115200);	//PA9;PA10; 
//	
//	delay_ms(100);
//	SendCmdFrame(StopMultiInventCMD);
//	delay_ms(100);
//	//set select mode
//	SendCmdFrame(SetSelectModeCMD);
//	while(1)
//	{  
//		//first step inventory a tag EPC;
//		if((flag1s == 1)&&(RF100Status.InventATagFlag == 0))// no tag
//		{
//			flag1s = 0; //1s;
//			SendCmdFrame(SingleInventCMD);
//			printf("c");
//		}
//		//second step write TagWriteDATA array data to the first tag;
//		if((RF100Status.InventATagFlag == 1)&&(RF100Status.WriteDataFlag == 0)&&(flag1s == 1))
//		{
//			TAG_WR_PARA.AccessPW[0]=TagAccessPassword[0]; //ACCESS PASSWORD
//			TAG_WR_PARA.AccessPW[1]=TagAccessPassword[1];
//			TAG_WR_PARA.AccessPW[2]=TagAccessPassword[2];
//			TAG_WR_PARA.AccessPW[3]=TagAccessPassword[3];
//			TAG_WR_PARA.MemBank=Membank_USER;
//			TAG_WR_PARA.Offset=0;// OFFSET 0 WORD;(2BYTE=1WORD)
//			TAG_WR_PARA.len=4;//LENGTH 4 WORDS=8 BYTES;
//			TAG_WR_PARA.dat=TagWriteDATA;
//			TagWriteProcess(TAG[0].EPC,TAG_WR_PARA);
//			flag1s = 0; //1s;
//			printf("d");
//		}
//		//third step read the first tag data to TagReadDATA array;
//		if((RF100Status.InventATagFlag == 1)&&(RF100Status.WriteDataFlag == 1)&&(flag1s == 1))
//		{
//			TAG_WR_PARA.AccessPW[0]=TagAccessPassword[0]; //ACCESS PASSWORD
//			TAG_WR_PARA.AccessPW[1]=TagAccessPassword[1];
//			TAG_WR_PARA.AccessPW[2]=TagAccessPassword[2];
//			TAG_WR_PARA.AccessPW[3]=TagAccessPassword[3];
//			TAG_WR_PARA.MemBank=Membank_USER;
//			TAG_WR_PARA.Offset=0;// OFFSET 0 WORD;(2BYTE=1WORD)
//			TAG_WR_PARA.len=4;//LENGTH 4 WORDS=8 BYTES;
//			TAG_WR_PARA.dat=TAG_READ_DATA.dat;// data buffer
//			TagReadProcess(TAG[0].EPC,TAG_WR_PARA);
//			flag1s = 0; //1s;
//			for(a=0;a<12;a++){
//			printf("%d",TAG[0].EPC[a]);}
//		}
//		//all done and stop running; user can check TAG_READ_DATA struct 
//		if((RF100Status.InventATagFlag == 1)&&(EPC_Compare(TAG[0].EPC,TAG_READ_DATA.EPC) == 1))
//		{
//			//check the data in TAG_WR_PARA.dat or TAG_READ_DATA.dat;
//			printf("b");
//		}
//		USART_RXD_Data_Process();
//		RF100_FRAME_RX_HANDLE();	
////		printf("%d\n",TAG_Index);
//		delay_us(100000);
//	}
//}

void epcini()
{
	SendCmdFrame(SetSelectModeCMD);  
}

void epcscan(void)
{
//	static unsigned char a[10]={0};
	RCC_HSICmd(ENABLE);
	delay_init();
	TIM3_INIT(100-1,480-1);  //1000us 
	USART1_INIT(115200);	//PA9;PA10; 
//	SendCmdFrame(SetSelectModeCMD);  
	delay_ms(100);
//	SendCmdFrame(StopMultiInventCMD);
	delay_ms(100);
	//set select mode
	
	
	SendCmdFrame(SingleInventCMD);
	moter();
	if(rad>=44)moterb();
		//first step inventory a tag EPC;
//		if((flag1s == 1)&&(RF100Status.InventATagFlag == 0))// no tag
//		{
//			flag1s = 0; //1s;
//			SendCmdFrame(SingleInventCMD);
//			printf("c");
//		}
//		//second step write TagWriteDATA array data to the first tag;
//		if((RF100Status.InventATagFlag == 1)&&(RF100Status.WriteDataFlag == 0)&&(flag1s == 1))
//		{
//			TAG_WR_PARA.AccessPW[0]=TagAccessPassword[0]; //ACCESS PASSWORD
//			TAG_WR_PARA.AccessPW[1]=TagAccessPassword[1];
//			TAG_WR_PARA.AccessPW[2]=TagAccessPassword[2];
//			TAG_WR_PARA.AccessPW[3]=TagAccessPassword[3];
//			TAG_WR_PARA.MemBank=Membank_USER;
//			TAG_WR_PARA.Offset=0;// OFFSET 0 WORD;(2BYTE=1WORD)
//			TAG_WR_PARA.len=4;//LENGTH 4 WORDS=8 BYTES;
//			TAG_WR_PARA.dat=TagWriteDATA;
//			TagWriteProcess(TAG[0].EPC,TAG_WR_PARA);
//			flag1s = 0; //1s;
//			printf("d");
//		}
//		//third step read the first tag data to TagReadDATA array;
//		if((RF100Status.InventATagFlag == 1)&&(RF100Status.WriteDataFlag == 1)&&(flag1s == 1))
//		{
//			TAG_WR_PARA.AccessPW[0]=TagAccessPassword[0]; //ACCESS PASSWORD
//			TAG_WR_PARA.AccessPW[1]=TagAccessPassword[1];
//			TAG_WR_PARA.AccessPW[2]=TagAccessPassword[2];
//			TAG_WR_PARA.AccessPW[3]=TagAccessPassword[3];
//			TAG_WR_PARA.MemBank=Membank_USER;
//			TAG_WR_PARA.Offset=0;// OFFSET 0 WORD;(2BYTE=1WORD)
//			TAG_WR_PARA.len=4;//LENGTH 4 WORDS=8 BYTES;
//			TAG_WR_PARA.dat=TAG_READ_DATA.dat;// data buffer
//			TagReadProcess(TAG[0].EPC,TAG_WR_PARA);
//			flag1s = 0; //1s;
////			for(a=0;a<12;a++){
////			printf("%d",TAG[0].EPC[a]);}
//		}
//		//all done and stop running; user can check TAG_READ_DATA struct 
//		if((RF100Status.InventATagFlag == 1)&&(EPC_Compare(TAG[0].EPC,TAG_READ_DATA.EPC) == 1))
//		{
//			//check the data in TAG_WR_PARA.dat or TAG_READ_DATA.dat;
//			printf("b");
//		}
		USART_RXD_Data_Process();
		RF100_FRAME_RX_HANDLE();	
//		printf("%d\n",TAG_Index);
		delay_us(400000);
	}

/*********************************************END OF FILE**********************/


