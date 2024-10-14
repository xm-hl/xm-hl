#include "sys.h"
#include "usart.h"	 

USART_DATA U1_DATA;
USART_DATA U3_DATA;

void UART1_DMA2_Init(void);
void UART3_DMA1_Init(void);

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}

/******************************************************************************
 * @function  :	uart_init(u32 pclk2,u32 bound)	//����1��ʼ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: pclk2-PCLK2ʱ��Ƶ��(Mhz)
								bound-������ 
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void uart1_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);	//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 									//�õ���������
	fraction=(temp-mantissa)*16; 						//�õ�С������@OVER8=0 
	mantissa<<=4;
	mantissa+=fraction; 

	RCC->AHB1ENR|=1<<0;   									//ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<4;  										//ʹ�ܴ���1ʱ�� 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOA,9,7);									//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);								//PA10,AF7  	   

	//����������
 	USART1->BRR=mantissa; 									//����������	 
	USART1->CR1&=~(1<<15); 									//����OVER8=0 
	USART1->CR1|=1<<3;  										//���ڷ���ʹ�� 
	USART1->CR1|=1<<2;  										//���ڽ���ʹ��
//	USART1->CR1|=0<<5;    									//��ֹ���ջ������ǿ��ж�
	USART1->CR1|=1<<4;    									//���ջ����������ж�ʹ��
	
	MY_NVIC_Init(2,2,USART1_IRQn,2);				//��2��������ȼ� 
	USART1->CR1|=1<<13;  										//����ʹ��
	UART1_DMA2_Init();											//��ʼ��DMA����
	//��ʼ���ṹ��
	U1_DATA.RX_LEN = 0;
	U1_DATA.RX_STA = 0;
	memset(U1_DATA.RX_BUFF,0,USART_RX_MAX);
	
	USART1->CR3|=1<<6;  										//����DMA����ʹ��

}
/******************************************************************************
 * @function  :	UART1_DMA2_Init()					//����1 DMA��ʼ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void UART1_DMA2_Init()
{
	RCC->AHB1ENR |= 1 << 22;										//ʹ��DMA2ʱ��
	
	DMA2_Stream2->CR &= ~( 1 << 0 );						//�ر�DMA2�Ĵ���
	while(DMA2_Stream2->CR & 0x01);							//�ȴ�DMA2���Ա�����
	DMA2->LIFCR |= 0x3D << 16;									//����жϱ�־λ

	DMA2_Stream2->CR = (4 & 7) << 25;						//ѡ��ͨ��4  λ 27:25 CHSEL[2:0]��ͨ��ѡ��	
	DMA2_Stream2->PAR |= ((u32)&USART1->DR);		//�����ַ
	DMA2_Stream2->M0AR |= (u32)&U1_DATA.RX_BUFF;//�洢����ַ 
	DMA2_Stream2->NDTR |= 100;									//Ҫ�������������Ŀ 
	DMA2_Stream2->FCR &= ~( 1 << 2 );						//ʹ��ֱ��ģʽ
	DMA2_Stream2->CR |= 0 << 6;									//���赽�洢�� λ 7:6 DIR[1:0]�����ݴ��䷽��
	DMA2_Stream2->CR |= 0 << 8;									//ʹ��ѭ��ģʽ  	λ 8 CIRC��ѭ��ģʽ
	DMA2_Stream2->CR |= 0 << 9;									//�����ַ�̶� λ 9 PINC
	DMA2_Stream2->CR |= 1 << 10 ;								//�洢����ַ���� λ 10 MINC
	DMA2_Stream2->CR |= 0 << 11;								//�������ݴ�СΪһ�ֽڴ��� ��8λ λ 12:11 PSIZE[1:0]���������ݴ�С
	DMA2_Stream2->CR |= 0 << 13;								//�洢�����ݴ�СΪһ�ֽڴ��� ��8λ λ 14:13 MSIZE[1:0]���洢�����ݴ�С
	DMA2_Stream2->CR |= 1 << 16;								//���ȼ�Ϊ�� λ 17:16 PL[1:0]�����ȼ�
	DMA2_Stream2->CR |= 0 << 21;								//����ͻ�����δ���
	DMA2_Stream2->CR |= 0 << 23;								//�洢��ͻ�����δ���
	
	DMA2_Stream2->CR |= 1 << 0;									//ʹ�������� λ 0 EN��������ʹ��/�����͵�ƽʱ������������־
}
/******************************************************************************
 * @function  :	usart_send(USART_TypeDef *Usart, uint8_t *Buff, uint8_t Len)		//���ڷ��ͺ���							
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: Usart-���ںţ�Buff-��Ҫ���͵����ݣ�Len-�������ݵĳ���
 * @output   	: null
 * @brief			��						
******************************************************************************/ 
void usart_send(USART_TypeDef *Usart, uint8_t *Buff, uint8_t Len)
{
	uint8_t i;
	for(i=0;i<Len;i++)
	{
		while((Usart->SR&0X40)==0);						//ѭ������,ֱ���������   
		Usart->DR = *Buff;
		Buff++;
	}
}
/******************************************************************************
 * @function  :	USART1_IRQHandler(void)		//�����жϷ�����									
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��interrupt							
******************************************************************************/ 
void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<4))									//���յ�����
	{
		DMA2_Stream2->CR &= ~(1 << 0);				//ֹͣDMA����
		USART1->DR;														//��ȡ�Ĵ���1�ֽ�����	
		U1_DATA.RX_STA = 0x01;
		USART1->CR3|=0<<6;										//���ô���DMA����  							
	}
}
/******************************************************************************
 * @function  :	uart3_init(u32 pclk1, u32 bound)				 //����3��ʼ��
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: pclk1:	PCLK1ʱ��Ƶ��(Mhz)
								bound:	������
 * @output   	: 
 * @brief			��			
******************************************************************************/ 
void uart3_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   

	temp = (float)(pclk1 * 1000000) / (bound * 16);				//�õ�USARTDIV@OVER8=0
	mantissa = temp;																			//�õ���������
	fraction = (temp - mantissa) * 16;										//�õ�С������@OVER8=0 
  mantissa <<=	4;
	mantissa +=	fraction; 

	RCC->AHB1ENR |= 1 << 1;																//ʹ��PORTB��ʱ��  
	RCC->APB1ENR |= 1 << 18;															//ʹ�ܴ���3ʱ�� 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PB10,PB11,���ù���,�������
 	GPIO_AF_Set(GPIOB,10,7);															//PB10,AF7
	GPIO_AF_Set(GPIOB,11,7);															//PB11,AF7  	  

 	USART3->BRR	 = mantissa;															//����������	 
	USART3->CR1 = 0;
	USART3->CR1	|= 0 << 15;																//����OVER8=0 
	USART3->CR1	|= 1 << 3;																//���ڷ���ʹ�� 
	USART3->CR1	|= 1 << 2;																//���ڽ���ʹ��
