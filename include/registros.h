#ifndef REGISTROS_H
#define REGISTROS_H

#include <stdint.h>
#include "consola.h"

#define CANT_REGISTROS 32

/** Codigos de registros de la Maquina Virtual */

/// Control de Instruccion
#define REGIP   0   // Instruction Pointer
#define REGOPC  1   // Operation Code
#define REGOP1  2   // Operando 1
#define REGOP2  3   // Operando 2

/// Acceso a Memoria
#define REGLAR  4   // Logic Address Register
#define REGMAR  5   // Memory Address Register
#define REGMBR  6   // Memory Buffer Register

/// Pila
#define REGSP   7   // Stack Pointer
#define REGBP   8   // Base Pointer

/// Registros de Proposito General
#define REGEAX 10
#define REGEBX 11
#define REGECX 12
#define REGEDX 13
#define REGEEX 14
#define REGEFX 15

/// Acumulador y Banderas
#define REGAC  16   // Accumulator
#define REGCC  17   // Condition Code (Flags N, Z, C, V)

/// Segmentos
#define REGCS  26   // Code Segment pointer
#define REGDS  27   // Data Segment pointer
#define REGES  28   // Extra Segment pointer
#define REGSS  29   // Stack Segment pointer
#define REGKS  30   // Const Segment pointer
#define REGPS  31   // Param Segment pointer

/**
 * Los registros son variables de 32 bits.
 * Para los registros de segmento y punteros (CS, DS, ES, SS, KS, PS, SP, IP):
 * - Los 16 bits mas significativos guardan el numero de segmento (indice en tabla_segmentos).
 * - Los 16 bits menos significativos guardan el desplazamiento (offset dentro del segmento).
 */
typedef uint32_t Registros[CANT_REGISTROS];

/**
 * Inicializa los 32 registros en 0, y configura CS, DS e IP segun la especificacion MV1.
 *
 * - CS: 16 bits altos = indice de segmento CODE (0), 16 bits bajos = 0.
 * - DS: 16 bits altos = indice de segmento DATA (1), 16 bits bajos = 0.
 * - IP: igual al valor inicial de CS.
 */
void inicializarRegistrosV1(Registros registros);

/**
 * Inicializa los 32 registros en 0 para un proceso MV2:
 * - CS, DS, ES, SS, KS, PS: 16 bits altos = indice del descriptor en la tabla, 16 bits bajos = 0.
 *   Si el segmento no existe (tamano 0), el registro queda cargado con -1 (0xFFFFFFFF).
 * - SP: 16 bits altos = indice de SS, 16 bits bajos = tamano de stack (tope inicial fuera del stack).
 * - IP: 16 bits altos = indice de CS, 16 bits bajos = offset de entry point.
 */
void inicializarRegistrosV2(Registros registros, const uint32_t tablaSegmentos[], const ConfiguracionMV *cfg);

/**
 * Muestra por consola los punteros cargados en los registros de segmentos
 */
void mostrarRegistros(const Registros registros);

void cargarRegistrosVMI(const uint8_t *bytes, Registros registros);

#endif // REGISTROS_H
