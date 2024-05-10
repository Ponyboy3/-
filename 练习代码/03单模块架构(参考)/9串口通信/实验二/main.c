/****************************************
���ö�ʱ��2��Ϊ�����ʷ�����������λ��ͨ��ʵ�������շ�����
*********************************************/
#include <STC15F2K60S2.H>
#include <intrins.h>
#include <string.h> //���������ַ�������
#include <stdio.h>  //ʹ��sprintf����


#define uchar unsigned char 
#define uint unsigned int 
	
uchar code type1[]={ 	0xC0,
										 	0xF9,
										 	0xA4,
										 	0xB0,
										 	0x99,
										 	0x92,
										 	0x82,
										 	0xF8,
										 	0x80,
										 	0x90,0xff};
uchar code type2[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uchar num1,num2,num3,num4,num5,num6,num7,num8;
uchar f_2ms;
uint t_2ms;
uchar GN;


bit buzy=0; //����Ƿ������
uchar code passwd[]="LOVE YOU";//����ȶ�����
uchar passwd_com=0;  //���볤�ȱ���

void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}

void smg_display(char n1,char n2,char n3,char n4,char n5,char n6,char n7,char n8)
{
	char i;
	P0=0xff;
	P2=(P2&0x1f)|0xc0;
	P0=type2[i];
	P2=(P2&0x1f);
	switch(i)
	{
		case 0:P2=(P2&0x1f)|0xe0; P0=type1[n1]; P2=(P2&0x1f);break;
		case 1:P2=(P2&0x1f)|0xe0; P0=type1[n2]; P2=(P2&0x1f);break;		
		case 2:P2=(P2&0x1f)|0xe0; P0=type1[n3]; P2=(P2&0x1f);break;		
		case 3:P2=(P2&0x1f)|0xe0; P0=type1[n4]; P2=(P2&0x1f);break;		
		case 4:P2=(P2&0x1f)|0xe0; P0=type1[n5]; P2=(P2&0x1f);break;		
		case 5:P2=(P2&0x1f)|0xe0; P0=type1[n6]; P2=(P2&0x1f);break;		
		case 6:P2=(P2&0x1f)|0xe0; P0=type1[n7]; P2=(P2&0x1f);break;		
		case 7:P2=(P2&0x1f)|0xe0; P0=type1[n8]; P2=(P2&0x1f);break;		
		
	}
	if(++i==8)
		i=0;
}


////////////////////////////key
uchar key_scan()
{
	uchar temp;
	temp=(~P3)&0x0f;
	if(temp==0) return 0;
	Delay10ms();
	temp=(~P3)&0x0f;
	if(temp==0) return 0;	
	return temp;
}

void key_change()
{
	uchar D;
	D= key_scan();
	switch(D)
	{
		case 8:GN=4;break;
		case 4:GN=5;break;
		case 2:GN=6;break;
		case 1:GN=7;break;
		case 0:GN=0;break;
	}	
}

//==============================================���ò�����Ҫ�봮�ڲ�����һ�£���Ȼ�����	
void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0x8F;		//�趨��ʱ��ֵ
	T2H = 0xFD;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	
	ES=1;
}



void time1_int1()
{
	EA=1;
	ET1=1;
	TR1=1;
	TMOD=(TMOD&0x0f)|0x10;
	TH1=(65536-2000)/256;
	TL1=(65536-2000)%256;		
}

void initial_all()
{
	P2=(P2&0x1f)|0xa0;
	P0=0x00;
	P2=(P2&0x1f);	
	P2=(P2&0x1f)|0x80;
	P0=0xff;
	P2=(P2&0x1f);	
	P2=(P2&0x1f)|0xc0;
	P0=0xff;
	P2=(P2&0x1f);		
	P2=(P2&0x1f)|0xe0;
	P0=0xff;
	P2=(P2&0x1f);		
}

//========================================����һ�ֽ�����
void send_byte(uchar *dat,uchar len)//���ָ���ָ���������飬�൱�����������len�ǳ���
{
	while(len--)
	{
		SBUF=*dat;	
		buzy=1;
		dat++;
		while(buzy);
	}	
}

void main()
{
//	uchar str[]="hello world \r\n";
	uchar str[40];   //welcome to jiangsu %f s \r\n  //����һ����ŷ����ַ������ݵ����龡����һ��

	int a;  //ʵʱ�Լӱ���
	
	
  initial_all();	
	time1_int1();
	UartInit();
  num1=num2=num3=num4=num5=num6=num7=num8=10;			
	while(1)
	{
	 if(f_2ms==1)
	 {
		 f_2ms=0;
	     key_change();
		 
		 if(++t_2ms==500)
		 {
		    t_2ms=0;
			  a++; 
		 }
		 
//         if(GN==4)  //���°�����������
//		 {
//			send_byte(str,strlen(str)); 
//			while(key_scan()); 
//		 }
		 
//         if(passwd_com >= 8) //������ȷ��������
//		 {
//			 passwd_com=0;
//			 send_byte(str,strlen(str));  
//		 }
		 
         if(passwd_com >= 8)  //������ȷ�����ַ��� �� ʵʱ����
		 {
			 passwd_com=0;
			 
			 sprintf(str,"��ӭ to jiangsu %f s \r\n", a);	/*���ѽ�������ָ�����ַ�����,
			                                                       "welcome to jiangsu %f s \r\n"��str������*/		 			 
			 send_byte(str,strlen(str));  
		 }		 
		 
		 
	
	
	 }//f_2ms
	}
}



void cl_time1_int1() interrupt 3
{
	TH1=(65536-2000)/256;
	TL1=(65536-2000)%256;		
	
	smg_display(num1,num2,num3,num4,num5,num6,num7,num8);
	f_2ms=1;
	
}


void cl_uart() interrupt 4
{
	if(RI)
	{
		RI=0;
		if(passwd[passwd_com] == SBUF)//�����յ����������ڲ��������һһ�ȶ�
		{
		  	passwd_com++;			  //�����ȷ�������һ��Ϊ��������ڷ���������׼��
		}
		else
			passwd_com=0;
		
	}
	if(TI)
	{
		TI=0;
		buzy=0;	//�������
	}
	
}








