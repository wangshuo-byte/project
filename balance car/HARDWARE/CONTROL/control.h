#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
//#define pwmout_1    GPIO_SetBits(GPIOB, GPIO_Pin_5)    //ֹͣ����
//#define pwmout_0    GPIO_ResetBits(GPIOB, GPIO_Pin_5)  //����
extern u8 _kp;
extern u8 _ti;
extern u8 _di;
typedef struct
{
float Sv;//�û��趨ֵ
float	temp;//ʵ��ֵ

float Kp;//��������ϵ��
float T;  //PID��������--��������
float Ti;//����
float Td; //΢��
float ki;
float kd;
float Ek;  //����ƫ��
float Ek_1;//�ϴ�ƫ��
float SEk; //��ʷƫ��֮��
	
float Iout;//�������
float Pout;//�������
float Dout;//΢�����
	
float OUT0;//�����
float kp0;
float ki0;
float di0;
float OUT;//�����
float P;
float I;
float D;
float PW;
float IW;
float DW;
}PID;

extern PID pid; //���PID�㷨����Ҫ������
void PID_Calc(void); //pid����
void PID_Init(void);
#endif


