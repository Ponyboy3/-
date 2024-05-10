#include "reg52.h"
#include "onewire.h"
#include "iic.h"
#include "ds1302.h"
#include "intrins.h"

#define uchar unsigned char 
#define uint unsigned int

// �������ʾ���ݣ� һ��ǰ10��Ϊ0-9�� �ɸ�����Ҫ���������
unsigned char code type1[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
							  0xff,0xbf,0x8e,0xc8,0xc1};
// �����λ
uchar code type2[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

unsigned char num1,num2,num3,num4,num5,num6,num7,num8;  // �����ÿһλ������ num1 = 2, ��һ���������ʾ 2��
unsigned char GN;  // ������־λ
bit f_2ms, f_50ms, f_500ms_smg, f_200ms_temperature;  // 2ms, 50ms, 500ms��־λ
unsigned int num_2ms_smg;  // ���ٸ�2ms
unsigned char temp_Led_P0 = 0xff;  // ��������һ��LED״̬
unsigned int temperature;  // �¶�
unsigned int num_2ms_temperature;  // ���ٸ�2ms
unsigned char ds1302_h, ds1302_m, ds1302_s;  // 
unsigned char dat1 = 0x16,dat2 = 0x55,dat3 = 0x50;  // ��16���ƣ�ds1302д����
unsigned char dat[8];
int set_ds1302_h = 23,set_ds1302_m = 59,set_ds1302_s = 45;  // Ĭ�ϣ�24c02д���ݣ�ʱ������
uchar set_ds1302_key_modele;  // ʱ�����õİ���ģʽ
bit f_set_time, f_key_long, is_key_set_time;
uchar t_key;  // ������סʱ��
uchar stat_led[8];

unsigned char dis_model = 0;  // 0:��ʾ�¶ȣ�DS18B20���� 1:��ʾʱ��
// ���������
uint distance;
// PCF8951
unsigned char dat_rd1, dat_rd2;  // ADC��ȡ����
float rd1_v;
unsigned int smg_rd1_v;
// ����
unsigned char stat_key;  // �������´���
unsigned int count_key_t;  // 10ms��λ�ۼ�


sbit S7 = P3^0;
sbit S6 = P3^1;
sbit S5 = P3^2;
sbit S4 = P3^3;

sbit L0 = P0^0;
sbit L1 = P0^1;
sbit L2 = P0^2;
sbit L3 = P0^3;
sbit L4 = P0^4;
sbit L5 = P0^5;
sbit L6 = P0^6;
sbit L7 = P0^7;

// =========================================== ���뿪ʼ ======================================
bit f_1s;  // 1s��־λ

unsigned int num_2ms = 0, num_s = 0, num_s_key = 0;  // ���ٸ�2ms, 1s, ��������ʱ��

bit f_time = 1;  // ��ʱ��0�����Ƿ���ͣ
unsigned char urdat;  // ���ڽ�������

unsigned int num_On_Off;  // ���ش���

// =========================================== ������� ======================================


//=========================================��ͨ��ʱ����(ģ�庯��)=================================
void Delay(uint t) {
	while( t -- );
	while( t -- );
}
//=======================================����LED���ƺ���(ģ�庯��)================================
// ������Ŀ��Ҫɾ��

// ======================LED0====================
void LED0_On() {
	P0 = 0xff & temp_Led_P0;  // ��������ط���P0��ֵ���ر��������һֱ��ˢ�£������ҽ��LEDǰһ�ε�״̬
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L0 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[0] = 1;
}
void LED0_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L0 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[0] = 0;
}
// ======================LED1====================
void LED1_On() {
	P0 = 0xff & temp_Led_P0;  // ��������ط���P0��ֵ���ر��������һֱ��ˢ�£������ҽ��LEDǰһ�ε�״̬
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L1 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[1] = 1;
}
void LED1_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L1 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[1] = 1;
}
// ======================LED2====================
void LED2_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L2 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED2_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L2 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
// ======================LED3====================
void LED3_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L3 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED3_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L3 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
// ======================LED4====================
void LED4_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L4 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED4_Off() {
	P0 = 0xff & temp_Led_P0 & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L4 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
// ======================LED5====================
void LED5_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L5 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED5_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L5 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
// ======================LED6====================
void LED6_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L6 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[6] = 1;
}
void LED6_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L6 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[6] = 0;
}
// ======================LED7====================
void LED7_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L7 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[7] = 1;
}
void LED7_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L7 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
	
	stat_led[7] = 0;
}
//=========================================�������ʾ==============================================
void Display_SMG(uchar n1,uchar n2,uchar n3,uchar n4,uchar n5,uchar n6,uchar n7,uchar n8)
{
	char i;
	P0 = 0xff;
	P2 = (P2 & 0x1f) | 0xc0;//�����λѡ
	P0 = type2[i];
	P2 &= 0x1f;
	switch(i)//�����λѡ
	{
		case 0:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n1];P2 &= 0x1f;break;
		case 1:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n2];P2 &= 0x1f;break;
		case 2:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n3];P2 &= 0x1f;break;
		case 3:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n4];P2 &= 0x1f;break;
		case 4:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n5];P2 &= 0x1f;break;
		case 5:P2 = (P2 & 0x1f) | 0xe0;
									if(dis_model == 1 || dis_model == 4)  // ������Ҫ������
									{
										P0 = type1[n6]&0x7f;  // ����С����
									}
									else
										P0 = type1[n6];
												  P2 &= 0x1f;break;
		case 6:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n7];P2 &= 0x1f;break;
		case 7:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n8];P2 &= 0x1f;break;
	}
	if(++i==8)
		i = 0;
}
//============================================��������============================================
/***************************************************
д��һ��
�а��������£�GN��ǣ����������Ҫ���ܽ��в���
***************************************************/
void Delay_Keys(uint t)
{
	while(t--);
}
uchar Scan_Keys_Alone()
{
	char d;
	d = (~P3) & 0x0f;
	if(d == 0) return 0;
	Delay_Keys(100);
	if(d == 0) return 0;
	Delay_Keys(100);
	return d;
}
void Change_Keys_Alone()
{
	switch(Scan_Keys_Alone())
	{
		case 8:GN = 1;break;//S4
		case 4:GN = 2;break;//S5
		case 2:GN = 3;break;//S6
		case 1:GN = 4;break;//S7
		case 0:GN = 0;break;
	}
}

