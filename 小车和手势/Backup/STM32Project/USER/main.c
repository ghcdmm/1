#include "bsp.h" 
#include<math.h>
#include<stdlib.h>
 
 
 	s32 temp0,temp1,temp2,temp3,temp4,temp00,temp11,temp22,temp33;
	s32 TA1[3][3][5],TA2[3],TA3[3],TB1[3],TB2[3],TB3[3],TB4[3],TB5[3],TB6[3],TB7[3],TB8[3];
/*******************************************************************************
* 函数名	: main
* 描述	    : 主函数
* 输入参数  : 无
* 返回参数  : 无
*******************************************************************************/	
//-----------------------我是knn的分割线------------------------------------------
//-----------------------我也是---------------------------------------------------
/*
#define K 3 //临近数k
typedef float type;

//动态二维数组
type **createarray(int n,int m)
{
    int i;
    type **array;
    array=(type **)malloc(n*sizeof(type *));
    array[0]=(type *)malloc(n*m*sizeof(type));
    for(i=1;i<n;i++) array[i]=array[i-1]+m;
    return array;
}
//读数据,格式：N=数据量,D=维数
void loaddata(int *n,int *d,type ***array,type ***karray)
{
    int i,j;
    FILE *fp;
    if((fp=fopen("data.txt","r"))==NULL)    fprintf(stderr,"can not open data.txt!\n");
    if(fscanf(fp,"N=%d,D=%d",n,d)!=2)    fprintf(stderr,"reading error!\n");
    *array=createarray(*n,*d);
    *karray=createarray(2,K);

    for(i=0;i<*n;i++)
        for(j=0;j<*d;j++)
            fscanf(fp,"%f",&(*array)[i][j]);    //????

    for(i=0;i<2;i++)
        for(j=0;j<K;j++)
            (*karray)[i][j]=9999.0;    //??????
    if(fclose(fp))    fprintf(stderr,"can not close data.txt");
}
//??????
type computedistance(int n,type *avector,type *bvector)
{
    int i;
    type dist=0.0;
    for(i=0;i<n;i++)
        dist+=pow(avector[i]-bvector[i],2);
    return sqrt(dist);
}
//????
void bublesort(int n,type **a,int choice)
{
    int i,j;
    type k;
    for(j=0;j<n;j++)
        for(i=0;i<n-j-1;i++){
            if(0==choice){
                if(a[0][i]>a[0][i+1]){
                    k=a[0][i];
                    a[0][i]=a[0][i+1];
                    a[0][i+1]=k;
                    k=a[1][i];
                    a[1][i]=a[1][i+1];
                    a[1][i+1]=k;
                }
            }
            else if(1==choice){
                if(a[1][i]>a[1][i+1]){
                    k=a[0][i];
                    a[0][i]=a[0][i+1];
                    a[0][i+1]=k;
                    k=a[1][i];
                    a[1][i]=a[1][i+1];
                    a[1][i+1]=k;
                }
            }
        }
}
//???????????
type orderedlist(int n,type *list)
{
    int i,count=1,maxcount=1;
    type value;
    for(i=0;i<(n-1);i++) {
        if(list[i]!=list[i+1]) {
            //printf("count of %d is value %d\n",list[i],count);
            if(count>maxcount){
                maxcount=count;
                value=list[i];
                count=1;
            }
        }
        else
            count++;
    }
    if(count>maxcount){
            maxcount=count;
            value=list[n-1];
    }
    //printf("value %f has a Maxcount:%d\n",value,maxcount);
    return value;
}



int main()
{
    int i,j,k;
    int D,N;    //??,???
    type **array=NULL;  //????
    type **karray=NULL; //  K??????????
    type *testdata; //????
    type dist,maxdist;

    loaddata(&N,&D,&array,&karray);
    testdata=(type *)malloc((D-1)*sizeof(type));
    printf("input test data containing %d numbers:\n",D-1);
    for(i=0;i<(D-1);i++)    scanf("%f",&testdata[i]);

    while(1){
    for(i=0;i<K;i++){
        if(K>N) exit(-1);
        karray[0][i]=computedistance(D-1,testdata,array[i]);
        karray[1][i]=array[i][D-1];
        //printf("first karray:%6.2f  %6.0f\n",karray[0][i],karray[1][i]);
    }

    bublesort(K,karray,0);
    //for(i=0;i<K;i++)    printf("after bublesort in first karray:%6.2f  %6.0f\n",karray[0][i],karray[1][i]);
    maxdist=karray[0][K-1]; //???k??????????

    for(i=K;i<N;i++){
        dist=computedistance(D-1,testdata,array[i]);
        if(dist<maxdist)
            for(j=0;j<K;j++){
                if(dist<karray[0][j]){
                    for(k=K-1;k>j;k--){ //j?????????,??????
                        karray[0][k]=karray[0][k-1];
                        karray[1][k]=karray[1][k-1];
                    }
                    karray[0][j]=dist;  //???j??
                    karray[1][j]=array[i][D-1];
                    //printf("i:%d  karray:%6.2f %6.0f\n",i,karray[0][j],karray[1][j]);
                    break;  //???karray????
                }
            }
        maxdist=karray[0][K-1];
        //printf("i:%d  maxdist:%6.2f\n",i,maxdist);
    }
    //for(i=0;i<K;i++)    printf("karray:%6.2f  %6.0f\n",karray[0][i],karray[1][i]);
    bublesort(K,karray,1);
    //for(i=0;i<K;i++)    printf("after bublesort in karray:%6.2f  %6.0f\n",karray[0][i],karray[1][i]);
    printf("\nThe data has a tag: %.0f\n\n",orderedlist(K,karray[1]));

    printf("input test data containing %d numbers:\n",D-1);
    for(i=0;i<(D-1);i++)    scanf("%f",&testdata[i]);
    }
    return 0;
}
*/
//-----------------------我是knn的分割线------------------------------------------
//-----------------------我也是---------------------------------------------------

