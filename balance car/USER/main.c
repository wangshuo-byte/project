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
#define START_TASK 3//任务优先级
#define START_SIZE 128//堆栈大小
OS_TCB StartTaskTCB;//任务控制块
CPU_STK START_TASK_STK[START_SIZE];//任务堆栈
void start_task(void *p_arg);

#define ALLPID_TASK 4//任务优先级
#define ALLPID_SIZE 512//堆栈大小
OS_TCB ALLPIDaskTCB;//任务控制块
CPU_STK ALLPID_TASK_STK[ALLPID_SIZE];//任务堆栈
void ALLPID_task(void *p_arg);

#define KEY_TASK 9//任务优先级
#define KEY_SIZE 256//堆栈大小
OS_TCB KEYTaskTCB;//任务控制块
CPU_STK KEY_TASK_STK[KEY_SIZE];//任务堆栈
void KEY_task(void *p_arg);

#define LED_TASK 10//任务优先级
#define LED_SIZE 64//堆栈大小
OS_TCB LEDTaskTCB;//任务控制块
CPU_STK LED_TASK_STK[LED_SIZE];//任务堆栈
void LED_task(void *p_arg);


extern float Angle_Balance;
extern float Balance_Kp,Balance_Kd,Velocity_Kp,Velocity_Ki;
float data1[10]={0,0,0,0,0,0,0,0,0,0};
char temp[20];
int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	NVIC_SetPriority(SysTick_IRQn,0x0);/*内核中断设置成最低优先级 行不通  设置成最高试试*/
	Stm32_Clock_Init(9);//将PLLCLK设置为系统时钟 PLL值为9 运行在72M下  
	delay_init(72); //工作在72M模式下
	MYDMA_Config(DMA1_Channel5,50);
	uart_init(72,115200);//115200
	MPU6050_initialize();			//初始化MPU6050
	while(DMP_Init())   //=====初始化DMP 
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
	OSInit(&err);//UCOSIII初始化
	OS_CRITICAL_ENTER();//进入临界代码保护区
	OSTaskCreate((OS_TCB*)&StartTaskTCB,//任务控制块
					(CPU_CHAR*)   &start_task,//任务名字
					(OS_TASK_PTR) start_task,//任务函数
                    (void  *)     0,//传递给任务函数的参数
                    (OS_PRIO)START_TASK,//任务优先级
                    (CPU_STK*)    &START_TASK_STK[0],//任务堆栈基地址
					(CPU_STK_SIZE)   START_SIZE/10,//任务堆栈深度限位
                    (CPU_STK_SIZE)   START_SIZE,//任务堆栈大小
                    (OS_MSG_QTY)0,//任务内部消息队列能够接收的最大消息数目，为0时禁止接受消息
                    (OS_TICK)   0,//使用时间片轮转调度长度 为.0时默认长度
                    (void*)         0,//用户补充的存储区
                    (OS_OPT)      OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,//任务选项，为了保险起见所有任务都保存浮点寄存器的值
                    (OS_ERR*)     &err//存放该函数错误时的返回值 如果出问题用串口打印出来调试时	
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
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
//使能时间片轮转调度功能,设置默认的时间片长度
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
	OS_CRITICAL_ENTER();	//进入临界区
//	OSSemCreate((OS_SEM* )&PID,  //创建一个信号量
//				(CPU_CHAR*)"PID",
//				(OS_SEM_CTR)0,
//				(OS_ERR*)&err);
	//创建LED任务
	
	OSTaskCreate((OS_TCB 	* )&LEDTaskTCB,	            //任务控制块
				 (CPU_CHAR	* )"LED TaskTCB", 			//任务名字		
                 (OS_TASK_PTR )LED_task, 				//任务函数
                 (void		* )0,						//传递给任务函数的任务参数
                 (OS_PRIO	  )LED_TASK,     			//任务优先级
                 (CPU_STK   * )&LED_TASK_STK[0],		//任务堆栈基地址
                 (CPU_STK_SIZE)LED_SIZE/10,				//任务堆栈深度限位
                 (CPU_STK_SIZE)LED_SIZE,				//任务堆栈大小
                 (OS_MSG_QTY  )0,						//任务内部消息队列能够接收的最大消息数目，为0时禁止接受消息
                 (OS_TICK	  )0,						//使用时间片轮转调度长度 为.0时默认长度
                 (void   	* )0,						//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR|OS_OPT_TASK_SAVE_FP,
                 (OS_ERR 	* )&err);
	//创建KEY任务			 
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
				 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}
void KEY_task(void *p_arg)//9级
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
	  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
	}
}
void LED_task(void *p_arg)//10级
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{	
		LED0=!LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);//延时1s
	}
}

void ALLPID_task(void *p_arg)//4级
{

	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
//		OSSemPend(&PID,5,OS_OPT_PEND_NON_BLOCKING,0,&err);         //请求信号量PID，延时0请求不到一直请求
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
	    OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
	}
}