/***************************************************
д������
�а��������£�������һ��״̬���ɿ�����һ��״̬
���磺����ʱĳ���������ɿ�ʱ����
***************************************************/
//void ScanKeys_Alone() 
//{
//	if (S7 == 0) {
//		Delay_Keys(100);
//		if (S7 == 0) {
//			// =========================================== ���뿪ʼ ======================================
//			
//			// =========================================== ������� ======================================
//			while(S7 == 0);
//			// =========================================== ���뿪ʼ ======================================

//			// =========================================== ������� ======================================
//		}
//	}
//	
//	if (S6 == 0) {
//		Delay_Keys(100);
//		if (S6 == 0) {
//			// =========================================== ���뿪ʼ ======================================
//			
//			// =========================================== ������� ======================================
//			while(S6 == 0);
//			// =========================================== ���뿪ʼ ======================================

//			// =========================================== ������� ======================================
//		}
//	}
//	
//	if (S5 == 0) {
//		Delay_Keys(100);
//		if (S5 == 0) {
//			// =========================================== ���뿪ʼ ======================================

//			// =========================================== ������� ======================================
//			while(S5 == 0);
//			// =========================================== ���뿪ʼ ======================================

//			// =========================================== ������� ======================================
//		}
//	}

//	if (S4 == 0) {
//		Delay_Keys(100);
//		if (S4 == 0) {
//			// =========================================== ���뿪ʼ ======================================

//			// =========================================== ������� ======================================
//			while(S4 == 0);
//			// =========================================== ���뿪ʼ ======================================

//			// =========================================== ������� ======================================
//		}
//	}
//}

