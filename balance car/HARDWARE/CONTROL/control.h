#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
//#define pwmout_1    GPIO_SetBits(GPIOB, GPIO_Pin_5)    //停止加热
//#define pwmout_0    GPIO_ResetBits(GPIOB, GPIO_Pin_5)  //加热
extern u8 _kp;
extern u8 _ti;
extern u8 _di;
typedef struct
{
float Sv;//用户设定值
float	temp;//实际值

float Kp;//比例调节系数
float T;  //PID计算周期--采样周期
float Ti;//积分
float Td; //微分
float ki;
float kd;
float Ek;  //本次偏差
float Ek_1;//上次偏差
float SEk; //历史偏差之和
	
float Iout;//积分输出
float Pout;//比例输出
float Dout;//微分输出
	
float OUT0;//补误差
float kp0;
float ki0;
float di0;
float OUT;//补误差
float P;
float I;
float D;
float PW;
float IW;
float DW;
}PID;

extern PID pid; //存放PID算法所需要的数据
void PID_Calc(void); //pid计算
void PID_Init(void);
#endif


