#ifndef ALU_H
#define ALU_H

#include <stdint.h>
#include "registros.h"

// Posición de cada indicador dentro de CC, según el diagrama de la pág. 8.
#define CC_BIT_N 31
#define CC_BIT_Z 30
#define CC_BIT_C 29
#define CC_BIT_V 28

// Suma a + b (32 bits), actualiza CC (N, Z, C, V) y devuelve el resultado.
uint32_t sumar(uint32_t a, uint32_t b, Registros r);

// Actualiza CC solo con N/Z de 'valor', dejando C y V en 0. Para
// instrucciones no aritméticas que igual afectan CC (ej. MOV), según
// la tabla de ejemplos de la pág. 8.
void actualizarFlagsValor(uint32_t valor, Registros r);

#endif