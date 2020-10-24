#include "timer.h"
#include "includes.h"//ucos 使用	   
void MiniBalance_Motor_Init(void)
{
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能   
	GPIOB->CRH&=0X0000FFFF;   //PORTB12 13 14 15推挽输出
	GPIOB->CRH|=0X22220000;   //PORTB12 13 14 15推挽输出
}
void Encoder_Init_TIM2(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	RCC->APB1ENR|=1<<0;     //TIM2时钟使能
	GPIOA->CRL&=0XFFFFFF00;//PA0 PA1
	GPIOA->CRL|=0X00000044;//浮空输入
	TIM2->DIER|=1<<0;   //允许更新中断				
	TIM2->DIER|=1<<6;   //允许触发中断
	MY_NVIC_Init(1,3,TIM2_IRQn,2);
	TIM2->PSC = 0x0;//预分频器
	TIM2->ARR = ENCODER_TIM_PERIOD;//设定计数器自动重装值 
	TIM2->CR1 &=~(3<<8);// 选择时钟分频：不分频
	TIM2->CR1 &=~(3<<5);// 选择计数模式:边沿对齐模式

	TIM2->CCMR1 |= 1<<0;//CC1S='01'CCMR1 //控制CH1 CH2 CCMR1 CH3 CH4
	TIM2->CCMR1 |= 1<<8;//CC2S='01' 
	TIM2->CCER &= ~(1<<1);//输入	 IC1FP1不反相，IC1映射到TI1
	TIM2->CCER &= ~(1<<5);//输入	 IC2FP2不反相，IC2映射到TI2
	TIM2->CCMR1 |= 3<<4;// 输入捕获1滤波器 一个周期采集8次
	TIM2->SMCR |= 3<<0;//SMS='011' 编码器模式 所有的输入均在上升沿和下降沿有效 4倍频模式
	TIM2->CR1 |= 0x01;//CEN=1，使能定时器
}
void Encoder_Init_TIM4(void)
{
	RCC->APB2ENR|=1<<3;     //使能PORTB时钟
	RCC->APB1ENR|=1<<2;     //TIM4时钟使能
	GPIOB->CRL&=0X00FFFFFF; //PB6 PB7
	GPIOB->CRL|=0X44000000; //浮空输入
	TIM4->DIER|=1<<0;   //允许更新中断				
	TIM4->DIER|=1<<6;   //允许触发中断
	MY_NVIC_Init(1,3,TIM4_IRQn,2);
	TIM4->PSC = 0x0;//预分频器
	TIM4->ARR = ENCODER_TIM_PERIOD;//设定计数器自动重装值 
	TIM4->CR1 &=~(3<<8);// 选择时钟分频：不分频
	TIM4->CR1 &=~(3<<5);// 选择计数模式:边沿对齐模式
		
	TIM4->CCMR1 |= 1<<0;//CC1S='01' IC1FP1映射到TI1 CCMR1控制CH1和CH2
	TIM4->CCMR1 |= 1<<8;//CC2S='01' IC2FP2映射到TI2
	TIM4->CCER &= ~(1<<1);//CC1P='0' IC1FP1不反相，IC1FP1=TI1
	TIM4->CCER &= ~(1<<5);//CC2P='0' IC2FP2不反相，IC2FP2=TI2
	TIM4->CCMR1 |= 3<<4;//IC1F='1000' 输入捕获1滤波器
	TIM4->SMCR |= 3<<0;//SMS='011' 所有的输入均在上升沿和下降沿有效
	TIM4->CR1 |= 0x01;//CEN=1，使能定时器
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 					 
	MiniBalance_Motor_Init();  //初始化电机控制所需IO
	RCC->APB2ENR|=1<<2;        //PORTA时钟使能  
	RCC->APB2ENR|=1<<11;       //使能TIM1时钟       
	GPIOA->CRH&=0XFFFF0FF0;    //PORTA8 11复用输出
	GPIOA->CRH|=0X0000B00B;    //PORTA8 11复用输出
	TIM1->ARR=arr;             //设定计数器自动重装值 
	TIM1->PSC=psc;             //预分频器不分频
	TIM1->CCMR2|=6<<12;        //CH4 PWM1模式向上计数时	
	TIM1->CCMR1|=6<<4;         //CH1 PWM1模式向上计数时	
	TIM1->CCMR2|=1<<11;        //CH4预装载使能	使能通道1 TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	TIM1->CCMR1|=1<<3;         //CH1预装载使能	  
	TIM1->CCER|=1<<12;         //CH4输出使能	开启OC1信号输出到对应的输出引脚
	TIM1->CCER|=1<<0;          //CH1输出使能	
	TIM1->BDTR |= 1<<15;       //TIM1必须要这句话才能输出PWM
	TIM1->CR1=0x8000;          //ARPE使能 
	TIM1->CR1|=0x01;          //使能定时器1 			
}
/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
   int Encoder_TIM;    
   switch(TIMX)
	{
	   case 2:  Encoder_TIM= (short)TIM2 -> CNT;  TIM2 -> CNT=0;break;
	   case 3:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;	
	   case 4:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;	
	   default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
} 
/**************************************************************************
函数功能：TIM4中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
void TIM4_IRQHandler(void)
{ 	 	
	OSIntEnter(); 			  			    
	if(TIM4->SR&0X0001)//溢出中断
	{    
		;
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	
	OSIntExit();  
}
/**************************************************************************
函数功能：TIM2中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
void TIM2_IRQHandler(void)
{ 	
	OSIntEnter(); 	
	if(TIM2->SR&0X0001)//溢出中断
	{  
		;
	}				   
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
	OSIntExit();  
}
