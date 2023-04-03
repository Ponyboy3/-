/*******************************************************************************
* �ļ����ƣ��������ʵ��
* ʵ��Ŀ�ģ�1.���վ������ɨ��Ļ���ԭ��
*           2.���հ���ɨ�輰���������Ļ���ԭ��
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪKBD��J2����Ϊ1-3��2-4
* ʵ�����󣺰���S7��S15��S19�������ʾ00-02
            ����S6��S14��S18�������ʾ03-05
            ����S5��S13��S17�������ʾ06-08
            ����S4��S12��S16�������ʾ09-11
* Ӳ��˵����IAP15F2K61S2@11.0592MHz����J13ѡΪMMģʽʱ��Ҫp42�ڴ����źţ�
*           ��ʹ�ð���s11��s10��s9��s8ʱ��Ҫp42ȷ��������ֵ����ͻ�������ڴ˲�ʹ����Щ����
* ����˵��������״̬�����а���ɨ�裬ϵͳʵʱ�Ժã���ÿ10msɨ����Է�ֹ������
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "STC15F2K60S2.H"  //�ô�ͷ�ļ�ֱ�Ӵ���REG52.H�����������ͷ�ļ��Ա�
#include "absacc.h"

                          //   0    1     2     3     4     5     6     7     8     9    Ϩ��
code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xFF};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������

bit key_flag = 0;//����ɨ���־λ
unsigned char key_value = 0xFF; //����ֵ
/******��������***********************/
void read_keyboard(void);
void display();
/************************************/

void cls_buzz(void)	//�رշ�����
{
    XBYTE[0xA000] = 0;
}

void cls_led(void)//�ر�����LED
{
    XBYTE[0x8000] = 0xFF;
}

//������
void main(void)
{
    cls_buzz();
    cls_led();
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
            read_keyboard();
            if(key_value != 0xFF)
            {
                dspbuf[7] = key_value % 10;
                dspbuf[6] = key_value / 10;
            }
        }

    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    static unsigned char intr = 0;
    display(); //1msִ��һ��
    if(++intr == 10)
    {
        intr = 0;
        key_flag = 1;  //10ms����ɨ���־λ��1
    }
}

//��ȡ������̼�ֵ:ת�Ӱ���ʹ��P42��P44����8051����
//˳���е�P36��P37����
void read_keyboard(void)
{
    static unsigned char hang;
    static unsigned char key_state = 0;
    switch(key_state)
    {
    case 0:
    {
        P3 = 0x0f;
        P44 = 0;
        if(P3 != 0x0f) //�а�������
            key_state = 1;
    }
    break;
    case 1:
    {
        P3 = 0x0f;
        P44 = 0;
        if(P3 != 0x0f) //�а�������
        {
            if(P30 == 0)hang = 1;
            if(P31 == 0)hang = 2;
            if(P32 == 0)hang = 3;
            if(P33 == 0)hang = 4;//ȷ����
            switch(hang)
            {
            case 1:
            {
                P3 = 0xf0;
                P44 = 1;
                if(P44 == 0)
                {
                    key_value = 0;
                    key_state = 2;
                }
                else if(P35 == 0)
                {
                    key_value = 1;
                    key_state = 2;
                }
                else if(P34 == 0)
                {
                    key_value = 2;
                    key_state = 2;
                }
            }
            break;
            case 2:
            {
                P3 = 0xf0;
                P44 = 1;
                if(P44 == 0)
                {
                  key_value = 3;
                  key_state = 2;
                }
                else if(P35 == 0)
                {
                    key_value = 4;
                    key_state = 2;
                }
                else if(P34 == 0)
                {
                    key_value = 5;
                    key_state = 2;
                }
            }
            break;
            case 3:
            {
                P3 = 0xf0;
                P44 = 1;
                if(P44 == 0)
                {
                    key_value = 6;
                    key_state = 2;
                }
                else if(P35 == 0)
                {
                    key_value = 7;
                    key_state = 2;
                }
                else if(P34 == 0)
                {
                    key_value = 8;
                    key_state = 2;
                }
            }
            break;
            case 4:
            {
                P3 = 0xf0;
                P44 = 1;
                if(P44 == 0)
                {
                    key_value = 9;
                    key_state = 2;
                }
                else if(P35 == 0)
                {
                    key_value = 10;
                    key_state = 2;
                }
                else if(P34 == 0)
                {
                    key_value = 11;
                    key_state = 2;
                }
            }
            break;
            }
        }
        else
        {
            key_state = 0;
        }
    }
    break;
    case 2:
    {
        P3 = 0x0f;
        P44 = 0;
        if(P3 == 0x0f) //�����ſ�
            key_state = 0;
    }
    break;

    }

}


void display(void)
{
    static unsigned char dspcom = 0;
    XBYTE[0xE000] = 0xff;
    XBYTE[0xC000] = 1 << dspcom; 
    XBYTE[0xE000] = tab[dspbuf[dspcom]]; 
    if(++dspcom == 8)
    {
        dspcom = 0;
    }
}



