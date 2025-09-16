/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* Pines MAX6675 (comparten DAT/CK) — BluePill típico
 * DAT -> PA5
 * CK  -> PA6
 * CS1 -> PA7
 * CS2 -> PA4
 * CS3 -> PA3  (nuevo)
 */
#define DAT_MAX_Pin        GPIO_PIN_5
#define DAT_MAX_GPIO_Port  GPIOA

#define CK_MAX_Pin         GPIO_PIN_6
#define CK_MAX_GPIO_Port   GPIOA

#define CS_MAX_Pin         GPIO_PIN_7          // CS1
#define CS_MAX_GPIO_Port   GPIOA

#define CS2_MAX_Pin        GPIO_PIN_4          // CS2
#define CS2_MAX_GPIO_Port  GPIOA

#define CS3_MAX_Pin        GPIO_PIN_3          // CS3 (nuevo)
#define CS3_MAX_GPIO_Port  GPIOA

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
