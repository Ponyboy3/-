#include "reg52.h"
#include "onewire.h"
#include "iic.h"
#include "ds1302.h"
#include "intrins.h"

#define uchar unsigned char 
#define uint unsigned int

// �������ʾ���ݣ� һ��ǰ10��Ϊ0-9�� �ɸ�����Ҫ���������
unsigned char code type1[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,
							  0xc6,0x89,0x8e,0x8c,0x86, 0x88};
// �����λ
uchar code type2[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

unsigned char num1,num2,num3,num4,num5,num6,num7,num8;  // �����ÿһλ������ num1 = 2, ��һ���������ʾ 2��
unsigned char GN;  // ������־λ
bit f_2ms, f_50ms, f_500ms_smg;  // 2ms, 50ms, 500ms��־λ
unsigned int num_2ms_smg;  // ���ٸ�2ms
unsigned char temp_Led_P0 = 0xff;  // ��������һ��LED״̬
unsigned char ds1302_h, ds1302_m, ds1302_s;  // ds1302����
bit f_200ms_ds1302;
unsigned int num_2ms_ds1302;  // // ���ٸ�2ms
bit f_key_long, is_key_set_time;
unsigned char t_key;  // ������סʱ��
// ���������
unsigned int distance;
// PCF8951
unsigned char dat_rd1, dat_rd2;  // ADC��ȡ����
float rd1_v;
unsigned int smg_rd1_v;
// ����
unsigned char stat_key;  // �������´���
unsigned int count_key_t;  // 10ms��λ�ۼ�

bit f_1s;  // 1s��־λ

unsigned int num_2ms = 0, num_s = 0, num_s_key = 0;  // ���ٸ�2ms, 1s, ��������ʱ��

bit f_time = 1;  // ��ʱ��0�����Ƿ���ͣ

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

unsigned char stat_led[8];  // LED״̬

// =========================================== ���뿪ʼ ======================================
unsigned char dis_model_s4 = 0;  // 0:��ʾ�¶ȣ�DS18B20���� 1:��ʾʱ�� 2:���ԣ� 3������
unsigned char dis_model_s5 = 0;  // 0���¶ȣ� 1��ʪ�ȣ�2��ʱ��
unsigned int temperature, max_temperature = 0;  // �¶�
int temperature_parameters = 30;  // �¶Ȳ���
unsigned int temper[2], Temper_all = 0, Temper_average = 0;  // ƽ���¶�
bit is_mesure_temp, is_mesure_temp_ing;  // �����¶ȴ���, �Ƿ��ڲ�����
unsigned int num_2ms_mesure, num_mesure_s;  // �����¶ȴ���ʱ��

unsigned char dat1 = 0x13,dat2 = 0x03,dat3 = 0x05;  // ��16���ƣ�ds1302д����

bit is_key_s9_pressing;  // ��������δ�ɿ�
unsigned char num_s_key_pressing;  // ��������ʱ��

// Record��¼
unsigned int is_mesure_temp_num, mesure_h, mesure_m;  // ��������


// ����
bit is_temp_more_mesure;  // �����¶Ȳ���
bit f_100ms_L4;  // L4����ʱ��
unsigned int num_2ms_L4;  // ���ٸ�2ms

unsigned int count_f_NE555 = 0, dat_f = 0;  // NE555����
unsigned int count_2ms_NE555 = 0;
unsigned int temp_f,temp_t;
bit f;
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
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;  // ��������ط���P0��ֵ���ر��������һֱ��ˢ�£������ҽ��LEDǰһ�ε�״̬
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L0 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[0] = 1;
}
void LED0_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L0 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[0] = 0;
}
// ======================LED1====================
void LED1_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;  // ��������ط���P0��ֵ���ر��������һֱ��ˢ�£������ҽ��LEDǰһ�ε�״̬
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L1 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[1] = 1;
}
void LED1_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L1 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[1] = 1;
}
// ======================LED2====================
void LED2_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L2 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
void LED2_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L2 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
// ======================LED3====================
void LED3_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L3 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
void LED3_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L3 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
// ======================LED4====================
void LED4_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L4 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
void LED4_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0 & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L4 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
// ======================LED5====================
void LED5_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L5 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
void LED5_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L5 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
}
// ======================LED6====================
void LED6_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L6 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[6] = 1;
}
void LED6_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L6 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[6] = 0;
}
// ======================LED7====================
void LED7_On() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L7 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
	P2 &= 0x1f;
	
	stat_led[7] = 1;
}
void LED7_Off() {
	TR1 = 0;
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L7 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	TR1 = 1;
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
									if(0)  // ������Ҫ������
									{
										P0 = type1[n6]&0x7f;  // ����С����
									}
									else
										P0 = type1[n6];
									P2 &= 0x1f;break;
		case 6:P2 = (P2 & 0x1f) | 0xe0;
									if(!dis_model_s5 && dis_model_s4 == 1 && !is_mesure_temp)  // ������Ҫ������
									{
										P0 = type1[n7]&0x7f;  // ����С����
									}
									else
										P0 = type1[n7];
									P2 &= 0x1f;break;
		case 7:P2 = (P2 & 0x1f) | 0xe0;P0 = type1[n8];P2 &= 0x1f;break;
	}
	if(++i==8)
		i = 0;
}

