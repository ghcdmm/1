/****************************************Copyright (c)**************************************************
**                               Guangzou cells-net Development 
***                              Science and technology company
**                                 http://www.cells-net.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			  rf100_drv.c
** Last modified Date:  2018-09-17
** Last Version:		V1.00
** Descriptions:		RF100 FRAME HANDLE
**------------------------------------------------------------------------------------------------------
** Created by:			Benny Yip
** Created date:		2018-09-17
** Version:			  	V1.00
** Descriptions:		RF100 FRAME HANDLE
**
********************************************************************************************************/
#define _RF100_DRV_MANL
#include "stm32f10x.h"
#include "RF100_DRV.h"
#include "usart.h"
#include "delay.h"
#include "exti.h"

unsigned char dbm[10][2]={0};
uint8_t SingleInventCMD[]=	{0xBB,0x00,0x22,0x00,0x00,0x22,0x7E};
uint8_t MultiInventCMD[]=		{0xBB,0x00,0x27,0x00,0x03,0x22,0x00,0x0a,0x56,0x7E};
uint8_t StopMultiInventCMD[]=	{0xBB,0x00,0x28,0x00,0x00,0x28,0x7E };
uint8_t SetRegionUSCMD[]=		{0xBB,0x00,0x07,0x00,0x01,0x02,0x0A,0x7E};
uint8_t SetRegionCH2CMD[]=	{0xBB,0x00,0x07,0x00,0x01,0x01,0x09,0x7E};
uint8_t SetSelectModeCMD[]=	{0xBB,0x00,0x12,0x00,0x01,0x02,0x15,0x7E};
uint8_t SetSelectParaCMD[]=	{0xBB,0x00,0x0C,0x00,0x13,0x23,0x00,0x00,0x00,0x20,0x60,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC2,0x7E};
uint8_t ReadDataCMD[]=			{0xBB,0x00,0x39,0x00,0x09,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x49,0x7E};
uint8_t WriteDataCMD[]=			{0xBB,0x00,0x49,0x00,0x11,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,
                             0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,
														 0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,
														 0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,
                             0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                             0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,
                             0x07,0x08,0x85,0x7E };


