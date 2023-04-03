#include "reg52.h"
#include "iic.h"

#define uchar unsigned char

unsigned char type1[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf};
unsigned char type2[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char type3[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
unsigned char type4[] = {0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
unsigned char type5[] = {0x7e,0xbd,0xdb,0xe7};
unsigned char type6[] = {0xe7,0xdb,0xbd,0x7e};

uchar num1,num2,num3,num4,num5,num6,num7,num8;
uchar GN;
uchar f_2ms;//2ms��׼
uchar t_10ms;
unsigned int t_2ms,tt_2ms;

uchar led1,led2,led3,led4;
uchar model,led_mod;//S6����ģʽ
unsigned int gap = 400;
uchar a,b;	
bit f_led;
bit shanshuo;

void Delay_Smg(unsigned int t)
{
	while(t--);
}
void Dispaly_Smg(uchar n1,uchar n2,uchar n3,uchar n4,uchar n5,uchar n6,uchar n7,uchar n8)
{
	char i;
	P0 = 0xff;
	P2 = (P2 & 0x1f) | 0xc0;
	P0 = type2[i];
	P2 &= 0x1f;
	switch(i)
	{
		case 0: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n1];P2 &= 0x1f;break;
		case 1: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n2];P2 &= 0x1f;break;
		case 2: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n3];P2 &= 0x1f;break;
		case 3: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n4];P2 &= 0x1f;break;
		case 4: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n5];P2 &= 0x1f;break;
		case 5: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n6];P2 &= 0x1f;break;
		case 6: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n7];P2 &= 0x1f;break;
		case 7: P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n8];P2 &= 0x1f;break;
	}
	if(++i == 8)
		i = 0;
}

unsigned char Scan_Keys()
{
	char tmp;
	tmp = (~P3) & 0x0f;
	if(tmp == 0) return 0;
	Delay_Smg(100);
	tmp = (~P3) & 0x0f;
	if(tmp == 0) return 0;
	return tmp;
}

void Keys_Change()
{
	char d;
	d = Scan_Keys();
	switch(d)
	{
		case 8: GN = 1;break;//S4
		case 4: GN = 2;break;//S5
		case 2: GN = 3;break;//S6
		case 1: GN = 4;break;//S7
		case 0: GN = 0;break;
	}
}

//=====ADC��ѹ�ɼ�===
void Init_ADC()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);//ѡ��RB2
	IIC_WaitAck();
	IIC_Stop();
}

unsigned char Read_ADc()
{
	uchar tmp;
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	tmp = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return tmp;
}

uchar ADc_Change()
{
	uchar d,f;
	d = Read_ADc();
	if(d < 64)
	{
		f = 1;a = 1;
	}
	else if((d >= 64) && (d < 128))
	{
		f = 3;a = 2;
	}
	else if((d >= 128) && (d < 192))
	{
		f = 5;a = 3;
	}
	else if((d >= 192) && (d < 256))
	{
		f = 7;a = 4;
	}
	return f;
}
//=====24C02�洢��===
void Write_24c02(uchar addr,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}
unsigned char Read_24c02(uchar addr)
{
	uchar tmp;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	tmp = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return tmp;
}

