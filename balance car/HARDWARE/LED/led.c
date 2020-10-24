#include "led.h"
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2; //PORTA
	GPIOA->CRH&=0XFFF0FFFF; 
	GPIOA->CRH|=0X00030000;//PA12
	GPIOA->ODR|=1<<12; //PA12
}






