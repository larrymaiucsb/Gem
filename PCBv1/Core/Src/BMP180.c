/*
 * BMP180.c
 *
 *  Created on: Apr 7, 2022
 *      Author: Michael
 */
#include "stm32l0xx_hal.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;

short AC1;
short AC2;
short AC3;
unsigned short AC4;
unsigned short AC5;
unsigned short AC6;
short B1;
short B2;
short MB;
short MC;
short MD;

long UT = 0;
short oss = 0;
long UP, X1, X2, X3, B3, B5 = 0;
unsigned long B4 = 0;
long B6 = 0;
unsigned long B7 = 0;
long Pressure = 0;
long TE = 0;

void dataCalibration() {
	uint8_t dataCalibration[22] = {0};
	uint16_t startCalibration = 0xAA;
	HAL_I2C_Mem_Read(&hi2c1, 0xEE, startCalibration, 1, dataCalibration, 22, HAL_MAX_DELAY);
	AC1 = ((dataCalibration[0] << 8) | dataCalibration[1]);
	AC2 = ((dataCalibration[2] << 8) | dataCalibration[3]);
	AC3 = ((dataCalibration[4] << 8) | dataCalibration[5]);
	AC4 = ((dataCalibration[6] << 8) | dataCalibration[7]);
	AC5 = ((dataCalibration[8] << 8) | dataCalibration[9]);
	AC6 = ((dataCalibration[10] << 8) | dataCalibration[11]);
	B1 = ((dataCalibration[12] << 8) | dataCalibration[13]);
	B2 = ((dataCalibration[14] << 8) | dataCalibration[15]);
	MB = ((dataCalibration[16] << 8) | dataCalibration[17]);
	MC = ((dataCalibration[18] << 8) | dataCalibration[19]);
	MD = ((dataCalibration[20] << 8) | dataCalibration[21]);
}

uint16_t utemp() {
	uint8_t data = 0x2E;
	uint8_t rawtemp[2] = {0};
	HAL_I2C_Mem_Write(&hi2c1, 0xEE, 0xF4, 1, &data, 1, 1000);
	HAL_Delay(5);
	HAL_I2C_Mem_Read(&hi2c1, 0xEE, 0xF6, 1, rawtemp, 2, 1000);
	return (rawtemp[0] << 8) + rawtemp[1];
}

uint16_t getTemp() {
	UT = utemp();
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	TE = (B5+8)/(pow(2,4));
	return TE/10.0;
}

uint32_t upress(int s) {
	uint8_t data = 0x34 + (s << 6);
	uint8_t rawpress[3] = {0};
	HAL_I2C_Mem_Write(&hi2c1, 0xEE, 0xF4, 1, &data, 1, 1000);
	switch(s) {
	case(0):
			HAL_Delay(5);
	break;
	case(1):
			HAL_Delay(8);
	break;
	case(2):
			HAL_Delay(14);
	break;
	case(3):
			HAL_Delay(26);
	break;
	}
	HAL_I2C_Mem_Read(&hi2c1, 0xEE, 0xF6, 1, rawpress, 3, 1000);
	return (((rawpress[0] << 16) + (rawpress[1] << 8) + rawpress[2]) >> (8-s));
}

float getPressure(int s) {
	UP = upress(s);
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	B6 = B5-4000;
	X1 = (B2 * (B6*B6/(pow(2,12))))/(pow(2,11));
	X2 = AC2*B6/(pow(2,11));
	X3 = X1+X2;
	B3 = (((AC1*4+X3)<<oss)+2)/4;
	X1 = AC3*B6/pow(2,13);
	X2 = (B1 * (B6*B6/(pow(2,12))))/(pow(2,16));
	X3 = ((X1+X2)+2)/pow(2,2);
	B4 = AC4*(unsigned long)(X3+32768)/(pow(2,15));
	B7 = ((unsigned long)UP-B3)*(50000>>s);
	if(B7 < 0x80000000) {
		Pressure = (B7*2) / B4;
	} else {
		Pressure = (B7/B4)*2;
	}
	X1 = (Pressure/(pow(2,8)))*(Pressure/(pow(2,8)));
	X1 = (X1*3038)/(pow(2,16));
	X2 = (-7357*Pressure)/(pow(2,16));
	Pressure = Pressure + (X1+X2+3791)/(pow(2,4));
	return Pressure;
}

float getAlt(int s) {
	getPressure(s);
	return 44330*(1-(pow((Pressure/(float)101325), 0.19029495718)));
}