//============================================����==============================================
void Delay_Keys(uint t)
{
	while(t--);
}
//============================================���󰴼�============================================
sfr P4 = 0xc0;
sbit P44 = P4^4;
sbit P42 = P4^2;

uchar Scan_Keys()
{
	char H,L;
	
	TR0 = 0;
	
	P3 =  0xcf;P44 = 0;P42 = 0;H = (~P3) & 0x0f;
	if(H == 0) return 0;
	Delay_Keys(100);
	P3 =  0xcf;P44 = 0;P42 = 0;H = (~P3) & 0x0f;
	if(H == 0) return 0;
	P3 =  0xf0;P44 = 1;P42 = 1;L = (~P3) & 0x30;
	if(P44 == 0) L |= 0x80;
	if(P42 == 0) L |= 0x40;
	
	TR0 = 1;
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

sbit R1 = P3^3;
sbit R2 = P3^2;
sbit R3 = P3^0;
sbit C1 = P4^4;
sbit C2 = P4^2;
//============================================DS18B20��ʼ��============================================
void Init_18b20()
{
	char i;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);  // �����¶�ת��
//	Write_DS18B20(0x00);
//	Write_DS18B20(0x00);
//	Write_DS18B20(0x7f);
	
	for (i = 0; i < 8; i ++ ) {  // �ȴ���һ�βɼ�������������ʾ85
		Delay_Keys(60000);
	}
}
unsigned int Read_Temperature(void)  // ����Ҫ����2ΪС��ʱ������Ҫ�õ�unsigned int,
									 // �������3�����ͣ�������unsigned int����������
							         // ��һλС������unsigned char.
{
	unsigned char H,L;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
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
	return ((H<<8) + L)*5/8;
	
	//3.����2λС��--23.45
//	return ((H<<8)+L)*25/4;  // (50/8)
}
//============================================Ds1302��ʼ��============================================
void My_Write_Ds1302()
{
	Write_Ds1302_Byte(0x8e,0x00);  // ���д����
	Write_Ds1302_Byte(0x84,dat1);//ʱ
	Write_Ds1302_Byte(0x82,dat2);//��
	Write_Ds1302_Byte(0x80,dat3);//��
	
	Write_Ds1302_Byte(0x8e,0x80);  // ʹ��д����
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
	TMOD = (TMOD & 0xf0) | 0x05;
	TH0 = 0;
	TL0 = 0;
	
	TR0 = 1;
}
//============================================��ʱ��T1��ʼ��=======================================
void Init_T1()
{
	TMOD = (TMOD & 0x0f) | 0x10;  // ��ʱ
	TH1 = (65536 - 2000) / 256;
	TL1 = (65536 - 2000) % 256;
	
	EA = 1;
	ET1 = 1;
	TR1 = 1;
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
	if (!is_mesure_temp && dis_model_s4 == 0) {  // ʱ����ʾ����
		LED0_On();  // L1��
	}
	else {
		LED0_Off();  // L1��
	}
	
	if (!is_mesure_temp && dis_model_s4 == 1) {  // ʱ����ʾ����
		LED1_On();  // L2��
	}
	else {
		LED1_Off();  // L2��
	}
	if (is_mesure_temp) {  // ʱ����ʾ����
		LED2_On();  // L3��
	}
	else {
		LED2_Off();  // L3��
	}
	
	// ����ָʾ��
	if (is_temp_more_mesure == 1) {
		if (f_100ms_L4) LED3_Off();
		else LED3_On();
	}
	else LED3_Off();  // L4��
	
	
	LED6_Off();
	// =========================================== ������� ======================================
}

