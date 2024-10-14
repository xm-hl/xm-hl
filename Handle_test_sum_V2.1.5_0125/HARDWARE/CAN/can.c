/******************************************************************************
 * @function  :	main.c
 * @author  	:	xianming.wu
 * @version 	:	V2.1.0
 * @date    	:	2023-09-05
 * @input   	: null
 * @output   	: null
 * @brief			��						
******************************************************************************/ 

#include "can.h"
#include "delay.h"
#include "usart.h"


/**
 * @brief       CAN��ʼ��
 * @param       tsjw    : ����ͬ����Ծʱ�䵥Ԫ.��Χ: 1~3;
 * @param       tbs2    : ʱ���2��ʱ�䵥Ԫ.��Χ: 1~8;
 * @param       tbs1    : ʱ���1��ʱ�䵥Ԫ.��Χ: 1~16;
 * @param       brp     : �����ʷ�Ƶ��.��Χ: 1~1024;
 *   @note      ����4������, �ں����ڲ����1, ����, �κ�һ�����������ܵ���0
 *              CAN����APB1����, ������ʱ��Ƶ��Ϊ Fpclk1 = PCLK1 = 42Mhz
 *              tq     = brp * tpclk1;
 *              ������ = Fpclk1 / ((tbs1 + tbs2 + 1) * brp);
 *              �������� can_init(1, 6, 7, 6, 1), ��CAN������Ϊ:
 *              42M / ((6 + 7 + 1) * 6) = 500Kbps
 *
 * @param       mode    : 0,��ͨģʽ;1,�ػ�ģʽ;
 * @retval      0,  ��ʼ���ɹ�; ����, ��ʼ��ʧ��;
 */
uint8_t CAN_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
	uint16_t i = 0;

	if (tsjw == 0 || tbs2 == 0 || tbs1 == 0 || brp == 0)
	{
		return 1;   /* �������Ϸ�, ֱ�ӷ��� ���� */
	}
	tsjw -= 1;      /* �ȼ�ȥ1.���������� */
	tbs2 -= 1;
	tbs1 -= 1;
	brp -= 1;
	
	RCC->AHB1ENR|=1<<0;   									//ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<4;  										//ʹ�ܴ���1ʱ�� 
	GPIO_Set(GPIOA,PIN11|PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOA,11,9);								//PA9,AF9
	GPIO_AF_Set(GPIOA,12,9);								//PA10,AF9  	

	RCC->APB1ENR |= 1 << 25;    /* ʹ��CANʱ�� CANʹ�õ���APB1��ʱ��(max: 42M) */
	CAN1->MCR = 0x0000;         /* �˳�˯��ģʽ(ͬʱ��������λΪ0) */
	CAN1->MCR |= 1 << 0;        /* ����CAN�����ʼ��ģʽ */

	while ((CAN1->MSR & 1 << 0) == 0)   /* �ȴ������ʼ��ģʽ�ɹ� */
	{
		i++;
		if (i > 100)            /* �����ʼ��ģʽʧ�� */
		{
			return 2;
		}
	}

	CAN1->MCR |= 0 << 7;        /* ��ʱ�䴥��ͨ��ģʽ */
	CAN1->MCR |= 0 << 6;        /* ����Զ����߹��� */
	CAN1->MCR |= 0 << 5;        /* ˯��ģʽͨ���������(���CAN1->MCR��SLEEPλ) */
	CAN1->MCR |= 1 << 4;        /* ��ֹ�����Զ����� */
	CAN1->MCR |= 0 << 3;        /* ���Ĳ�����,�µĸ��Ǿɵ� */
	CAN1->MCR |= 0 << 2;        /* ���ȼ��ɱ��ı�ʶ������ */
	CAN1->BTR = 0;              /* ���ԭ��������. */
	CAN1->BTR |= mode << 30;    /* LBKM = mode, ģʽ����  0, ��ͨģʽ; 1, �ػ�ģʽ; */
	CAN1->BTR |= tsjw << 24;    /* SJW[1:0] = tsjw, ����ͬ����Ծ���(Tsjw)Ϊ tsjw + 1 ��ʱ�䵥λ */
	CAN1->BTR |= tbs2 << 20;    /* TS2[2:0] = tbs2, Tbs2 = tbs2 + 1 ��ʱ�䵥λ */
	CAN1->BTR |= tbs1 << 16;    /* TS1[2:0] = tbs1, Tbs1 = tbs1 + 1 ��ʱ�䵥λ */
	CAN1->BTR |= brp << 0;      /* BRP[9:0] = brp, Fdiv = brp + 1 */
    
	/* ������: Fpclk1 / ((Tbs1 + Tbs2 + 1) * Fdiv) */
	CAN1->MCR &= ~(1 << 0);     /* ����CAN�˳���ʼ��ģʽ */

	while ((CAN1->MSR & 1 << 0) == 1)
	{
			i++;

			if (i > 0XFFF0)
			{
					return 3;           /* �˳���ʼ��ģʽʧ�� */
			}
	}

	/* ��������ʼ��, ����ֻ�õ��˹�����0, �Ҳ������κ�ID */
	CAN1->FMR |= 1 << 0;        /* �������鹤���ڳ�ʼ��ģʽ */
	CAN1->FA1R &= ~(1 << 0);    /* ������0������ */
	CAN1->FS1R |= 1 << 0;       /* ������λ��Ϊ32λ. */
	CAN1->FM1R |= 0 << 0;       /* ������0�����ڱ�ʶ������λģʽ */
	CAN1->FFA1R |= 0 << 0;      /* ������0������FIFO0 */
	CAN1->sFilterRegister[0].FR1 = 0X00000000;  /* 32λID */
	CAN1->sFilterRegister[0].FR2 = 0X00000000;  /* 32λMASK */
	CAN1->FA1R |= 1 << 0;       /* ���������0 */
	CAN1->FMR &= 0 << 0;        /* ���������������ģʽ */

