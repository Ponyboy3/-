/*******************************************************************************
* �ļ����ƣ�����ͨѶʵ��
* ʵ��Ŀ�ģ�1.����51��Ƭ�����ڹ���ģʽ����ؼĴ������÷���
*           2.�˽�51��Ƭ�������ʵļ��㷽��
*           3.���յ�Ƭ�����ڽ����жϷ���������Ʒ���
* ʵ�����ã�J3��������Ϊ	MM��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* ��������ͨ�����ڵ������ַ�������1~8����������Ķ�Ӧled1~led8��
* Ӳ��˵����IAP15F2K61S2@11.0592MHz
* ����˵����1.ͨ��USB����������PC�����ڼ����Ӳ���������в鿴���ں�
*           2.�򿪴��ڵ��Թ����������������ͨѶ�������趨Ϊ9600
*			      3.������J13����ΪMMģʽ
*			      4.����1-8�ַ�������8��LEDָʾ��״̬
* ���ڰ汾��2022/02/08
*******************************************************************************/

#include "STC15F2K60S2.h " //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"

#define BAUD	       9600  		//������
#define SYSTEMCLOCK  11059200L  //ϵͳʱ��Ƶ��

bit rxc = 0; //���ݽ��ձ�־λ
char rx = '1';//���յ�������
void uart_sendstring(unsigned char *str); //���ݷ���


//������
void main(void)
{
    SCON = 0x50;

    AUXR = 0x40;                //1T
    TMOD = 0x00;                
    TL1 = (65536 - (SYSTEMCLOCK / 4 / BAUD)); 
    TH1 = (65536 - (SYSTEMCLOCK / 4 / BAUD)) >> 8;
    TR1 = 1;
    ES = 1;
    EA = 1;

    while(1)
    {
        if(rxc == 1)
        {
            rxc = 0;
            switch(rx)
            {
            case '1':
            {
                XBYTE[0x8000] = 0xFE;
                uart_sendstring("1");
            }
            break;
            case '2':
            {
                XBYTE[0x8000] = 0xFD;
                uart_sendstring("2");
            }
            break;
            case '3':
            {
                XBYTE[0x8000] = 0xFB;
                uart_sendstring("3");
            }
            break;
            case '4':
            {
                XBYTE[0x8000] = 0xF7;
                uart_sendstring("4");
            }
            break;
            case '5':
            {
                XBYTE[0x8000] = 0xEF;
                uart_sendstring("5");
            }
            break;
            case '6':
            {
                XBYTE[0x8000] = 0xDF;
                uart_sendstring("6");
            }
            break;
            case '7':
            {
                XBYTE[0x8000] = 0xBF;
                uart_sendstring("7");
            }
            break;
            case '8':
            {
                XBYTE[0x8000] = 0x7F;
                uart_sendstring("8");
            }
            break;
            default:
                uart_sendstring("error\r\n");	//�����������ݷ���error
                break;
            }
            ES = 1;
        }
    }
}

//�����жϷ�����
void isr_uart(void) interrupt 4
{
    if(RI)
    {
        RI = 0;  //������ձ�־λ
        rx = SBUF;
        ES = 0;
        rxc = 1;
    }
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