void main()
{
	Init_Sys();
	
	Init_T1();
//	TMOD = (TMOD & 0x0f) | 0x00;//��ʱ
	Init_T0();
	Init_18b20();
	Read_Temperature();

	My_Write_Ds1302();
	
	while(1)
	{
//		LED_control();
//		Scan_Keys_2x2();
        if(f_2ms == 1)
        {
            f_2ms = 0;			
			LED_control();
			// =======================������ʼ========================
			/*������Ҫѡ���Ӧ�İ�������*/
//			Change_Keys_Alone();  // 1.��������ɨ��(1)
//			ScanKeys_Alone();     // 2.��������ɨ��(2)
			
			if (!is_mesure_temp)  // ����NE555ʱ���������ܳ�ͻ
			{
				TR1 = 0;
				Change_Keys();        // 3.���󰴼�ɨ��
				TR1 = 1;
			}
			
			/*������Ҫ��Ӷ�Ӧ����*/
			if (GN == 4) {  // S4
				// =========================================== ���뿪ʼ ======================================
				
				dis_model_s4 ++ ;
				if (dis_model_s4 > 2) dis_model_s4 = 0;
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				
				// =========================================== ������� ======================================
			}
			
			if (GN == 5) {  // S5
				// =========================================== ���뿪ʼ ======================================
				
				dis_model_s5 ++ ;
				if (dis_model_s5 > 2) dis_model_s5 = 0;
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================

				// =========================================== ������� ======================================
			}
			if (GN == 8) {  // S8
				// =========================================== ���뿪ʼ ======================================

				if (dis_model_s4 == 2 && !is_mesure_temp) {  // �������ý���
					temperature_parameters ++ ;
					if (temperature_parameters > 99) temperature_parameters = 99;
				}
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================

				// =========================================== ������� ======================================
			}
			if (GN == 9) {  // S9
				// =========================================== ���뿪ʼ ======================================

				if (dis_model_s4 == 2 && !is_mesure_temp) {  // �������ý���
					temperature_parameters -- ;
					if (temperature_parameters < 0) temperature_parameters = 0;
				}
				if (dis_model_s5 == 2 && !is_mesure_temp) {  // ʱ���ת����
					is_key_s9_pressing = 1;  // ��������δ�ɿ�
				}
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================

				LED4_Off();
				
				is_key_s9_pressing = 0;  // ���������ɿ�
				num_s_key_pressing = 0;
				
				if (f_key_long) {  // ����ת��ģʽ
					is_mesure_temp_num = 0;
					f_key_long = 0;
				}
				// =========================================== ������� ======================================
			}
			
			if (GN == 7) {  // S7
				// =========================================== ���뿪ʼ ======================================
				// =========================================== ������� ======================================
				while(Scan_Keys());
				// =========================================== ���뿪ʼ ======================================
				if (!is_mesure_temp_ing) {
					num_2ms_mesure = 0;
					num_mesure_s = 0;
					
					is_mesure_temp_ing = 1;
					is_mesure_temp = 1;
					is_mesure_temp_num ++ ;  // ��������
				}
				// =========================================== ������� ======================================
			}
			// =======================��������========================
						
			// =========================================== ���Դ��뿪ʼ ======================================			
			// =======================1.�¶ȶ�ȡ��ʼ========================
			
			// =======================1.�¶ȶ�ȡ����========================
			
			
			
			// =========================================== ���Դ������ ======================================
			
			// =========================================== ���뿪ʼ ======================================
			if (f_200ms_ds1302) {
				f_200ms_ds1302 = 0;
				My_Read_Ds1302();
			}
			
			if (is_mesure_temp) {  // �������� 
				
				TR0 = 1;
				
				mesure_h = ds1302_h, mesure_m = ds1302_s;
				
//				TR1 = 0;
				temperature = Read_Temperature();
//				TR1 = 1;
				temper[(is_mesure_temp_num-1) % 10] = temperature;
				
				if (max_temperature < temperature && temperature < 8500)
					max_temperature = temperature;  // ����¶�
				
				Temper_all = 0;
				

				if (is_mesure_temp_num < 10) {
					unsigned char i;
					for (i = 0; i < is_mesure_temp_num; i ++ ) {
						Temper_all += temper[i];
					}
					Temper_average = Temper_all / is_mesure_temp_num;
				}
				else {
					unsigned char i;
					for (i = 0; i < 10; i ++ ) {
						Temper_all += temper[i];
					}
					Temper_average = Temper_all / 10;
				}
			}
			
			if ((temperature_parameters * 10) < temperature) {
					is_temp_more_mesure = 1;
				}
			else 
				is_temp_more_mesure = 0;
			
			if (!is_mesure_temp) {  // û�д�����ʪ�Ȳ���
				if (dis_model_s4 == 0) {  // ʱ����ʾ
					num1=ds1302_h/10;num2=ds1302_h%10;num3=11;
					num4=ds1302_m/10;num5=ds1302_m%10;num6=11;
					num7=ds1302_s/10;num8=ds1302_s%10;
					
					dis_model_s5 = 0;
				}
				else if (dis_model_s4 == 1) {  // ��ת
					
					if (dis_model_s5 == 0) {
						num1=12;num2=10;
						num3=max_temperature/100;num4=max_temperature%100/10;
						num5=max_temperature % 10;
						num6=Temper_average/ 100;num7=Temper_average % 100 / 10;num8=Temper_average%10;
					}
					if (dis_model_s5 == 1) {  // 
						num1=13;num2=10;num3=10;num4=10;
						num4=11;num5=11;num6=11;
						num7=11;num8=11;
					}
					if (dis_model_s5 == 2) {  // F
						num1=14;num2=is_mesure_temp_num / 10;num3=is_mesure_temp_num % 10;
						num4=mesure_h / 10;num5=mesure_h % 10;
						num6=11;
						num7=mesure_m / 10;num8=mesure_m % 10;
					}

				}
				else if (dis_model_s4 == 2) {  // P�¶Ȳ���
					num1=15;
//					num2=10;num3=10;num4=10;
//					num4=10;num5=10;num6=10;
					
					num2=dat_f / 10000;num3=(dat_f % 10000) / 1000;num4=(dat_f % 1000) / 100;
					num5=(dat_f % 100) / 10;num6=dat_f % 10;
					
					num7=temperature_parameters/10;num8=temperature_parameters%10;
				}
			}
			else {   // ������ʪ�Ȳ���
				num1=16;num2=10;num3=10;num4=10;
				num4=temperature/100;num5=temperature%100/10;
				num6=11;
				num7=(dat_f % 100) / 10;num8=dat_f % 10;
			}
			 
			// =========================================== ������� ======================================
        }
    }
}

