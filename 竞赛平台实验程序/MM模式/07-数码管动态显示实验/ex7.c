/*******************************************************************************
* �ļ����ƣ�����ܿ���ʵ��
* ʵ��Ŀ�ģ�1.��������ܶ�̬ɨ��Ļ���ԭ��,�����������������Ӱ���ķ���
*           2.����51��Ƭ����ʱ������ģʽ�����÷���
*           3.����keil uvision���ɿ��������£��жϷ�������Ʒ���
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* ������������ܶ�̬��ʾ
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�������⶯̬��ʾ�;�̬��ʾ
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"

sfr AUXR = 0x8E;    //���帨���Ĵ�������Ϊ��51��Ƭ��ͷ�ļ���û�ж���
                           //0     1      2      3     4     5     6     7      8    9    Ϩ��
code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xFF};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������
unsigned char dspcom = 0;  //��ʾλѡ

unsigned char ms = 0;
unsigned char counter = 0;	//200ms ��1,���ֵ255

void display(void);

void cls_buzz(void)
{
    XBYTE[0xA000] = 0x00;
}

void cls_led(void)
{
    XBYTE[0x8000] = 0xFF;
}

//������
void main(void)
{
    cls_buzz();
    cls_led();

    AUXR |= 0x80;	//��ʱģʽ�£�1Tģʽ��IAP15F2K61S2��Ƭ�����⹦�ܼĴ�������Ƶ
    TMOD &= 0xF0;      //T1��ģʽ3�±���ֹ��T0�����ڷ�ʽ1�£�16λ�Զ���װ��ʱ��
    TL0 = 0xCD;	    	//1ms��ʱ
    TH0 = 0xD4;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;

    while(1)
    {
        if(counter >= 100)
        {
            dspbuf[0] = counter / 100;
        }
        else
        {
            dspbuf[0] = 10;  //10��ӦϨ��
        }
        if(counter >= 10)
        {
            dspbuf[1] = counter % 100 / 10;
        }
        else
        {
            dspbuf[1] = 10;
        }
        dspbuf[2] = counter % 10;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    if(++ms == 200)
    {
        ms = 0;
        counter++;
    }
    display();  //1msִ��һ��
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

