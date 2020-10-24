#include "control.h"
#include "oled.h"
#include "key.h"
#include "data_upper.h"
PID pid; //存放PID算法所需要的数据
u8 _kp;
u8 _ti;
u8 _di;
u8 _she;
void PID_Init()
{
	pid.Sv=_she;
	pid.Kp=_kp;
	pid.Ti=_ti;//积分时间
	pid.Td=_di;//微分时间
	pid.T=1;
}
void PID_Calc()  //pid计算
{
  float DelEk;
	float ti;
	//float td;
 float out;
 pid.Ek=pid.Sv-pid.temp;                //得到当前的偏差值
 pid.Pout=pid.Kp*pid.Ek;               //比例输出
	
 pid.SEk+=pid.Ek;                     //历史偏差总和
 DelEk=pid.Ek-pid.Ek_1;              //最近两次偏差之差  
 ti=pid.T/pid.Ti;
 //pid.ki=ti*pid.Kp;     
 pid.Iout=ti*pid.SEk*pid.Kp;   //积分输出 Iout=pid.SEK*pid.Kp历史偏差
 //td=pid.Td/pid.T;								//比例输出 Pout=pid.Kp*pid.Ek;
 //pid.kd=pid.Kp*td;             //微分输出 Dout=kd*DelEk;当前偏差
 //pid.Dout=pid.kd*DelEk;
	
//	pid.Iout=pid.Ti*pid.Ek;
	
	pid.Dout=pid.Td*DelEk;
	
 out=pid.Pout+ pid.Iout+ pid.Dout+10;//本次输出的脉宽 三种可能 1000
 OLED_ShowNum(90,0,out,3,16); 
 if(out>1000)//两种异常情况
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
 pid.Ek_1=pid.Ek;//更新偏差
}