// ================= ������� =============

//============================================���󰴼�============================================
sfr P4 = 0xc0;
sbit P44 = P4^4;
sbit P42 = P4^2;

uchar Scan_Keys()
{
	char H,L;
	P3 =  0xcf;P44 = 0;P42 = 0;H = (~P3) & 0x0f;
	if(H == 0) return 0;
	Delay_Keys(100);
	P3 =  0xcf;P44 = 0;P42 = 0;H = (~P3) & 0x0f;
	if(H == 0) return 0;
	P3 =  0xf0;P44 = 1;P42 = 1;L = (~P3) & 0x30;
	if(P44 == 0) L |= 0x80;
	if(P42 == 0) L |= 0x40;
	return (H+L);
}

void Change_Keys()
{
	switch(Scan_Keys())
	{
		case 0x88:GN = 4;break;  // S4
		case 0x84:GN = 5;break;  // S5
		case 0x82:GN = 6;break;  // S6
		case 0x81:GN = 7;break;  // S7
		case 0x48:GN = 8;break;  // S8
		case 0x44:GN = 9;break;  // S9
		case 0x42:GN = 10;break;  // S10
		case 0x41:GN = 11;break;  // S11
		case 0x28:GN = 12;break;  // S12
		case 0x24:GN = 13;break;  // S13
		case 0x22:GN = 14;break;  // S14
		case 0x21:GN = 15;break;  // S15
		case 0x18:GN = 16;break;  // S16
		case 0x14:GN = 17;break;  // S17
		case 0x12:GN = 18;break;  // S18
		case 0x11:GN = 19;break;  // S19
		case 0:GN = 0;break;
	}
}
//============================================24C02��ʼ��============================================
void Write_24C02_Byte(unsigned char addr,unsigned char dat)
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

unsigned char Read_24C02_Byte(unsigned char addr)
{
	unsigned char tmp;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	tmp = IIC_RecByte(); 
	IIC_SendAck(1);
	IIC_Stop();
	
	return tmp;
}

void Write_24C02_Page(unsigned char addr, unsigned char *dat, unsigned char len)
{
	unsigned char i;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	
	for (i = 0; i < len; i ++ ) {
		IIC_SendByte(dat[i]);
		IIC_WaitAck();
	}
	IIC_Stop(); 
}

void Read_24C02_Page(unsigned char addr, unsigned char *dat, unsigned char len)
{
	unsigned char i;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	for (i = 0; i < len - 1; i ++ ) {
		dat[i] = IIC_RecByte();
		IIC_SendAck(0);
	}
	dat[len - 1] = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
}
//============================================DS18B20��ʼ��============================================
void Init_18b20()
{
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x4e);
	Write_DS18B20(0x00);
	Write_DS18B20(0x00);
	Write_DS18B20(0x7f);
}
unsigned int Read_Temperature(void)  // ����Ҫ����2ΪС��ʱ������Ҫ�õ�unsigned int,
									 // �������3�����ͣ�������unsigned int����������
							         // ��һλС������unsigned char.
{
	unsigned char H,L;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	L = Read_DS18B20();
	H = Read_DS18B20();
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	//1.��������--23
	//temp = (H<<4) + (L>>4);
	
	//2.����1λС��--23.4
	//temp = ((H<<8) + L)*5/8;
	
	//3.����2λС��--23.45
	return ((H<<8)+L)*25/4;  // (50/8)
}
//============================================Ds1302��ʼ��============================================
void My_Write_Ds1302()
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x84,dat1);//ʱ
	Write_Ds1302_Byte(0x82,dat2);//��
	Write_Ds1302_Byte(0x80,dat3);//��
}
void My_Read_Ds1302()
{
	ds1302_h = Read_Ds1302_Byte(0x85);//ʱ
	ds1302_h = (ds1302_h >> 4) * 10 + (ds1302_h & 0x0f);
	ds1302_m = Read_Ds1302_Byte(0x83);//��
	ds1302_m = (ds1302_m >> 4) * 10 + (ds1302_m & 0x0f);
	ds1302_s = Read_Ds1302_Byte(0x81);//��
	ds1302_s = (ds1302_s >> 4) * 10 + (ds1302_s & 0x0f);
}
//============================================PCF8951����=======================================
// A/D��ȡ��������(RD1)���ѹ�ɼ�(RB2)��ֵ
unsigned char Read_ADC(unsigned char n)
{
	unsigned char temp;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	if (n == 1)
	{
		IIC_SendByte(0x01);//��ȡ��������RB1
	}
	else if (n == 2)
	{
		IIC_SendByte(0x03);//��ȡ����������RB2
	}
	IIC_WaitAck();
	IIC_Stop();
	
	Delay_Keys(500);//while(500--);��ʱ�ȴ�
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	temp = IIC_RecByte();//��ȡ
	IIC_SendAck(1); 
	IIC_Stop();
	return temp;
}

