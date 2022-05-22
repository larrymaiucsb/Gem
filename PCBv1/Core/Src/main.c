/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "DHT22.h"
#include "DS18B20.h"
#include "BMP180.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART4_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	float DHT_Temp = 0;
	float DHT_Hum = 0;
	float BMP_Temp, BMP_Alt, BMP_Pressure;
	char DHT22_TempString[] = "The External Temperature is ";
	char DHT22_HumString[] = "The Humidity is ";
			  char PHOTO_String[] = "\n\rThe Photoresistor Voltage is "; //270 ohm resistor
			  char DS18B20_TempString[] = "The Internal Temperature is ";
			  char SEN1367_MoistString[] = "\n\rThe Soil Moisture is ";
			  char BMP180_PressString[] = "The Surface Pressure is ";
			  char BMP180_AltString[] = "The Altitude is ";
			  char BMP180_TempString[] = "\n\rThe BMP Temperature is ";
			  char newLine[] = "\n\r";
			  char moistbuffer[50];

	uint8_t t1, t2, h1, h2, p;
	uint16_t temp, hum, sum;

	uint8_t DS18B20_Temp1, DS18B20_Temp2;
	uint16_t DS18B20_T;
	float internalTemp = 0;

	uint16_t light = 0;
	float voltage;

	uint16_t  moisture = 0;
	uint8_t buf[71] = "";
		uint8_t msg0[32] = "\n\rFUll NMEA String: ";
		uint8_t msg1[32] = "\n\rLongitude: ";
		uint8_t msg2[32] = "\n\rLatitude: ";
		uint8_t command[45] = "PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"; //For GGA and RMC information only
		uint8_t skipp[] = "Y";

		char DHT22_TempBuffer[50];
			char DHT22_HumBuffer[50];
			char voltBuffer[50];
			//-----------------------------------------------------------
			//char moistbuffer[50];
			char DS18B20_TempBuffer[50];
			char BMP180_PressBuffer[50];
			char BMP180_AltBuffer[50];
			char BMP180_TempBuffer[50];

			char newLineBuffer[50];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_TIM2_Init();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC_Init();
  MX_USART1_UART_Init();
  MX_USART4_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
 // int __io_putchar(int ch) {
	//  ITM_SendChar(ch);
	  //return ch;
  //}
  dataCalibration();
  HAL_UART_Transmit(&huart1, command, 45, 4000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  strcpy((char*)buf, "");
	  HAL_UART_Receive(&huart1, buf, 71, 4000);
	  HAL_UART_Transmit(&huart1, msg0, 32, 1000);
	  HAL_UART_Transmit(&huart1, buf, 71, 4000);

	  char* LongT = (char*) buf;
	  LongT = LongT + 18;
	  char* LatT = (char*) buf;
	  LatT = LatT + 30;

	  HAL_UART_Transmit(&huart1, msg1, 32, 1000);
	  HAL_UART_Transmit(&huart1, LongT, 11, 1000);
	  HAL_UART_Transmit(&huart1, msg2, 32, 1000);
	  HAL_UART_Transmit(&huart1, LatT, 12, 1000);


	  BMP_Temp = getTemp(); //Get Temperature
	  BMP_Temp = ((BMP_Temp * 9) / 5) + 32; //Convert to F
	  BMP_Pressure = getPressure(0); //Get Pressure
	  BMP_Alt = getAlt(0); //Get Altitude

	  sensorInit(); //Initialize the DHT22
	  p = sensorVerify();
	  if(p) {
		  h1 = sensorRead(); //Read Lower 8 Humidity
		  h2 = sensorRead(); //Read Upper 8 Humidity
		  t1 = sensorRead(); //Read Lower 8 Temperature
		  t2 = sensorRead(); //Read Upper 8 Temperature
		  sum = sensorRead(); //Read Checksum

		  temp = ((t1 << 8) | t2); //Assemble Temperature
	  	  hum = ((h1 << 8 | h2)); //Assemble Humidity
	  	  DHT_Temp = (float) (temp/10.0); //Get Temperature
	  	  DHT_Temp = ((DHT_Temp * 9) / 5) + 32; //Convert from Celsius
	  	  DHT_Hum = (float) (hum/10.0); //Get Humidity
	  	  }

	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	  p = dsInit(); //Initialize the DS18B20
	  if(p) {
		  HAL_Delay(1);
		  dsWrite(0xCC); //Skip ROM Command
		  dsWrite(0x44); //Convert Temperature
	  }
	  p = dsInit(); //Initialize DS18B20 Again
	  if(p) {
		  HAL_Delay(1);
		  dsWrite(0xCC); //Skip ROM Command
		  dsWrite(0xBE); //Read Scratchpad
		  DS18B20_Temp1 = dsRead(); //Read Lower 8
		  DS18B20_Temp2 = dsRead(); //Read Upper 8
		  DS18B20_T = (DS18B20_Temp2 << 8) | DS18B20_Temp1; //Combine
		  internalTemp = (float) DS18B20_T/16; //Get Temperature
		  internalTemp = ((internalTemp * 9) / 5) + 32; //Convert to F
	  }

	  ADC_ChannelConfTypeDef sConfig = {0};
	  sConfig.Channel = ADC_CHANNEL_0;
	  sConfig.Rank = 1;
	 // sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES; //l0 hal has no .samplingtime attribute?
	  if(HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
		  Error_Handler();
	  }
	  HAL_ADC_Start(&hadc); //Start Light ADC
	  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
	  light = HAL_ADC_GetValue(&hadc);
	  voltage = 3.3 * light;
	  voltage = voltage / 4095;


	  ADC_ChannelConfTypeDef sConfig2 = {13};
	  sConfig2.Channel = ADC_CHANNEL_13;
	  //sConfig.Rank = 1;
	 // sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	  if(HAL_ADC_ConfigChannel(&hadc, &sConfig2) != HAL_OK) {
		  Error_Handler();
	  }
	  HAL_ADC_Start(&hadc); //Start Soil ADC
	  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
	  moisture = HAL_ADC_GetValue(&hadc);
	  //gcvt(voltage, 6, voltBuffer);
	 // HAL_Delay(1000);
	//  HAL_UART_Transmit(&huart4, "HELLO WORLD", sizeof("HELLO WORLD"), HAL_MAX_DELAY);
	  //HAL_UART_Transmit(&huart4, "HELLO WORLD", sizeof("HELLO WORLD"), HAL_MAX_DELAY);
	  //HAL_UART_Transmit(&huart4, "HELLO WORLD", sizeof("HELLO WORLD"), HAL_MAX_DELAY);

