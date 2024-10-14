#include "sys.h"
#include "usart.h"	 

USART_DATA U1_DATA;
USART_DATA U3_DATA;

void UART1_DMA2_Init(void);
void UART3_DMA1_Init(void);

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

/******************************************************************************
 * @function  :	uart_init(u32 pclk2,u32 bound)	//串口1初始化程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: pclk2-PCLK2时钟频率(Mhz)
								bound-波特率 
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void uart1_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);	//得到USARTDIV@OVER8=0
	mantissa=temp;				 									//得到整数部分
	fraction=(temp-mantissa)*16; 						//得到小数部分@OVER8=0 
	mantissa<<=4;
	mantissa+=fraction; 

	RCC->AHB1ENR|=1<<0;   									//使能PORTA口时钟  
	RCC->APB2ENR|=1<<4;  										//使能串口1时钟 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);									//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);								//PA10,AF7  	   

	//波特率设置
 	USART1->BRR=mantissa; 									//波特率设置	 
	USART1->CR1&=~(1<<15); 									//设置OVER8=0 
	USART1->CR1|=1<<3;  										//串口发送使能 
	USART1->CR1|=1<<2;  										//串口接收使能
//	USART1->CR1|=0<<5;    									//禁止接收缓冲区非空中断
	USART1->CR1|=1<<4;    									//接收缓冲区空闲中断使能
	
	MY_NVIC_Init(2,2,USART1_IRQn,2);				//组2，最低优先级 
	USART1->CR1|=1<<13;  										//串口使能
	UART1_DMA2_Init();											//初始化DMA设置
	//初始化结构体
	U1_DATA.RX_LEN = 0;
	U1_DATA.RX_STA = 0;
	memset(U1_DATA.RX_BUFF,0,USART_RX_MAX);
	
	USART1->CR3|=1<<6;  										//串口DMA接收使能

}
/******************************************************************************
 * @function  :	UART1_DMA2_Init()					//串口1 DMA初始化程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void UART1_DMA2_Init()
{
	RCC->AHB1ENR |= 1 << 22;										//使能DMA2时钟
	
	DMA2_Stream2->CR &= ~( 1 << 0 );						//关闭DMA2的传输
	while(DMA2_Stream2->CR & 0x01);							//等待DMA2可以被设置
	DMA2->LIFCR |= 0x3D << 16;									//清空中断标志位

	DMA2_Stream2->CR = (4 & 7) << 25;						//选择通道4  位 27:25 CHSEL[2:0]：通道选择	
	DMA2_Stream2->PAR |= ((u32)&USART1->DR);		//外设地址
	DMA2_Stream2->M0AR |= (u32)&U1_DATA.RX_BUFF;//存储器地址 
	DMA2_Stream2->NDTR |= 100;									//要传输的数据项数目 
	DMA2_Stream2->FCR &= ~( 1 << 2 );						//使能直接模式
	DMA2_Stream2->CR |= 0 << 6;									//外设到存储器 位 7:6 DIR[1:0]：数据传输方向
	DMA2_Stream2->CR |= 0 << 8;									//使能循环模式  	位 8 CIRC：循环模式
	DMA2_Stream2->CR |= 0 << 9;									//外设地址固定 位 9 PINC
	DMA2_Stream2->CR |= 1 << 10 ;								//存储器地址自增 位 10 MINC
	DMA2_Stream2->CR |= 0 << 11;								//外设数据大小为一字节传输 即8位 位 12:11 PSIZE[1:0]：外设数据大小
	DMA2_Stream2->CR |= 0 << 13;								//存储器数据大小为一字节传输 即8位 位 14:13 MSIZE[1:0]：存储器数据大小
	DMA2_Stream2->CR |= 1 << 16;								//优先级为中 位 17:16 PL[1:0]：优先级
	DMA2_Stream2->CR |= 0 << 21;								//外设突发单次传输
	DMA2_Stream2->CR |= 0 << 23;								//存储器突发单次传输
	
	DMA2_Stream2->CR |= 1 << 0;									//使能数据流 位 0 EN：数据流使能/读作低电平时数据流就绪标志
}
/******************************************************************************
 * @function  :	usart_send(USART_TypeDef *Usart, uint8_t *Buff, uint8_t Len)		//串口发送函数							
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: Usart-串口号；Buff-需要发送的数据；Len-发送数据的长度
 * @output   	: null
 * @brief			：						
******************************************************************************/ 
void usart_send(USART_TypeDef *Usart, uint8_t *Buff, uint8_t Len)
{
	uint8_t i;
	for(i=0;i<Len;i++)
	{
		while((Usart->SR&0X40)==0);						//循环发送,直到发送完毕   
		Usart->DR = *Buff;
		Buff++;
	}
}
/******************************************************************************
 * @function  :	USART1_IRQHandler(void)		//串口中断服务函数									
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：interrupt							
******************************************************************************/ 
void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<4))									//接收到数据
	{
		DMA2_Stream2->CR &= ~(1 << 0);				//停止DMA传输
		USART1->DR;														//读取寄存器1字节数据	
		U1_DATA.RX_STA = 0x01;
		USART1->CR3|=0<<6;										//禁用串口DMA接收  							
	}
}
/******************************************************************************
 * @function  :	uart3_init(u32 pclk1, u32 bound)				 //串口3初始化
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: pclk1:	PCLK1时钟频率(Mhz)
								bound:	波特率
 * @output   	: 
 * @brief			：			
******************************************************************************/ 
void uart3_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   

	temp = (float)(pclk1 * 1000000) / (bound * 16);				//得到USARTDIV@OVER8=0
	mantissa = temp;																			//得到整数部分
	fraction = (temp - mantissa) * 16;										//得到小数部分@OVER8=0 
  mantissa <<=	4;
	mantissa +=	fraction; 

	RCC->AHB1ENR |= 1 << 1;																//使能PORTB口时钟  
	RCC->APB1ENR |= 1 << 18;															//使能串口3时钟 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PB10,PB11,复用功能,上拉输出
 	GPIO_AF_Set(GPIOB,10,7);															//PB10,AF7
	GPIO_AF_Set(GPIOB,11,7);															//PB11,AF7  	  

 	USART3->BRR	 = mantissa;															//波特率设置	 
	USART3->CR1 = 0;
	USART3->CR1	|= 0 << 15;																//设置OVER8=0 
	USART3->CR1	|= 1 << 3;																//串口发送使能 
	USART3->CR1	|= 1 << 2;																//串口接收使能
