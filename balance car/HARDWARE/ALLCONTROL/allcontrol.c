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
int Encoder_Left,Encoder_Right;             //左右编码器的脉冲计数
float Angle_Balance,Gyro_Balance,Gyro_Turn;           //平衡倾角 平衡陀螺仪 转向陀螺仪 
static u8 Flag_Target;
float Acceleration_Z;                       //Z轴加速度计
/**************************************************************************
函数功能：所有的控制代码都在这里面
         5ms定时中断由MPU6050的INT引脚触发
         严格保证采样和数据处理的时间同步				 
**************************************************************************/
void EXTI9_5_IRQHandler(void) 
{ 
	OSIntEnter(); 
	 if(INT==0)		
	{ 
		EXTI->PR=1<<5; 
		Flag_Target=!Flag_Target;		//清除中断标志位//10ms控制一次，为了保证M法测速的时间基准，首先读取编码器数据
		if(Flag_Target==1)                                                  //5ms读取一次陀螺仪和加速度计的值，更高的采样频率可以改善卡尔曼滤波和互补滤波的效果
		{
			Encoder_Left=-Read_Encoder(2);                                       //===读取编码器的值
			Encoder_Right=Read_Encoder(4);                                      //===读取编码器的值
			Get_Angle();                                                        //===更新姿态	
			Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===平衡PID控制	
			Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);                  //===速度环PID控制	 记住，速度反馈是正反馈，就是小车快的时候要慢下来就需要再跑快一
			Turn_Pwm=turn(Encoder_Left,Encoder_Right,Gyro_Turn);            //===转向环PID控制   
			Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                            //===计算左轮电机最终PWM
			Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                            //===计算右轮电机最终PWM
			Xianfu_Pwm();                                                       //===PWM限幅
			if(Turn_Off(Angle_Balance)==0)                              //===如果不存在异常
			Set_Pwm(Moto1,Moto2);                                                //===更新姿态			                                               
		}
		Get_Angle();  
	}                                                      //===更新姿态	                                            //===赋值给PWM寄存器  
    OSIntExit(); 	  
 
} 
/**************************************************************************
函数功能：直立PD控制
入口参数：角度、角速度
返回  值：直立控制PWM
作    者：平衡小车之家
**************************************************************************/
int balance(float Angle,float Gyro)
{  
	float Bias;
	float Balance_Kp=300,Balance_Kd=1;
	int balance;
	Bias=Angle-ZHONGZHI;       //===求出平衡的角度中值 和机械相关
	balance=Balance_Kp*Bias+Gyro*Balance_Kd;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	return balance;
}

/**************************************************************************
函数功能：速度PI控制 修改前进后退速度，请修Target_Velocity，比如，改成60就比较慢了
入口参数：左轮编码器、右轮编码器
返回  值：速度控制PWM
作    者：平衡小车之家
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
     static float Velocity,Encoder_Least,Encoder,Movement;
	 static float Encoder_Integral,Target_Velocity;
	 float Velocity_Kp=80,Velocity_Ki=0.4;
 //=============遥控前进后退部分=======================//                                 
		Target_Velocity=55;  	
		if(1==Flag_Qian)    	
			Movement=-Target_Velocity;	         //===前进标志位置1 
		else if(1==Flag_Hou)	
			Movement=Target_Velocity;         //===后退标志位置1
	  else 
		Movement=0;
   //=============速度PI控制器=======================//	
		Encoder_Least =(encoder_left+encoder_right)-0;                    //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
		Encoder *= 0.8;		                                                //===一阶低通滤波器       
		Encoder += Encoder_Least*0.2;	                                    //===一阶低通滤波器    
		Encoder_Integral +=Encoder;                                       //===积分出位移 积分时间：10ms
		Encoder_Integral=Encoder_Integral-Movement;                       //===接收遥控器数据，控制前进后退
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===积分限幅
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===积分限幅	
		Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;                          //===速度控制	
	  	if(Turn_Off(Angle_Balance)==1)  
			Encoder_Integral=0;      //===电机关闭后清除积分
	  return Velocity;
}
int turn(int encoder_left,int encoder_right,float gyro)//转向控制
{
	 static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=88,Kp=35,Kd=0;     
	  //=============遥控左右旋转部分=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
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
		Turn_Target=Turn_Amplitude;    //===转向速度限幅
	  if(Turn_Target<-Turn_Amplitude) 
		  Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  
			Kd=0.5;        
		else Kd=0;   //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
  	//=============转向PD控制器=======================//
		Turn=-Turn_Target*Kp -gyro*Kd;                 //===结合Z轴陀螺仪进行PD控制
	  return Turn;
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
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
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=6900;    //===PWM满幅是7200 限制在6900
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：倾角和电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(float angle)
{
	u8 temp;
	if(angle<-40||angle>40)
	{	                                                 //===倾角大于40度关闭电机
		temp=1;                                            //===Flag_Stop置1关闭电机
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
函数功能：获取角度 三种算法经过我们的调校，都非常理想 
入口参数：获取角度的算法 1：DMP  2：卡尔曼 3：互补滤波
返回  值：无
**************************************************************************/
void Get_Angle(void)
{ 
			Read_DMP();                      //===读取加速度、角速度、倾角
			Angle_Balance=-Roll;             //===更新平衡倾角
			Gyro_Balance=-gyro[0];            //===更新平衡角速度
			Gyro_Turn=gyro[2];               //===更新转向角速度
		    Acceleration_Z=accel[2];         //===更新Z轴加速度计
}
/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
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

