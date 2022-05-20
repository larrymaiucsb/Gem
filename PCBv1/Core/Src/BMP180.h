/*
 * BMP180.h
 *
 *  Created on: Apr 7, 2022
 *      Author: Michael
 */

#ifndef SRC_BMP180_H_
#define SRC_BMP180_H_

#include "stm32l0xx_hal.h"

void dataCalibration();
uint16_t utemp();
uint32_t getTemp();
uint32_t upress(int s);
float getPressure(int s);
float getAlt(int s);

#endif /* SRC_BMP180_H_ */