#if CAN_RX0_INT_ENABLE

    /* ʹ���жϽ��� */
    CAN1->IER |= 1 << 1;        /* FIFO0��Ϣ�Һ��ж����� */
    sys_nvic_init(1, 0, CAN1_RX0_IRQn, 2);  /* ��2 */

#endif
    return 0;
}

/**
 * @brief       CAN����һ����Ϣ
 * @param       id      : ��׼ID(11λ)/��չID(11λ+18λ)
 * @param       ide     : 0,��׼֡;1,��չ֡
 * @param       rtr     : 0,����֡;1,Զ��֡
 * @param       len     : Ҫ���͵����ݳ���(�̶�Ϊ8���ֽ�, ��ʱ�䴥��ģʽ��, ��Ч����Ϊ6���ֽ�)
 * @param       data    : ����ָ��
 * @retval      ���η����������õ������
 *   @arg       0 ~ 3   : ������
 *   @arg       0XFF    : ����Ч����(���ݷ���ʧ��)
 */
static uint8_t can_tx_msg(uint32_t id, uint8_t ide, uint8_t rtr, uint8_t len, uint8_t *data)
{
	uint8_t mbox;

	if (CAN1->TSR & (1 << 26))      /* ����0Ϊ��, mbox = 0 */
	{
		mbox = 0;
	}
	else if (CAN1->TSR & (1 << 27)) /* ����1Ϊ��, mbox = 1 */
	{
		mbox = 1;
	}
	else if (CAN1->TSR & (1 << 28)) /* ����2Ϊ��, mbox = 2 */
	{
		mbox = 2;
	}
	else
	{
		return 0XFF;                /* �޿�����, �޷����� */
	}
	
	CAN1->sTxMailBox[mbox].TIR = 0; /* ���֮ǰ������ */

	if (ide == 0)           /* ��׼֡ */
	{
		id &= 0x7ff;        /* ȡ��11λstdid */
		id <<= 21;
	}
	else                    /* ��չ֡ */
	{
		id &= 0X1FFFFFFF;   /* ȡ��32λextid */
		id <<= 3;
	}

	CAN1->sTxMailBox[mbox].TIR |= id;
	CAN1->sTxMailBox[mbox].TIR |= ide << 2;
	CAN1->sTxMailBox[mbox].TIR |= rtr << 1;
	len &= 0X0F;            /* �õ�����λ, �8���ֽ� */
	CAN1->sTxMailBox[mbox].TDTR &= ~(0XF << 0);
	CAN1->sTxMailBox[mbox].TDTR |= len; /* ����DLC[3:0] */
	
	/* ���������ݴ������� */
	CAN1->sTxMailBox[mbox].TDHR = (((uint32_t)data[7] << 24) |
																 ((uint32_t)data[6] << 16) |
																 ((uint32_t)data[5] << 8) |
																 ((uint32_t)data[4]));

	CAN1->sTxMailBox[mbox].TDLR = (((uint32_t)data[3] << 24) |
																 ((uint32_t)data[2] << 16) |
																 ((uint32_t)data[1] << 8) |
																 ((uint32_t)data[0]));

	CAN1->sTxMailBox[mbox].TIR |= 1 << 0;   /* �������������� */
	return mbox;
}

