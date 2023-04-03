/*******************************************************************************
* �ļ����ƣ���ʱ��ɨ�谴��ʵ��
* ʵ��Ŀ�ģ�1.����ͨ����ʱ��ɨ�谴����ֵ�Ļ�������
*           2.����51��Ƭ����ʱ������ģʽ�����÷���
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������󣺰�S7��S6�������������ʾ���ֱַ��1����1
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�������
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "reg52.h"
#include "intrins.h"

sfr AUXR = 0x8E;
bit key_re;
unsigned char key_press;
unsigned char key_value;
bit key_flag;
unsigned char intr;
                           //  0     1     2     3     4     5    6     7     8     9    Ϩ��
code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������
unsigned char dspcom = 0;
unsigned char read_key(void);
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

//������
void main(void)
{
    unsigned char key_temp;
    unsigned char num;
    cls_buzz();
    cls_led();
    P3 = 0xff;  //�����ӿڳ�ʼ��
    AUXR |= 0x80;	//1Tģʽ��IAP15F2K61S2��Ƭ�����⹦�ܼĴ���
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
            num++;
            break;
        case 2:
            num--;
            break;
        }
        key_temp = 0;  //��ɰ����������������ֵ

        (num >= 100) ? (dspbuf[5] = num / 100) : (dspbuf[5] = 10);
        (num >= 10) ? (dspbuf[6] = num % 100 / 10) : (dspbuf[6] = 10);
        dspbuf[7] = num % 10;
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

//��ȡ��ֵ
unsigned char read_key(void)
{

    unsigned char key_temp;

    key_temp = (P3 & 0x0f);
    /**************��μ�⣬���Կ��Է�ֹ����*********************/
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
            key_value = 1;  //
            break;
        case 0x0d:
            key_value = 2;  //
            break;
        case 0x0b:
            key_value = 3;  //
            break;
        case 0x07:
            key_value = 4;  //
            break;
        }
    }
    //����5�μ�⵽���������£����Ҹð����Ѿ��ͷ�
    if((key_re == 1) && (key_temp == 0x0f))
    {
        key_re = 0;
        return key_value;
    }

    return 0xff;  //�ް������»򱻰��µİ���δ���ͷ�
}

void display(void)
{
    P2 = ((P2 & 0x1f) | 0xe0);
    P0 = 0xff;
    P2 &= 0x1f;

    P2 = ((P2 & 0x1f) | 0xc0);
    P0 = (1 << dspcom);
    P2 &= 0x1f;

    P2 = ((P2 & 0x1f) | 0xe0);
    P0 = tab[dspbuf[dspcom]];
    P2 &= 0x1f;
    if(++dspcom == 8)
    {
        dspcom = 0;
    }
}