void Btn_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_4); 	          //初始化高电平
	GPIOB->CRL&=0X0000FFFF;GPIOB->CRL|=8<<16;GPIOB->CRL|=8<<20;GPIOB->CRL|=8<<24;GPIOB->CRL|=(unsigned long)8<<28;
}

u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{
  
 if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin)==0)
  {
      delay_ms(200);
        if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin)==0)
     {
        while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin)==0);
      return 0;
     }
     else
      return 1;
  }
  else
   return 1;
}

void train_1()
{
	unsigned char i,j,k;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			//while(Key_Scan(GPIOB,GPIO_Pin_4)==1);
			//while(Key_Scan(GPIOB,GPIO_Pin_4)==0);
			for(k=0;k<10;k++)
			{
				temp0 += (ReadRegfdc2214(0x00)<<16)+(ReadRegfdc2214(0x01));
				temp1 += (ReadRegfdc2214(0x02)<<16)+(ReadRegfdc2214(0x03));
				temp2 += (ReadRegfdc2214(0x04)<<16)+(ReadRegfdc2214(0x05));
				temp3 += (ReadRegfdc2214(0x06)<<16)+(ReadRegfdc2214(0x07));
				temp4 += (ReadRegfdc22142(0x00)<<16)+(ReadRegfdc22142(0x01));
			}
			TA1[i][j][0]=temp0/10;temp0=0;
			TA1[i][j][1]=temp1/10;temp1=0;
			TA1[i][j][2]=temp2/10;temp2=0;
			TA1[i][j][3]=temp3/10;temp3=0;
			TA1[i][j][4]=temp4/10;temp4=0;
		}
}

