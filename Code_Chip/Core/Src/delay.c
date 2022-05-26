/*
 * delay.c
 *
 *  Created on: Apr 7, 2022
 *      Author: Michael
 */

#include "stm32l0xx_hal.h"
extern TIM_HandleTypeDef htim2;

void delay(uint16_t time) {
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while((__HAL_TIM_GET_COUNTER(&htim2)) < time);
}
