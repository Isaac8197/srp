/*
 * uart.h
 *  Created on: 15/11/2021
 *      Author: Alcides Ramos
 */

#ifndef LIBRERIAS_UART_H_
#define LIBRERIAS_UART_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* Si el módulo UART de HAL está habilitado, exponemos las funciones reales.
 * Si NO está habilitado, damos stubs no-op para que el proyecto compile.
 */
#ifdef HAL_UART_MODULE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

void    uartx_write      (UART_HandleTypeDef *huart, uint8_t ch);
void    uartx_write_text (UART_HandleTypeDef *huart, uint8_t *info);
uint8_t uartx_read       (UART_HandleTypeDef *huart);
void    uartx_read_text  (UART_HandleTypeDef *huart, uint8_t *info,
                          const uint8_t final, uint8_t cuanto);

#ifdef __cplusplus
}
#endif

#else  /* !HAL_UART_MODULE_ENABLED -> STUBS COMPILABLES */

static inline void uartx_write(void *huart, uint8_t ch) {
  (void)huart; (void)ch;
}

static inline void uartx_write_text(void *huart, uint8_t *info) {
  (void)huart; (void)info;
}

static inline uint8_t uartx_read(void *huart) {
  (void)huart; return 0;
}

static inline void uartx_read_text(void *huart, uint8_t *info,
                                   const uint8_t final, uint8_t cuanto) {
  (void)huart; (void)info; (void)final; (void)cuanto;
}

#endif /* HAL_UART_MODULE_ENABLED */

#endif /* LIBRERIAS_UART_H_ */
