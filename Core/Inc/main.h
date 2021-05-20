/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "utils.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */


//--------------------------------------------------------------------------
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
//--------------------------------------------------------------------------
//void SetDacA(uint16_t);
//void SetDacB(uint16_t);
void SetDacA(int16_t da);
void SetDacB(int16_t db);
void SetAllDAC();
//--------------------------------------------------------------------------
uint16_t GetDacA();
uint16_t GetDacB();
uint16_t GetADC();
uint8_t GetBtnRunState();
uint8_t GetBtnUpState();
uint8_t GetBtnDownState();
//--------------------------------------------------------------------------
void runCommands(uint8_t *Buf, uint32_t *Len);
//--------------------------------------------------------------------------
uint8_t GetInHL();
uint8_t GetInLL();
void ResInHL();
void ResInLL();
//--------------------------------------------------------------------------
void EnableTIM3_PB4();
void resValTIM3_PB4();

void EnableTIM4_PB6();
void resValTIM4_PB6();
//--------------------------------------------------------------------------
uint32_t getCRC_table_a_m12();
uint32_t getCRC_table_b_m12();
uint32_t getCRC_table_a_m27();
uint32_t getCRC_table_b_m27();

void writeTable();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AD5312_LDAC_Pin GPIO_PIN_1
#define AD5312_LDAC_GPIO_Port GPIOA
#define Relay_Pin GPIO_PIN_2
#define Relay_GPIO_Port GPIOA
#define AD5312_SYNC_Pin GPIO_PIN_4
#define AD5312_SYNC_GPIO_Port GPIOA
#define AD5312_SCLK_Pin GPIO_PIN_5
#define AD5312_SCLK_GPIO_Port GPIOA
#define AD5312_DIN_Pin GPIO_PIN_7
#define AD5312_DIN_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
