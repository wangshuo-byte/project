#include "control.h"
#include "oled.h"
#include "key.h"
#include "data_upper.h"
PID pid; //���PID�㷨����Ҫ������
u8 _kp;
u8 _ti;
u8 _di;
u8 _she;
void PID_Init()
{
	pid.Sv=_she;
	pid.Kp=_kp;
	pid.Ti=_ti;//����ʱ��
	pid.Td=_di;//΢��ʱ��
	pid.T=1;
}
void PID_Calc()  //pid����
{
  float DelEk;
	float ti;
	//float td;
 float out;
 pid.Ek=pid.Sv-pid.temp;                //�õ���ǰ��ƫ��ֵ
 pid.Pout=pid.Kp*pid.Ek;               //�������
	
 pid.SEk+=pid.Ek;                     //��ʷƫ���ܺ�
 DelEk=pid.Ek-pid.Ek_1;              //�������ƫ��֮��  
 ti=pid.T/pid.Ti;
 //pid.ki=ti*pid.Kp;     
 pid.Iout=ti*pid.SEk*pid.Kp;   //������� Iout=pid.SEK*pid.Kp��ʷƫ��
 //td=pid.Td/pid.T;								//������� Pout=pid.Kp*pid.Ek;
 //pid.kd=pid.Kp*td;             //΢����� Dout=kd*DelEk;��ǰƫ��
 //pid.Dout=pid.kd*DelEk;
	
//	pid.Iout=pid.Ti*pid.Ek;
	
	pid.Dout=pid.Td*DelEk;
	
 out=pid.Pout+ pid.Iout+ pid.Dout+10;//������������� ���ֿ��� 1000
 OLED_ShowNum(90,0,out,3,16); 
 if(out>1000)//�����쳣���
	 {
		pid.OUT=1000;
	 }
 if(out<0)
	 {
		pid.OUT=10;
	 }
	 else 
	 {
		pid.OUT=out;
	 }
 pid.Ek_1=pid.Ek;//����ƫ��
}



