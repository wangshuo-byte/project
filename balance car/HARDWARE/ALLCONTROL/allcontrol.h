#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#include "includes.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
#define ZHONGZHI 0 
#define DIFFERENCE 100

//extern float Balance_Kp,Balance_Kd,Velocity_Kp,Velocity_Ki;
void EXTI9_5_IRQHandler(void);
extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
extern float Balance_Kp,Balance_Kd,Velocity_Kp,Velocity_Ki;
int balance(float angle,float gyro);
int velocity(int encoder_left,int encoder_right);
int turn(int encoder_left,int encoder_right,float gyro);
void Set_Pwm(int moto1,int moto2);
void Key(void);
void Xianfu_Pwm(void);
int myabs(int a);
int Pick_Up(float Acceleration,float Angle,int encoder_left,int encoder_right);
int Put_Down(float Angle,int encoder_left,int encoder_right);
void allPID(void);
void Get_Angle(void);
u8 Turn_Off(float angle);
#endif
