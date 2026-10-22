#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "decodificador.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "traductor.h"
#include "alu.h"

// Máscaras de bits para evaluar flags de CC en instrucciones de salto condicional
#define NMASK (1u << CC_BIT_N)
#define ZMASK (1u << CC_BIT_Z)
#define CMASK (1u << CC_BIT_C)
#define VMASK (1u << CC_BIT_V)

/**
 * @brief Estructura que representa una instrucción completa decodificada.
 */
typedef struct {
    int opcode;
    int categoria;
    dirFisica dirInicio;
    int largoTotal;
    Operando operandoA;
    Operando operandoB;
} Instruccion;

uint32_t armarRegistroOperando(int tipo, uint8_t *bytes);
Instruccion buscarInstruccion(tabla_segmentos t, Memoria m, Registros r);
uint32_t leerValorOperando(Operando o, tabla_segmentos t, Memoria m, Registros r);
void escribirValorOperando(Operando o, uint32_t valor, tabla_segmentos t, Memoria m, Registros r);
int hayMasInstrucciones(tabla_segmentos t, Registros r);
void saltarA(uint32_t desplazamiento, Registros r);
void ejecutarInstruccion(Instruccion instr, tabla_segmentos t, Memoria m, Registros r);
void ejecutarPrograma(tabla_segmentos t, Memoria m, Registros r, int flagD);
uint32_t LDH(uint32_t b, uint16_t h);
uint32_t LDL(uint32_t b, uint16_t l);

#endif // CPU_H
