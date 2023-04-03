/*******************************************************************************
* �ļ����ƣ�DS18B20�¶ȴ�����ʵ��-С���㴦��
* ʵ��Ŀ�ģ�1.���ճ��õ��ַ���������
*           2.����DS18B20�¶ȴ������Ĳ�������
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������󣺰���ͨ�����ڽ��¶ȼ����ֵ���͵�����
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ����˵����1.ͨ���������ʵʱ�¶����ݣ�ʹ�ô��ڵ��Թ��߽�������
*           2.����ͨѶ�������趨Ϊ2400
* ���ڰ汾��2022/02/08
*******************************************************************************/
#include "STC15F2K60S2.H"  //����51��Ƭ�����⹦�ܼĴ���
#include "onewire.h"  //�����ߺ�����
#include <stdio.h>
#include "intrins.h"
#include "absacc.h"

#define BAUD	     2400  //������
#define SYSTEMCLOCK  11059200L  //ϵͳʱ��Ƶ��
//��������
/*****************************************/
void delay(void);
void cls_buzz();
void uart_sendstring(unsigned char *str);
/*****************************************/

//������
void main(void)
{
    float temperature;
    unsigned char str[40];
    cls_buzz();
    SCON = 0x50;
    AUXR = 0x40;                //1T
    TMOD = 0x00;
    TL1 = (65536 - (SYSTEMCLOCK / 4 / BAUD));
    TH1 = (65536 - (SYSTEMCLOCK / 4 / BAUD)) >> 8;
    TR1 = 1;

    while(1)
    {
        sprintf(str, "%s%6.3f%c%c", "temperature:", temperature, '\r', '\n');

        temperature = rd_temperature_f();  //���¶�
        uart_sendstring(str);
        delay();
        delay();
    }
}

void delay()		//@11.0592MHz  300ms
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
    i = 13;
    j = 156;
    k = 83;
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