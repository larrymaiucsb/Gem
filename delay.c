/*
 * delay.c
 *
 *  Created on: Feb 2, 2022
 *      Author: Michael
 */
#include "stm32l4xx_hal.h"
extern TIM_HandleTypeDef htim1;

void delay(uint16_t time) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while((__HAL_TIM_GET_COUNTER(&htim1)) < time);
}
