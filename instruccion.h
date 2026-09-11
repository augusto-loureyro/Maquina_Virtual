#ifndef INSTRUCCION_H_INCLUDED
#define INSTRUCCION_H_INCLUDED


#include <stdio.h>
#include <stdint.h>
#include "maquinaVirtual.h"
#include "traductor.h"
#include "memoriaPrincipal.h"


void ejecutarInstruction(ETMaquinaVirtual *maqVirt,TRInstruction inst);
bool verificarOverflow(int32_t valA, int32_t valB, int32_t resultado, char op);

#endif // INSTRUCCION_H_INCLUDED
