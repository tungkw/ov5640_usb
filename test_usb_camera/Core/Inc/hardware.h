#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "main.h"
#include "ov5640.h"
#include "led.h"

void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);

extern HW_LED_HandleTypeDef led0;
extern HW_LED_HandleTypeDef led1;
void led_init(void);

extern HW_OV5640_HandleTypeDef ov5640;
void ov5640_init(void);


#endif
