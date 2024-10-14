#include "math.h"																																			
#include "sys.h"
#include "timer.h"
#include "delay.h"
#include "led.h"
#include "key.h" 
#include "usart.h"
#include "adc.h"	

#define	TIM_NUM 	10					//�����Զ����ж�ʱ�䣬��ʱ����ʱʱ��ı���

uint8_t TIM3_IRQ_Flag	= 0;		//�Զ��嶨ʱ����־
uint32_t PWM_ARR = 100;				//PWM��ռ�ձ����ֵ
	
/******************************************************************************
 * @function  :	TIM3_Int_Init(u16 arr,u16 psc)	// ͨ�ö�ʱ����ʼ��
 * @author  	:	hy.tan
 * @version 	:	V1.1.0
 * @date    	:	2023-05-15
 * @input   	: arr���Զ���װ��ֵ
								psc��Ԥ��Ƶֵ
 * @output   	: null
 * @brief			���趨ͨ�ö�ʱ��������					
******************************************************************************/ 
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR	|=	(1 << 1);						//TIM3ʱ��ʹ��    
 	TIM3->ARR			 =	arr;  							//�趨�������Զ���װֵ 
	TIM3->PSC			 = 	psc;  							//Ԥ��Ƶ��	  
	TIM3->DIER		|=	(1 << 0);  	 				//��������ж�	  
	TIM3->CR1			|=	0x01;    						//ʹ�ܶ�ʱ��3

	MY_NVIC_Init(1, 3, TIM3_IRQn, 2);
}

/******************************************************************************
 * @function  :	TIM9_PWM_Init(u32 arr, u32 psc)	// ��ʱ��PWM��ʼ��
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: arr���Զ���װ��ֵ
								psc��Ԥ��Ƶֵ
 * @output   	: null
 * @brief			���趨ͨ�ö�ʱ��������					
******************************************************************************/ 
void TIM9_PWM_Init(u32 arr, u32 psc)
{
	RCC->APB2ENR |= 1 << 16;									//TIM9ʱ��ʹ��
	RCC->AHB1ENR |= 1 << 4;										//ʹ��PORTEʱ��
	GPIO_Set(GPIOE, PIN5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);	//���ù��ܣ��������
	GPIO_AF_Set(GPIOE,5,3);										//PE5,AF3
	
	TIM9->ARR = arr;
	TIM9->PSC = psc;
	
	//����Ƚ�ģʽʹ�� -- ʹ����Ӧ��Ԥװ�ؼĴ���
	TIM9->CCMR1 |= 6 << 4;										//TIM9_CH1 PWM1ģʽ -- λ6��4 �OOC1Mλѡ��PWMģʽ -- 110 PWM1 111 PWM2
	TIM9->CCMR1 |= 1 << 3;										//CH1Ԥװ��ʹ��--λ3��OC1PE ʹ��
	
	//����Ƚ�ʹ�ܼĴ��� -- ȷ�� PWM ��������Ժ����ʹ��
	TIM9->CCER |= 1 << 0;											//OC1���ʹ��
	TIM9->CCER |= 0 << 1;											//OC1�͵�ƽ��Ч
	
	//ʹ�ܿ��ƼĴ���
	TIM9->CR1 |= 1 << 7;											//ARPEλʹ��--�Զ�����Ԥװ��ʹ��
	TIM9->CR1 |= 1 << 0;											//ʹ�ܶ�ʱ��9
	
}
/******************************************************************************
 * @function  :	TIM3_IRQHandler(void)				// ��ʱ��3�жϷ������
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 	
void TIM3_IRQHandler(void)
{ 	
	static uint8_t count_num = 0;
	if(TIM3->SR&0X0001)//����ж�
	{
		count_num ++;
		ADC1->CR2 |= 1 << 30;						//��ʼת������ͨ���� λ 30 SWSTART��
		DMA2_Stream0->CR |= 1 << 0;			//ʹ�������� λ 0 EN��������ʹ��/�����͵�ƽʱ������������־		
		if(count_num >= TIM_NUM)				//���ƴ��ڷ���Ƶ��
		{
			TIM3_IRQ_Flag = 1;
			count_num = 0;
		}
	}
	TIM3->SR&=~(1<<0);								//����жϱ�־λ 
} 		    				   				     	    	
/******************************************************************************
 * @function  :	Set_PWM_Duty(TIM_TypeDef *TIM,uint16_t num)	// ��ʱ��3�жϷ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: TIM-��ʱ��
								Duty-ռ�ձȣ�0~100
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 			
void Set_PWM_Duty(TIM_TypeDef *TIM,uint8_t Duty)
{
	TIM->CCR1 = (uint32_t)(PWM_ARR*(Duty/100.f));
}

