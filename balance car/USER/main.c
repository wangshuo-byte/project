#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "includes.h"
#include "oled.h"
#include "timer.h"
#include "mpuiic.h"
#include "data_upper.h"
#include "data_flash.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "data_flash.h"
#include "filter.h"
#include "allcontrol.h"
#include "exti.h"
#include "stm32f10x.h"  
#define START_TASK 3//�������ȼ�
#define START_SIZE 128//��ջ��С
OS_TCB StartTaskTCB;//������ƿ�
CPU_STK START_TASK_STK[START_SIZE];//�����ջ
void start_task(void *p_arg);

#define ALLPID_TASK 4//�������ȼ�
#define ALLPID_SIZE 512//��ջ��С
OS_TCB ALLPIDaskTCB;//������ƿ�
CPU_STK ALLPID_TASK_STK[ALLPID_SIZE];//�����ջ
void ALLPID_task(void *p_arg);

#define KEY_TASK 9//�������ȼ�
#define KEY_SIZE 256//��ջ��С
OS_TCB KEYTaskTCB;//������ƿ�
CPU_STK KEY_TASK_STK[KEY_SIZE];//�����ջ
void KEY_task(void *p_arg);

#define LED_TASK 10//�������ȼ�
#define LED_SIZE 64//��ջ��С
OS_TCB LEDTaskTCB;//������ƿ�
CPU_STK LED_TASK_STK[LED_SIZE];//�����ջ
void LED_task(void *p_arg);


extern float Angle_Balance;
extern float Balance_Kp,Balance_Kd,Velocity_Kp,Velocity_Ki;
float data1[10]={0,0,0,0,0,0,0,0,0,0};
char temp[20];
int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	NVIC_SetPriority(SysTick_IRQn,0x0);/*�ں��ж����ó�������ȼ� �в�ͨ  ���ó��������*/
	Stm32_Clock_Init(9);//��PLLCLK����Ϊϵͳʱ�� PLLֵΪ9 ������72M��  
	delay_init(72); //������72Mģʽ��
	MYDMA_Config(DMA1_Channel5,50);
	uart_init(72,115200);//115200
	MPU6050_initialize();			//��ʼ��MPU6050
	while(DMP_Init())   //=====��ʼ��DMP 
	{
	  OLED_ShowNum(0,0,0,1,16);
	}  
//	flash_read(0X0800C000,(u16*)&data1[0],3);
//	flash_read(0X0800D000,(u16*)&data1[1],3);
//	flash_read(0X0800E000,(u16*)&data1[2],3);
//	flash_read(0X0800EC00,(u16*)&data1[3],3);
//	Velocity_Kp=data1[0];
//	Velocity_Ki=data1[1];
//	Balance_Kp=data1[2];
//	Balance_Kd=data1[3];
	LED_Init();//PA12 LED2
	key_init();
	OLED_Init();
	OLED_Clear();
    IIC_Init();   
//	OLED_ShowNum(0,6,Velocity_Kp,3,16);
//	OLED_ShowNum(25,6,Velocity_Ki,3,16);
//	OLED_ShowNum(50,6,Balance_Kp,3,16);
//	OLED_ShowNum(75,6,Balance_Kd,3,16);
	//while(mpu_dmp_init());
	Encoder_Init_TIM2();
	Encoder_Init_TIM4();
	MiniBalance_Motor_Init();
	MiniBalance_PWM_Init(7199,0);/*PWMA=1000;PWMB=1000;AIN2=0;AIN1=0;BIN1=0;BIN2=0;*/
	EXTI_Init();  
	OSInit(&err);//UCOSIII��ʼ��
	OS_CRITICAL_ENTER();//�����ٽ���뱣����
	OSTaskCreate((OS_TCB*)&StartTaskTCB,//������ƿ�
					(CPU_CHAR*)   &start_task,//��������
					(OS_TASK_PTR) start_task,//������
                    (void  *)     0,//���ݸ��������Ĳ���
                    (OS_PRIO)START_TASK,//�������ȼ�
                    (CPU_STK*)    &START_TASK_STK[0],//�����ջ����ַ
					(CPU_STK_SIZE)   START_SIZE/10,//�����ջ�����λ
                    (CPU_STK_SIZE)   START_SIZE,//�����ջ��С
                    (OS_MSG_QTY)0,//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ��Ϊ0ʱ��ֹ������Ϣ
                    (OS_TICK)   0,//ʹ��ʱ��Ƭ��ת���ȳ��� Ϊ.0ʱĬ�ϳ���
                    (void*)         0,//�û�����Ĵ洢��
                    (OS_OPT)      OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,//����ѡ�Ϊ�˱�������������񶼱��渡��Ĵ�����ֵ
                    (OS_ERR*)     &err//��Ÿú�������ʱ�ķ���ֵ ����������ô��ڴ�ӡ��������ʱ	
							);
	OS_CRITICAL_EXIT();
	OSStart(&err);
}
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
//ʹ��ʱ��Ƭ��ת���ȹ���,����Ĭ�ϵ�ʱ��Ƭ����
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
	OS_CRITICAL_ENTER();	//�����ٽ���
