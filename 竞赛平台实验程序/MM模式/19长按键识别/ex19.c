/*******************************************************************************
* �ļ����ƣ�������ʶ��ʵ��
* ʵ��Ŀ�ģ�1.���յ�Ƭ��IO�ڲ����Ļ�������
*           2.���հ���ɨ�輰�����ʱ���������Ļ���ԭ��
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �������󣺳���S4�������ر�����LEDָʾ�ƣ��̰�S4������ָʾ��λ��
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע������������Ŀ�����ڽ��ܳ�������ʶ�𷽷�����ʵ����Ŀ�У�Ӧ���Ǹ�Ϊ��׼��
			����ɨ�����ںͳ�����ʶ����������ʹ�û��ڶ�ʱ���ķ����԰�������ɨ��
* ���ڰ汾��2022/02/08
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"

sbit P33 = P3 ^ 3;
unsigned char read_key(void);

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
    unsigned char temp;
    unsigned char mov = 0;
    unsigned int scan_period = 0;
    cls_buzz();
    cls_led();

    while(1)
    {
        if(++scan_period == 1000)
        {
            scan_period = 0;
            temp = read_key();
            if(temp != 0xff)
            {
                if(temp == 1)
                {
                    XBYTE[0x8000] = ~(1 << mov);
                    if(++mov == 8)
                    {
                        mov = 0;
                    }
                }
                else
                {
                    XBYTE[0x8000] = 0xff;
                }


            }
        }

    }
}

//����ɨ�躯��
unsigned char read_key(void)
{
    static unsigned int press_counter = 0;	//���̰���������
    static unsigned char status = 0;
    unsigned char key_val = 0xFF;

    switch(status)
    {
    case 0:
        if(P33 == 0)
        {
            status = 1;
        }
        break;
    case 1:
        if(P33 == 0)
        {
            if(press_counter < 400)
            {
                press_counter++;
            }
        }
        else
        {
            if(press_counter > 20)
            {
                status = 2;
            }
            else
            {
                status = 0;	//����
                press_counter = 0;
            }
        }
        break;
    case 2:
        if(P33 == 1)
        {
            if(press_counter < 400)
            {
                key_val = 1;	//�̰���
            }
            else
            {
                key_val = 2	;	//������
            }
            status = 0;
            press_counter = 0;
        }
        break;
    }

    return key_val;
}

