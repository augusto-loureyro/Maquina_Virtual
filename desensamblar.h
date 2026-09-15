#ifndef DESENSAMBLAR_H_INCLUDED
#define DESENSAMBLAR_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "traductor.h"

/// Muestra la instruccion en la consola.
char* mnemonico(uint8_t code);
char* nombreRegistro(uint8_t reg);
void mostrarInstruccion(ETMaquinaVirtual *maqVirt, TRInstruction inst, int32_t dirFisicaInc, int32_t dirFisicaFin);

#endif // DESENSAMBLAR_H_INCLUDED
