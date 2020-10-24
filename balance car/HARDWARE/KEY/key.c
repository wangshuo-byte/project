#include "key.h"
unsigned char Trg;//单次触发
unsigned char Cont;//持续触发
void key_init()
{
	RCC->APB2ENR|=1<<2;//使能PORTA时钟
	GPIOA->CRL&=0XF0FFF0FF;//	  
	GPIOA->CRL|=0X08000800;
	GPIOA->ODR|=0x0044;
}
void key_scan()
{
	unsigned char ReadData =KEYPORT^0xffff; //读取键值^异或相同为0不同为1
    Trg = ReadData & (ReadData ^ Cont);//   
    Cont = ReadData;    
}

