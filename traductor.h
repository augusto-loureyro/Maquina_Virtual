#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "maquinaVirtual.h"


/// Tipos de operandos
#define OPNONE      0
#define OPREG       1   ///Registro
#define OPIMM       2   ///Inmediatos
#define OPMEM       3   ///Memoria

/*typedef struct {
    uint8_t tipoOpB, tipoOpA, operacion, cantOperand;
    int32_t opAValor, opBValor;
}TRInstruction;*/

/// Decodifica una instruccion desde la memoria.
int obtenerValorOperando(ETMaquinaVirtual *maqVirt, int tipoOp, int op);
void leerInstruccion(ETMaquinaVirtual *maqVirt, unsigned int *dirFisica);
int leerOperando(ETMaquinaVirtual *maqVirt, unsigned int *dirFisica, unsigned int bytesALeer);
int operandoDest(ETMaquinaVirtual *maqVirt, int tipoOp, int op);
void llamadaSistema(ETMaquinaVirtual *maqVirt, int tipoLlamada);

#endif // TRADUCTOR_H_INCLUDED
