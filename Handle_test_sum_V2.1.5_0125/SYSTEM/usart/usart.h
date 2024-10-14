#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include <stdio.h>
#include <string.h>


#define USART_RX_MAX	100	//定义最大接收字节数 200

typedef struct usart_data{
	uint8_t RX_LEN;									//接受数据长度
	uint16_t RX_STA;								//接收状态
	uint8_t RX_BUFF[USART_RX_MAX];	//接收数据缓冲数据
}USART_DATA;

extern USART_DATA U1_DATA;
extern USART_DATA U3_DATA;

void uart1_init(u32 pclk2,u32 bound); 
void uart3_init(u32 pclk1,u32 bound);
void usart_send(USART_TypeDef *Usart, uint8_t *Buff, uint8_t Len);
	
#endif	   






