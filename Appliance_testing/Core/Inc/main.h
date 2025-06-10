/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_4
#define LED_GPIO_Port GPIOA
#define Switch_Led_Pin GPIO_PIN_5
#define Switch_Led_GPIO_Port GPIOA
#define MDINA_Pin GPIO_PIN_6
#define MDINA_GPIO_Port GPIOA
#define MDINB_Pin GPIO_PIN_7
#define MDINB_GPIO_Port GPIOA
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOB
#define IQR_Pin GPIO_PIN_3
#define IQR_GPIO_Port GPIOB
#define CSN_Pin GPIO_PIN_4
#define CSN_GPIO_Port GPIOB
#define CS2_Pin GPIO_PIN_5
#define CS2_GPIO_Port GPIOB
#define CS1_Pin GPIO_PIN_6
#define CS1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
