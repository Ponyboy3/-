/*******************************************************************************
* �ļ����ƣ����������洢ʵ��
* ʵ��Ŀ�ģ�1.����I2C����ͨѶ�����ص�͹���ʱ��
*           2.����51��Ƭ��ģ��I2C����ʱ��ĳ�����Ʒ���
*           3.����EEPROM�洢�������Լ�AT24C02�Ķ�д��������
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������������ϵ磬�������ʾ+1
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�������
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "reg52.h"
#include "i2c.h"
#include "intrins.h"


sbit p27 = P2 ^ 7;
sbit p26 = P2 ^ 6;
sbit p25 = P2 ^ 5;
sfr AUXR = 0x8E;
code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������
unsigned char dspcom = 0;
void display(void);
void cls_buzz()
{
    P2 = (P2 & 0x1F | 0xA0);
    P0 = 0x00;
    P2 &= 0x1F;
}

void cls_led()
{
    P2 = (P2 & 0x1F | 0x80);
    P0 = 0xFF;
    P2 &= 0x1F;
}


void delay()		//10ms @11.0592MHz
{
    unsigned char i, j;

    i = 108;
    j = 145;
    do
    {
        while (--j);
    }
    while (--i);
}

//������
void main(void)
{
    unsigned char reset_cnt;  //���������洢 (���洢ֵ255)
    cls_buzz();
    cls_led();
    AUXR |= 0x80;
    TMOD &= 0xF0;
    TL0 = 0xCD;
    TH0 = 0xD4;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    reset_cnt = read_eeprom(0x00);  //��AT24C02��ַ0x00�ж�ȡ����
    reset_cnt++;
    delay();	//��ʱ10ms
    write_eeprom(0x00, reset_cnt); //��AT24C02��ַ0x00��д������
    delay();
    EA = 1;	//����д����ɺ󣬿����ж�
    //������ʾ����
    (reset_cnt >= 100) ? (dspbuf[5] = reset_cnt / 100) : (dspbuf[5] = 10);
    (reset_cnt >= 10) ? (dspbuf[6] = reset_cnt % 100 / 10) : (dspbuf[6] = 10);
    dspbuf[7] = reset_cnt % 10;

    while(1)
    {
        ;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    display();
}


void display(void)
{

    p27 = 1;
    p26 = 1;
    p25 = 1;
    P0 = 0xff;
    p27 = 0;
    p26 = 0;
    p25 = 0;

    p27 = 1;
    p26 = 1;
    p25 = 0;
    P0 = (1 << dspcom);
    p27 = 0;
    p26 = 0;
    p25 = 0;

    p27 = 1;
    p26 = 1;
    p25 = 1;
    P0 = tab[dspbuf[dspcom]];
    p27 = 0;
    p26 = 0;
    p25 = 0;
    if(++dspcom == 8)
    {
        dspcom = 0;
    }
}
