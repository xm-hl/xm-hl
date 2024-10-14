#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

#define KEY1 PDin(8)		//PD8，前进
#define KEY2 PDin(9)		//PD9，后退
#define KEY3 PDin(10)		//PD10，抽肺液
#define KEY4 PDin(11)		//PD11，拍照
//#define KEY5 PEin(7)		//PE7，语音
#define KEY6 PEin(6)		//PE6，档位
//#define KEY7 PEin(0)		//PE0，悬停
//悬停跟语音交换
#define KEY5 PEin(0)		//PE7，语音
#define KEY7 PEin(7)		//PE0，悬停

#define KEY8 PGin(3)		//PG3，开机键

void KEY_Init(void);				//IO初始化
void KEY_Scan(uint8_t *BUFF);	//按键扫描函数

#endif
