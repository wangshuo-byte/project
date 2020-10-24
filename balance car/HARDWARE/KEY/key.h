#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
extern unsigned char Trg;//´¥·¢
extern unsigned char Cont;//³ÖÐø
#define KEYPORT GPIOA->IDR
void key_init(void);
void key_scan(void);
#endif

