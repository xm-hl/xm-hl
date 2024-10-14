
#ifndef __CAN_H
#define __CAN_H

#include "sys.h"


/* CAN接收RX0中断使能 */
#define CAN_RX0_INT_ENABLE      0               /* 0,不使能; 1,使能; */

/* 静态函数, 仅can.c可以调用 */
static uint8_t can_pend_msg(uint8_t fifox);     /* CAN 查询数据 */
static uint8_t can_tx_status(uint8_t mbox);     /* CAN 查询发送状态 */
static uint8_t can_tx_msg(uint32_t id, uint8_t ide, uint8_t rtr, uint8_t len, uint8_t *data);
static void can_rx_msg(uint8_t fifox, uint32_t *id, uint8_t *ide, uint8_t *rtr, uint8_t *len, uint8_t *data);

/* 对外接口函数 */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf);             /* CAN接收数据, 查询 */
uint8_t can_send_msg(uint32_t id, uint8_t *msg, uint8_t len);   /* CAN发送数据 */
uint8_t CAN_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode); /* CAN初始化 */

#endif

















