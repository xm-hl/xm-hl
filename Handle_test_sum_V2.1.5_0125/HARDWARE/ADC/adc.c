#include "sys.h"
#include <string.h>
#include "adc.h"

uint16_t ADC_BUFF[10];
void DMA2_Init(void);

/******************************************************************************
 * @function  :	ADC1_Init(void)							//ADC��ʼ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void ADC1_Init(void)
{
	RCC->APB2ENR |= 1 << 8;										//ʹ��ADC1ʱ��
	RCC->AHB1ENR |= 1 << 2;										//ʹ��GPIOCʱ��
	GPIO_Set( GPIOC, PIN0 | PIN1, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE );
	
	RCC->APB2RSTR |= 1 << 8;   								//ADCs��λ
	RCC->APB2RSTR &= ~( 1 << 8 );							//��λ����
	ADC->CCR |= 1 << 16;											//ʱ��4��Ƶ 
	ADC->CCR &= ~( 31 << 0 );									//ADC����Ϊ����ģʽ λ4:0 MULTI[4:0]��
	
	ADC1->CR1 = 0;   													//CR1��������
	ADC1->CR2 = 0;   													//CR2��������
	ADC1->CR1 |=  0 << 24 ;										//12λ�ֱ��� λ 25:24 RES[1:0]���ֱ��� (Resolution)
	ADC1->CR1 |= 1 << 8 ;											//ɨ��ģʽ λ 8 SCAN��ɨ��ģʽ (Scan mode)
	ADC1->CR2 |= 0 << 1;											//����ת��ģʽ  λ 1 CONT������ת��
	ADC1->CR2 &= ~( 3 << 28 );								//��ֹ������� λ 29:28 EXTEN
	ADC1->CR2 &= ~( 1 << 11 );								//�Ҷ��� λ 11 ALIGN�����ݶ���
	
	ADC1->SQR1 &= ~( 0XF << 20 );
	ADC1->SQR1 |= 1 << 20;										//����ת�� λ 23:20 L[3:0]������ͨ�����г��� ͨ�����д����Щλ�ɶ������ͨ��ת�������е�ת��������
	
	//����ͨ��10�Ĳ���ʱ��
	ADC1->SMPR1 &= ~( 7 << ( 3*(10-10) ) );		//ͨ��10����ʱ�����	  
	ADC1->SMPR1 |= 7 << ( 3*(10-10) );				//ͨ��10  480������,��߲���ʱ�������߾�ȷ��		
	ADC1->SQR3 |= 10 << 0;										//ͨ��10��һ��ת��
	
	//����ͨ��5�Ĳ���ʱ��
	ADC1->SMPR1 &= ~( 7 << ( 3*(11-10) ) );		//ͨ��11����ʱ�����	
	ADC1->SMPR1 |= ( 7 << 3*(11-10) );				//ͨ��11�Ĳ���ʱ��
	ADC1->SQR3 |= 11 << 5;										//ͨ��11��һ��ת��
	
	ADC1->CR2 |= 1 << 8;											//ʹ��DMAģʽ λ 8 DMA��ֱ�Ӵ洢������ģʽ
	ADC1->CR2 |= 1 << 9;											//ֻҪ��������ת����DMA=1���㷢��DMA���� λ 9 DDS��DMA ��ֹѡ��
	ADC1->CR2 |= 1 << 0;											//ʹ��ADC λ 0 ADON��A/D ת�������� / �ر�
	ADC1->CR2 |= 1 << 30;											//��ʼת������ͨ���� λ 30 SWSTART��
	DMA2_Init();															//DMA��ʼ��
}
/******************************************************************************
 * @function  :	DMA2_Init(void)							//DMA2��ʼ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void DMA2_Init(void)
{
	RCC->AHB1ENR |= 1 << 22;								//ʹ��DMA2ʱ��
	
	DMA2_Stream0->CR &= ~( 1 << 0 );				//�ر�DMA2�Ĵ���
	while(DMA2_Stream0->CR & 0x01);					//�ȴ�DMA2���Ա�����
	DMA2->LIFCR |= 0x3D << 0;								//����жϱ�־λ
	
	DMA2_Stream0->PAR |= ((u32)&ADC1->DR);	//�����ַ
	DMA2_Stream0->M0AR |= (u32)&ADC_BUFF;		//�洢����ַ 
	
	DMA2_Stream0->NDTR |= 2;								//Ҫ�������������Ŀ 	
	DMA2_Stream0->CR &= ~( 7 << 25);				//ѡ��ͨ��0  λ 27:25 CHSEL[2:0]��ͨ��ѡ��	
	DMA2_Stream0->CR |= 1 << 16;						//���ȼ�Ϊ�� λ 17:16 PL[1:0]�����ȼ�
	DMA2_Stream0->FCR &= ~( 1 << 2 );				//ʹ��ֱ��ģʽ
	DMA2_Stream0->CR &= ~( 3 << 6 );				//���赽�洢�� λ 7:6 DIR[1:0]�����ݴ��䷽��
	DMA2_Stream0->CR &= ~( 1 << 9 );				//�����ַ�̶� λ 9 PINC
	DMA2_Stream0->CR |=  1 << 10 ;					//�洢����ַ���� λ 10 MINC
	DMA2_Stream0->CR |= 1 << 11;						//�������ݴ�СΪ���ִ��� ��16λ λ 12:11 PSIZE[1:0]���������ݴ�С
	DMA2_Stream0->CR |= 1 << 13;						//�洢�����ݴ�СΪ���ִ��� ��16λ λ 14:13 MSIZE[1:0]���洢�����ݴ�С
	DMA2_Stream0->CR |= 1 << 8;							//ʹ��ѭ��ģʽ  	λ 8 CIRC��ѭ��ģʽ
	DMA2_Stream0->CR |= 1 << 0;							//ʹ�������� λ 0 EN��������ʹ��/�����͵�ƽʱ������������־
}
/******************************************************************************
 * @function  :	Get_ADC_Value(ADC_TypeDef *ch,uint16_t num,uint16_t *BUFF)					
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void Get_ADC_Value(ADC_TypeDef *ch,uint16_t num,uint16_t *BUFF)
{
	*BUFF = ADC_BUFF[0];
	*(BUFF+1) = ADC_BUFF[1];
}
