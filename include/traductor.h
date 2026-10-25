#ifndef TRADUCTOR_H
#define TRADUCTOR_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "registros.h"
#include "errores.h"

typedef uint32_t dirLogica;
typedef uint32_t dirFisica;


dirFisica traducir(dirLogica l, tabla_segmentos t, Registros r, int cantidadBytes, int esAccesoAMemoria);

#endif // TRADUCTOR_H
