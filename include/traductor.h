#ifndef TRADUCTOR_H
#define TRADUCTOR_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "errores.h"

typedef uint32_t dirLogica;
typedef uint32_t dirFisica;

// Traduce una dirección lógica a física. Si verificarLimite es 1, además
// valida que el acceso de 'cantidadBytes' entre dentro del segmento
// (excepción: la lectura de la instrucción, que llama con verificarLimite=0).
dirFisica traducir(dirLogica l, tabla_segmentos t, int cantidadBytes, int verificarLimite);

#endif