void Usart_buff_copy(uint8_t (*usart_buff)[200],uint8_t data_len,unsigned char j)
{
	uint8_t i;
	for(i=0;i<data_len;i++)
		RF100FRAME.dat[j][i]=usart_buff[j][i];// copy to frame handle buffer;
	RF100FRAME.byteLen[j]=data_len;
}
unsigned char Save_Tag(unsigned char (*FrameDat)[127],unsigned char j)
{
	uint8_t i,k,l;
	for(k=0;k<TAG_Index;k++)
	{
		for(i=0;i<12;i++)
			if(TAG[k].EPC[i]!=FrameDat[j][i+8])break;
		if(i>=12){TAG[k].RSSI=0XFF-FrameDat[j][5];l=k;break;}
	}
	if(k>=TAG_Index){
	for(i=0;i<12;i++)
		TAG[TAG_Index].EPC[i]=FrameDat[j][i+8];// copy to frame handle buffer;
	TAG[TAG_Index].RSSI=0XFF-FrameDat[j][5];
	TAG[TAG_Index].flag = 1;
	TAG_Index++;
	if(TAG_Index>=MAX_TAG_NUM)
		TAG_Index = 0;}
	return l;
}
void Save_Tag_Data(unsigned char (*FrameDat)[127],unsigned char j)
{
	uint8_t i;
	for(i=0;i<12;i++)
		TAG_READ_DATA.EPC[j][i]=FrameDat[j][i+8];// EPC copy ;
	TAG_READ_DATA.len[j]=FrameDat[j][4]-15;
	for(i=0;i<TAG_READ_DATA.len[j];i++)
		TAG_READ_DATA.dat[j][i]=FrameDat[j][i+20];// data copy ;
}
 uint8_t RF100_FRAME_RX_HANDLE(void)
{

	unsigned char j=0,l=0;
	for(j=0;j<num;j++){
	if(RF100FRAME.byteLen[j] == 0) //no frame
	{
		return 0;//error
	}
	if(RF100FRAME.dat[j][1]==FRAME_RES)//frame type response
	{
		switch(RF100FRAME.dat[j][2])//command code
		{
			case CMD_STOP_MULTI :
				RF100Status.MultiInventFlag = 0;
				break;
			case CMD_READ_DATA :
				RF100Status.ReadDataFlag = 1;
				Save_Tag_Data(RF100FRAME.dat,j);
				break;
			case CMD_WRITE_DATA :
				RF100Status.WriteDataFlag = 1;
				break;
			case CMD_SET_REGION :
				RF100Status.SetRegionFlag = 1;
				break;
			case CMD_SET_POWER :
				RF100Status.SetPowerFlag = 1;
				break;
			case CMD_SET_SELECT_PARA :
				RF100Status.SetSelectParaFlag = 1;
				break;
			case CMD_SET_INV_MODE : //select mode
				RF100Status.SetSelectModeFlag = 1;
				break;
			case CMD_FAIL :
				break;
			default :
				break;
		}
	}
	else if(RF100FRAME.dat[j][1]==FRAME_ERROR)//frame type error
	{
		;
	}
	else if(RF100FRAME.dat[j][1]==FRAME_INFO)//frame type information
	{
		switch(RF100FRAME.dat[j][2])//command code
		{
			case CMD_SINGLE_ID :
				l=Save_Tag(RF100FRAME.dat,j);
			  RF100Status.InventATagFlag = 1;
			  RF100Status.TotalTags++;
				if(dbm[l][0]<TAG[l].RSSI)
				{
					dbm[l][0]=TAG[l].RSSI;
					dbm[l][1]=rad;
				}
				break;
			case CMD_MULTI_ID :
				l=Save_Tag(RF100FRAME.dat,j);
			  RF100Status.InventATagFlag = 1;
			  RF100Status.TotalTags++;
				if(dbm[l][0]<TAG[l].RSSI){dbm[l][0]=TAG[l].RSSI;dbm[l][1]=rad;}
				break;
			default :
				break;
		}
	}
	RF100FRAME.byteLen[j] =0;
	return 1;}
}
uint8_t Rf100_checksum(uint8_t * t_dat)
{
	uint8_t checksum,i,cmdDatLen;
	cmdDatLen = t_dat[4];
  checksum = t_dat[1] + t_dat[2] + t_dat[4];
	for (i = 0; i < cmdDatLen; i++)
	{
		checksum += t_dat[i+5];
	}
	t_dat[cmdDatLen+5]=checksum;
	return 0;
}
void TagReadProcess(uint8_t *t_EPC,TAG_WR_PARA_t t_TAG_WR_PARA)
{
	uint8_t i;
	for (i = 0; i < 12; i++)//set epc
	{
		SetSelectParaCMD[i+12] = t_EPC[i];
	}
	SetSelectParaCMD[5] = (SetSelectParaCMD[5]&0xf0)+t_TAG_WR_PARA.MemBank;
	Rf100_checksum(SetSelectParaCMD);
	SendCmdFrame(SetSelectParaCMD);
	delay_10us(2000);//must delay 20MS;
	//ACCESS PASSWORD
	ReadDataCMD[5]=t_TAG_WR_PARA.AccessPW[0];
	ReadDataCMD[6]=t_TAG_WR_PARA.AccessPW[1];
	ReadDataCMD[7]=t_TAG_WR_PARA.AccessPW[2];
	ReadDataCMD[8]=t_TAG_WR_PARA.AccessPW[3];
	//MEMBANK
	ReadDataCMD[9]=t_TAG_WR_PARA.MemBank;
	//OFFSET
	ReadDataCMD[10]=00;
	ReadDataCMD[11]=t_TAG_WR_PARA.Offset;
	//length
	ReadDataCMD[12]=00;
	ReadDataCMD[13]=t_TAG_WR_PARA.len;
	Rf100_checksum(ReadDataCMD);
	SendCmdFrame(ReadDataCMD);
}
void TagWriteProcess(uint8_t *t_EPC,TAG_WR_PARA_t t_TAG_WR_PARA)
{
	uint8_t i;
	for (i = 0; i < 12; i++)//set epc
	{
		SetSelectParaCMD[i+12] = t_EPC[i];
	}
	SetSelectParaCMD[5] = (SetSelectParaCMD[5]&0xf0)+t_TAG_WR_PARA.MemBank;
	Rf100_checksum(SetSelectParaCMD);
	SendCmdFrame(SetSelectParaCMD);
	delay_10us(40);//must delay 400us;
	WriteDataCMD[4] = (t_TAG_WR_PARA.len*2)+9;// cmd data length
//ACCESS PASSWORD
	WriteDataCMD[5]=t_TAG_WR_PARA.AccessPW[0];
	WriteDataCMD[6]=t_TAG_WR_PARA.AccessPW[1];
	WriteDataCMD[7]=t_TAG_WR_PARA.AccessPW[2];
	WriteDataCMD[8]=t_TAG_WR_PARA.AccessPW[3];
	//MEMBANK
	WriteDataCMD[9]=t_TAG_WR_PARA.MemBank;
	//OFFSET
	WriteDataCMD[10]=00;
	WriteDataCMD[11]=t_TAG_WR_PARA.Offset;
	//length
	WriteDataCMD[12]=00;
	WriteDataCMD[13]=t_TAG_WR_PARA.len;
	for (i = 0; i < (t_TAG_WR_PARA.len*2); i++)//set epc
	{
		WriteDataCMD[i+14] = t_TAG_WR_PARA.dat[i];
	}
	Rf100_checksum(WriteDataCMD);
	WriteDataCMD[(t_TAG_WR_PARA.len*2)+9+6]=0x7e;//frame end
	SendCmdFrame(WriteDataCMD);
}
uint8_t EPC_Compare(uint8_t *t_EPC1,uint8_t *t_EPC2)
{
	uint8_t i;
	i=12;
	do
	{
		if(t_EPC1[i-1]!=t_EPC2[i-1])
			return 0;//different
		i--;
	}while(i);
	return 1; //same
}

