#include "math.h"																																			
#include "sys.h"
#include "timer.h"
#include "delay.h"
#include "led.h"
#include "key.h" 
#include "usart.h"
#include "adc.h"	

#define	TIM_NUM 	10					//控制自定义中断时间，定时器定时时间的倍数

uint8_t TIM3_IRQ_Flag	= 0;		//自定义定时器标志
uint32_t PWM_ARR = 100;				//PWM的占空比最大值
	
/******************************************************************************
 * @function  :	TIM3_Int_Init(u16 arr,u16 psc)	// 通用定时器初始化
 * @author  	:	hy.tan
 * @version 	:	V1.1.0
 * @date    	:	2023-05-15
 * @input   	: arr：自动重装载值
								psc：预分频值
 * @output   	: null
 * @brief			：设定通用定时器的周期					
******************************************************************************/ 
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR	|=	(1 << 1);						//TIM3时钟使能    
 	TIM3->ARR			 =	arr;  							//设定计数器自动重装值 
	TIM3->PSC			 = 	psc;  							//预分频器	  
	TIM3->DIER		|=	(1 << 0);  	 				//允许更新中断	  
	TIM3->CR1			|=	0x01;    						//使能定时器3

	MY_NVIC_Init(1, 3, TIM3_IRQn, 2);
}

/******************************************************************************
 * @function  :	TIM9_PWM_Init(u32 arr, u32 psc)	// 定时器PWM初始化
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: arr：自动重装载值
								psc：预分频值
 * @output   	: null
 * @brief			：设定通用定时器的周期					
******************************************************************************/ 
void TIM9_PWM_Init(u32 arr, u32 psc)
{
	RCC->APB2ENR |= 1 << 16;									//TIM9时钟使能
	RCC->AHB1ENR |= 1 << 4;										//使能PORTE时钟
	GPIO_Set(GPIOE, PIN5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);	//复用功能，上拉输出
	GPIO_AF_Set(GPIOE,5,3);										//PE5,AF3
	
	TIM9->ARR = arr;
	TIM9->PSC = psc;
	
	//输出比较模式使能 -- 使能相应的预装载寄存器
	TIM9->CCMR1 |= 6 << 4;										//TIM9_CH1 PWM1模式 -- 位6：4 OC1M位选择PWM模式 -- 110 PWM1 111 PWM2
	TIM9->CCMR1 |= 1 << 3;										//CH1预装载使能--位3：OC1PE 使能
	
	//输出比较使能寄存器 -- 确定 PWM 的输出极性和输出使能
	TIM9->CCER |= 1 << 0;											//OC1输出使能
	TIM9->CCER |= 0 << 1;											//OC1低电平有效
	
	//使能控制寄存器
	TIM9->CR1 |= 1 << 7;											//ARPE位使能--自动重载预装载使能
	TIM9->CR1 |= 1 << 0;											//使能定时器9
	
}
/******************************************************************************
 * @function  :	TIM3_IRQHandler(void)				// 定时器3中断服务程序
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 	
void TIM3_IRQHandler(void)
{ 	
	static uint8_t count_num = 0;
	if(TIM3->SR&0X0001)//溢出中断
	{
		count_num ++;
		ADC1->CR2 |= 1 << 30;						//开始转换规则通道的 位 30 SWSTART：
		DMA2_Stream0->CR |= 1 << 0;			//使能数据流 位 0 EN：数据流使能/读作低电平时数据流就绪标志		
		if(count_num >= TIM_NUM)				//控制串口发送频率
		{
			TIM3_IRQ_Flag = 1;
			count_num = 0;
		}
	}
	TIM3->SR&=~(1<<0);								//清除中断标志位 
} 		    				   				     	    	
/******************************************************************************
 * @function  :	Set_PWM_Duty(TIM_TypeDef *TIM,uint16_t num)	// 定时器3中断服务程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: TIM-定时器
								Duty-占空比，0~100
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 			
void Set_PWM_Duty(TIM_TypeDef *TIM,uint8_t Duty)
{
	TIM->CCR1 = (uint32_t)(PWM_ARR*(Duty/100.f));
}

