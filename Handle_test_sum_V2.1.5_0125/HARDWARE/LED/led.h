#ifndef __LED_H
#define __LED_H	 
#include "sys.h" 


//LED�˿ڶ���
#define LED_R PFout(9)	//red
#define LED_G PFout(10)	//green	 
#define LED_B PFout(11)	//blue 

#define POWR_ON PGout(2)

void LED_Init(void);//��ʼ��		 				    
#endif

















