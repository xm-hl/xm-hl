#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

#define ADC_CH1				1
#define ADC_CH4				4
#define ADC_CH5				5

//===========================================2023-05-11
#define ADC_CH10				10
#define ADC_CH11				11
//===========================================2023-05-11

void ADC1_Init(void);
void Get_ADC_Value(ADC_TypeDef *ch,uint16_t num,uint16_t *BUFF);

#endif
