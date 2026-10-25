#ifndef ALU_H
#define ALU_H

#include <stdint.h>
#include "registros.h"

/// Posición de los flags dentro del registro CC (Condition Code)
#define CC_BIT_N 31  // Signo (1 si el resultado es negativo)
#define CC_BIT_Z 30  // Cero (1 si el resultado es cero)
#define CC_BIT_C 29  // Acarreo / Carry (1 si excede 32 bits sin signo)
#define CC_BIT_V 28  // Desbordamiento / Overflow (1 si desborda en complemento a 2)


void actualizarCC(Registros r, int n, int z, int c, int v);
void actualizarFlagsValor(uint32_t valor, Registros r);

/** Operaciones Aritmeticas (32 bits) */
uint32_t sumar(uint32_t a, uint32_t b, Registros r);
uint32_t restar(uint32_t a, uint32_t b, Registros r);
uint32_t multiplicar(uint32_t a, uint32_t b, Registros r);
uint32_t dividir(uint32_t a, uint32_t b, Registros r);

/** Operaciones Logicas Bit a Bit */
uint32_t and_logico(uint32_t a, uint32_t b, Registros r);
uint32_t or_logico(uint32_t a, uint32_t b, Registros r);
uint32_t xor_logico(uint32_t a, uint32_t b, Registros r);
uint32_t not_logico(uint32_t a, Registros r);

/** Desplazamientos */
uint32_t shift_izq(uint32_t a, uint32_t b, Registros r);
uint32_t shift_der(uint32_t a, uint32_t b, Registros r);
uint32_t shift_der_aritmetico(uint32_t a, uint32_t b, Registros r);

#endif // ALU_H
