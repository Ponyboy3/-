#include "reg52.h"

sbit HC138_A = P2^5;
sbit HC138_B = P2^6;
sbit HC138_C = P2^7;

void Delay(unsigned int t)
{
	while(t--);
	while(t--);
}

void InitHC138(unsigned char n)
{
	switch(n)
	{
		case 4:
			HC138_C = 1;
			HC138_B = 0;
			HC138_A = 0;   //��Y4����͵�ƽ
			//   P2 = (P2 & 0x1f) | 0x80 
		break;
		case 5:
			HC138_C = 1;
			HC138_B = 0;
			HC138_A = 1;   //��Y5����͵�ƽ
		break;
		case 6:
			HC138_C = 1;
			HC138_B = 1;
			HC138_A = 0;   //��Y6����͵�ƽ
		break;
		case 7:
			HC138_C = 1;
			HC138_B = 1;
			HC138_A = 1;   //��Y7����͵�ƽ
		break;
	}
}

void OutputP0(unsigned char channel,unsigned char date)
{
	InitHC138(channel);//��Yn����͵�ƽ
	P0 = date;
}

//LED��ƺ���
void LEDRunning()//Y4 --> CBA = 100                         
{
	unsigned int i;
	
	for(i=0;i<3;i++)
	{
		OutputP0(4,0x00);
		Delay(60000);
		Delay(60000);
		
		OutputP0(4,0xff);
		Delay(60000);
		Delay(60000);
	}
	
	for(i=1;i<=8;i++)
	{
		OutputP0(4,(0xff << i));
		Delay(60000);
		Delay(60000);
	}
	
	InitHC138(5);//��Y5����͵�ƽ
	P0 = 0x10;
	Delay(60000);
	Delay(60000);
	P0 = 0x00;
	
	InitHC138(4);//��Y4����͵�ƽ
	for(i=1;i<=8;i++)
	{
		P0 = ~(0xff << i);
		Delay(60000);
		Delay(60000);
	}
	
	InitHC138(5);//��Y5����͵�ƽ
	P0 = 0x40;
	Delay(60000);
	Delay(60000);
	P0 = 0x00;
}

//��ʼ������
void InitSystem()
{
	InitHC138(5);//��Y5����͵�ƽ
	
	P0 = 0x00;
	
}

void main()
{
	InitSystem();
	while(1)
	{
		LEDRunning();
	}
}