//=========��ʱ��1====
void Init_T1()         //��ʱ2msˢ�������
{
	TMOD = 0x10;
	TH1 = (65536 - 2000) / 256;
	TL1 = (65536 - 2000) % 256;
	
	EA = 1;
	ET1 = 1;
	TR1 = 1;
}
//=======led������ܿ���============================
void control()
{
	if(f_2ms == 1)
	{
		f_2ms = 0;
		Keys_Change();
		if(GN == 4)
		{
			f_led = ~f_led;
			while(Scan_Keys());
		}
		if(f_led == 0)//Ϩ����ͣ
		{
			P0 = 0xff;
			P2 = (P2 & 0x1f) | 0x80;
			P0 = 0xff;
			P2 &= 0x1f;
			b = ADc_Change();//��ȡRB2��ֵ��Ӧ������ֵ
			if(GN == 1)
			{
				num1=10,num2=10,num3=10,num4=10,num5=10,num6=10,num7=11,num8=a;//aָ����
				while(Scan_Keys());
				num1=10,num2=10,num3=10,num4=10,num5=10,num6=10,num7=10,num8=10;
			}
		}
		if(f_led == 1)
		{
			if(GN == 3)
			{
				model++;
				if(model == 3)
					model = 0;
				while(Scan_Keys());
			}
			switch(model)
			{
				case 0://led��ʾ
					if(GN == 1)//S4������ʾled����
					{
						num1=10;num2=10;num3=10;num4=10;num5=10;num6=10;num7=11;num8=a;//aָ����
						while(Scan_Keys());
					}
					num1=10;num2=10;num3=10;num4=10;num5=10;num6=10;num7=10;num8=10;
					if(led_mod == 0)
					{
						if(++t_10ms == 10)
						{
							t_10ms = 0;
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = type3[led1];
							P2 &= 0x1f;
						}
						if(t_10ms == ADc_Change())
						{
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = 0xff;
							P2 &= 0x1f;
						}
						if(++t_2ms == gap/2)//һ��led����ʱ��
						{
							t_2ms = 0;
							led1++;
							if(led1 == 8)
							led1 = 0;
						}
					}
					if(led_mod == 1)
					{
						if(++t_10ms == 10)
						{
							t_10ms = 0;
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = type4[led2];
							P2 &= 0x1f;
						}
						if(t_10ms == ADc_Change())
						{
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = 0xff;
							P2 &= 0x1f;
						}
						if(++t_2ms == gap/2)//һ��led����ʱ��
						{
							t_2ms = 0;
							led2++;
							if(led2 == 8)
							led2 = 0;
						}
					}
					if(led_mod == 2)
					{
						if(++t_10ms == 10)
						{
							t_10ms = 0;
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = type5[led3];
							P2 &= 0x1f;
						}
						if(t_10ms == ADc_Change())
						{
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = 0xff;
							P2 &= 0x1f;
						}
						if(++t_2ms == gap/2)//һ��led����ʱ��
						{
							t_2ms = 0;
							led3++;
							if(led3 == 4)
							led3 = 0;
						}
					}
					if(led_mod == 3)
					{
						if(++t_10ms == 10)
						{
							t_10ms = 0;
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = type6[led4];
							P2 &= 0x1f;
						}
						if(t_10ms == ADc_Change())
						{
							P0 = 0xff;
							P2 = (P2 & 0x1f) | 0x80;
							P0 = 0xff;
							P2 &= 0x1f;
						}
						if(++t_2ms == gap/2)//һ��led����ʱ��
						{
							t_2ms = 0;
							led4++;
							if(led4 == 4)
							led4 = 0;
						}
					}
				break;
				
				case 1://led��ʾģʽѡ��
					if(GN == 2)
					{
						if(led_mod == 3)
							led_mod = 3;
						else
							led_mod++;
						while(Scan_Keys());
					}
					if(GN == 1)
					{
						if(led_mod == 0)
							led_mod = 0;
						else
							led_mod--;
						while(Scan_Keys());
					}
					if(++tt_2ms == 300)
					{
						tt_2ms = 0;
						shanshuo = ~shanshuo;
					}
					if(shanshuo == 0)
					{
						num1=11;num2=led_mod+1;num3=11;
					}
					else
					{
						num1=10;num2=10;num3=10;
					}
					num4=10;
					if(gap > 999)
					{
						num5=gap/1000;num6=(gap/100%10);num7=(gap/10%10);num8=gap%10;
					}
					else
					{
						num5=10;num6=gap/100;num7=gap/10%10;num8=gap%10;
					}
				break;
				
				case 2://led��ת���ʱ��
					if(GN == 2)
					{
						if(gap > 1100)
							gap = 1200;
						else
							gap+=100;
						Write_24c02(0,gap/10);
						while(Scan_Keys());
					}
					if(GN == 1)
					{
						if(gap < 500)
							gap = 400;
						else
							gap-=100;
						Write_24c02(0,gap/10);
						while(Scan_Keys());
					}
					if(++tt_2ms == 300)
					{
						tt_2ms = 0;
						shanshuo = ~shanshuo;
					}
					if(shanshuo == 0)
					{
						if(gap > 999)
						{
							num5=gap/1000;num6=(gap/100%10);num7=(gap/10%10);num8=gap%10;
						}
						else
						{
							num5=10;num6=gap/100;num7=(gap/10%10);num8=gap%10;
						}
					}
					else
					{
						num5=10;num6=10;num7=10;num8=10;
					}
					num1=11;num2=led_mod+1;num3=11;num4=10;
				break;
			}
		}
	}
}
//==========================================
void Init_Sys()
{
	P2 = (P2 & 0x1f) | 0x80;
	P0 = 0xff;
	P2 &= 0x1f;
	
	P2 = (P2 & 0x1f) | 0xa0;
	P0 = 0x00;
	P2 &= 0x1f;
	
	Init_ADC();
	Init_T1();
	
	gap = Read_24c02(0)*10;
	num1=10;num2=10;num3=10;num4=10;num5=10;num6=10;num7=10;num8=10;
}
void main()
{
	Init_Sys();
	while(1)
	{
		control();
	}
}
void Service_T1() interrupt 3
{
	TH1 = (65536 - 2000) / 256;
	TL1 = (65536 - 2000) % 256;
	
	Dispaly_Smg(num1,num2,num3,num4,num5,num6,num7,num8);
	f_2ms = 1;
}