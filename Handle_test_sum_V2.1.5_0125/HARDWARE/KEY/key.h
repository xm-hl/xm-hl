#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

#define KEY1 PDin(8)		//PD8��ǰ��
#define KEY2 PDin(9)		//PD9������
#define KEY3 PDin(10)		//PD10�����Һ
#define KEY4 PDin(11)		//PD11������
//#define KEY5 PEin(7)		//PE7������
#define KEY6 PEin(6)		//PE6����λ
//#define KEY7 PEin(0)		//PE0����ͣ
//��ͣ����������
#define KEY5 PEin(0)		//PE7������
#define KEY7 PEin(7)		//PE0����ͣ

#define KEY8 PGin(3)		//PG3��������

void KEY_Init(void);				//IO��ʼ��
void KEY_Scan(uint8_t *BUFF);	//����ɨ�躯��

#endif
