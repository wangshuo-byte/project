#include "key.h"
unsigned char Trg;//���δ���
unsigned char Cont;//��������
void key_init()
{
	RCC->APB2ENR|=1<<2;//ʹ��PORTAʱ��
	GPIOA->CRL&=0XF0FFF0FF;//	  
	GPIOA->CRL|=0X08000800;
	GPIOA->ODR|=0x0044;
}
void key_scan()
{
	unsigned char ReadData =KEYPORT^0xffff; //��ȡ��ֵ^�����ͬΪ0��ͬΪ1
    Trg = ReadData & (ReadData ^ Cont);//   
    Cont = ReadData;    
}