//	USART3->CR1	|= 1 << 5;																//���ջ������ǿ��ж�ʹ��	
	USART3->CR1|=1<<4;    																//���ջ����������ж�ʹ��

	MY_NVIC_Init(2, 2, USART3_IRQn, 2);										//��2��������ȼ� 
	UART3_DMA1_Init();
	USART3->CR1	|= 1 << 13;																//����ʹ��
	USART3->CR3 |=1<<6;  																	//����DMA����ʹ��
}
/******************************************************************************
 * @function  :	UART3_DMA2_Init()							//����3 DMA��ʼ������
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��null							
******************************************************************************/ 
void UART3_DMA1_Init()
{
	RCC->AHB1ENR |= 1 << 21;										//ʹ��DMA1ʱ��

	DMA1_Stream1->CR &= ~( 1 << 0 );						//�ر�DMA1�Ĵ���
	while(DMA1_Stream1->CR & 0x01);							//�ȴ�DMA2���Ա�����
	DMA1->LIFCR |= 0x3D << 16;									//����жϱ�־λ

	DMA1_Stream1->CR = (4 & 7) << 25;						//ѡ��ͨ��4  λ 27:25 CHSEL[2:0]��ͨ��ѡ��	
	DMA1_Stream1->PAR |= ((u32)&USART3->DR);		//�����ַ
	DMA1_Stream1->M0AR |= (u32)&U3_DATA.RX_BUFF;//�洢����ַ 
	DMA1_Stream1->NDTR |= 100;									//Ҫ�������������Ŀ 
	DMA1_Stream1->FCR &= ~( 1 << 2 );						//ʹ��ֱ��ģʽ
	DMA1_Stream1->CR |= 0 << 6;									//���赽�洢�� λ 7:6 DIR[1:0]�����ݴ��䷽��
	DMA1_Stream1->CR |= 0 << 8;									//ʹ��ѭ��ģʽ  	λ 8 CIRC��ѭ��ģʽ
	DMA1_Stream1->CR |= 0 << 9;									//�����ַ�̶� λ 9 PINC
	DMA1_Stream1->CR |= 1 << 10 ;								//�洢����ַ���� λ 10 MINC
	DMA1_Stream1->CR |= 0 << 11;								//�������ݴ�СΪһ�ֽڴ��� ��8λ λ 12:11 PSIZE[1:0]���������ݴ�С
	DMA1_Stream1->CR |= 0 << 13;								//�洢�����ݴ�СΪһ�ֽڴ��� ��8λ λ 14:13 MSIZE[1:0]���洢�����ݴ�С
	DMA1_Stream1->CR |= 1 << 16;								//���ȼ�Ϊ�� λ 17:16 PL[1:0]�����ȼ�
	DMA1_Stream1->CR |= 0 << 21;								//����ͻ�����δ���
	DMA1_Stream1->CR |= 0 << 23;								//�洢��ͻ�����δ���
	
	DMA1_Stream1->CR |= 1 << 0;									//ʹ�������� λ 0 EN��������ʹ��/�����͵�ƽʱ������������־
}
/******************************************************************************
 * @function  :	USART3_IRQHandler(void)									//����3�жϷ������				
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��interrupt							
******************************************************************************/ 
void USART3_IRQHandler(void)
{
	if(USART3->SR & (1<<4))									//���յ�����
	{
		DMA1_Stream1-> CR &= ~(1 << 0);				//ֹͣDMA����
		USART3->DR;														//��ȡ�Ĵ���1�ֽ�����	
		U3_DATA.RX_STA = 0x01;
		USART3->CR3 |= 0<<6;										//���ô���DMA����  							
	}
}










