/******************************************************************************
 * @function  :	main.c
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��						
******************************************************************************/ 
#include "led.h"
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "key.h"
#include "can.h"

extern uint16_t ADC_BUFF[3];		//ADCֵ������
extern uint8_t KEY_BUFF[30];		//����ֵ������

uint8_t Send_buff[25] = {0x55,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf1,0xf2};
uint8_t Test_buff[25] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t CAN_RX_buff[30] ={0};
uint8_t	Error_Flag = 0;
uint8_t CAN_ID1 = 0x12;
uint8_t CAN_mode = 0;		//�ػ�ģʽ
uint8_t RX_MESS_Flag = 0;
uint16_t LED_Turn = 0;
	
int main(void)
{
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);								//168M���̶�ΪAHBʱ�ӵ�1/8	
	//��ʱ����
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	LED_Init();											//��ɫ�Ƴ�ʼ��
	KEY_Init(); 										//������ʼ��
	while(!KEY8);										//ֱ�������ɿ�
	ADC1_Init();										//ADC��ʼ�������β���
	TIM3_Int_Init(99,8399);					//��ʱ����ʼ����1000ms
	TIM9_PWM_Init(PWM_ARR-1,84-1);	//84M��Ƶ��2Khz��PWM����ƽ��Ч
	Set_PWM_Duty(TIM9,0);						//����ռ�ձ�Ϊ0
	uart1_init(84,115200);					//����תUSB
	uart3_init(42,115200);					//����ת232
	CAN_Init(1, 6, 7, 6, CAN_mode);	//CAN��ʼ��, ������500Kbps	
	//����ʱ��ӡ
	usart_send(USART1,Test_buff,sizeof(Test_buff));
	usart_send(USART3,Test_buff,sizeof(Test_buff));
	//CANһ�η���8�ֽ�
	can_send_msg(CAN_ID1, Test_buff, 8);
	can_send_msg(CAN_ID1, Test_buff+8, 8);
	can_send_msg(CAN_ID1, Test_buff+8, 8);
	can_send_msg(CAN_ID1, Test_buff+24, 1);
	delay_ms(500);
	
	while(1)
	{
		//100ms����һ������-----------------------------------------------
		if(TIM3_IRQ_Flag == 1)
		{
			TIM3_IRQ_Flag = 0;
			usart_send(USART1,Send_buff,sizeof(Send_buff));
			usart_send(USART3,Send_buff,sizeof(Send_buff));
			//CANһ�η���8�ֽ�
			can_send_msg(CAN_ID1, Send_buff, 8);
			can_send_msg(CAN_ID1, Send_buff+8, 8);
			can_send_msg(CAN_ID1, Send_buff+8, 8);
			can_send_msg(CAN_ID1, Send_buff+24, 1);
//			if(Send_buff[12] == 0x01)
//			{
//				LED_Turn++;
//				if(LED_Turn>5)
//				{
//					LED_Turn = 0;
//					LED_G = ~LED_G;
//				}
//			}
//			else
//			{
//				LED_G = 1;
//				LED_Turn = 0;
//			}
		}
		//����1�������ݴ���------------------------------------------------
		if(U1_DATA.RX_STA == 0x01)
		{
			//printf("%s",U1_DATA.RX_BUFF);
			if(U1_DATA.RX_BUFF[0] == 0x55 && U1_DATA.RX_BUFF[1] == 0xaa && U1_DATA.RX_BUFF[23] == 0xf1 && U1_DATA.RX_BUFF[24] == 0xf2)
			{
				Error_Flag = U1_DATA.RX_BUFF[15];
			}
			memset(U1_DATA.RX_BUFF,0,USART_RX_MAX);
			DMA2->LIFCR |= 0x3D << 16;							//����жϱ�־λ
			DMA2_Stream2->CR |= 1 << 0;							//����DMA����
			USART1->CR3|=1<<6;  										//����DMA����ʹ��	
			U1_DATA.RX_LEN = 0;
			U1_DATA.RX_STA = 0;
			RX_MESS_Flag = 1;
		}
		//����3�������ݴ���------------------------------------------------
		if(U3_DATA.RX_STA == 0x01)
		{
			//printf("%s",U1_DATA.RX_BUFF);
			if(U3_DATA.RX_BUFF[0] == 0x55 && U3_DATA.RX_BUFF[1] == 0xaa && U3_DATA.RX_BUFF[23] == 0xf1 && U3_DATA.RX_BUFF[24] == 0xf2)
			{
				Error_Flag = U3_DATA.RX_BUFF[15];
			}
			memset(U3_DATA.RX_BUFF,0,USART_RX_MAX);
			DMA1->LIFCR |= 0x3D << 8;								//����жϱ�־λ
			DMA1_Stream1->CR |= 1 << 0;							//����DMA����
			USART3->CR3|=1<<6;  										//����DMA����ʹ��	
			U3_DATA.RX_LEN = 0;
			U3_DATA.RX_STA = 0;
			RX_MESS_Flag = 1;
		}
		//CAN�������ݴ���-------------------------------------------------
		if(can_receive_msg(CAN_ID1, CAN_RX_buff))
		{
			if(CAN_RX_buff[0] == 0x55 && CAN_RX_buff[1] == 0xaa && CAN_RX_buff[23] == 0xf1 && CAN_RX_buff[24] == 0xf2)
			{
				Error_Flag = U3_DATA.RX_BUFF[15];
			}
			memset(CAN_RX_buff,0,sizeof(CAN_RX_buff));
			RX_MESS_Flag = 1;
		}
		//����ɨ�账��----------------------------------------------------
		Send_buff[7] = 0xAA;											//�ֲ�ʽ
		KEY_Scan(KEY_BUFF);
		Send_buff[8]	= KEY_BUFF[1];							//ǰ������
		Send_buff[9]	= KEY_BUFF[3];							//���Һ
		Send_buff[10] = KEY_BUFF[4];							//����
		Send_buff[12] = KEY_BUFF[5];							//��ͣ
		Send_buff[18] = KEY_BUFF[7];							//����
		if(RX_MESS_Flag)
		{
			switch(Error_Flag)												//�쳣��������
			{
				case 0x01:Set_PWM_Duty(TIM9,60);break;
				case 0x02:Set_PWM_Duty(TIM9,70);break;
				case 0x03:Set_PWM_Duty(TIM9,90);break;
				default:Set_PWM_Duty(TIM9,0);break;
			}
			RX_MESS_Flag = 0;
		}		
		if(KEY5 == 1)		//��ͣ���£����ò���
		{
			//12λADC��ȡ�м�ֵ
			Send_buff[3]	= 0x08;
			Send_buff[4]	= 0x00;
			Send_buff[5]	= 0x08;
			Send_buff[6]	= 0x00;	
			//����ǰ������
			Send_buff[8]	= 0x00;
		}
		else
		{
			Send_buff[3]	= ADC_BUFF[1] >> 8;
			Send_buff[4]	= ADC_BUFF[1];
			Send_buff[5]	= (0x1000-ADC_BUFF[0]) >> 8;
			Send_buff[6]	= 0x1000-ADC_BUFF[0];
		}

	}
}
















