/*******************************************************************************
* �ļ����ƣ�ADCת��ʵ��
* ʵ��Ŀ�ģ�1.����I2C����ͨѶ�����ص�͹���ʱ��
*           2.����51��Ƭ��ģ��I2C����ʱ��ĳ�����Ʒ���
*           3.����PCF8591 ADCоƬ�Ĳ�������
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* ����������תRb2��λ�����������ʾ������0~255֮����б仯
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�������
* ���ڰ汾��2022/02/08
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "i2c.h"    //I2C���������� 
#include "intrins.h"

sbit p27 = P2 ^ 7;
sbit p26 = P2 ^ 6;
sbit p25 = P2 ^ 5;
sfr AUXR = 0x8E; 	//IAP15F2K61S2��Ƭ�����⹦�ܼĴ���

code unsigned char tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90,0xff};
unsigned char dspbuf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; //��ʾ������
unsigned char dspcom = 0;
unsigned char intr;
bit adc_flag;

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
    unsigned char adc_value;  //ADCת������
    cls_buzz();
    cls_led();
    P1 &= 0x7f;
    AUXR |= 0x80;
    TMOD &= 0xF0;
    TL0 = 0xCD;
    TH0 = 0xD4;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;
    init_pcf8591();  //PCF8591��ʼ��

    while(1)
    {
        if(adc_flag)
        {
            adc_flag = 0;  //���ADCɨ���־λ
            adc_value = adc_pcf8591();
            //������ʾ����
            (adc_value >= 100) ? (dspbuf[5] = adc_value / 100) : (dspbuf[5] = 10);
            (adc_value >= 10) ? (dspbuf[6] = adc_value % 100 / 10) : (dspbuf[6] = 10);
            dspbuf[7] = adc_value % 10;
        }
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    if(++intr == 50)
    {
        intr = 0;
        adc_flag = 1;
    }
    display();
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