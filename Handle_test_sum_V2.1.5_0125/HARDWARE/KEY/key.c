/******************************************************************************
 * @function  :	main.c
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��						
******************************************************************************/ 
#include "key.h"
#include "led.h"
#include "delay.h" 
#include "timer.h"
#include "led.h"	 

uint8_t KEY_BUFF[30] = {0};					//����ֵ������
uint8_t Con_KEY = 0x01;

/******************************************************************************
 * @function  :	KEY_Init(void)			//������ʼ��						
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��					
******************************************************************************/ 
void KEY_Init(void)
{	
	RCC->AHB1ENR |= 1 << 3;     																					//ʹ��PORTDʱ��
	GPIO_Set(GPIOD,PIN8|PIN9|PIN10|PIN11,GPIO_MODE_IN,0,0, GPIO_PUPD_PD);	//PD8~11������������

	RCC->AHB1ENR |= 1 << 4;     																					//ʹ��PORTEʱ��
	GPIO_Set(GPIOE,PIN0|PIN6|PIN7,GPIO_MODE_IN,0,0, GPIO_PUPD_PD);				//PD8~11������������

	RCC->AHB1ENR |= 1 << 6;     																					//ʹ��PORTEʱ��
	GPIO_Set(GPIOG,PIN3,GPIO_MODE_IN,0,0, GPIO_PUPD_PD);									//PD8~11������������
} 
/******************************************************************************
 * @function  :	KEY_Scan(u8 mode)	//����ɨ��						
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��
KEY1��ǰ��-byte9��KEY2������-byte9��KEY3�����Һ-byte10��KEY4������-byte11��
KEY5����ͣ-byte13��KEY6����λ-�ޣ�KEY7��δ����
******************************************************************************/ 
void KEY_Scan(uint8_t *BUFF)
{	 
	static uint8_t num1 = 0,num2 = 0;
	if(KEY1 == 1 || KEY2 == 1 || KEY3 == 1 || KEY4 == 1 || KEY5 == 1 || KEY6 == 1 || KEY7 == 1 || KEY8 == 0)
	{
		delay_ms(15);											//����
		//ǰ�������ж�
		if(KEY1 == 1 && KEY2 == 0) BUFF[1] = 0x01;
		else if(KEY1 == 0 && KEY2 == 1) BUFF[1] = 0x02;
		else BUFF[1] = 0x00;
		//���Һ�ж�
		if(KEY3 == 1) BUFF[3] = 0x01;
		else BUFF[3] = 0x00;
		//�����ж�
		if(KEY4 == 1) BUFF[4] = 0x01;
		else BUFF[4] = 0x00;

		//��ͣ���ж�
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
		//�������ж�
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

		//��λ���ж�
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
		BUFF[1] = 0x00;//ǰ������
		BUFF[3] = 0x00;//���Һ
		BUFF[4] = 0x00;//����
		BUFF[7] = 0x00;//����
//		LED_B = 1;
		num1 = 0;			//�����ɿ��ſ�ʼ���¼���
	}
}





