//	USART3->CR1	|= 1 << 5;																//接收缓冲区非空中断使能	
	USART3->CR1|=1<<4;    																//接收缓冲区空闲中断使能

	MY_NVIC_Init(2, 2, USART3_IRQn, 2);										//组2，最低优先级 
	UART3_DMA1_Init();
	USART3->CR1	|= 1 << 13;																//串口使能
	USART3->CR3 |=1<<6;  																	//串口DMA接收使能
}
/******************************************************************************
 * @function  :	UART3_DMA2_Init()							//串口3 DMA初始化程序
 * @author  	: xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：null							
******************************************************************************/ 
void UART3_DMA1_Init()
{
	RCC->AHB1ENR |= 1 << 21;										//使能DMA1时钟

	DMA1_Stream1->CR &= ~( 1 << 0 );						//关闭DMA1的传输
	while(DMA1_Stream1->CR & 0x01);							//等待DMA2可以被设置
	DMA1->LIFCR |= 0x3D << 16;									//清空中断标志位

	DMA1_Stream1->CR = (4 & 7) << 25;						//选择通道4  位 27:25 CHSEL[2:0]：通道选择	
	DMA1_Stream1->PAR |= ((u32)&USART3->DR);		//外设地址
	DMA1_Stream1->M0AR |= (u32)&U3_DATA.RX_BUFF;//存储器地址 
	DMA1_Stream1->NDTR |= 100;									//要传输的数据项数目 
	DMA1_Stream1->FCR &= ~( 1 << 2 );						//使能直接模式
	DMA1_Stream1->CR |= 0 << 6;									//外设到存储器 位 7:6 DIR[1:0]：数据传输方向
	DMA1_Stream1->CR |= 0 << 8;									//使能循环模式  	位 8 CIRC：循环模式
	DMA1_Stream1->CR |= 0 << 9;									//外设地址固定 位 9 PINC
	DMA1_Stream1->CR |= 1 << 10 ;								//存储器地址自增 位 10 MINC
	DMA1_Stream1->CR |= 0 << 11;								//外设数据大小为一字节传输 即8位 位 12:11 PSIZE[1:0]：外设数据大小
	DMA1_Stream1->CR |= 0 << 13;								//存储器数据大小为一字节传输 即8位 位 14:13 MSIZE[1:0]：存储器数据大小
	DMA1_Stream1->CR |= 1 << 16;								//优先级为中 位 17:16 PL[1:0]：优先级
	DMA1_Stream1->CR |= 0 << 21;								//外设突发单次传输
	DMA1_Stream1->CR |= 0 << 23;								//存储器突发单次传输
	
	DMA1_Stream1->CR |= 1 << 0;									//使能数据流 位 0 EN：数据流使能/读作低电平时数据流就绪标志
}
/******************************************************************************
 * @function  :	USART3_IRQHandler(void)									//串口3中断服务程序				
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			：interrupt							
******************************************************************************/ 
void USART3_IRQHandler(void)
{
	if(USART3->SR & (1<<4))									//接收到数据
	{
		DMA1_Stream1-> CR &= ~(1 << 0);				//停止DMA传输
		USART3->DR;														//读取寄存器1字节数据	
		U3_DATA.RX_STA = 0x01;
		USART3->CR3 |= 0<<6;										//禁用串口DMA接收  							
	}
}










