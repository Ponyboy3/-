/*******************************************************************************
* �ļ����ƣ�����ܿ���ʵ��
* ʵ��Ŀ�ģ�1.���������������·����Ʒ���
*           2.�����������ʾ�Ļ���ԭ��
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������������������ʾ0-9
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�������
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "reg52.h"
#include "intrins.h"

code unsigned char tab[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};


void cls_buzz()
{
    P2 = (P2 & 0x1F | 0xA0);
    P0 = 0x00;
    P2 &= 0x1F;
}

void delay()	//delay 200ms @ 11.0592M
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 9;
    j = 104;
    k = 139;
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
void main(void)
{
    unsigned char i;
    cls_buzz();
    while(1)
    {
        for(i = 0; i < 9; i++)
        {
            P2 = ((P2 & 0x1f) | 0xc0);
            P0 = 0x55;
            P2 &= 0x1f;

            P2 = ((P2 & 0x1f) | 0xe0);
            P0 = tab[i];
            P2 &= 0x1f;
            delay();
            delay();
            delay();

            P2 = ((P2 & 0x1f) | 0xc0);
            P0 = 0xAA;
            P2 &= 0x1f;

            P2 = ((P2 & 0x1f) | 0xe0);
            P0 = tab[i + 1];
            P2 &= 0x1f;
            delay();
            delay();
            delay();

        }

    }
}