void Service_T1() interrupt 3
{
	TH1 = (65536 - 2000) / 256;
	TL1 = (65536 - 2000) % 256;
	
	f_2ms = 1;
	
	num_2ms ++ ;
	
	if (num_2ms == 500) {
		f_1s = 1;
		num_2ms = 0;
	}
	
	if (f_1s == 1) {
		f_1s = 0;
		
		if (f_time) num_s ++ ;
		
		// ��������2s
		if (is_key_s9_pressing) {
			num_s_key_pressing ++ ;
			if (num_s_key_pressing == 2) {
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
	
	// ʱ���ȡˢ��
	num_2ms_ds1302 ++ ;
	if (num_2ms_ds1302 == 100) {
		f_200ms_ds1302 = 1;
		num_2ms_ds1302 = 0;
	}
	
	// ����ģʽ
	if (is_mesure_temp) {
		num_2ms_mesure ++ ;
		if (num_2ms_mesure == 500) {
			num_mesure_s ++ ;
			num_2ms_mesure = 0;
		}
		if (num_mesure_s >= 3) {
			num_mesure_s = 0;
			is_mesure_temp = 0;  // ��������
			is_mesure_temp_ing = 0;  // �������������ÿ�ʼ��һ�β���
			
			dis_model_s4 = 1;
			dis_model_s5 = 0;
		}
	}
	
	// L4����˸
	num_2ms_L4 ++ ;
	if (num_2ms_L4 == 50) {
		num_2ms_L4 = 0;
		f_100ms_L4 = ~f_100ms_L4;
	}
	
//	++ count_2ms_NE555;
//	if( count_2ms_NE555 == 500)
//	{
//		dat_f = (TH0 << 8) | TL0;
//		count_2ms_NE555 = 0;
//		
//		TH0 = 0;
//		TL0 = 0;
//	}
	
	if (is_mesure_temp)
	{
//		if(++count_2ms_NE555 == 500)
//		{
//			count_2ms_NE555 = 0;
//			dat_f = (TH0 << 8) | TL0;
//			TH0 = 0;
//			TL0 = 0;
//		}
	}
	else 
		TR0 = 0;
	/***************************************************
	��ʱ����صı����ĸ��·�������
	ע�⣺��Ҫ�����Ĺ���ʱ��Ĵ��루��forѭ���ȣ��������������ɴ���
	***************************************************/
	// =========================================== ���뿪ʼ ======================================
	
	// =========================================== ������� ======================================
	
	Display_SMG(num1,num2,num3,num4,num5,num6,num7,num8);//2msˢ�������
}