/*
	  gcvt(BMP_Pressure, 6, BMP180_PressBuffer); //Convert Float to String
	  	  gcvt(BMP_Alt, 6, BMP180_AltBuffer); //Convert Float to String
	  	  gcvt(BMP_Temp, 6, BMP180_TempBuffer); //Convert Float to String
*/

	  	  gcvt(DHT_Temp, 6, DHT22_TempBuffer);
	  	  gcvt(DHT_Hum, 6, DHT22_HumBuffer);
	  	  gcvt(internalTemp, 6, DS18B20_TempBuffer);
	  	 sprintf(moistbuffer, "%hu\r\n", moisture);
	  	//  gcvt(newLine, 6, newLineBuffer);

	  	 // gcvt(buf, 2, buf2);
/*
	  HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_TempString, strlen(DHT22_TempString), HAL_MAX_DELAY);

		//HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_TempString, strlen(DHT22_TempString), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_TempBuffer, strlen(DHT22_TempBuffer), HAL_MAX_DELAY);

		HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart4, (uint8_t*)DS18B20_TempString, strlen(DS18B20_TempString), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)DS18B20_TempBuffer, strlen(DHT22_TempBuffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_HumString, strlen(DHT22_HumString), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_HumBuffer, strlen(DHT22_HumBuffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart4, (uint8_t*)PHOTO_String, strlen(PHOTO_String), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)voltBuffer, strlen(voltBuffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_PressString, strlen(BMP180_PressString), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_PressBuffer, strlen(BMP180_PressBuffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_AltString, strlen(BMP180_AltString), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_AltBuffer, strlen(BMP180_AltBuffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);

		HAL_UART_Transmit(&huart4, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
		*/


	  	//----------------------------------------------------------------------frame format
	  	//@72D47S7400M#
	  	//open: @
	  	//DHT22 - temp: T
	  	//DHT22 - hum: H
	  	//DS18B20: S
	  	//Moisture: M
	  	//close: #

	  	//----------------------------------------------------------------------message frame - head
	  	HAL_UART_Transmit(&huart4, (uint8_t*)"@", strlen("@"), HAL_MAX_DELAY);

	  	//----------------------------------------------------------------------DHT22
	  	//----------------------------------------------------------------------temp
	  	//HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_TempString, strlen(DHT22_TempString), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_TempBuffer, strlen(DHT22_TempBuffer), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)"T", strlen("T"), HAL_MAX_DELAY);

	  	//HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);

	  	//----------------------------------------------------------------------humidity
	  	//HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_HumString, strlen(DHT22_HumString), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)DHT22_HumBuffer, strlen(DHT22_HumBuffer), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)"H", strlen("H"), HAL_MAX_DELAY);

	  	//HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);

	  	//----------------------------------------------------------------------DS18B20
	  	//HAL_UART_Transmit(&huart4, (uint8_t*)DS18B20_TempString, strlen(DS18B20_TempString), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)DS18B20_TempBuffer, strlen(DHT22_TempBuffer), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)"S", strlen("S"), HAL_MAX_DELAY);

	  	//HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);

	  	//----------------------------------------------------------------------BMP180
/*	  	//HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_AltString, strlen(BMP180_AltString), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_AltBuffer, strlen(BMP180_AltBuffer), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);

	  	HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);

	  	//HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_PressString, strlen(BMP180_PressString), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)BMP180_PressBuffer, strlen(BMP180_PressBuffer), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);

	  	HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);

	  	HAL_UART_Transmit(&huart4, buf, strlen(buf), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);

	  	HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);

	  	HAL_UART_Transmit(&huart4, buf, strlen(buf), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)skipp, strlen(skipp), HAL_MAX_DELAY);

	  	HAL_UART_Transmit(&huart4, (uint8_t*)newLine, strlen(newLine), HAL_MAX_DELAY);*/

/*	  	//-----------------------------------------------------------------------Moisture
	  	HAL_UART_Transmit(&huart4, (uint8_t*)moisture, strlen(moistbuffer), HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart4, (uint8_t*)"M", strlen("M"), HAL_MAX_DELAY);*/

	  	//----------------------------------------------------------------------message frame - tail
	  	HAL_UART_Transmit(&huart4, (uint8_t*)"#", strlen("#"), HAL_MAX_DELAY);

	    //HAL_Delay(100);
	    //HAL_Delay(250)
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 24;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC1 PC2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/