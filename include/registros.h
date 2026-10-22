#ifndef REGISTROS_H
#define REGISTROS_H

#include <stdint.h>

#define CANT_REGISTROS 32

/* Códigos de registros de la Máquina Virtual */

// Control de Instrucción
#define REGIP   0   // Instruction Pointer
#define REGOPC  1   // Operation Code
#define REGOP1  2   // Operando 1
#define REGOP2  3   // Operando 2

// Acceso a Memoria
#define REGLAR  4   // Logic Address Register
#define REGMAR  5   // Memory Address Register
#define REGMBR  6   // Memory Buffer Register

// Registros de Propósito General
#define REGEAX 10
#define REGEBX 11
#define REGECX 12
#define REGEDX 13
#define REGEEX 14
#define REGEFX 15

// Acumulador y Banderas
#define REGAC  16   // Accumulator
#define REGCC  17   // Condition Code (Flags N, Z, C, V)

// Segmentos
#define REGCS  26   // Code Segment pointer
#define REGDS  27   // Data Segment pointer

typedef uint32_t Registros[CANT_REGISTROS];

/**
 * Inicializa los 32 registros en 0, y configura CS, DS e IP según la especificación.
 *
 * - CS: 16 bits altos = índice de segmento CODE (0), 16 bits bajos = 0.
 * - DS: 16 bits altos = índice de segmento DATA (1), 16 bits bajos = 0.
 * - IP: igual al valor inicial de CS.
 */
void inicializarRegistros(Registros r);

#endif // REGISTROS_H
