/*******************************************************************************
* �ļ����ƣ���������LEDλ��
* ʵ��Ŀ�ģ�1.���յ�Ƭ��IO�ڲ����Ļ�������
*           2.���հ���ɨ�輰�����ʱ���������Ļ���ԭ��
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪBTN
* �������󣺰���Ӧ����led��ʾ�����仯
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�����1.��������ʹ�õ������ʱ���������ή�͵�Ƭ��ϵͳ��ʵʱ�ԣ���Ŀ������
*             ��ⰴ�����������Ĺ��̺������������ں�����ʵ�������л��𲽽��ܻ���
*             ��ʱ��ɨ�������������
*           2.��ذ������ܿ��Բο�����������void key_proc(unsigned char key)
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "reg52.h"
#include "intrins.h"
#include "absacc.h"

sbit LCD_E = P1 ^ 7;

unsigned char temp1;

unsigned char read_key(void);
void key_proc(unsigned char key);


void cls_buzz()
{
    XBYTE[0xA000] = 0x00;
}
void cls_led()
{
    XBYTE[0x8000] = 0xff;
}
void delay(void)
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


void main(void)
{
    unsigned char temp;

    cls_buzz();
    cls_led();

    while(1)
    {
        temp = read_key();
        if(temp != 0xff)
        {
            delay();  //��������
            temp = read_key();
            if(temp != 0xff)
            {
                //���밴��������
                key_proc(temp);
                while(P3 != 0xff);  //�ȴ������ͷ�
            }
        }
    }
}
//����ɨ�躯��
unsigned char read_key(void)
{
    unsigned char temp;
    unsigned char key_value = 0xff;

    temp = P3 & 0x0f;
    switch(temp)
    {
    case 0x0e:
        key_value = 1;  //S7
        break;
    case 0x0d:
        key_value = 2;  //S6
        break;
    case 0x0b:
        key_value = 3;  //S5
        break;
    case 0x07:
        key_value = 4;  //S4
        break;
    }
    return key_value;
}

void key_proc(unsigned char key)
{
    switch(key)
    {
    case 1:
        temp1++;
        XBYTE[0x8000] = temp1;
        break;
    case 2:
        temp1--;
        XBYTE[0x8000] = temp1;
        break;
    case 3:
        temp1 = temp1 + 2;
        XBYTE[0x8000] = temp1;
        break;
    case 4:
        temp1 = temp1 - 2;
        XBYTE[0x8000] = temp1;
        break;



    }
}
