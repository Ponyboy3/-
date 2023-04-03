#include "reg52.h"

sbit L1 = P0^0;
sbit L8 = P0^7;

void SelectHC573();

void Delay(unsigned int t)
{
	while(t--);
	while(t--);
	while(t--);
}

void Working()
{
	SelectHC573();
	L1 = 0;
	Delay(60000);
	L1 = 1;
	Delay(60000);
}
	
void SelectHC573()
{
	P2 = (P2 & 0x1f) | 0x80;
}

//=================================
void Init_INT0()
{
	IT0 = 1;//�½��س���
	EX0 = 1;//�ⲿ�ж�0��
	EA = 1;//�ܿ��ؿ�
}
unsigned char Stat_int = 0;
void ServiceINT0() interrupt 0
{
	Stat_int = 1;
}

void LEDINT()
{
	if(Stat_int == 1)
	{
		L8 = 0;
		Delay(60000);
		Delay(60000);
		Delay(60000);
		Delay(60000);
		Delay(60000);
		Delay(60000);
		Delay(60000);
		L8 = 1;
		Stat_int = 0;
	}
	
}
//=================================

void main()
{
	Init_INT0();
	while(1)
	{
		Working();
		LEDINT();
	}
}