#ifndef ALU_H
#define ALU_H

#include <stdint.h>
#include "registros.h"

// Posición de cada indicador dentro de CC, según el diagrama de la pág. 8.
#define CC_BIT_N 31
#define CC_BIT_Z 30
#define CC_BIT_C 29
#define CC_BIT_V 28

void actualizarCC(Registros r, int n, int z, int c, int v);

// Suma a + b (32 bits), actualiza CC (N, Z, C, V) y devuelve el resultado.
uint32_t sumar(uint32_t a, uint32_t b, Registros r);
uint32_t restar(uint32_t a, uint32_t b, Registros r);
uint32_t multiplicar(uint32_t a, uint32_t b, Registros r);
uint32_t dividir(uint32_t a, uint32_t b, Registros r);
uint32_t and_logico(uint32_t a, uint32_t b, Registros r);
uint32_t or_logico(uint32_t a, uint32_t b, Registros r);
uint32_t xor_logico(uint32_t a, uint32_t b, Registros r);
uint32_t not_logico(uint32_t a, Registros r);

uint32_t shift_izq(uint32_t a, uint32_t b, Registros r);
uint32_t shift_der(uint32_t a, uint32_t b, Registros r);
uint32_t shift_der_aritmetico(uint32_t a, uint32_t b, Registros r);

// Actualiza CC solo con N/Z de 'valor', dejando C y V en 0. Para instrucciones no aritméticas que igual afectan CC (ej. MOV), según la tabla de ejemplos de la pág. 8.
void actualizarFlagsValor(uint32_t valor, Registros r);

#endif