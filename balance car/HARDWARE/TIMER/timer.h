#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define ENCODER_TIM_PERIOD (u16)(65535)   //不可大于65535 因为F103的定时器是16位的。
#define PWMA   TIM1->CCR1  //PA8
#define AIN2   PBout(15)
#define AIN1   PBout(14)
#define BIN1   PBout(13)
#define BIN2   PBout(12)
#define PWMB   TIM1->CCR4  //PA11

void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
void Encoder_Init_TIM2(void);
void Encoder_Init_TIM4(void);
void MiniBalance_PWM_Init(u16 arr,u16 psc);
int Read_Encoder(u8 TIMX);
#endif
