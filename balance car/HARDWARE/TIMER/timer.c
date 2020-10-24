#include "timer.h"
#include "includes.h"//ucos ʹ��	   
void MiniBalance_Motor_Init(void)
{
	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ��   
	GPIOB->CRH&=0X0000FFFF;   //PORTB12 13 14 15�������
	GPIOB->CRH|=0X22220000;   //PORTB12 13 14 15�������
}
void Encoder_Init_TIM2(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	RCC->APB1ENR|=1<<0;     //TIM2ʱ��ʹ��
	GPIOA->CRL&=0XFFFFFF00;//PA0 PA1
	GPIOA->CRL|=0X00000044;//��������
	TIM2->DIER|=1<<0;   //���������ж�				
	TIM2->DIER|=1<<6;   //���������ж�
	MY_NVIC_Init(1,3,TIM2_IRQn,2);
	TIM2->PSC = 0x0;//Ԥ��Ƶ��
	TIM2->ARR = ENCODER_TIM_PERIOD;//�趨�������Զ���װֵ 
	TIM2->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM2->CR1 &=~(3<<5);// ѡ�����ģʽ:���ض���ģʽ

	TIM2->CCMR1 |= 1<<0;//CC1S='01'CCMR1 //����CH1 CH2 CCMR1 CH3 CH4
	TIM2->CCMR1 |= 1<<8;//CC2S='01' 
	TIM2->CCER &= ~(1<<1);//����	 IC1FP1�����࣬IC1ӳ�䵽TI1
	TIM2->CCER &= ~(1<<5);//����	 IC2FP2�����࣬IC2ӳ�䵽TI2
	TIM2->CCMR1 |= 3<<4;// ���벶��1�˲��� һ�����ڲɼ�8��
	TIM2->SMCR |= 3<<0;//SMS='011' ������ģʽ ���е�������������غ��½�����Ч 4��Ƶģʽ
	TIM2->CR1 |= 0x01;//CEN=1��ʹ�ܶ�ʱ��
}
void Encoder_Init_TIM4(void)
{
	RCC->APB2ENR|=1<<3;     //ʹ��PORTBʱ��
	RCC->APB1ENR|=1<<2;     //TIM4ʱ��ʹ��
	GPIOB->CRL&=0X00FFFFFF; //PB6 PB7
	GPIOB->CRL|=0X44000000; //��������
	TIM4->DIER|=1<<0;   //���������ж�				
	TIM4->DIER|=1<<6;   //���������ж�
	MY_NVIC_Init(1,3,TIM4_IRQn,2);
	TIM4->PSC = 0x0;//Ԥ��Ƶ��
	TIM4->ARR = ENCODER_TIM_PERIOD;//�趨�������Զ���װֵ 
	TIM4->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM4->CR1 &=~(3<<5);// ѡ�����ģʽ:���ض���ģʽ
		
	TIM4->CCMR1 |= 1<<0;//CC1S='01' IC1FP1ӳ�䵽TI1 CCMR1����CH1��CH2
	TIM4->CCMR1 |= 1<<8;//CC2S='01' IC2FP2ӳ�䵽TI2
	TIM4->CCER &= ~(1<<1);//CC1P='0' IC1FP1�����࣬IC1FP1=TI1
	TIM4->CCER &= ~(1<<5);//CC2P='0' IC2FP2�����࣬IC2FP2=TI2
	TIM4->CCMR1 |= 3<<4;//IC1F='1000' ���벶��1�˲���
	TIM4->SMCR |= 3<<0;//SMS='011' ���е�������������غ��½�����Ч
	TIM4->CR1 |= 0x01;//CEN=1��ʹ�ܶ�ʱ��
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 					 
	MiniBalance_Motor_Init();  //��ʼ�������������IO
	RCC->APB2ENR|=1<<2;        //PORTAʱ��ʹ��  
	RCC->APB2ENR|=1<<11;       //ʹ��TIM1ʱ��       
	GPIOA->CRH&=0XFFFF0FF0;    //PORTA8 11�������
	GPIOA->CRH|=0X0000B00B;    //PORTA8 11�������
	TIM1->ARR=arr;             //�趨�������Զ���װֵ 
	TIM1->PSC=psc;             //Ԥ��Ƶ������Ƶ
	TIM1->CCMR2|=6<<12;        //CH4 PWM1ģʽ���ϼ���ʱ	
	TIM1->CCMR1|=6<<4;         //CH1 PWM1ģʽ���ϼ���ʱ	
	TIM1->CCMR2|=1<<11;        //CH4Ԥװ��ʹ��	ʹ��ͨ��1 TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 
	TIM1->CCMR1|=1<<3;         //CH1Ԥװ��ʹ��	  
	TIM1->CCER|=1<<12;         //CH4���ʹ��	����OC1�ź��������Ӧ���������
	TIM1->CCER|=1<<0;          //CH1���ʹ��	
	TIM1->BDTR |= 1<<15;       //TIM1����Ҫ��仰�������PWM
	TIM1->CR1=0x8000;          //ARPEʹ�� 
	TIM1->CR1|=0x01;          //ʹ�ܶ�ʱ��1 			
}
/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
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
�������ܣ�TIM4�жϷ�����
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM4_IRQHandler(void)
{ 	 	
	OSIntEnter(); 			  			    
	if(TIM4->SR&0X0001)//����ж�
	{    
		;
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	
	OSIntExit();  
}
/**************************************************************************
�������ܣ�TIM2�жϷ�����
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM2_IRQHandler(void)
{ 	
	OSIntEnter(); 	
	if(TIM2->SR&0X0001)//����ж�
	{  
		;
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
	OSIntExit();  
}