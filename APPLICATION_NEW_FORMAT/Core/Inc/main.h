/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define TURN_ON_LED_MCU()			HAL_GPIO_WritePin(LED_MCU_GPIO_Port, LED_MCU_Pin, GPIO_PIN_SET)
#define TURN_OFF_LED_MCU()			HAL_GPIO_WritePin(LED_MCU_GPIO_Port, LED_MCU_Pin, GPIO_PIN_RESET);
#define TURN_ON_LED_GSM()			HAL_GPIO_WritePin(LED_GSM_GPIO_Port, LED_GSM_Pin, GPIO_PIN_SET)
#define TURN_OFF_LED_GSM()			HAL_GPIO_WritePin(LED_GSM_GPIO_Port, LED_GSM_Pin, GPIO_PIN_RESET);

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BATTERY_ENABLE_Pin GPIO_PIN_13
#define BATTERY_ENABLE_GPIO_Port GPIOC
#define SIM7600_RI_Pin GPIO_PIN_0
#define SIM7600_RI_GPIO_Port GPIOA
#define SIM7600_RI_EXTI_IRQn EXTI0_IRQn
#define BATTERY_IN_Pin GPIO_PIN_1
#define BATTERY_IN_GPIO_Port GPIOA
#define LOG_TX_Pin GPIO_PIN_2
#define LOG_TX_GPIO_Port GPIOA
#define LOG_RX_Pin GPIO_PIN_3
#define LOG_RX_GPIO_Port GPIOA
#define SIM7600_RESET_Pin GPIO_PIN_4
#define SIM7600_RESET_GPIO_Port GPIOA
#define SIM7600_PWRKEY_Pin GPIO_PIN_5
#define SIM7600_PWRKEY_GPIO_Port GPIOA
#define LED_MCU_Pin GPIO_PIN_6
#define LED_MCU_GPIO_Port GPIOA
#define LED_GSM_Pin GPIO_PIN_7
#define LED_GSM_GPIO_Port GPIOA
#define SIM7600_FLIGHT_Pin GPIO_PIN_0
#define SIM7600_FLIGHT_GPIO_Port GPIOB
#define RAG_SENSOR_Pin GPIO_PIN_1
#define RAG_SENSOR_GPIO_Port GPIOB
#define RAG_SENSOR_EXTI_IRQn EXTI1_IRQn
#define SIM7600_DTR_Pin GPIO_PIN_2
#define SIM7600_DTR_GPIO_Port GPIOB
#define SIM7600_TX_Pin GPIO_PIN_10
#define SIM7600_TX_GPIO_Port GPIOB
#define SIM7600_RX_Pin GPIO_PIN_11
#define SIM7600_RX_GPIO_Port GPIOB
#define SD_CS_Pin GPIO_PIN_12
#define SD_CS_GPIO_Port GPIOB
#define SD_SCK_Pin GPIO_PIN_13
#define SD_SCK_GPIO_Port GPIOB
#define SD_MISO_Pin GPIO_PIN_14
#define SD_MISO_GPIO_Port GPIOB
#define SD_MOSI_Pin GPIO_PIN_15
#define SD_MOSI_GPIO_Port GPIOB
#define SD_DETECT_Pin GPIO_PIN_8
#define SD_DETECT_GPIO_Port GPIOA
#define SD_DETECT_EXTI_IRQn EXTI9_5_IRQn
#define CHARGE_PG_Pin GPIO_PIN_4
#define CHARGE_PG_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
