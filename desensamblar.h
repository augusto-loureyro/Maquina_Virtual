#ifndef DESENSAMBLAR_H_INCLUDED
#define DESENSAMBLAR_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "traductor.h"
#include "memoriaPrincipal.h"

/// Muestra la instruccion en la consola.
char* mnemonico(int code);
char* nombreRegistro(int reg);
void armarOperando(char *operando, ETMaquinaVirtual *maqVirt, int op);
void mostrarInstruccion(ETMaquinaVirtual *maqVirt, unsigned int dirFisicaInc, unsigned int dirFisicaFin);

#endif // DESENSAMBLAR_H_INCLUDED
