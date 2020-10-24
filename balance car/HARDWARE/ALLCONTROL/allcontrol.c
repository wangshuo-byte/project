#include "allcontrol.h"	
#include "filter.h"
#include "timer.h"
#include "delay.h"
#include "mpu6050.h"
#include "mpuiic.h"
#include "data_upper.h"
#include "exti.h"
#include "stm32f10x.h"
#include "usart.h"
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
int Moto1,Moto2;  
int Encoder_Left,Encoder_Right;             //���ұ��������������
float Angle_Balance,Gyro_Balance,Gyro_Turn;           //ƽ����� ƽ�������� ת�������� 
static u8 Flag_Target;
float Acceleration_Z;                       //Z����ٶȼ�
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
void EXTI9_5_IRQHandler(void) 
{ 
	OSIntEnter(); 
	 if(INT==0)		
	{ 
		EXTI->PR=1<<5; 
		Flag_Target=!Flag_Target;		//����жϱ�־λ//10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
		if(Flag_Target==1)                                                  //5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��
		{
			Encoder_Left=-Read_Encoder(2);                                       //===��ȡ��������ֵ
			Encoder_Right=Read_Encoder(4);                                      //===��ȡ��������ֵ
			Get_Angle();                                                        //===������̬	
			Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===ƽ��PID����	
			Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);                  //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ
			Turn_Pwm=turn(Encoder_Left,Encoder_Right,Gyro_Turn);            //===ת��PID����   
			Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                            //===�������ֵ������PWM
			Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                            //===�������ֵ������PWM
			Xianfu_Pwm();                                                       //===PWM�޷�
			if(Turn_Off(Angle_Balance)==0)                              //===����������쳣
			Set_Pwm(Moto1,Moto2);                                                //===������̬			                                               
		}
		Get_Angle();  
	}                                                      //===������̬	                                            //===��ֵ��PWM�Ĵ���  
    OSIntExit(); 	  
 
} 
/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
	float Bias;
	float Balance_Kp=300,Balance_Kd=1;
	int balance;
	Bias=Angle-ZHONGZHI;       //===���ƽ��ĽǶ���ֵ �ͻ�е���
	balance=Balance_Kp*Bias+Gyro*Balance_Kd;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ�����Target_Velocity�����磬�ĳ�60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
     static float Velocity,Encoder_Least,Encoder,Movement;
	 static float Encoder_Integral,Target_Velocity;
	 float Velocity_Kp=80,Velocity_Ki=0.4;
 //=============ң��ǰ�����˲���=======================//                                 
		Target_Velocity=55;  	
		if(1==Flag_Qian)    	
			Movement=-Target_Velocity;	         //===ǰ����־λ��1 
		else if(1==Flag_Hou)	
			Movement=Target_Velocity;         //===���˱�־λ��1
	  else 
		Movement=0;
   //=============�ٶ�PI������=======================//	
		Encoder_Least =(encoder_left+encoder_right)-0;                    //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
		Encoder *= 0.8;		                                                //===һ�׵�ͨ�˲���       
		Encoder += Encoder_Least*0.2;	                                    //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� ����ʱ�䣺10ms
		Encoder_Integral=Encoder_Integral-Movement;                       //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===�����޷�
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===�����޷�	
		Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;                          //===�ٶȿ���	
	  	if(Turn_Off(Angle_Balance)==1)  
			Encoder_Integral=0;      //===����رպ��������
	  return Velocity;
}
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
	 static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=88,Kp=35,Kd=0;     
	  //=============ң��������ת����=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
		{
			if(++Turn_Count==1)
			Encoder_temp=myabs(encoder_left+encoder_right);
			Turn_Convert=50/Encoder_temp;
			if(Turn_Convert<0.6)Turn_Convert=0.6;
			if(Turn_Convert>3)Turn_Convert=3;
		}	
	  else
		{
			Turn_Convert=0.9;
			Turn_Count=0;
			Encoder_temp=0;
		}			
		if(1==Flag_Left)	         
			Turn_Target-=Turn_Convert;
		else if(1==Flag_Right)	     
			Turn_Target+=Turn_Convert; 
		else 
			Turn_Target=0;
    if(Turn_Target>Turn_Amplitude)  
		Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	  if(Turn_Target<-Turn_Amplitude) 
		  Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  
			Kd=0.5;        
		else Kd=0;   //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��
  	//=============ת��PD������=======================//
		Turn=-Turn_Target*Kp -gyro*Kd;                 //===���Z�������ǽ���PD����
	  return Turn;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
    	if(moto1>0)			
			AIN2=0,AIN1=1;
			else 	          
			AIN2=1,AIN1=0;
			PWMA=myabs(moto1);
		  if(moto2>0)	
			BIN1=0,BIN2=1;
			else        
			BIN1=1,BIN2=0;
			PWMB=myabs(moto2);	
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=6900;    //===PWM������7200 ������6900
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle)
{
	u8 temp;
	if(angle<-40||angle>40)
	{	                                                 //===��Ǵ���40�ȹرյ��
		temp=1;                                            //===Flag_Stop��1�رյ��
		AIN1=0;                                            
		AIN2=0;
		BIN1=0;
		BIN2=0;
    }
	else
      temp=0;
      return temp;			
}

	
/**************************************************************************
�������ܣ���ȡ�Ƕ� �����㷨�������ǵĵ�У�����ǳ����� 
��ڲ�������ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(void)
{ 
			Read_DMP();                      //===��ȡ���ٶȡ����ٶȡ����
			Angle_Balance=-Roll;             //===����ƽ�����
			Gyro_Balance=-gyro[0];            //===����ƽ����ٶ�
			Gyro_Turn=gyro[2];               //===����ת����ٶ�
		    Acceleration_Z=accel[2];         //===����Z����ٶȼ�
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
//void control_function(u8* flag)
//{
//	
//}

