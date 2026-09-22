#ifndef MEMORIAPRINCIPAL_H_INCLUDED
#define MEMORIAPRINCIPAL_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "maquinaVirtual.h"

/// Traduccion de direccion logica a fisica. Retorna -1 si hay error (Fallo de segmento).
unsigned int cambioLogicFisic(ETMaquinaVirtual *maqVirt, unsigned int dirLogica, unsigned int tamBytes);
/// Lectura y Escritura
int8_t leerByteDirFisica(ETMaquinaVirtual *maqVirt, unsigned int dirFisica);
int readMem(ETMaquinaVirtual *maqVirt, unsigned int dirLogica, unsigned int tamBytes);
void writeMem(ETMaquinaVirtual *maqVirt, unsigned int dirLogica, int valor, unsigned int tamBytes);

#endif // MEMORIAPRINCIPAL_H_INCLUDED
