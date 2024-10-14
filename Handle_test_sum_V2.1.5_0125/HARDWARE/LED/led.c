#include "led.h"  

/******************************************************************************
 * @function  :	LED_Init(void)							//ADC初始化程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void LED_Init(void)
{    	 
	RCC->AHB1ENR|=1<<5;//使能PORTF时钟 
	GPIO_Set(GPIOF,PIN9|PIN10|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10设置

	RCC->AHB1ENR|=1<<6;//使能PORTG时钟 
	GPIO_Set(GPIOG,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10设置
	
	LED_R = 1;//LED0关闭
	LED_G = 1;//LED1关闭
	LED_B = 1;//LED2关闭
	
	POWR_ON = 1;
}