/**
 * @brief       ��ȡCAN����״̬
 * @param       mbox    : ������ (0 ~ 2)
 * @retval      ����״̬
 *   @arg       0X00    : ����
 *   @arg       0X05    : ����ʧ��
 *   @arg       0X07    : ���ͳɹ�
 */
static uint8_t can_tx_status(uint8_t mbox)
{
	uint8_t sta = 0;

	switch (mbox)
	{
		case 0:
			sta |= CAN1->TSR & (1 << 0);            /* RQCP0 */
			sta |= CAN1->TSR & (1 << 1);            /* TXOK0 */
			sta |= ((CAN1->TSR & (1 << 26)) >> 24); /* TME0 */
			break;

		case 1:
			sta |= CAN1->TSR & (1 << 8) >> 8;       /* RQCP1 */
			sta |= CAN1->TSR & (1 << 9) >> 8;       /* TXOK1 */
			sta |= ((CAN1->TSR & (1 << 27)) >> 25); /* TME1 */
			break;

		case 2:
			sta |= CAN1->TSR & (1 << 16) >> 16;     /* RQCP2 */
			sta |= CAN1->TSR & (1 << 17) >> 16;     /* TXOK2 */
			sta |= ((CAN1->TSR & (1 << 28)) >> 26); /* TME2 */
			break;

		default:
			sta = 0X05; /* ����Ų���,�϶�ʧ�� */
			break;
	}

	return sta;
}

/**
 * @brief       �õ���FIFO0/FIFO1�н��յ��ı��ĸ���
 * @param       fifox   : 0, FIFO0; 1, FIFO1;
 * @retval      FIFO0 / FIFO1�б��ĵĸ���
 */
uint8_t can_pend_msg(uint8_t fifox)
{
	if (fifox == 0)
	{
		return CAN1->RF0R & 0x03;   /* ����FIFO0�б��ĸ��� */
	}
	else
	{
		return CAN1->RF1R & 0x03;   /* ����FIFO1�б��ĸ��� */
	}
}

/**
 * @brief       CAN����һ����Ϣ
 * @param       fifox   : FIFO��� 0, FIFO0; 1, FIFO1;
 * @param       id      : ��׼ID(11λ)/��չID(11λ+18λ)
 * @param       ide     : 0,��׼֡;1,��չ֡
 * @param       rtr     : 0,����֡;1,Զ��֡
 * @param       len     : ���յ������ݳ���(һ��Ϊ8���ֽ�)
 * @param       data    : ���ݻ�����
 * @retval      ��
 */
