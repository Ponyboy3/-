#include "reg52.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"

#define uchar unsigned char 
#define uint unsigned int

// �������ʾ���ݣ� һ��ǰ10��Ϊ0-9�� �ɸ�����Ҫ���������
uchar code type1[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,
					  0xc6,  // C 12
					  0x89,  // H 13
					  0x8e,  // F 14
					  0x8c,  // P 15
					  0x86,  // E 16
					  0x88}; // A 17
// xxx
uchar code type2[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

uchar num1,num2,num3,num4,num5,num6,num7,num8;  // �����ÿһλ������ num1 = 2, ��һ���������ʾ 2��
//uchar GN;  // ������־λ
bit f_2ms;  // 2ms��־λ
uchar temp_Led_P0 = 0xff;  // ��������һ��LED״̬

sbit S7 = P3^0;
sbit S6 = P3^1;
sbit S5 = P3^2;
sbit S4 = P3^3;

sbit L1 = P0^0;
sbit L2 = P0^1;
sbit L3 = P0^2;
sbit L4 = P0^3;
sbit L5 = P0^4;
sbit L6 = P0^5;
sbit L7 = P0^6;
sbit L8 = P0^7;

uint num_2ms = 0;  // ���ٸ�2ms

uint num_2ms_ne555=0, freq;
// =========================================== ���뿪ʼ ======================================
//=========�������ʾģʽ
uchar dis_smg_model = 1, dis_smg_model_old;
//=========ds1302����
uint h_ds1302;  // ʱ
uint m_ds1302;  // ��
uint s_ds1302;  // ��
// =========ADC����
uchar adc_value_read = 0;  // ��ǰ����
uchar adc_value_old = 0;  // �ϴζ���
// =========�¶Ȳ���
char temp_param = 30;
// =========��������
bit trigger;
uint trigger_time_2ms;
uchar trigger_times = 0;  // ��������
uint trigger_h_ds1302;  // ʱ
uint trigger_m_ds1302;  // ��
uint trigger_s_ds1302;  // ��
// =========�¶�ʪ������
uint humi_smg, humi_max, humi_ave;
uint temper_smg, temper_max, temper_ave;
uint humi[10], temper[10];
//LED��˸
uint num_2ms_led = 0;
bit is_num_2ms_led;
// S9����
uint press_num_2ms_s9;
bit is_long_press_s9;
// =========================================== ������� ======================================


//=========================================��ͨ��ʱ����(ģ�庯��)=================================
void Delay(uint t) {
	while( t -- );
	while( t -- );
}
//=======================================����LED���ƺ���(ģ�庯��)================================
// ======================LED1====================
void LED1_On() {
	P0 = 0xff & temp_Led_P0;  // ��������ط���P0��ֵ���ر��������һֱ��ˢ�£������ҽ��LEDǰһ�ε�״̬
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L1 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED1_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L1 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
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
	P0 = 0xff & temp_Led_P0;
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
}
void LED6_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L6 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
// ======================LED7====================
void LED7_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L7 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED7_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L7 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
// ======================LED68====================
void LED8_On() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L8 = 0;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
}
void LED8_Off() {
	P0 = 0xff & temp_Led_P0;
	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
	L8 = 1;  // Ҫ��P0�������������֮�䣬�������
	temp_Led_P0 = P0;
	P2 &= 0x1f;
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
									if ((dis_smg_model == 2 && trigger_times > 0) || (dis_smg_model == 3 && trigger_times > 0)) {
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
//============================================���󰴼�============================================
sfr P4 = 0xc0;
sbit P44 = P4^4;
sbit P42 = P4^2;
sbit P32 = P3^2;
sbit P33 = P3^3;
void Delay_Keys(uint t)
{
	while(t--);
}

void key_array_init() {
	P32 = 1;
	P33 = 1;
	P42 = 0;
	P44 = 0;
}

void key_array_scan() {
	if (P32 == 0) {
		P32 = 0;
		P42 = 1;
		P44 = 1;
		if (P42 == 0) {
			Delay_Keys(100);
			if (P42 == 0) {  // S9
				// =========================================== ���뿪ʼ ======================================
				if (dis_smg_model == 5) {
					temp_param -- ;
					if (temp_param <= 0) {
						temp_param = 0;
					}
				}
				
				is_long_press_s9 = 1;
//				press_num_2ms_s9
				// =========================================== ������� ======================================
				while (P42 == 0);
				// =========================================== ���뿪ʼ ======================================
//				is_long_s9 = 0;
//				is_long_press_s9 = 0;
				// =========================================== ������� ======================================
			}
		}
		else if (P44 == 0) {
			Delay_Keys(100);
			if (P44 == 0) {  // S5
				// =========================================== ���뿪ʼ ======================================
				if (dis_smg_model == 2) {
					dis_smg_model = 3;
				}
				else if (dis_smg_model == 3) {
					dis_smg_model = 4;
				}
				else if (dis_smg_model == 4) {
					dis_smg_model = 2;
				}
				// =========================================== ������� ======================================
				while (P44 == 0);
				// =========================================== ���뿪ʼ ======================================
				// =========================================== ������� ======================================
			}
		}
	}
	
	if (P33 == 0) {
		P33 = 0;
		P42 = 1;
		P44 = 1;
		if (P42 == 0) {
			Delay_Keys(100);
			if (P42 == 0) {  // S8
				// =========================================== ���뿪ʼ ======================================
				if (dis_smg_model == 5) {
					temp_param ++ ;
					if (temp_param >= 99) {
						temp_param = 99;
					}
				}
				// =========================================== ������� ======================================
				while (P42 == 0);
				// =========================================== ���뿪ʼ ======================================

				// =========================================== ������� ======================================
			}
		}
		else if (P44 == 0) {
			Delay_Keys(100);
			if (P44 == 0) {  // S4
				// =========================================== ���뿪ʼ ======================================
				if (dis_smg_model == 1) {
					dis_smg_model = 2;
				}
				else if (dis_smg_model == 2 | dis_smg_model == 3 | dis_smg_model == 4) {
					dis_smg_model = 5;
				}
				else if (dis_smg_model == 5) {
					dis_smg_model = 1;
				}
				// =========================================== ������� ======================================
				while(P44 == 0);
				// =========================================== ���뿪ʼ ======================================

				// =========================================== ������� ======================================
			}
		}
	}
	
	key_array_init();
}
//============================================Ds13023============================================
void My_Write_Ds1302()
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x84,0x13);  // ʱ
	Write_Ds1302_Byte(0x82,0x03);  // ��
	Write_Ds1302_Byte(0x80,0x05);  // ��
}
void My_Read_Ds1302()
{
	ET1 = 0;
	
	h_ds1302 = Read_Ds1302_Byte(0x85);  // ʱ
	h_ds1302 = (h_ds1302 >> 4) * 10 + (h_ds1302 & 0x0f);
	m_ds1302 = Read_Ds1302_Byte(0x83);  // ��
	m_ds1302 = (m_ds1302 >> 4) * 10 + (m_ds1302 & 0x0f);
	s_ds1302 = Read_Ds1302_Byte(0x81);  // ��
	s_ds1302 = (s_ds1302 >> 4) * 10 + (s_ds1302 & 0x0f);
	
	ET1 = 1;
}
//==========================================�¶����ݴ���=============================================
void Handle_temper() {
	uint sum = 0;
	uchar i = 0;
	temper[(trigger_times - 1) % 10] = temper_smg;
	
	if (temper_smg > temper_max) {
		temper_max = temper_smg;
	}
	
	// ��ƽ���¶�
	if (trigger_times < 10) {
		for (i = 0; i < trigger_times; i ++ ) {
			sum += temper[i];
		}
		
		temper_ave = sum / trigger_times;
	}
	else {
		for (i = 0; i < 10; i ++ ) {
			sum += temper[i];
		}
		
		temper_ave = sum / 10;
	}
}
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
//============================================��ʱ��T1��ʼ��=======================================
void Init_T1()
{
	TMOD = (TMOD & 0xf0) | 0x10;//��ʱ
	TH1 = (65536 - 2000) / 256;//2ms
	TL1 = (65536 - 2000) % 256;
	
	EA = 1;
	ET1 = 1;
	TR1 = 1;
}
//============================================��ʱ��T0��ʼ��=======================================
void Init_T0()
{
	TMOD = (TMOD & 0xf0) | 0x05;//��ʱ
	TH0 = 0;
	TL0 = 0;
	
	TR0 = 1;
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
//==========================================LED����=============================================
void LED_control() {
	// ================= ���Կ�ʼ =============
//	P2 = (P2 & 0x1f) | 0x80;  // LED���ƿ���
//	P0 = 0x7f;  // Ҫ��P0�������������֮�䣬�������
//	P2 &= 0x1f;
	// ================= ���Խ��� =============
	
	// =========================================== ���뿪ʼ ======================================
//	if (trigger) {
//		LED4_On();
//	}
//	else {
//		LED4_Off();
//	}
	
	if (dis_smg_model == 1) {
		LED1_On();
	}
	else {
		LED1_Off();
	}
	
	if (dis_smg_model == 2 || dis_smg_model == 3 || dis_smg_model == 4) {
		LED2_On();
	}
	else {
		LED2_Off();
	}
	
	if (dis_smg_model == 6) {
		LED3_On();
	}
	else {
		LED3_Off();
	}
	
	LED7_Off();
	LED8_Off();
	// =========================================== ������� ======================================
}
//==========================================Ƶ�����ݴ���=============================================
void Handle_humi() {
	uint sum = 0;
	uchar i = 0;
	humi[(trigger_times - 1) % 10] = humi_smg;
	
	if (humi_smg > humi_max) {
		humi_max = humi_smg;
	}
	
	// ��ƽ���¶�
	if (trigger_times < 10) {
		for (i = 0; i < trigger_times; i ++ ) {
			sum += humi[i];
		}
		
		humi_ave = sum / trigger_times;
	}
	else {
		for (i = 0; i < 10; i ++ ) {
			sum += humi[i];
		}
		
		humi_ave = sum / 10;
	}
}
//==========================================Ƶ��תʪ��=============================================
void Freq_to_humi() {
	float humi = 0;
	if (freq >= 200 && freq <= 2000) {
		humi = 2.0 * (freq - 200) / 45.0;
		
		humi_smg = humi * 10;
		LED5_Off();
		
		Handle_humi();
	}
	else {
		humi_smg = 99;
		LED5_On();
	}
}
void main()
{
	Init_Sys();
	Init_T1();
	Init_T0();
	My_Write_Ds1302();
	
	Init_18b20();
	
	while(1)
	{
//		LED_control();
        if(f_2ms == 1)
        {
            f_2ms = 0;			
			LED_control();  // xxx
			
			My_Read_Ds1302();  // ʱ��ɨ��
			
			// =======================����========================
//			Change_Keys_Alone();  // 1.��������ɨ��(1)
//			ScanKeys_Alone();     // 2.��������ɨ��(2)
//			Change_Keys();        // 3.���󰴼�ɨ��
			key_array_scan();     // 4.���󰴼�ɨ��(Ne555ר��)
			
			// =========================================== ���뿪ʼ ======================================
			if (dis_smg_model == 1) {
				dis_smg_model_old = dis_smg_model;
				
				num1 = h_ds1302 / 10;num2 = h_ds1302 % 10;
				num3 = num6 = 11;
				num4 = m_ds1302 / 10;num5 = m_ds1302 % 10;
				num7 = s_ds1302 / 10;num8 = s_ds1302 % 10;
			}
			else if (dis_smg_model == 2) {  // C
				dis_smg_model_old = dis_smg_model;
				
				num1 = 12;
				
				if (trigger_times >= 1) {
					num2 = 10;
					num3 = temper_max / 100;num4 = temper_max % 100 / 10;
					num5 = 11;
					num6 = temper_ave / 100;num7 = temper_ave % 100 / 10;num8 = temper_ave % 10;
				}
				else {
					num2 = 10;
					num3 = 10;
					num6 = 10;
					num4 = 10;num5 = 10;
					num7 = 10;num8 = 10;
				}
			}
			else if (dis_smg_model == 3) {  // H
				dis_smg_model_old = dis_smg_model;
				
				num1 = 13;num2 = 10;
				
				if (trigger_times >= 1) {
					num3 = humi_max / 100;num4 = humi_max % 100/ 10;
					num5 = 11;
					num6 = humi_ave / 100;num7 = humi_ave % 100 / 10;num8 = humi_ave % 10;
				}
				else {
					num3 = 10;
					num6 = 10;
					num4 = 10;num5 = 10;
					num7 = 10;num8 = 10;
				}
			}
			else if (dis_smg_model == 4) {  // F
				dis_smg_model_old = dis_smg_model;
				
				num1 = 14;
				
				num2 = trigger_times / 10;
				num3 = trigger_times % 10;
				if (trigger_times >= 1) {
					num6 = 11;
					num4 = trigger_h_ds1302 / 10;num5 = trigger_h_ds1302 % 10;
					num7 = trigger_m_ds1302 / 10;num8 = trigger_m_ds1302 % 10;
				}
				else {
					num6 = 10;
					num4 = 10;num5 = 10;
					num7 = 10;num8 = 10;
				}
			}
			else if (dis_smg_model == 5) {  // P
				dis_smg_model_old = dis_smg_model;
				
				num1 = 15;
				num2 = 10;
				num3 = 10;
				num4 = 10;
				num5 = 10;
				num6 = 10;
				num7 = temp_param / 10;num8 = temp_param % 10;
			}
			else if (dis_smg_model == 6) {  // E
				num1 = 16;
				num2 = 10;
				num3 = 10;
				num4 = temper_smg / 100; num5 = temper_smg % 100 / 10;
				num6 = 11;
				if (humi_smg == 99) {
					num7 = 17;num8 = 17;
				}
				else {
					num7 = humi_smg / 100;num8 = humi_smg % 100 / 10;
				}
			}
			
//			num3 = freq / 10000;
//			num4 = freq % 10000 / 1000;
//			num5 = freq % 1000 / 100;
//			num6 = freq % 100 / 10;
//			num7 = freq % 10;
			
			// ��ȡ
			if (trigger == 0) {  // ֻ����δ����ʱ���ܴ���
				adc_value_read = Read_ADC(1);  // ��ȡ��������
				if (adc_value_read < 50 && adc_value_old >= 50)
				{
					trigger = 1;
					trigger_time_2ms = 0;
					dis_smg_model = 6;
				}
				else {
					trigger = 0;
				}
				adc_value_old = adc_value_read;
			}
			
			
			if (trigger) {
//				LED1_On();
				
				Freq_to_humi();
				temper_smg = Read_Temperature();
				if (humi_smg != 99) {
					Handle_temper();
				}
				
				
				if (trigger_times >= 2) {
					uchar n;
					n = (trigger_times -1)%10;
					if (n == 0) {
						if (temper[0] > temper[9] && humi[0] > humi[9]) {
							LED6_On();
						}
						else {
							LED6_Off();
						}
					}
					else {
						if (temper[n] > temper[n - 1] && humi[n] > humi[n - 1]) {
							LED6_On();
						}
						else {
							LED6_Off();
						}
					}
				}
				
				My_Read_Ds1302();  // ʱ��ɨ��
				trigger_h_ds1302 = h_ds1302;
				trigger_m_ds1302 = m_ds1302;
				trigger_s_ds1302 = s_ds1302;
				
//				dis_smg_model = 6;
			}
			else {
//				LED1_Off();
			}
			
			if (temper_smg > temp_param * 10) {
				if (is_num_2ms_led) {
					LED4_On();
				}
				else {
					LED4_Off();
				}
			}
			else {
				LED4_Off();
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
	/***************************************************
	��ʱ����صı����ĸ��·�������
	ע�⣺��Ҫ�����Ĺ���ʱ��Ĵ��루��forѭ���ȣ��������������ɴ���
	***************************************************/
	// =========================================== ���뿪ʼ ======================================
	if (++ num_2ms_ne555 >= 500)
	{
		num_2ms_ne555 = 0;
		freq = (TH0 << 8) | TL0;
		TH0 = 0;
		TL0 = 0;
	}
	
	if (++ num_2ms_led >= 50) {
		num_2ms_led = 0;
		is_num_2ms_led = ~is_num_2ms_led;
	}

	// ��������
	if (trigger) {
		if (++ trigger_time_2ms >= 1500) {  // 3s����
			trigger = 0;
			trigger_time_2ms = 0;
			
			trigger_times ++ ;
			dis_smg_model = dis_smg_model_old;  // ����ԭ״̬
		}
	}
	
	// s9����
	if (is_long_press_s9) {
		if (++ press_num_2ms_s9 >= 1000) {
			press_num_2ms_s9 = 0;
			
			trigger_times = 0;  // ��������
			trigger_h_ds1302 = 0;  // ʱ
			trigger_m_ds1302 = 0;  // ��
			trigger_s_ds1302 = 0;  // ��
			humi_smg = 0;
			humi_max = 0;
			humi_ave = 0;
			temper_smg = 0;
			temper_max = 0;
			temper_ave = 0;
			
			is_long_press_s9 = 0;
		}
	}
	// =========================================== ������� ======================================
	
	Display_SMG(num1,num2,num3,num4,num5,num6,num7,num8);//2msˢ�������
}