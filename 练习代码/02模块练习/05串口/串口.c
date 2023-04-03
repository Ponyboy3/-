#include "reg52.h"

sfr AUXR = 0x8e;

#define BAUD	     9600  //������
#define SYSTEMCLOCK  11059200L  //ϵͳʱ��Ƶ��

unsigned char urdat;

void GetByte(unsigned char dat);
void Init_Uart()
{
	TMOD = 0x00;
	TL1 = (65536 - (SYSTEMCLOCK / 4 / BAUD));
  TH1 = (65536 - (SYSTEMCLOCK / 4 / BAUD)) >> 8;
	ET1 = 0;		    //��ֹ��ʱ��1�ж�
	TR1 = 1;		    //������ʱ��1
	
	SCON = 0x50;
  AUXR = 0x40;
    
	ES = 1;
	EA = 1;
}

void ServiceUart() interrupt 4
{
	if(RI == 1)
	{
		RI = 0;//�˹�����
		urdat = SBUF;
		GetByte(urdat + 1);
		GetByte(0x5a);
	  GetByte(0xa5);
	}
}

void GetByte(unsigned char dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}

void main()
{
	Init_Uart();
	GetByte(0x5a);
	GetByte(0xa5);
	while(1)
	{
		
	}
}