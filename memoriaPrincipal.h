#ifndef MEMORIAPRINCIPAL_H_INCLUDED
#define MEMORIAPRINCIPAL_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "maquinaVirtual.h"

/// Traduccion de direccion logica a fisica. Retorna -1 si hay error (Fallo de segmento).
int32_t cambioLogicFisic(ETMaquinaVirtual *maqVirt, int32_t dirLogica);
uint8_t siguienteByte(ETMaquinaVirtual *maqVirt, int32_t *dirLogica);

/// Lectura y Escritura
int32_t readMem(ETMaquinaVirtual *maqVirt, int32_t dirLogica);
void writeMem(ETMaquinaVirtual *maqVirt, int32_t dirLogica, int32_t valor);

#endif // MEMORIAPRINCIPAL_H_INCLUDED