s32 rec_1()
{
	s32 d0[3][3];
	unsigned char i,j,k,m[3];
	//while(Key_Scan(GPIOB,GPIO_Pin_4)==1);
	//while(Key_Scan(GPIOB,GPIO_Pin_4)==0);
	for(k=0;k<10;k++)
	{
		temp0 += (ReadRegfdc2214(0x00)<<16)+(ReadRegfdc2214(0x01));
		temp1 += (ReadRegfdc2214(0x02)<<16)+(ReadRegfdc2214(0x03));
		temp2 += (ReadRegfdc2214(0x04)<<16)+(ReadRegfdc2214(0x05));
		temp3 += (ReadRegfdc2214(0x06)<<16)+(ReadRegfdc2214(0x07));
		temp4 += (ReadRegfdc22142(0x00)<<16)+(ReadRegfdc22142(0x01));
	}
	temp0/=10;temp1/=10;temp2/=10;temp3/=10;temp4/=10;
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			//d0[i][j]=sqrt((TA1[i][j][0]-temp0)*(TA1[i][j][0]-temp0)+(TA1[i][j][1]-temp1)*(TA1[i][j][1]-temp1)+(TA1[i][j][2]-temp2)*(TA1[i][j][2]-temp2)+(TA1[i][j][3]-temp3)*(TA1[i][j][3]-temp3)+(TA1[i][j][4]-temp4)*(TA1[i][j][4]-temp4));
			d0[i][j]=fabs(TA1[i][j][0]-temp0)+fabs(TA1[i][j][1]-temp1)+fabs(TA1[i][j][2]-temp2)+fabs(TA1[i][j][3]-temp3)+fabs(TA1[i][j][4]-temp4);
		}
	}
	for(k=0;k<3;k++)
		for(i=0;i<3;i++)
			for(j=0;j<3;j++)
			{
				if(d0[i][j]<1000000000)
				{
					m[k]=i;
					d0[i][j]=100000000;
					printf("%d ",d0[i][j]);
				}
			}
	return((unsigned char)(m[0]+m[1]+m[2]+1.5)/3);
}


int main()
{
	unsigned int i;

	delay_init();
	uart_init(115200);
	IIC_Init();
	IIC_Init2();
	InitMultiFDC2214();
	InitMultiFDC22142();//初始化单通道
	while(1)
	{
		//train_1();
		//while(Key_Scan(GPIOB,GPIO_Pin_4)==1);
	//while(Key_Scan(GPIOB,GPIO_Pin_4)==0);
		//rec_1();
//				//while(Key_Scan(GPIOB,GPIO_Pin_4)==1);
	//while(Key_Scan(GPIOB,GPIO_Pin_4)==0);
//		for(i=0;i<500;i++)
//		{
			temp0 = (ReadRegfdc2214(0x00)<<16)+(ReadRegfdc2214(0x01));
//			ss0[i]=temp0;
//			temp00+=temp0;
//		}
//		for(i=0;i<500;i++)
//		{
			temp1 = (ReadRegfdc2214(0x02)<<16)+(ReadRegfdc2214(0x03));
//			ss1[i]=temp1;
//			temp11+=temp1;
//		}
//		for(i=0;i<500;i++)
//		{
			temp2 = (ReadRegfdc2214(0x04)<<16)+(ReadRegfdc2214(0x05));
//			ss2[i]=temp2;
//			temp22+=temp2;
//		}
//		for(i=0;i<500;i++)
//		{
			temp3 = (ReadRegfdc2214(0x06)<<16)+(ReadRegfdc2214(0x07));
			temp4 = (ReadRegfdc22142(0x00)<<16)+(ReadRegfdc22142(0x01));
//			ss3[i]=temp3;
//			temp33+=temp3;
//		}
//		temp00=temp00/500;
//		temp11=temp11/500;
//		temp22=temp22/500;
//		temp33=temp33/500;
		//printf("DataTEMP0:");
		printf("%d ",temp0);
		//printf("ok\n");
		//printf("DataTEMP1:");
		printf("%d ",temp1);
		//printf("DataTEMP2:");
		printf("%d ",temp2);
		//printf("DataTEMP3:");
		printf("%d ",temp3);
		printf("%d\n",temp4);
	}		
		
}