//	OSSemCreate((OS_SEM* )&PID,  //����һ���ź���
//				(CPU_CHAR*)"PID",
//				(OS_SEM_CTR)0,
//				(OS_ERR*)&err);
	//����LED����
	
	OSTaskCreate((OS_TCB 	* )&LEDTaskTCB,	            //������ƿ�
				 (CPU_CHAR	* )"LED TaskTCB", 			//��������		
                 (OS_TASK_PTR )LED_task, 				//������
                 (void		* )0,						//���ݸ����������������
                 (OS_PRIO	  )LED_TASK,     			//�������ȼ�
                 (CPU_STK   * )&LED_TASK_STK[0],		//�����ջ����ַ
                 (CPU_STK_SIZE)LED_SIZE/10,				//�����ջ�����λ
                 (CPU_STK_SIZE)LED_SIZE,				//�����ջ��С
                 (OS_MSG_QTY  )0,						//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ��Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,						//ʹ��ʱ��Ƭ��ת���ȳ��� Ϊ.0ʱĬ�ϳ���
                 (void   	* )0,						//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP,
                 (OS_ERR 	* )&err);
	//����KEY����			 
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		
				 (CPU_CHAR	* )"KEY TaskTCB", 		
                 (OS_TASK_PTR )KEY_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEY_TASK,     
                 (CPU_STK   * )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_SIZE/10,	
                 (CPU_STK_SIZE)KEY_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP,
                 (OS_ERR 	* )&err);
	 
	OSTaskCreate((OS_TCB 	* )&ALLPIDaskTCB,		
				 (CPU_CHAR	* )"ALLPID askTCB", 		
                 (OS_TASK_PTR )ALLPID_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ALLPID_TASK,     
                 (CPU_STK   * )&ALLPID_TASK_STK[0],	
                 (CPU_STK_SIZE)ALLPID_SIZE/10,	
                 (CPU_STK_SIZE)ALLPID_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP,
                 (OS_ERR 	* )&err);	
				 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}
void KEY_task(void *p_arg)//9��
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
	  key_scan();
	  if(Trg & 0x0040)//SW2
	  {
//		data1[0]=2;
//		STMFLASH_Write(0X0800C000,(u16*)&data1[0],1);
		//LED0=!LED0; 
		 OLED_ShowNum(0,0,0,3,16);
	  }
	  if(Trg & 0x0004)//SW3
	  {
//		data1 [0]=3;
//		STMFLASH_Write(0X0800C000,(u16*)&data1[0],1);
		//LED0=!LED0; 
		OLED_ShowNum(0,0,1,3,16);		  
	  }
	  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
}
void LED_task(void *p_arg)//10��
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{	
		LED0=!LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);//��ʱ1s
	}
}

void ALLPID_task(void *p_arg)//4��
{

	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
//		OSSemPend(&PID,5,OS_OPT_PEND_NON_BLOCKING,0,&err);         //�����ź���PID����ʱ0���󲻵�һֱ����
//	    allPID();
		OLED_ShowCHinese(0,0,10);
		OLED_ShowCHinese(16,0,11);
		OLED_ShowCHinese(32,0,12);	
		OLED_ShowString(48,0,"17",16);
		OLED_ShowCHinese(64,0,8);
		OLED_ShowCHinese(80,0,9);
		OLED_ShowCHinese(96,0,6);
		OLED_ShowCHinese(112,0,7);
		
		OLED_ShowString(16,2,"WIFI",16);
		OLED_ShowCHinese(48,2,2);
		OLED_ShowCHinese(64,2,3);
		OLED_ShowCHinese(80,2,4);
		OLED_ShowCHinese(96,2,5);
		OLED_ShowString(0,4,wifiadrr,16);

		OLED_ShowString(0,6,control,16);		
		if(Angle_Balance<0)
		{
		 OLED_ShowNum(80,6,Angle_Balance+360,4,16);
		}
		else
		 OLED_ShowNum(80,6,Angle_Balance,4,16);
	    OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
	}
}


