/******************************************************************************
 * @function  :	main.c
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：						
******************************************************************************/ 
#include "key.h"
#include "led.h"
#include "delay.h" 
#include "timer.h"
#include "led.h"	 

uint8_t KEY_BUFF[30] = {0};					//按键值缓冲区
uint8_t Con_KEY = 0x01;

/******************************************************************************
 * @function  :	KEY_Init(void)			//按键初始化						
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：					
******************************************************************************/ 
void KEY_Init(void)
{	
	RCC->AHB1ENR |= 1 << 3;     																					//使能PORTD时钟
	GPIO_Set(GPIOD,PIN8|PIN9|PIN10|PIN11,GPIO_MODE_IN,0,0, GPIO_PUPD_PD);	//PD8~11设置下拉输入

	RCC->AHB1ENR |= 1 << 4;     																					//使能PORTE时钟
	GPIO_Set(GPIOE,PIN0|PIN6|PIN7,GPIO_MODE_IN,0,0, GPIO_PUPD_PD);				//PD8~11设置下拉输入

	RCC->AHB1ENR |= 1 << 6;     																					//使能PORTE时钟
	GPIO_Set(GPIOG,PIN3,GPIO_MODE_IN,0,0, GPIO_PUPD_PD);									//PD8~11设置下拉输入
} 
/******************************************************************************
 * @function  :	KEY_Scan(u8 mode)	//按键扫描						
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：
KEY1：前进-byte9，KEY2：后退-byte9，KEY3：抽肺液-byte10，KEY4：拍照-byte11，
KEY5：悬停-byte13，KEY6：档位-无，KEY7：未定义
******************************************************************************/ 
void KEY_Scan(uint8_t *BUFF)
{	 
	static uint8_t num1 = 0,num2 = 0;
	if(KEY1 == 1 || KEY2 == 1 || KEY3 == 1 || KEY4 == 1 || KEY5 == 1 || KEY6 == 1 || KEY7 == 1 || KEY8 == 0)
	{
		delay_ms(15);											//消抖
		//前进后退判断
		if(KEY1 == 1 && KEY2 == 0) BUFF[1] = 0x01;
		else if(KEY1 == 0 && KEY2 == 1) BUFF[1] = 0x02;
		else BUFF[1] = 0x00;
		//抽肺液判断
		if(KEY3 == 1) BUFF[3] = 0x01;
		else BUFF[3] = 0x00;
		//拍照判断
		if(KEY4 == 1) BUFF[4] = 0x01;
		else BUFF[4] = 0x00;

		//悬停键判断
	  if(KEY5 == 1)
		{	
			num1++;
			if(num1 == 50 )
			{			
				BUFF[5] = Con_KEY;
				LED_B = ~Con_KEY;
				Con_KEY ^= 0x01;
				//num1 = 100;
			}
			else if(num1>50)
			{
				num1 = 50;
			}
		}
		else 
		{
			num1 = 0;
		}	
		//语音键判断
	  if(KEY7 == 1)
		{
//			BUFF[7] = 0x01;
//			LED_B = 0;
		}
		else
		{		
//			LED_B = 1;			
//			BUFF[7] = 0x00;
		}	

		//档位键判断
	  if(KEY6 == 1) BUFF[6] = 0x01;
		else BUFF[6] = 0x00;
		
		if(KEY8 == 0)
		{
			num2++;
			if(num2 > 100 )
			{
				num2 = 0;
				LED_G = 0;
				POWR_ON = 0;
			}
		}
		else num2 = 0;
	}
	else
	{
		BUFF[1] = 0x00;//前进后退
		BUFF[3] = 0x00;//抽肺液
		BUFF[4] = 0x00;//拍照
		BUFF[7] = 0x00;//语音
//		LED_B = 1;
		num1 = 0;			//按键松开才开始重新计数
	}
}





















