/*******************************************************************************
* �ļ����ƣ�DACת��ʵ��
* ʵ��Ŀ�ģ�1.����I2C����ͨѶ�����ص�͹���ʱ��
*           2.����51��Ƭ��ģ��I2C����ʱ��ĳ�����Ʒ���
*           3.����PCF8591 ADC�Ĳ�������
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������󣺰�S7��S6������ʵ��ת��ֵ�仯������ǰ����λ�������
            ʾ��ģת��������ֵ������λ��ʾģ���ѹֵ����λmV
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע����������ñ����J3���������D\A���ţ�������ѹ����ʾ��ѹֵ���жԱ�
* ���ڰ汾��2022/02/08
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "i2c.h"    //I2C����������
#include "intrins.h"

#define VREF  4850  //PCF8591�ο���ѹ

sbit p27 = P2 ^ 7;
sbit p26 = P2 ^ 6;
sbit p25 = P2 ^ 5;

sfr AUXR = 0x8E;
                            //0,   1,    2,    3,     4,    5,    6,    7,    8,   9,   Ϩ��
code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90,0xff};
unsigned char dspbuf[8] = {10, 10, 10, 10, 0, 0, 0, 0}; //��ʾ������
unsigned char dspcom = 0;
bit key_re;
unsigned char key_press;
unsigned char key_value;
bit key_flag;
unsigned char intr;
void display(void);
unsigned char read_key(void);

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


//������
void main(void)
{
    unsigned char key_temp;
    unsigned char dac_value;  //DACת������
    unsigned int Vaout;  //�����ѹ
    cls_buzz();
    cls_led();
    init_pcf8591();
    AUXR |= 0x80;
    TMOD &= 0xF0;
    TL0 = 0xCD;
    TH0 = 0xD4;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;

    while(1)
    {
        if(key_flag)
        {
            key_flag = 0;
            key_temp = read_key();
        }
        //���밴��������򲿷�
        switch(key_temp)
        {
        case 1:
        {
            dac_value = dac_value + 5;
            if(dac_value > 255)dac_value = 255;
            dac_pcf8591(dac_value);
        }
        break;
        case 2:
        {
            dac_value = dac_value - 5;
            if(dac_value < 0)dac_value = 0;
            dac_pcf8591(dac_value);
        }
        break;
        }
        key_temp = 0;  //��ɰ����������������ֵ
        Vaout = VREF / 255 * dac_value;
        dspbuf[0] = 10;
        (Vaout >= 100) ? (dspbuf[1] = dac_value / 100) : (dspbuf[1] = 10);
        (Vaout >= 10) ? (dspbuf[2] = dac_value % 100 / 10) : (dspbuf[2] = 10);
        dspbuf[3] = dac_value % 10;

        //��ʾ���ݸ���
        (Vaout >= 1000) ? (dspbuf[4] = Vaout / 1000) : (dspbuf[4] = 10);
        (Vaout >= 100) ? (dspbuf[5] = Vaout % 1000 / 100) : (dspbuf[5] = 10);
        (Vaout >= 10) ? (dspbuf[6] = Vaout % 100 / 10) : (dspbuf[6] = 10);
        dspbuf[7] = Vaout % 10;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    display();
    if(++intr == 10)  //1msִ��һ��
    {
        intr = 0;
        key_flag = 1;  //10ms����ɨ���־λ��1
    }
}

//��ʾ����
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

//��ȡ��ֵ
unsigned char read_key(void)
{

    unsigned char key_temp;

    key_temp = (P3 & 0x0f);

    if(key_temp != 0x0f) //�а�������
        key_press++;
    else
        key_press = 0;  //����

    if(key_press == 5)
    {
        key_press = 0;
        key_re = 1;
        switch(key_temp)
        {
        case 0x0e:
            key_value = 1;  //S1
            break;
        case 0x0d:
            key_value = 2;  //S5
            break;
        case 0x0b:
            key_value = 3;  //S9
            break;
        case 0x07:
            key_value = 4;  //S13
            break;
        }
    }
    //�������μ�⵽���������£����Ҹð����Ѿ��ͷ�
    if((key_re == 1) && (key_temp == 0x0f))
    {
        key_re = 0;
        return key_value;
    }

    return 0xff;  //�ް������»򱻰��µİ���δ���ͷ�
}