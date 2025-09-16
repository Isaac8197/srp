/*
 * uart.c
 *  Created on: 15/11/2021
 *      Author: Alcides Ramos
 */

#include "uart.h"
#include <stdio.h>
#include <string.h>

/* Solo compilar estas funciones si el módulo UART está habilitado.
 * Si no, los stubs del header bastan y este archivo no aporta nada.
 */
#ifdef HAL_UART_MODULE_ENABLED

void uartx_write(UART_HandleTypeDef *huart, uint8_t ch)
{
  HAL_UART_Transmit(huart, &ch, 1, 0xFFFF);
}

void uartx_write_text(UART_HandleTypeDef *huart, uint8_t *info)
{
  while (*info) uartx_write(huart, *info++);
}

uint8_t uartx_read(UART_HandleTypeDef *huart)
{
  uint8_t dato_rx_ = 0;
  HAL_UART_Receive(huart, &dato_rx_, 1, HAL_MAX_DELAY);
  return dato_rx_;
}

void uartx_read_text(UART_HandleTypeDef *huart, uint8_t *info,
                     const uint8_t final, uint8_t cuanto)
{
  uint8_t dato_rx = 0;
  char i = 0;
  memset(info, 0, cuanto);
  while (dato_rx != final) {
    dato_rx = uartx_read(huart);
    *info = dato_rx;
    info++;
    i++;
    if (i > (char)(cuanto - 1)) break;
  }
}

#endif /* HAL_UART_MODULE_ENABLED */
