#ifndef ALU_H
#define ALU_H

#include <stdint.h>
#include "registros.h"

// Posición de los flags dentro del registro CC (Condition Code)
#define CC_BIT_N 31  // Signo (1 si el resultado es negativo)
#define CC_BIT_Z 30  // Cero (1 si el resultado es cero)
#define CC_BIT_C 29  // Acarreo / Carry (1 si excede 32 bits sin signo)
#define CC_BIT_V 28  // Desbordamiento / Overflow (1 si desborda en complemento a 2)


void actualizarCC(Registros registros, int flagN, int flagZ, int flagC, int flagV);
void actualizarFlagsValor(uint32_t valor, Registros registros);

/* Operaciones Aritmeticas (32 bits) */
uint32_t sumar(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t restar(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t multiplicar(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t dividir(uint32_t operandoA, uint32_t operandoB, Registros registros);

/* Operaciones Logicas Bit a Bit */
uint32_t and_logico(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t or_logico(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t xor_logico(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t not_logico(uint32_t operandoA, Registros registros);

/* Desplazamientos */
uint32_t shift_izq(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t shift_der(uint32_t operandoA, uint32_t operandoB, Registros registros);
uint32_t shift_der_aritmetico(uint32_t operandoA, uint32_t operandoB, Registros registros);

#endif // ALU_H
