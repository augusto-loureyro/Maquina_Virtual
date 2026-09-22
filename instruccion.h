#ifndef INSTRUCCION_H_INCLUDED
#define INSTRUCCION_H_INCLUDED


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "maquinaVirtual.h"
#include "traductor.h"
#include "memoriaPrincipal.h"


void ejecutarInstruccion(ETMaquinaVirtual *maqVirt);
bool verificarOverflow(int valA, int valB, int resultado, char op);

#endif // INSTRUCCION_H_INCLUDED
