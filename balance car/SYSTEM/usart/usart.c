#include "usart.h"	  
#include "includes.h"//ucos 使用	    								 
#include "led.h"
#include "data_upper.h"

//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
u8 USART_RX_BUF[50];     //接收缓冲,最大USART_REC_LEN个字节.
u16 Usart1_Rec_Cnt=0;             //本帧数据长度	
u8 wifiadrr[16]={0};
u8 control[2]={0};
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; //遥控相关的变量
void uart_init(u32 pclk2,u32 bound)
{  	 
		 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置 PA10 RX
	GPIOA->CRH|=0X000008B0;//IO状态设置 PA9 TX
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
	 
//	//使能接收中断 
	USART1->CR1|=1<<8;
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	
	USART1->CR3=1<<6; //使能DMA接收	
	USART1->CR1|=1<<4;    	//使能串口1的空闲中断
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
	
	USART1->CR1|=1<<13;  	//串口使能
}
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u16 cndtr)
{
	RCC->AHBENR|=1<<0;			//开启DMA1时钟

	DMA_CHx->CPAR=(u32)&USART1->DR;//DMA1 外设地址 
	DMA_CHx->CMAR=(u32)USART_RX_BUF;//DMA1,存储器地址
	DMA_CHx->CNDTR=50;    	//DMA1,传输数据量
	DMA_CHx->CCR=0X00000000;	//复位
	
	
	DMA_CHx->CCR|=0<<4;  		//从外设读
	DMA_CHx->CCR|=0<<5;  		//普通模式
	DMA_CHx->CCR|=0<<6; 		//外设地址非增量模式
	DMA_CHx->CCR|=1<<7; 	 	//存储器增量模式
	DMA_CHx->CCR|=0<<8; 	 	//外设数据宽度为8位
	DMA_CHx->CCR|=0<<10; 		//存储器数据宽度8位
	DMA_CHx->CCR|=1<<12; 		//中等优先级
	DMA_CHx->CCR|=0<<14; 		//非存储器到存储器模式	

	MYDMA_Enable(DMA1_Channel5);
} 
//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
	DMA_CHx->CNDTR=50; //DMA1,传输数据量 
	DMA_CHx->CCR|=1<<0;          //开启DMA传输
}	  
void USART1_IRQHandler(void)
{
	u8 res;
	OSIntEnter();
	if(USART1->SR&(1<<4))	//空闲接收到数据
	{	
		res=USART1->SR;/*先读SR 在读DR 清除空闲标志位*/
		res=USART1->DR;
		if(DMA1->ISR&(1<<17))//等待通道5传输完成
		{
			DMA1->IFCR|=1<<17;//清除通道5传输完成标志
		}
		if(USART_RX_BUF[0]>46&&USART_RX_BUF[0]<57)
		{
			memcpy(wifiadrr,USART_RX_BUF,sizeof(char)*16);
			memset(USART_RX_BUF,0,(u32)50);
		}
		if(USART_RX_BUF[0]>65)
		{
			memcpy(control,USART_RX_BUF,sizeof(char)*1);
			memset(USART_RX_BUF,0,(u32)50);
			if(control[0]=='S')	
				Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////刹车
			else if(control[0]=='B')	
				Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////前
			else if(control[0]=='G')	
				Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////后
			else if(control[0]=='L')	
				Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;//////////////左
			else if(control[0]=='R')	   
				Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;////////////////右
		}
		MYDMA_Enable(DMA1_Channel5);
	}
	OSIntExit();										 

} 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
//void uart_init(u32 pclk2,u32 bound)
//{  	 
//		 
//	float temp;
//	u16 mantissa;
//	u16 fraction;	   
//	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
//	mantissa=temp;				 //得到整数部分
//	fraction=(temp-mantissa)*16; //得到小数部分	 
//    mantissa<<=4;
//	mantissa+=fraction; 
//	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
//	RCC->APB2ENR|=1<<14;  //使能串口时钟 
//	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
//	GPIOA->CRH|=0X000008B0;//IO状态设置 
//	RCC->APB2RSTR|=1<<14;   //复位串口1
//	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
//	//波特率设置
// 	USART1->BRR=mantissa; // 波特率设置	 
//	USART1->CR1|=0X200C;  //1位停止,无校验位.
//	
////	//使能接收中断 
//	USART1->CR1|=1<<8;
//	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
//	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
//}
//void USART1_IRQHandler(void)
//{
//	
//	u8 res;
//	OSIntEnter();
//	if(USART1->SR&(1<<5))	//接收到数据
//	{		
//		res=USART1->DR;
////		ANO_DT_Data_Receive_Prepare(res);
//		while(!(USART1->SR & (1 << 7)));//数据已经被转移到移位寄存器
//		USART1->SR&=(0<<5);				//手动清零
//	}
//	OSIntExit();
//} 


