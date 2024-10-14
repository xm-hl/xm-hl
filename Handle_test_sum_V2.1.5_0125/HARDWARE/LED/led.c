#include "led.h"  

/******************************************************************************
 * @function  :	LED_Init(void)							//ADC��ʼ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void LED_Init(void)
{    	 
	RCC->AHB1ENR|=1<<5;//ʹ��PORTFʱ�� 
	GPIO_Set(GPIOF,PIN9|PIN10|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10����

	RCC->AHB1ENR|=1<<6;//ʹ��PORTGʱ�� 
	GPIO_Set(GPIOG,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10����
	
	LED_R = 1;//LED0�ر�
	LED_G = 1;//LED1�ر�
	LED_B = 1;//LED2�ر�
	
	POWR_ON = 1;
}






