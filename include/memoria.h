#ifndef MEMORIA_H
#define MEMORIA_H

#include <stddef.h>
#include<stdint.h>
#include "loader.h"
#include "consola.h"
#include "registros.h"


#define BYTES_POR_KIB 1024
#define TAMANIO_MEMORIA_DEFAULT 16*BYTES_POR_KIB
#define TAMANIO_DATO 4 // los registros (y por lo tanto los accesos a memoria) son de 32 bits

typedef uint8_t *Memoria;

void reservarMemoria(Memoria *m,int sizeM);
void cargarMemoria(const char *nombreArchivo, Memoria m, ConfiguracionMV *cfg);
uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r);
void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r);

void cargarParamSegment(const char *archVMX, Memoria m, size_t capacidad, int argc, char *p[], uint16_t *sizePS);

#endif 