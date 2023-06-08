/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Chương trình xây dựng lại theo format chung của các bộ datalogger
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

#include <lowlib.h>
#include <midlib.h>

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

CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
SIM_HandleTypeDef *simPtr;
SENSOR_HandleTypeDef *sensorPtr;
POWER_HandleTypeDef *powerPtr;
SDCARD_HandleTypeDef *sdcardPtr;
CLOCK_HandleTypeDef *clockPtr;
CFG_HandleTypeDef *cfgPtr;
Telegram_HandleTypeDef *telePtr;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
void POWER_Init(void);
void SIM_Init(void);
void SENSOR_Init(void);
void CLOCK_Init(void);
void CONFIG_Init(void);
void SDCARD_Init(void);
void TELE_Init(void);

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
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk |
	SCB_SHCSR_MEMFAULTENA_Msk |
	SCB_SHCSR_BUSFAULTENA_Msk);
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
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_RTC_Init();
	MX_SPI2_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_FATFS_Init();
	MX_ADC_Init();
	MX_CRC_Init();
	/* USER CODE BEGIN 2 */
#if TEST

#else
	printf("This is the version 1.0.0 used to test the hardware of RFD board\n"
			"Writter: Hoang Van Binh\n"
			"Date: 27/04/2023\n");
	printf("Power on\n");
	TURN_ON_LED_MCU();
	/* Init power */
	POWER_Init();
	/* Init sim, must be called before CLOCK_Init and SDCARD_Init
	 * because, CLOCK and SDCARD need the time get from internet
	 * using SIM
	 */
	SIM_Init();
	/* Init config, read parameters saved in the eeprom */
	CONFIG_Init();
	/* Init clock, set time and alarm for device */
	CLOCK_Init();
	/* Init sensor, config and choose type of sensor */
	SENSOR_Init();
	/* Init sd card */
	SDCARD_Init();
	/* Init telegram, used for send device status */
	TELE_Init();

	TURN_OFF_LED_MCU();

	printf("Power off\n");
