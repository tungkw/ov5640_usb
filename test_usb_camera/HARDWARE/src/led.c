#include "led.h"

void HW_LED_set(HW_LED_HandleTypeDef* led, HW_LED_STATE state)
{
	HAL_GPIO_WritePin(led->GPIO, led->Pin, (GPIO_PinState)state);
}

void HW_LED_turn(HW_LED_HandleTypeDef* led)
{
//	HAL_GPIO_WritePin(led->GPIO, led->Pin, (GPIO_PinState)!HAL_GPIO_ReadPin(led->GPIO, led->Pin));
	HAL_GPIO_TogglePin(led->GPIO, led->Pin);
}