//Read_ADC(1) -> (0~255) -> *2 -> (0~500)V

// D/Aд��ѹ
void Xie_ADC(unsigned char dat)
{
	IIC_Start();
	IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(0x40);
    IIC_WaitAck();
    IIC_SendByte(dat);
    IIC_WaitAck();
    IIC_Stop();
}
//============================================��ʱ��T0��ʼ��=======================================
uchar count = 0;
uchar pwm_dut = 0;//ռ�ձ�
void Init_T0()
{
	TMOD = (TMOD & 0xf0) | 0x01;  // ��ʱ
	TH0 = (65536 - 2000) / 256;
	TL0 = (65536 - 2000) % 256;
	
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}
//============================================��ʱ��T1��ʼ��=======================================
void Init_T1()
{
	TMOD = (TMOD & 0x0f) | 0x00;//��ʱ
//	TH1 = (65536 - 12) / 256;
//	TL1 = (65536 - 12) % 256;
	
//	EA = 1;
//	ET1 = 1;
//	TR1 = 1;
}
//============================================���������=======================================
sbit TX = P1 ^ 0;
sbit RX = P1 ^ 1;

//======���䳬�����ź�====
void send_Wave()
{
	unsigned char i = 8;
	// 12MHZ��12��Ƶ�������ʱ12us;
	TR1 = 0;
	TH1 = 0xff;
	TL1 = 0xf4;
	TF1 = 0;
	TR1 = 1;
	// ����8��40khz�ĳ������ź�
	while (i -- ) {
		TX = 0;
		while (!TF1);
		TF1 = 0;
		TX = 1;
		while (!TF1);
		TF1 = 0;
	}
}

void Get_Distance() {
	// �ȷ���8��40khz�ĳ������ź�
	send_Wave();
	
	TR1 = 0;
	TL1 = 0x00;
	TH1 = 0xe0;
	TF1 = 0;
	TR1 = 1;
	while (RX && !TF1);
	TR1 = 0;
	
	if (TF1 == 1)
	{
		TF1 = 0;
		distance = 999;
	}
	// �������ٶ�Լ344m/s
	// ���� L = ��344m/s*t��/2=172m/s*t
	// tȡ΢�룬����L = 0.0172cm/us*t
	else
		distance = (((TH1 & 0x1f) << 8) | TL1) * 0.0172;
}

uchar Get_Distance_2()
{
	uchar temp = 16;
	TX = 0;
	TR1 = 1;
	// ���ŷ���40Khz�����ź�
	while (temp--) // ÿ��12us��8������
	{
		while (!TF1)
			;
		TX ^= 1; // if(TX==1) { Delay20us();}//���Բ���
		TH1 = (65536 - 12) / 256;
		TL1 = (65536 - 12) % 256;
		TF1 = 0;
	}
	//	while(temp--)
	//	{
	//		while(!TF0);
	//		TX ^= 1;//TX = ~TX;
	//		TH0=(65536-12)/256;
	//		TL0=(65536-12)%256;
	//		TF0 = 0;//TF0����Ҫ�ȸ�ֵ(�洦�жϣ��Զ���1)������
	//	}
	TR1 = 0;
	TL1 = 0;
	TH1 = 0;
	TR1 = 1;
	while (RX && !TF1)
		;
	TR1 = 0;

	if (TF1 == 1)
	{
		TF1 = 0;
		return 999;
	}
	else
		return ((TH1 << 8) + TL1) * 0.017;
}
//============================================���ڳ�ʼ��============================================
#include "intrins.h"
#include "string.h"