#endif
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1)
	{
#if TEST

#else
		/* Xử lý các trư�?ng hợp như treo SIM, lỗi giao tiếp */
		SIM_process(simPtr);

		/* Xử lý tin nhắn SMS */
		SMS_process();

		/* Xử lý gửi dữ liệu lên máy chủ */
		HTTP_process();

		printf("\nMCU enters LPM\n");
		TURN_OFF_LED_MCU();
		PWR->CR |= PWR_CR_ULP;
		PWR->CR |= PWR_CR_FWU;
		FLASH->ACR |= FLASH_ACR_SLEEP_PD;
		HAL_SuspendTick();
		HAL_PWR_EnableSleepOnExit();
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();
		while ((PWR->CSR & PWR_CSR_VREFINTRDYF) != PWR_CSR_VREFINTRDYF);
		TURN_ON_LED_MCU();
		printf("MCU wakes up from LPM\n");

		/* Gọi lại hàm cấu hình clock, vì ADC sử dụng HSI nên sau chế độ STOP HSI bị dừng */
		SystemClock_Config();
#endif
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
	/** Enables the Clock Security System
	 */
	HAL_RCCEx_EnableLSECSS();
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

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC_Init 1 */

	/* USER CODE END ADC_Init 1 */
	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	hadc.Init.ContinuousConvMode = ENABLE;
	hadc.Init.NbrOfConversion = 1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(&hadc) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC_Init 2 */

	/* USER CODE END ADC_Init 2 */

}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{

	/* USER CODE BEGIN CRC_Init 0 */

	/* USER CODE END CRC_Init 0 */

	/* USER CODE BEGIN CRC_Init 1 */

	/* USER CODE END CRC_Init 1 */
	hcrc.Instance = CRC;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CRC_Init 2 */

	/* USER CODE END CRC_Init 2 */

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
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void)
{

	/* USER CODE BEGIN RTC_Init 0 */
#if 0
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
#endif
	/* USER CODE END RTC_Init 2 */

}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{

	/* USER CODE BEGIN SPI2_Init 0 */

	/* USER CODE END SPI2_Init 0 */

	/* USER CODE BEGIN SPI2_Init 1 */

	/* USER CODE END SPI2_Init 1 */
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */

	/* USER CODE END SPI2_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */
#if 0
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
#endif
	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 9600;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(BATTERY_ENABLE_GPIO_Port, BATTERY_ENABLE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, SIM7600_RESET_Pin | SIM7600_PWRKEY_Pin | LED_MCU_Pin | LED_GSM_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, SIM7600_FLIGHT_Pin | SIM7600_DTR_Pin | SD_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : BATTERY_ENABLE_Pin */
	GPIO_InitStruct.Pin = BATTERY_ENABLE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(BATTERY_ENABLE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PH0 PH1 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/*Configure GPIO pins : SIM7600_RI_Pin SD_DETECT_Pin */
	GPIO_InitStruct.Pin = SIM7600_RI_Pin | SD_DETECT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : SIM7600_RESET_Pin SIM7600_PWRKEY_Pin LED_MCU_Pin LED_GSM_Pin */
	GPIO_InitStruct.Pin = SIM7600_RESET_Pin | SIM7600_PWRKEY_Pin | LED_MCU_Pin | LED_GSM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : SIM7600_FLIGHT_Pin SIM7600_DTR_Pin SD_CS_Pin */
	GPIO_InitStruct.Pin = SIM7600_FLIGHT_Pin | SIM7600_DTR_Pin | SD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : RAG_SENSOR_Pin */
	GPIO_InitStruct.Pin = RAG_SENSOR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(RAG_SENSOR_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PA9 PA10 PA11 PA12
	 PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB3 PB5 PB8 PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : CHARGE_PG_Pin */
	GPIO_InitStruct.Pin = CHARGE_PG_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(CHARGE_PG_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
/**
 * @brief POWER Initialization Function
 * @param None
 * @retval None
 */
void POWER_Init(void)
{
	/* Khởi tạo biến tĩnh để các giá trị khởi tạo không bị mất đi */
	static BATTERY_HandleTypeDef battery = { 0 };

	/* Khởi tạo giá trị cho con tr�?, biến con tr�? là biến toàn cục */
	powerPtr = (POWER_HandleTypeDef*) &battery;

	/* Khởi tạo giá trị của các member của struct */
	battery.hadc = &hadc;
	battery.parent.getPercent = (powerGetPercent) &BATTERY_getBatteryPercent;
	battery.parent.getPowerState = (powerGetPowerState) &BATTERY_getPowerState;
	battery.parent.getVoltage = (powerGetVoltage) &BATTERY_getBatteryVoltage;
	battery.parent.init = (powerInit) &BATTERY_init;
	battery.bq24298.hi2c = &hi2c1;

	/* Khởi tạo nguồn */
	if (THT_ERROR == POWER_init(powerPtr))
	{
		printf("POWER INIT FAIL!\n");
	}
	else
	{
		/* Bật c�? cho phép cảnh báo pin yếu ngay khi khởi động thiết bị */
		POWER_SET_FLAG(powerPtr, POWER_FLAG_WARNEN);
		POWER_getBatPercent(powerPtr);
		printf("Power Voltage: %.2fV\n", POWER_getBatVoltage(powerPtr));
		printf("POWER INIT DONE!\n");
	}
}

/**
 * @brief CLOCK Initialization Function
 * @param None
 * @retval None
 */
void CLOCK_Init(void)
{
	/* Khởi tạo biến tĩnh để các giá trị khởi tạo không bị mất đi */
	static CLOCK_HandleTypeDef clock = { 0 };

	/* Khởi tạo giá trị cho con tr�?, biến con tr�? là biến toàn cục */
	clockPtr = &clock;

	/* Khởi tạo giá trị của các member của struct */
	clock.hrtc = &hrtc;
	clock.readPeriod = 10u;
	clock.sendPeriod = 10u;

	/* Khởi tạo đồng hồ cho thiết bị */
	if (THT_OK == CLOCK_init(clockPtr))
	{
		/* �?�?c cấu hình th�?i gian đ�?c và th�?i gian gửi */
		CLOCK_readEPPROMs(clockPtr);

		/* �?�?c giá trị th�?i gian đã được đồng bộ cho module SIM và đồng
		 * bộ cho RTC của vi đi�?u khiển
		 */
		SIM_DataTypeDef simData = { 0 };
		SIM_getData(simPtr, SIM_DR_TIME, &simData);

		/* Cấu hình th�?i gian cho thiết bị */
		CLOCK_setTime(clockPtr, simData.timezone, strlen(simData.timezone));

		/* Lưu th�?i gian thiết bị khởi động */
		CLOCK_setResetTime(clockPtr);

		/* Cài đặt th�?i gian báo thức đ�?c dữ liệu */
		CLOCK_setAlarm(clockPtr);

		/* Cấu hình các mốc th�?i gian đ�?c trước và th�?i gian gửi */
		CLOCK_setLastAlarm(clockPtr);
		CLOCK_setNextAlarm(clockPtr);
		printf("CLOCK INIT DONE\n");
	}
	else
	{
		printf("CLOCK INIT FAIL\n");
	}
}

/**
 * @brief SENSOR Initialization Function
 * @param None
 * @retval None
 */
void SENSOR_Init(void)
{
	static SENSOR_HandleTypeDef ragSensor;
	sensorPtr = &ragSensor;

	ragSensor.ragPort = RAG_SENSOR_GPIO_Port;
	ragSensor.ragPin = RAG_SENSOR_Pin;
	ragSensor.SR = 0;
	ragSensor.resolution = 0.2f;
	ragSensor.warning[0] = 50u;
	/* Lưu thông tin cấu hình cảm biến trong bộ nhớ EEPROM */
	SENSOR_readEEPROMs(sensorPtr);

	/* �?�?c giá trị mực nước lưu trong thanh ghi backup */
	SENSOR_readBKUPRegister(sensorPtr);
}

/**
 * @brief SIM Initialization Function
 * @param None
 * @retval None
 */
void SIM_Init(void)
{
	/* Khởi tạo biến tĩnh để các giá trị khởi tạo không bị mất đi */
	static SIM7600_HandleTypeDef sim7600 = { 0 };

	/* Khởi tạo giá trị cho con tr�?, biến con tr�? là biến toàn cục */
	simPtr = (SIM_HandleTypeDef*) &sim7600;

	/* Khởi tạo giá trị của các member của struct */
	sim7600.hdma = &hdma_usart2_rx;
	sim7600.huart = &huart2;
	sim7600.smsCnt = 0;
	sim7600.smsPos = 0;
	sim7600.pwrkeyPin = SIM7600_PWRKEY_Pin;
	sim7600.pwrkeyPort = SIM7600_PWRKEY_GPIO_Port;
	sim7600.resetPort = SIM7600_RESET_GPIO_Port;
	sim7600.resetPin = SIM7600_RESET_Pin;
	sim7600.sleepPort = SIM7600_DTR_GPIO_Port;
	sim7600.sleepPin = SIM7600_DTR_Pin;
	sim7600.parent.CSR = 0;
	sim7600.parent.simSendATCommand = (simSendATCommand) &SIM7600_sendATCommand;
	sim7600.parent.simWakeup = (simWakeup) &SIM7600_wakeup;
	sim7600.parent.simSleep = (simSleep) &SIM7600_sleep;
	sim7600.parent.simInit = (simInit) &SIM7600_init;
	sim7600.parent.simSetFunction = (simSetFunction) &SIM7600_setFunction;
	sim7600.parent.simProcess = (simProcess) &SIM7600_process;
	sim7600.parent.simCheckEvent = (simCheckEvent) &SIM7600_checkEvent;
	sim7600.parent.simReadSMS = (simReadSMS) &SIM7600_readSMS;
	sim7600.parent.simSendSMS = (simSendSMS) &SIM7600_sendSMS;
	sim7600.parent.simGetSMSCounter = (simGetSMSCounter) &SIM7600_getSMSCounter;
	sim7600.parent.simCallUSSD = (simCallUSSD) &SIM7600_callUSSD;
	sim7600.parent.simGetRSSI = (simGetRSSI) &SIM7600_getRSSI;
	sim7600.parent.simGetTemperature = (simGetTemperature) &SIM7600_getTemperature;
	sim7600.parent.simGetNwtype = (simGetNwtype) &SIM7600_getNetworkType;
	sim7600.parent.simGetSimPhone = (simGetSimPhone) &SIM7600_getSimPhone;
	sim7600.parent.simGetIMEI = (simGetIMEI) &SIM7600_getIMEI;
	sim7600.parent.simGetLocalTime = (simGetLocalTime) &SIM7600_getLocalTime;
	sim7600.parent.simGetFileSize = (simGetFileSize) &SIM7600_getFileSize;
	sim7600.parent.simDeleteFile = (simDeleteFile) &SIM7600_deleteFile;
	sim7600.parent.simSetLocalTimeAuto = (simSetLocalTimeAuto) &SIM7600_setLocalTimeAuto;
	sim7600.parent.simSetLocalTimeManual = (simSetLocalTimeManual) &SIM7600_setLocalTimeManual;
	sim7600.parent.simSelectPhonebookMem = (simSelectPhonebookMem) &SIM7600_selectPhonebookMem;
	sim7600.parent.simSavePhonebook = (simSavePhonebook) &SIM7600_savePhonebook;
	sim7600.parent.simReadPhonebook = (simReadPhonebook) &SIM7600_readPhonebook;
	sim7600.parent.simFindPhonebook = (simFindPhonebook) &SIM7600_findPhonebook;
	sim7600.parent.simSendHTTP = (simSendHTTP) &SIM7600_sendHTTP;
	sim7600.parent.simGetFileHTTP = (simGetFileHTTP) &SIM7600_getFileHTTP;

	/* Khởi tạo module SIM */
	if (THT_OK != SIM_init(simPtr))
	{
		printf("SIM	INIT FAIL\n");
	}
	else
	{
		SIM_DataTypeDef simData = { 0 };
		/* �?ưa module SIM vào chế độ hoạt động đầy đủ chức năng */
		SIM_setFunction(simPtr, SIM_MODE_FULL);
		SIM_setLocalTimeAuto(simPtr);
		SIM_getData(simPtr, SIM_DR_IMEI | SIM_DR_RSSI | SIM_DR_NWTYPE | SIM_DR_TEMP, &simData);
		printf("SIM INIT DONE\n");
	}
}

/**
 * @brief SDCARD Initialization Function
 * @param None
 * @retval None
 */
void SDCARD_Init(void)
{
	/* Khởi tạo biến tĩnh để các giá trị khởi tạo không bị mất đi */
	static SD_HandleTypeDef sd = { 0 };

	/* Khởi tạo giá trị cho con tr�?, biến con tr�? là biến toàn cục */
	sdcardPtr = (SDCARD_HandleTypeDef*) &sd;

	/* Khởi tạo giá trị của các member của struct */
	sd.parent.sdMount = (sdMount) &SD_mount;
	sd.parent.sdUnmount = (sdUnmount) &SD_unmount;
	sd.parent.sdCreateFile = (sdCreateFile) &SD_createFile;
	sd.parent.sdCreateDir = (sdCreateDir) &SD_createDir;
	sd.parent.sdGetFileSize = (sdGetFileSize) &SD_getFileSize;
	sd.parent.sdUpdateFile = (sdUpdateFile) &SD_updateFile;

	/* Khởi tạo thẻ nhớ */
	if (THT_OK == SDCARD_init(sdcardPtr))
	{
		char pathname[16] = { 0 };

		/* Tạo ra tên đư�?ng dẫn để lưu trữ dữ liệu */
		CLOCK_createPathForSD(clockPtr, pathname, sizeof(pathname));

		/* Tạo ra folder trên thẻ nhớ với đư�?ng dẫn vừa tạo ở trên */
		SDCARD_setCurrentFile(sdcardPtr, pathname);
		printf("SDCARD INIT DONE\n");
	}
}

/**
 * @brief CONFIG Initialization Function
 * @param None
 * @retval None
 */
void CONFIG_Init(void)
{
	/* Khởi tạo biến tĩnh để các giá trị khởi tạo không bị mất đi */
	static CFG_HandleTypeDef cfg = { 0 };

	/* Khởi tạo giá trị cho con tr�?, biến con tr�? là biến toàn cục */
	cfgPtr = &cfg;

	/* �?�?c số điện thoại của thẻ SIM làm ID cho thiết bị */
	SIM_DataTypeDef simData = { 0 };
	SIM_getData(simPtr, SIM_DR_SIMPHONE, &simData);

	/* �?�?c các giá trị đã được lưu trong bộ nhớ EEPROM */
	CFG_readEEPROM(cfgPtr);

	/* Cài đặt ID cho thiết bị */
	CFG_setDeviceID(cfgPtr, simData.simPhone, strlen(simData.simPhone));
}

/**
 * @brief TELE Initialization Function
 * @param None
 * @retval None
 */
void TELE_Init(void)
{
	/* Khởi tạo biến tĩnh để các giá trị khởi tạo không bị mất đi */
	static Telegram_HandleTypeDef tele = { .chatID = { 0 }, .disableNotification = TELEGRAM_FALSE_STRING,
			.disableWebPagePreview = TELEGRAM_FALSE_STRING, .method =
			TELEGRAM_METHOD_SEND_MSG, .parseMode = "HTML", .replyToMessageID =
			TELEGRAM_NULL_STRING, .token = { 0 } };

	/* Khởi tạo giá trị cho con tr�?, biến con tr�? là biến toàn cục */
	telePtr = &tele;

	/* Khởi tạo Telegram */
	TELEGRAM_init(telePtr);

	TELEGRAM_writeEEPROM(telePtr);
}

/**
 * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
		SIM_checkEvent(simPtr, Size);
	}
}

/**
 * @brief  Alarm A callback.
 * @param  hrtc RTC handle
 * @retval None
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	CLOCK_callback(clockPtr);

	HAL_PWR_DisableSleepOnExit();
}

/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Wake up MCU from stop mode by any EXTI line */
	if (GPIO_Pin == RAG_SENSOR_Pin)
	{
		/* Set flag to read LoRa message */
		SENSOR_callback(sensorPtr);
	}

	if (GPIO_Pin == SIM7600_RI_Pin)
	{
		/* Wake up module SIM to read SMS */
		HAL_PWR_DisableSleepOnExit();
	}
}

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart3, (uint8_t*) &ch, 1, 50);
	return ch;
}
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
		NVIC_SystemReset();
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

