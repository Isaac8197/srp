/*
 * max6675.c
 */

#include "max6675.h"
#include <math.h>

/* Pulso de reloj y lectura de SO (devuelve 0/1) */
static inline uint8_t MAX6675_pulso_read(void) {
    /* CK = 0 -> 1, samplea durante flanco */
    HAL_GPIO_WritePin(CK_MAX_GPIO_Port, CK_MAX_Pin, GPIO_PIN_RESET);
    /* pequeño hold si fuese necesario (MAX6675 admite hasta 4.3MHz, con HAL suele sobrar) */
    __NOP(); __NOP(); __NOP();
    HAL_GPIO_WritePin(CK_MAX_GPIO_Port, CK_MAX_Pin, GPIO_PIN_SET);
    /* leer SO */
    uint8_t bit = (uint8_t)HAL_GPIO_ReadPin(DAT_MAX_GPIO_Port, DAT_MAX_Pin);
    return bit ? 1u : 0u;
}

void MAX6675_init(void)
{
    /* CK a 0 */
    HAL_GPIO_WritePin(CK_MAX_GPIO_Port, CK_MAX_Pin, GPIO_PIN_RESET);
    /* Todos los CS en alto (inactivos) */
    HAL_GPIO_WritePin(CS1_MAX_GPIO_Port, CS1_MAX_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS2_MAX_GPIO_Port, CS2_MAX_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS3_MAX_GPIO_Port, CS3_MAX_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
}

/* Lectura genérica con CS parametrizado */
float MAX6675_lee_cs(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    uint16_t raw = 0;

    /* Selecciona el chip */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    /* Pequeño delay de setup si se desea */
    __NOP(); __NOP(); __NOP();

    /* 16 bits: [D15..D0]; D2=1 -> TC abierto */
    for (int8_t i = 15; i >= 0; i--) {
        uint8_t b = MAX6675_pulso_read();
        raw |= ((uint16_t)b << i);
    }

    /* Des-selecciona el chip */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    /* Error de termocupla abierta (bit D2 = 1) */
    if (raw & 0x0004) {
        return NAN;
    }

    /* Temperatura = (raw >> 3) * 0.25 °C */
    uint16_t data = (raw >> 3) & 0x0FFF;
    return (float)data * 0.25f;
}

/* Compatibilidad: usa CS1 */
float MAX6675_lee(void)
{
    return MAX6675_lee_cs(CS1_MAX_GPIO_Port, CS1_MAX_Pin);
}