sfr AUXR = 0x8e;  // �����Ĵ���
sfr T2H=0xd6;  // ��ʱ��2��8λ
sfr T2L=0xd7;  // ��ʱ��2��8λ
#define BAUD         9600
//#define SYSTEMCLOCK  11059200L
#define SYSTEMCLOCK  12000000L
uchar command = 0x00;
void Init_Uart()
{
	// ===================T1ʵ��===================
//	TMOD = (TMOD & 0x0f) | 0x00;
//	
//	TL1 = (65535-(SYSTEMCLOCK / 4 / BAUD));
//	TH1 = (65535-(SYSTEMCLOCK / 4 / BAUD)) >> 8l;
//	
////	ET1 = 0;		    //��ֹ��ʱ��1�ж�
//	TR1 = 1;		    //������ʱ��1
//	
//	SCON = 0x50;
//	AUXR = 0x40;
	// ===================T1ʵ��===================
	
	// ===================T2ʵ��===================
	SCON = 0x50;  // 8λ����,�ɱ䲨����
    AUXR |= 0x01; // ����1ѡ��ʱ��2Ϊ�����ʷ�����
    AUXR |= 0x04; // ��ʱ��ʱ��1Tģʽ
	
	T2L = (65535-(SYSTEMCLOCK / 4 / BAUD));
	T2H = (65535-(SYSTEMCLOCK / 4 / BAUD)) >> 8l;
	
//	ET1 = 0;		    //��ֹ��ʱ��1�ж�
//	TR1 = 1;		    //������ʱ��1
//	SCON = 0x50;
//	AUXR = 0x40;
	
	AUXR |= 0x10; // ��ʱ��2��ʼ��ʱ
	// ===================T2ʵ��===================
	
	ES = 1;
	EA = 1;
	
//	SCON = 0x50;		//8λ����,�ɱ䲨����
//	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
//	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
//	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
//	TL1 = 0xE8;		//�趨��ʱ��ֵ
//	TH1 = 0xFF;		//�趨��ʱ��ֵ
//	ET1 = 0;		//��ֹ��ʱ��1�ж�

//	ES = 1;
//	EA = 1;  
//	TR1 = 1;		//������ʱ��1
}
// ����һ���ֽ�����
void Send_Byte(unsigned char dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}
//����һ���ַ���
void Send_String(unsigned char* str)
{
  
  while(*str != '\0')
  {
    Send_Byte(*(str++));
  }
}
//==========================================ϵͳ��ʼ��=============================================
void Init_Sys()
{
	P2 = (P2 & 0x1f) | 0x80;  // LED
	P0 = 0xff;
	P2 &= 0x1f;
	P2 = (P2 & 0x1f) | 0xa0;  // ���������Ĵ���
	P0 = 0x00;
	P2 &= 0x1f;
	P2 = (P2 & 0x1f) | 0xc0;  // �����
	P0 = 0xff;
	P2 &= 0x1f;
	P2 = (P2 & 0x1f) | 0xe0;
	P0 = 0xff;
	
	num1=num2=num3=num4=num5=num6=num7=num8=10;
}

void LED_control() {
	// ================= ���Կ�ʼ =============
//	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
//	P0 = 0x7f;  // Ҫ��P0�������������֮�䣬�������
//	P2 &= 0x1f;
	// ================= ���Խ��� =============
	
	// =========================================== ���뿪ʼ ======================================
	
	// =========================================== ������� ======================================
}

