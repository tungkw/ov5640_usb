#ifndef __LED_H
#define __LED_H

#include "main.h"

typedef struct{
	GPIO_TypeDef * 	GPIO;
	uint16_t				Pin;
}HW_LED_HandleTypeDef;

typedef enum{
	HW_LED_ON=0,
	HW_LED_OFF
}HW_LED_STATE;

void HW_LED_set(HW_LED_HandleTypeDef*, HW_LED_STATE);
void HW_LED_turn(HW_LED_HandleTypeDef*);

#endif
