#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED

#include <stdio.h>
#include "maquinaVirtual.h"


/// Tipos de operandos
#define OPNONE      0
#define OPREG       1   ///Registro
#define OPIMM       2   ///Inmediatos
#define OPMEM       3   ///Memoria

typedef struct {
    uint8_t operacion;
    uint8_t cantOperand;
    uint8_t tipoOpA;
    uint8_t tipoOpB;
    int32_t opAValor;
    int32_t opBValor;
}TRInstruction;

/// Decodifica una instruccion desde la memoria.
TRInstruction leerInstruccion(ETMaquinaVirtual *maqVirt, int32_t *dirFisica);
int32_t leerOperando(ETMaquinaVirtual *maqVirt, int32_t *dirFisica, uint8_t tipo);
int32_t operandoDest(ETMaquinaVirtual *maqVirt, uint8_t tipoOp, int32_t valorOp, int *memoriaP);

///-------------
void llamadaSistema(ETMaquinaVirtual *maqVirt, int32_t tipoLlamada);

#endif // TRADUCTOR_H_INCLUDED
