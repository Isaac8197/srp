#include "max6675.h"
#include <math.h>

uint8_t MAX6675_pulso(void)
{
    uint8_t bit;
    HAL_GPIO_WritePin(CK_MAX_GPIO_Port, CK_MAX_Pin, GPIO_PIN_SET);
    Delay_us(10);
    bit = (uint8_t)HAL_GPIO_ReadPin(DAT_MAX_GPIO_Port, DAT_MAX_Pin);
    HAL_GPIO_WritePin(CK_MAX_GPIO_Port, CK_MAX_Pin, GPIO_PIN_RESET);
    Delay_us(10);
    return bit;
}

void MAX6675_init(void)
{
    HAL_GPIO_WritePin(CK_MAX_GPIO_Port,  CK_MAX_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CS_MAX_GPIO_Port,  CS_MAX_Pin,  GPIO_PIN_SET);
#ifdef CS2_MAX_Pin
    HAL_GPIO_WritePin(CS2_MAX_GPIO_Port, CS2_MAX_Pin, GPIO_PIN_SET);
#endif
#ifdef CS3_MAX_Pin
    HAL_GPIO_WritePin(CS3_MAX_GPIO_Port, CS3_MAX_Pin, GPIO_PIN_SET);
#endif
    HAL_Delay(100);
}

float MAX6675_lee_cs(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    uint16_t raw = 0;

    /* Subir todos los CS; bajar solo el solicitado */
    HAL_GPIO_WritePin(CS_MAX_GPIO_Port,  CS_MAX_Pin,  GPIO_PIN_SET);
#ifdef CS2_MAX_Pin
    HAL_GPIO_WritePin(CS2_MAX_GPIO_Port, CS2_MAX_Pin, GPIO_PIN_SET);
#endif
#ifdef CS3_MAX_Pin
    HAL_GPIO_WritePin(CS3_MAX_GPIO_Port, CS3_MAX_Pin, GPIO_PIN_SET);
#endif

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);

    for (int8_t i = 15; i >= 0; i--) {
        if (MAX6675_pulso()) raw |= (uint16_t)(1u << i);
    }

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    if (raw & 0x0004u) return NAN;  /* termocupla abierta */

    raw >>= 3;
    return (float)raw * 0.25f;
}

float MAX6675_lee(void)
{
    return MAX6675_lee_cs(CS_MAX_GPIO_Port, CS_MAX_Pin);
}
