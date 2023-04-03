/*******************************************************************************
* �ļ����ƣ�DS18B20�¶ȴ�����ʵ��
* ʵ��Ŀ�ģ�1.���յ�����ͨѶ�����ص�͹���ʱ��
*           2.����51��Ƭ��ģ�ⵥ����ʱ��ĳ�����Ʒ���
*           3.����DS18B20�¶ȴ������Ĳ�������
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �����������ִ����¶ȴ��������������ʾ�б仯
* Ӳ��˵����IAP15F2K61S2@11.0592MHz
* ע�������������Ϊ1���϶�
* ���ڰ汾��2022/02/08
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "onewire.h"  //�����ߺ�����
#include "absacc.h"

sfr AUXR = 0x8E;

code unsigned char tab[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������

bit temper_flag = 0;//�¶ȶ�ȡ��־
//��������
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
/**********************************/

//������
void main(void)
{
    unsigned char temperature;
    cls_buzz();
    cls_led();
    AUXR |= 0x80;
    TMOD &= 0xF0;
    TL0 = 0xCD;	//1ms,1Tmoshi
    TH0 = 0xD4;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;

    while(1)
    {
        if(temper_flag)
        {
            temper_flag = 0;
            temperature = rd_temperature();  //���¶�
        }

        //��ʾ���ݸ���
        (temperature >= 10) ? (dspbuf[6] = temperature / 10) : (dspbuf[6] = 10);
        dspbuf[7] = temperature % 10;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    static unsigned char intr;
    if(++intr == 50)  //1msִ��һ��
    {
        intr = 0;
        temper_flag = 1;  //50ms�¶ȶ�ȡ��־λ��1
    }
    display();
}

void cls_buzz()
{
    P2 = ((P2 & 0x1f) | 0xA0);
    P0 = 0x00;
    P2 &= 0x1f;
}
void cls_led()
{
    P2 = ((P2 & 0x1f) | 0x80);
    P0 = 0xFF;
    P2 &= 0x1f;
}

//��ʾ����
void display(void)
{
    static unsigned char dspcom = 0;
    P2 = ((P2 & 0x1f) | 0xE0); //��ѡ573
    P0 = 0xff;               //��������
    P2 &= 0x1f;

    P0 = 1 << dspcom;	       //ѭ��λѡ
    P2 = ((P2 & 0x1f) | 0xC0); //λѡ573
    P2 &= 0x1f;

    P0 = tab[dspbuf[dspcom]];//��ʾ����
    P2 = ((P2 & 0x1f) | 0xE0); //��ѡ573
    P2 &= 0x1f;

    if(++dspcom == 8)
    {
        dspcom = 0;
    }
}

