/*******************************************************************************
* �ļ����ƣ�DS1302ʱ������ʵ��
* ʵ��Ŀ�ģ�1.��������ܶ�̬ɨ��Ļ���ԭ��,�����������������Ӱ���ķ���
*           2.����51��Ƭ����ʱ������ģʽ�����÷���
*           3.����keil uvision���ɿ��������£��жϷ�������Ʒ���
*			      4.����DS1302ʱ��оƬ�����͹���ģʽ���õķ���
* ʵ�����ã�J3��������ΪMM��ʽ
* ������������ܶ�̬��ʾ
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�����1.DS1302ʵ�������Ӧ�������ֽӴ�ʱ���ⲿ�����ⲿ�ĸ��ſ��ܵ��¾���ͣ
			        ֹ�񵴣������ʱ�쳣
			      2.��ͬ�ľ�������ʱ����ڲ��죬�����ϵ��ʼ�����ӳ�С��ʱ�䣬�ٶ�RTC
			        оƬ���в���
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"
#include "ds1302.h"

sfr AUXR = 0x8E;    //���帨���Ĵ���
                           //  0     1    2     3     4     5     6     7     8     9    10     Ϩ��
code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xFF, 0xbf};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������
unsigned char dspcom = 0;  //��ʾλѡ

unsigned char ms = 0;
bit rtc_flag = 0;


void cls_buzz(void)
{
    XBYTE[0xA000] = 0x00;
}

void cls_led(void)
{
    XBYTE[0x8000] = 0xFF;
}


void Delay100ms()		//@11.0592MHz
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 5;
    j = 52;
    k = 195;
    do
    {
        do
        {
            while (--k);
        }
        while (--j);
    }
    while (--i);
}


//������
void main(void)
{
    unsigned char *p_rtc;

    cls_buzz();
    cls_led();
    AUXR |= 0x80;
    TMOD &= 0xF0;
    TL0 = 0xCD;
    TH0 = 0xD4;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;

    Delay100ms();
    SetRTC();


    while(1)
    {
        if(rtc_flag)
        {
            rtc_flag = 0;
            p_rtc = ReadRTC();
            dspbuf[0] = p_rtc[0];
            dspbuf[1] = p_rtc[1];
            dspbuf[2] = 11;
            dspbuf[3] = p_rtc[2];
            dspbuf[4] = p_rtc[3];
            dspbuf[5] = 11;
            dspbuf[6] = p_rtc[4];
            dspbuf[7] = p_rtc[5];
        }
    }
}

//��ʾ����
void display(void)
{
    XBYTE[0xE000] = 0xff;  //����
    XBYTE[0xC000] = (1 << dspcom);
    XBYTE[0xE000] = tab[dspbuf[dspcom]]; //���� ������ʾ������������ 8msˢ��һ�Σ�

    if(++dspcom == 8)
    {
        dspcom = 0;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    if(++ms == 200)
    {
        ms = 0;
        rtc_flag = 1;
    }
    display();  //1msִ��һ��
}


