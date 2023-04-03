/*******************************************************************************
* �ļ����ƣ�����ͨѶʵ��
* ʵ��Ŀ�ģ�1.����51��Ƭ�����ڹ���ģʽ����ؼĴ������÷���
*           2.�˽�51��Ƭ�������ʵļ��㷽��
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������󣺰���ͨ�����ڽ����ݷ��͵�����
* Ӳ��˵����IAP15F2K61S2@11.0592MHz
* ע�����1.ͨ��USB����������PC�����ڼ����Ӳ���������в鿴���ں�
*           2.�򿪴��ڵ��Թ����������������ͨѶ�������趨Ϊ9600
* ���ڰ汾��2022/02/08
*******************************************************************************/

#include "STC15F2K60S2.H"  //����51��Ƭ�����⹦�ܼĴ���
#include "intrins.h"
#include "absacc.h"
#define BAUD	     9600  //������
#define SYSTEMCLOCK  11059200L  //ϵͳʱ��Ƶ��


unsigned char *number = "0123456789";
unsigned char *string = "GXCT_TEST";
unsigned char i = 96;


//��������
/*******************************************/
void delay();
void cls_buzz(void);
void uart_sendstring(unsigned char *str);
/*******************************************/

//������
void main(void)
{
    cls_buzz();
    SCON = 0x50;
    AUXR = 0x40;
    TMOD = 0x00;
    TL1 = (65536 - (SYSTEMCLOCK / 4 / BAUD));
    TH1 = (65536 - (SYSTEMCLOCK / 4 / BAUD)) >> 8;
    TR1 = 1;
    while(1)
    {
        uart_sendstring("\r\n");
        uart_sendstring(number);
        uart_sendstring("\r\n");
        uart_sendstring(string);
        uart_sendstring("\r\n");
        i++;
        if(i == 123)
            i = 96;
        delay();
    }
}

void delay()
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 43;
    j = 6;
    k = 203;
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

void cls_buzz(void)
{
    XBYTE[0xA000] = 0;
}

//ͨ�����ڷ����ַ���
void uart_sendstring(unsigned char *str)
{
    unsigned char *p;

    p = str;
    while(*p != '\0')
    {
        SBUF = *p;
        while(TI == 0);  //�ȴ����ͱ�־λ��λ
        TI = 0;
        p++;
    }
}

