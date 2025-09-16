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

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* ================= Pines MAX6675 (Blue Pill) =================
 * SO / DAT = PA5 (entrada)
 * SCK      = PA6 (salida)
 * CS1      = PA7 (salida)
 * CS2      = PA4 (salida)
 * CS3      = PA3 (salida)
 * ============================================================ */
#define DAT_MAX_Pin        GPIO_PIN_5
#define DAT_MAX_GPIO_Port  GPIOA

#define CK_MAX_Pin         GPIO_PIN_6
#define CK_MAX_GPIO_Port   GPIOA

#define CS1_MAX_Pin        GPIO_PIN_7
#define CS1_MAX_GPIO_Port  GPIOA

#define CS2_MAX_Pin        GPIO_PIN_4
#define CS2_MAX_GPIO_Port  GPIOA

#define CS3_MAX_Pin        GPIO_PIN_3
#define CS3_MAX_GPIO_Port  GPIOA

/* Compatibilidad con código viejo (si alguna unidad lo usa) */
#define CS_MAX_Pin         CS1_MAX_Pin
#define CS_MAX_GPIO_Port   CS1_MAX_GPIO_Port

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
