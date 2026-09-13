#ifndef CPU_H
#define CPU_H

#include "decodificador.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "traductor.h"
#include "errores.h"

typedef struct {
    int opcode;
    int categoria;
    Operando operandoA;
    Operando operandoB;
} Instruccion;

Instruccion buscarInstruccion(tabla_segmentos t, Memoria m, Registros r);

uint32_t leerValorOperando(Operando o, tabla_segmentos t, Memoria m, Registros r);
void escribirValorOperando(Operando o, uint32_t valor, tabla_segmentos t, Memoria m, Registros r);

int hayMasInstrucciones(tabla_segmentos t, Registros r);
void ejecutarInstruccion(Instruccion instr, tabla_segmentos t, Memoria m, Registros r);
void ejecutarPrograma(tabla_segmentos t, Memoria m, Registros r);

#endif