
#ifndef __CAN_H
#define __CAN_H

#include "sys.h"


/* CAN����RX0�ж�ʹ�� */
#define CAN_RX0_INT_ENABLE      0               /* 0,��ʹ��; 1,ʹ��; */

/* ��̬����, ��can.c���Ե��� */
static uint8_t can_pend_msg(uint8_t fifox);     /* CAN ��ѯ���� */
static uint8_t can_tx_status(uint8_t mbox);     /* CAN ��ѯ����״̬ */
static uint8_t can_tx_msg(uint32_t id, uint8_t ide, uint8_t rtr, uint8_t len, uint8_t *data);
static void can_rx_msg(uint8_t fifox, uint32_t *id, uint8_t *ide, uint8_t *rtr, uint8_t *len, uint8_t *data);

/* ����ӿں��� */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf);             /* CAN��������, ��ѯ */
uint8_t can_send_msg(uint32_t id, uint8_t *msg, uint8_t len);   /* CAN�������� */
uint8_t CAN_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode); /* CAN��ʼ�� */

#endif

