static void can_rx_msg(uint8_t fifox, uint32_t *id, uint8_t *ide, uint8_t *rtr, uint8_t *len, uint8_t *data)
{
	*ide = CAN1->sFIFOMailBox[fifox].RIR & 0x04;    /* �õ���ʶ��ѡ��λ��ֵ */

	if (*ide == 0)  /* ��׼��ʶ�� */
	{
			*id = CAN1->sFIFOMailBox[fifox].RIR >> 21;
	}
	else            /* ��չ��ʶ�� */
	{
			*id = CAN1->sFIFOMailBox[fifox].RIR >> 3;
	}

	*rtr = CAN1->sFIFOMailBox[fifox].RIR & 0x02;    /* �õ�Զ�̷�������ֵ */
	*len = CAN1->sFIFOMailBox[fifox].RDTR & 0x0F;   /* �õ�DLC[3:0] */

	/* �������� */
	data[0] = CAN1->sFIFOMailBox[fifox].RDLR & 0XFF;
	data[1] = (CAN1->sFIFOMailBox[fifox].RDLR >> 8) & 0XFF;
	data[2] = (CAN1->sFIFOMailBox[fifox].RDLR >> 16) & 0XFF;
	data[3] = (CAN1->sFIFOMailBox[fifox].RDLR >> 24) & 0XFF;
	data[4] = CAN1->sFIFOMailBox[fifox].RDHR & 0XFF;
	data[5] = (CAN1->sFIFOMailBox[fifox].RDHR >> 8) & 0XFF;
	data[6] = (CAN1->sFIFOMailBox[fifox].RDHR >> 16) & 0XFF;
	data[7] = (CAN1->sFIFOMailBox[fifox].RDHR >> 24) & 0XFF;

	if (fifox == 0)
	{
			CAN1->RF0R |= 0X20; /* �ͷ�FIFO0���� */
	}
	else
	{
			CAN1->RF1R |= 0X20; /* �ͷ�FIFO1���� */
	}
}

#if CAN_RX0_INT_ENABLE      /* ʹ��RX0�ж� */

/**
 * @brief       CAN RX0 �жϷ�����
 *   @note      ����CAN FIFO0�Ľ����ж�
 * @param       ��
 * @retval      ��
 */
void CAN1_RX0_IRQHandler(void)
{
	uint8_t rxbuf[8];
	uint32_t id;
	uint8_t ide, rtr, len;
	can_rx_msg(0, &id, &ide, &rtr, &len, rxbuf);
	printf("id:%d\r\n", id);
	printf("ide:%d\r\n", ide);
	printf("rtr:%d\r\n", rtr);
	printf("len:%d\r\n", len);
	printf("rxbuf[0]:%d\r\n", rxbuf[0]);
	printf("rxbuf[1]:%d\r\n", rxbuf[1]);
	printf("rxbuf[2]:%d\r\n", rxbuf[2]);
	printf("rxbuf[3]:%d\r\n", rxbuf[3]);
	printf("rxbuf[4]:%d\r\n", rxbuf[4]);
	printf("rxbuf[5]:%d\r\n", rxbuf[5]);
	printf("rxbuf[6]:%d\r\n", rxbuf[6]);
	printf("rxbuf[7]:%d\r\n", rxbuf[7]);
}

#endif

/**
 * @brief       CAN ����һ������
 *   @note      ���͸�ʽ�̶�Ϊ: ��׼ID, ����֡
 * @param       id      : ��׼ID(11λ)
 * @retval      ����״̬ 0, �ɹ�; 1, ʧ��;
 */
uint8_t can_send_msg(uint32_t id, uint8_t *msg, uint8_t len)
{
	uint8_t mbox;
	uint16_t t = 0;
	uint8_t rval = 1;   /* Ĭ�ϱ�Ƿ���ʧ�� */
	mbox = can_tx_msg(id, 0, 0, len, msg);  /* ������Ϣ */

	while (t < 0XFFF)
	{
		if(can_tx_status(mbox) == 0X07)     /* ������� */
		{
				rval = 0;   /* ��ǳɹ� */
				break;
		}

		t++;
	}

	return rval;        /* ���ط��ͽ�� */
}

/**
 * @brief       CAN �������ݲ�ѯ
 *   @note      �������ݸ�ʽ�̶�Ϊ: ��׼ID, ����֡
 * @param       id      : Ҫ��ѯ�� ��׼ID(11λ)
 * @param       buf     : ���ݻ�����
 * @retval      ���ս��
 *   @arg       0   , �����ݱ����յ�;
 *   @arg       ����, ���յ����ݳ���
 */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf)
{
	uint32_t rid = 0;
	uint8_t ide, rtr, len = 0;

	if (can_pend_msg(0) == 0)return 0;          /* û�н��յ�����,ֱ���˳� */

	can_rx_msg(0, &rid, &ide, &rtr, &len, buf); /* ��ȡ���� */

	if (rid!= id || ide != 0 || rtr != 0)       /* ���յ���ID���� / ���Ǳ�׼֡ / ��������֡ */
	{
			len = 0;    /* ���մ��� */
	}

	return len;
}














