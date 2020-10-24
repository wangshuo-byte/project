#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "stdio.h"	 
extern u8 USART_RX_BUF[50]; 
extern u8 wifiadrr[16];
extern u8 control[2];
extern u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; //遥控相关的变量
void uart_init(u32 pclk2,u32 bound);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
#endif	   
