void main()
{
	Init_Sys();
	
	num_On_Off = Read_24C02_Byte(0x00);
	
	Init_T1();
//	TMOD = (TMOD & 0x0f) | 0x00;//��ʱ
	Init_T0();
	Init_18b20();
//	My_Write_Ds1302();

//	num_On_Off = Read_24C02_Byte(0x00);
//	num_On_Off ++ ;
//	Write_24C02_Byte(0x00, num_On_Off);

	Read_24C02_Page(0x01, dat, 3);
	
	
//	dat2 = (dat[2] / 10) << 4 | (dat[2] % 10);
	dat1 = dat[0] /10*16 + dat[0] % 10;
	dat2 = dat[1] /10*16 + dat[1] % 10;
	dat3 = dat[2] /10*16 + dat[2] % 10;
	
	My_Write_Ds1302();
	
	Init_Uart();
//	Send_Byte(dat[2] /10*16 + dat[2] % 10);
//	Send_Byte(0xA0);
	Send_String("I am OK\r\n");
	
	while(1)
	{
//		LED_control();
        if(f_2ms == 1)
        {
            f_2ms = 0;			
			LED_control();  // xxx
//			Write_24C02_Byte(0x02,ds1302_s);	

			distance = Get_Distance_2();  // ���������		
			// =======================������ʼ========================
			/*������Ҫѡ���Ӧ�İ�������*/
//			Change_Keys_Alone();  // 1.��������ɨ��(1)
//			ScanKeys_Alone();     // 2.��������ɨ��(2)
			Change_Keys();        // 3.���󰴼�ɨ��
			if (GN == 13) {  // S13
				// =========================================== ���뿪ʼ ======================================
				LED1_On();
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				LED1_Off();
				dis_model ++ ;
				if (dis_model > 5) dis_model = 0;
				// =========================================== ������� ======================================
			}
			
			if (GN == 12) {  // S12 num_s ��ͣ
				// =========================================== ���뿪ʼ ======================================
				LED2_On();
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				LED2_Off();
				f_time = ~f_time;
				// =========================================== ������� ======================================
			}
			
			if (GN == 5) {  // S12 num_s ��ͣ
				// =========================================== ���뿪ʼ ======================================
				LED0_On();
				
//				count_key_t =0 ;  // ʱ�������0
				
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				LED0_Off();
				stat_key ++ ;  // �������´���
				// =========================================== ������� ======================================
			}
			if (count_key_t > 500) {
				// ����
				if (stat_key == 1) {
					if (stat_led[6] == 0) LED6_On();
					else LED6_Off();
				}
				// ˫��
				if (stat_key == 2) {
					if (stat_led[7] == 0) LED7_On();
					else LED7_Off();
				}
				count_key_t = 0;
				stat_key = 0;
			}
			
			if (GN == 8) {  // S8
				// =========================================== ���뿪ʼ ======================================
				LED5_On();
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				LED5_Off();
				
				// ʱ���
				if (set_ds1302_key_modele == 1) {  // Сʱ����
					set_ds1302_h ++ ;
					if (set_ds1302_h > 23) set_ds1302_h = 0;
				}
				else if (set_ds1302_key_modele == 2) {  // ��������
					set_ds1302_m ++ ;
					if (set_ds1302_m > 59) set_ds1302_m = 0;
				}
				else if (set_ds1302_key_modele == 3) {  // ������
					set_ds1302_s ++ ;
					if (set_ds1302_s > 59) set_ds1302_s = 0;
				}
				
				// =========================================== ������� ======================================
			}
			
			if (GN == 9) {  // S9
				// =========================================== ���뿪ʼ ======================================
				LED5_On();
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				LED5_Off();
				
				// ʱ���
				if (set_ds1302_key_modele == 1) {  // Сʱ����
					set_ds1302_h -- ;
					if (set_ds1302_h < 0) set_ds1302_h = 23;
				}
				else if (set_ds1302_key_modele == 2) {  // ��������
					set_ds1302_m -- ;
					if (set_ds1302_m < 0) set_ds1302_m = 59;
				}
				else if (set_ds1302_key_modele == 3) {  // ������
					set_ds1302_s -- ;
					if (set_ds1302_s < 0) set_ds1302_s = 59;
				}
				// =========================================== ������� ======================================
			}

			if (GN == 4) {  // S4
				// =========================================== ���뿪ʼ ======================================
				LED3_On();
				is_key_set_time = 1;  // ��������δ�ɿ�
				
				
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				LED3_Off();
				LED4_Off();
				is_key_set_time = 0;  // ���������ɿ�
				num_s_key = 0;
				
				if (f_key_long) {  // ����ת��ģʽ
					set_ds1302_key_modele ++ ;
					f_key_long = 0;
				}
				
				
//				if (set_ds1302_key_modele == 1) {  // Сʱ����
//					set_ds1302_h = 23;
//				}
//				else if (set_ds1302_key_modele == 2) {  // ��������
//					set_ds1302_m = 29;
//				}
//				else if (set_ds1302_key_modele == 3) {  // ������
//					set_ds1302_s = 45;
//				}
				if (set_ds1302_key_modele == 4) {  // ȷ��
					if (dis_model == 2) {
						dat[0] = set_ds1302_h;
						dat[1] = set_ds1302_m;
						dat[2] = set_ds1302_s;
						Write_24C02_Page(0x01, dat, 3);
						
						dat1 = dat[0] /10*16 + dat[0] % 10;
						dat2 = dat[1] /10*16 + dat[1] % 10;
						dat3 = dat[2] /10*16 + dat[2] % 10;

						My_Write_Ds1302();
					}
					set_ds1302_key_modele = 0;
				}
				// =========================================== ������� ======================================
			}
			/*������Ҫ��Ӷ�Ӧ����*/
			// =======================��������========================
			
			// =======================���ڵ��Կ�ʼ====================
			if(command != 0x00)
			{
				switch(command & 0xf0)
				{
					case 0xa0:
						P0 = temp_Led_P0;
						P2 = (P2 & 0x1f) | 0x80;  // LED
						P0 = (P0 | 0x0f) & ((~command) | 0xf0);
						temp_Led_P0 = P0;
						P2 &= 0x1f;
						command = 0x00;
					break;
					case 0xb0:
						P0 = temp_Led_P0;
						P2 = (P2 & 0x1f) | 0x80;  // LED
						P0 = (P0 | 0xf0) & (((~command) << 4) | 0x0f);
						temp_Led_P0 = P0;
						P2 &= 0x1f;
						command = 0x00;
					break;
					case 0xc0:
						Send_String("system is running...!\r\n");
						command = 0x00;
					break;
				}
			}
			// =======================���ڵ��Խ���====================		
			
			// =========================================== ���뿪ʼ ======================================			
			// =======================1.�¶ȶ�ȡ��ʼ========================
			if (f_200ms_temperature) {
				f_200ms_temperature = 0;
				
				My_Read_Ds1302();
			}
			temperature = Read_Temperature();
//			num1=10;num2=10;num3=10;num4=10;
//			num5=temperature/1000;num6=temperature/100%10;
//			num7=temperature/10%10;num8=temperature%10;
			// =======================1.�¶ȶ�ȡ����========================
			
			dat[0] = ds1302_h;
			dat[1] = ds1302_m;
			dat[2] = ds1302_s;
			Write_24C02_Page(0x01, dat, 3);
			
			if (dis_model == 0) {  // ʱ����ʾ
				num1=ds1302_h/10;num2=ds1302_h%10;num3=11;
				num4=ds1302_m/10;num5=ds1302_m%10;num6=11;
				num7=ds1302_s/10;num8=ds1302_s%10;
			}
			else if (dis_model == 1) {  // �¶���ʾ
				num1=dis_model;num2=10;num3=10;num4=10;
				num5=temperature/1000;num6=temperature/100%10;
				num7=temperature/10%10;num8=temperature%10;
			}
			else if (dis_model == 2) {  // ʱ������
//				num_On_Off = Read_24C02_Byte(0x00);
//				num_On_Off ++ ;
//				Write_24C02_Byte(0x00, num_On_Off);
				f_set_time = 1;  // ʱ�����ñ�־
				
				if (set_ds1302_key_modele == 1) {
					if (f_500ms_smg) num1 = 10, num2 = 10;
					else num1 = set_ds1302_h / 10, num2 = set_ds1302_h % 10;
				}
				else num1 = set_ds1302_h / 10, num2 = set_ds1302_h % 10;
				
				if (set_ds1302_key_modele == 2) {
					if (f_500ms_smg) num4 = 10, num5 = 10;
					else num4 = set_ds1302_m / 10, num5 = set_ds1302_m % 10;
				}
				else num4 = set_ds1302_m / 10, num5 = set_ds1302_m % 10;
				
				if (set_ds1302_key_modele == 3) {
					if (f_500ms_smg) num7 = 10, num8 = 10;
					else num7 = set_ds1302_s / 10, num8 = set_ds1302_s % 10;
				}
				else num7 = set_ds1302_s / 10, num8 = set_ds1302_s % 10;
				
				num3 = 11; 
				num6 = 11;
				
			}
			else if (dis_model == 3) {
				num1 = dis_model;num2 = 10; num3 = 10; num4 = 10;
				num5= 10;
				num6=distance / 100; num7=distance % 100 / 10; num8=distance % 10;
			}
			else if (dis_model ==4) {
				dat_rd1 = Read_ADC(1);
				rd1_v = dat_rd1*5/255;
				smg_rd1_v = rd1_v * 100;
				num1=dis_model;
				num2=dat_rd1 / 100;num3=(dat_rd1 % 100) / 10;num4 = dat_rd1 % 10;
				num5=11;
				num6=smg_rd1_v / 100;num7=(smg_rd1_v % 100) / 10;num8=smg_rd1_v % 10;
			}
			else if (dis_model == 5) {
				num1 = dis_model;num2 = 10; num3 = 10; num4 = 10;
				num5= 10;
				num6=11; num7=stat_key / 10; num8=stat_key%10;
			}
			// =========================================== ������� ======================================
        }
    }
}

