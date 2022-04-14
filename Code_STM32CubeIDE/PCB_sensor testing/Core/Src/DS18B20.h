/*
 * DS18B20.h
 *
 *  Created on: Apr 7, 2022
 *      Author: Michael
 */

#ifndef SRC_DS18B20_H_
#define SRC_DS18B20_H_

void setOutDS(GPIO_TypeDef* GPIOx);
void setInDS(GPIO_TypeDef* GPIOx);
uint8_t dsInit();
void dsWrite(uint8_t data);
uint8_t dsRead();

#endif /* SRC_DS18B20_H_ */
