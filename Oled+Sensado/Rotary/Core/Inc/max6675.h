/*
 * max6675.h
 */

#ifndef LIBRERIAS_MAX6675_H_
#define LIBRERIAS_MAX6675_H_

#include "main.h"
#include <stdint.h>

void  MAX6675_init(void);
/* Lee usando el CS que se pase por argumento */
float MAX6675_lee_cs(GPIO_TypeDef *cs_port, uint16_t cs_pin);
/* Opción por defecto (CS1) */
float MAX6675_lee(void);

#endif /* LIBRERIAS_MAX6675_H_ */
