#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern uint8_t TIM3_IRQ_Flag;
extern uint32_t PWM_ARR;

void TIM3_IRQHandler(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM14_PWM_Init(u32 arr,u32 psc);
void TIM9_PWM_Init(u32 arr,u32 psc);
void Set_PWM_Duty(TIM_TypeDef *TIM,uint8_t Duty);


#endif