void Service_T0() interrupt 1
{
	TH0 = (65536 - 2000) / 256;
	TL0 = (65536 - 2000) % 256;
	
	f_2ms = 1;
	
	/***************************************************
	��ʱ����صı����ĸ��·�������
	ע�⣺��Ҫ�����Ĺ���ʱ��Ĵ��루��forѭ���ȣ��������������ɴ���
	***************************************************/
	// =========================================== ���뿪ʼ ======================================
	num_2ms ++ ;
	num_2ms_temperature ++ ;
	if (num_2ms_temperature == 100) {
		f_200ms_temperature = 1;
		num_2ms_temperature = 0;
	}
	
	if (num_2ms == 500) {
		f_1s = 1;
		num_2ms = 0;
	}
	
	if (f_1s == 1) {
		f_1s = 0;
		
		if (f_time) num_s ++ ;
		
		// ��������
		if (is_key_set_time && dis_model == 2) {
			num_s_key ++ ;
//			t_key = 0;
			if (num_s_key == 2) {
				LED4_On();
				f_key_long = 1;
			}
		}			
			
		
		if (num_s == 9) 
			num_s = 0;
		
	}
	
	// �������˸
	num_2ms_smg ++ ;
	if (num_2ms_smg == 250) {
		num_2ms_smg = 0;
		f_500ms_smg = ~f_500ms_smg;
	}
	
	// ����˫��
	if (stat_key != 0) {
		count_key_t ++ ;
	}
	// =========================================== ������� ======================================
	
	Display_SMG(num1,num2,num3,num4,num5,num6,num7,num8);//2msˢ�������
}

void ServiceUart() interrupt 4
{
	if(RI == 1)
	{
		RI = 0;//�˹�����
		urdat = SBUF;
		command = SBUF;
//		Send_Byte(urdat + 1);
//		send_byte(0x5a);
//		send_byte(0xa5);
	}
}