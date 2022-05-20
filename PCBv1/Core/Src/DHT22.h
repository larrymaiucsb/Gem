/*
 * DHT22.h
 *
 *  Created on: Apr 7, 2022
 *      Author: Michael
 */

#ifndef SRC_DHT22_H_
#define SRC_DHT22_H_

void setOut(GPIO_TypeDef* GPIOx);
void setIn(GPIO_TypeDef* GPIOx);
void sensorInit();
uint8_t sensorRead();
uint8_t sensorVerify();

#endif /* SRC_DHT22_H_ */
