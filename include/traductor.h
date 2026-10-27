#ifndef TRADUCTOR_H
#define TRADUCTOR_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "registros.h"
#include "errores.h"

typedef uint32_t dirLogica;
typedef uint32_t dirFisica;


dirFisica traducir(dirLogica direccionLogica, tabla_segmentos tablaSegmentos, Registros registros, int cantidadBytes, int esAccesoAMemoria);

#endif // TRADUCTOR_H
