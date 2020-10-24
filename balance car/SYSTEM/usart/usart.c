#include "usart.h"	  
#include "includes.h"//ucos ʹ��	    								 
#include "led.h"
#include "data_upper.h"

//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
u8 USART_RX_BUF[50];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 Usart1_Rec_Cnt=0;             //��֡���ݳ���	
u8 wifiadrr[16]={0};
u8 control[2]={0};
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; //ң����صı���
void uart_init(u32 pclk2,u32 bound)
{  	 
		 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬���� PA10 RX
	GPIOA->CRH|=0X000008B0;//IO״̬���� PA9 TX
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
	 
//	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<8;
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	
	USART1->CR3=1<<6; //ʹ��DMA����	
	USART1->CR1|=1<<4;    	//ʹ�ܴ���1�Ŀ����ж�
	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ� 
	
	USART1->CR1|=1<<13;  	//����ʹ��
}
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u16 cndtr)
{
	RCC->AHBENR|=1<<0;			//����DMA1ʱ��

	DMA_CHx->CPAR=(u32)&USART1->DR;//DMA1 �����ַ 
	DMA_CHx->CMAR=(u32)USART_RX_BUF;//DMA1,�洢����ַ
	DMA_CHx->CNDTR=50;    	//DMA1,����������
	DMA_CHx->CCR=0X00000000;	//��λ
	
	
	DMA_CHx->CCR|=0<<4;  		//�������
	DMA_CHx->CCR|=0<<5;  		//��ͨģʽ
	DMA_CHx->CCR|=0<<6; 		//�����ַ������ģʽ
	DMA_CHx->CCR|=1<<7; 	 	//�洢������ģʽ
	DMA_CHx->CCR|=0<<8; 	 	//�������ݿ��Ϊ8λ
	DMA_CHx->CCR|=0<<10; 		//�洢�����ݿ��8λ
	DMA_CHx->CCR|=1<<12; 		//�е����ȼ�
	DMA_CHx->CCR|=0<<14; 		//�Ǵ洢�����洢��ģʽ	

	MYDMA_Enable(DMA1_Channel5);
} 
//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
	DMA_CHx->CCR&=~(1<<0);       //�ر�DMA���� 
	DMA_CHx->CNDTR=50; //DMA1,���������� 
	DMA_CHx->CCR|=1<<0;          //����DMA����
}	  
void USART1_IRQHandler(void)
{
	u8 res;
	OSIntEnter();
	if(USART1->SR&(1<<4))	//���н��յ�����
	{	
		res=USART1->SR;/*�ȶ�SR �ڶ�DR ������б�־λ*/
		res=USART1->DR;
		if(DMA1->ISR&(1<<17))//�ȴ�ͨ��5�������
		{
			DMA1->IFCR|=1<<17;//���ͨ��5������ɱ�־
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
				Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
			else if(control[0]=='B')	
				Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ǰ
			else if(control[0]=='G')	
				Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////��
			else if(control[0]=='L')	
				Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;//////////////��
			else if(control[0]=='R')	   
				Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;////////////////��
		}
		MYDMA_Enable(DMA1_Channel5);
	}
	OSIntExit();										 

} 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
//void uart_init(u32 pclk2,u32 bound)
//{  	 
//		 
//	float temp;
//	u16 mantissa;
//	u16 fraction;	   
//	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
//	mantissa=temp;				 //�õ���������
//	fraction=(temp-mantissa)*16; //�õ�С������	 
//    mantissa<<=4;
//	mantissa+=fraction; 
//	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
//	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
//	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
//	GPIOA->CRH|=0X000008B0;//IO״̬���� 
//	RCC->APB2RSTR|=1<<14;   //��λ����1
//	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
//	//����������
// 	USART1->BRR=mantissa; // ����������	 
//	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
//	
////	//ʹ�ܽ����ж� 
//	USART1->CR1|=1<<8;
//	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
//	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ� 
//}
//void USART1_IRQHandler(void)
//{
//	
//	u8 res;
//	OSIntEnter();
//	if(USART1->SR&(1<<5))	//���յ�����
//	{		
//		res=USART1->DR;
////		ANO_DT_Data_Receive_Prepare(res);
//		while(!(USART1->SR & (1 << 7)));//�����Ѿ���ת�Ƶ���λ�Ĵ���
//		USART1->SR&=(0<<5);				//�ֶ�����
//	}
//	OSIntExit();
//} 


