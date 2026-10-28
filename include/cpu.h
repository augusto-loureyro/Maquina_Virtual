#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "decodificador.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "traductor.h"
#include "alu.h"
#include "pila.h"

// Mascaras de bits para evaluar flags de CC en instrucciones de salto condicional
#define NMASK (1u << CC_BIT_N)
#define ZMASK (1u << CC_BIT_Z)
#define CMASK (1u << CC_BIT_C)
#define VMASK (1u << CC_BIT_V)


typedef struct {
    int opcode;
    int categoria;
    dirFisica dirInicio;
    int largoTotal;
    Operando operandoA;
    Operando operandoB;
} Instruccion;

uint32_t armarRegistroOperando(int tipo, uint8_t *bytes);
Instruccion buscarInstruccion(tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros);
uint32_t leerValorOperando(Operando operando, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros);
void escribirValorOperando(Operando operando, uint32_t valor, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros);
int hayMasInstrucciones(tabla_segmentos tablaSegmentos, Registros registros);
void saltarA(uint32_t desplazamiento, Registros registros);
void ejecutarInstruccion(Instruccion instr, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros, ConfiguracionMV *cfg, int *breakpoint);
void ejecutarPrograma(tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros, ConfiguracionMV *cfg);
uint32_t LDH(uint32_t valorBase, uint16_t parteAlta);
uint32_t LDL(uint32_t valorBase, uint16_t parteBaja);

#endif // CPU_H
