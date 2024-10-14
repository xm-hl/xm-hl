#include "sys.h"
#include <string.h>
#include "adc.h"

uint16_t ADC_BUFF[10];
void DMA2_Init(void);

/******************************************************************************
 * @function  :	ADC1_Init(void)							//ADC初始化程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void ADC1_Init(void)
{
	RCC->APB2ENR |= 1 << 8;										//使能ADC1时钟
	RCC->AHB1ENR |= 1 << 2;										//使能GPIOC时钟
	GPIO_Set( GPIOC, PIN0 | PIN1, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE );
	
	RCC->APB2RSTR |= 1 << 8;   								//ADCs复位
	RCC->APB2RSTR &= ~( 1 << 8 );							//复位结束
	ADC->CCR |= 1 << 16;											//时钟4分频 
	ADC->CCR &= ~( 31 << 0 );									//ADC配置为独立模式 位4:0 MULTI[4:0]：
	
	ADC1->CR1 = 0;   													//CR1设置清零
	ADC1->CR2 = 0;   													//CR2设置清零
	ADC1->CR1 |=  0 << 24 ;										//12位分辨率 位 25:24 RES[1:0]：分辨率 (Resolution)
	ADC1->CR1 |= 1 << 8 ;											//扫描模式 位 8 SCAN：扫描模式 (Scan mode)
	ADC1->CR2 |= 0 << 1;											//单次转换模式  位 1 CONT：连续转换
	ADC1->CR2 &= ~( 3 << 28 );								//禁止触发检测 位 29:28 EXTEN
	ADC1->CR2 &= ~( 1 << 11 );								//右对齐 位 11 ALIGN：数据对齐
	
	ADC1->SQR1 &= ~( 0XF << 20 );
	ADC1->SQR1 |= 1 << 20;										//两次转换 位 23:20 L[3:0]：规则通道序列长度 通过软件写入这些位可定义规则通道转换序列中的转换总数。
	
	//设置通道10的采样时间
	ADC1->SMPR1 &= ~( 7 << ( 3*(10-10) ) );		//通道10采样时间清空	  
	ADC1->SMPR1 |= 7 << ( 3*(10-10) );				//通道10  480个周期,提高采样时间可以提高精确度		
	ADC1->SQR3 |= 10 << 0;										//通道10第一次转换
	
	//设置通道5的采样时间
	ADC1->SMPR1 &= ~( 7 << ( 3*(11-10) ) );		//通道11采样时间清空	
	ADC1->SMPR1 |= ( 7 << 3*(11-10) );				//通道11的采样时间
	ADC1->SQR3 |= 11 << 5;										//通道11第一次转换
	
	ADC1->CR2 |= 1 << 8;											//使能DMA模式 位 8 DMA：直接存储器访问模式
	ADC1->CR2 |= 1 << 9;											//只要发生数据转换且DMA=1，便发出DMA请求 位 9 DDS：DMA 禁止选择
	ADC1->CR2 |= 1 << 0;											//使能ADC 位 0 ADON：A/D 转换器开启 / 关闭
	ADC1->CR2 |= 1 << 30;											//开始转换规则通道的 位 30 SWSTART：
	DMA2_Init();															//DMA初始化
}
/******************************************************************************
 * @function  :	DMA2_Init(void)							//DMA2初始化程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void DMA2_Init(void)
{
	RCC->AHB1ENR |= 1 << 22;								//使能DMA2时钟
	
	DMA2_Stream0->CR &= ~( 1 << 0 );				//关闭DMA2的传输
	while(DMA2_Stream0->CR & 0x01);					//等待DMA2可以被设置
	DMA2->LIFCR |= 0x3D << 0;								//清空中断标志位
	
	DMA2_Stream0->PAR |= ((u32)&ADC1->DR);	//外设地址
	DMA2_Stream0->M0AR |= (u32)&ADC_BUFF;		//存储器地址 
	
	DMA2_Stream0->NDTR |= 2;								//要传输的数据项数目 	
	DMA2_Stream0->CR &= ~( 7 << 25);				//选择通道0  位 27:25 CHSEL[2:0]：通道选择	
	DMA2_Stream0->CR |= 1 << 16;						//优先级为中 位 17:16 PL[1:0]：优先级
	DMA2_Stream0->FCR &= ~( 1 << 2 );				//使能直接模式
	DMA2_Stream0->CR &= ~( 3 << 6 );				//外设到存储器 位 7:6 DIR[1:0]：数据传输方向
	DMA2_Stream0->CR &= ~( 1 << 9 );				//外设地址固定 位 9 PINC
	DMA2_Stream0->CR |=  1 << 10 ;					//存储器地址自增 位 10 MINC
	DMA2_Stream0->CR |= 1 << 11;						//外设数据大小为半字传输 即16位 位 12:11 PSIZE[1:0]：外设数据大小
	DMA2_Stream0->CR |= 1 << 13;						//存储器数据大小为半字传输 即16位 位 14:13 MSIZE[1:0]：存储器数据大小
	DMA2_Stream0->CR |= 1 << 8;							//使能循环模式  	位 8 CIRC：循环模式
	DMA2_Stream0->CR |= 1 << 0;							//使能数据流 位 0 EN：数据流使能/读作低电平时数据流就绪标志
}
/******************************************************************************
 * @function  :	Get_ADC_Value(ADC_TypeDef *ch,uint16_t num,uint16_t *BUFF)					
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void Get_ADC_Value(ADC_TypeDef *ch,uint16_t num,uint16_t *BUFF)
{
	*BUFF = ADC_BUFF[0];
	*(BUFF+1) = ADC_BUFF[1];
}
